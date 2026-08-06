#include "qtheme/engine.hpp"

#include "qtheme/accent.hpp"
#include "qtheme/pack_registry.hpp"
#include "qtheme/settings.hpp"
#include "qtheme/types.hpp"

#include <QApplication>
#include <QColor>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QPalette>
#include <QScreen>
#include <QSettings>
#include <QStyleHints>
#include <QWidget>

namespace qtheme
{
	namespace
	{
		Engine* g_default = nullptr;

		void publishWindowChromeProperties(QApplication* app, const ThemeStore* store)
		{
			if (!app || !store)
			{
				return;
			}
			const int radius = store->metric(QStringLiteral("window"), QStringLiteral("radius"), 8);
			const int borderWidth = store->metric(QStringLiteral("window"), QStringLiteral("borderWidth"), 1);
			app->setProperty(kWindowRadiusProperty, radius);
			app->setProperty(kWindowBorderWidthProperty, borderWidth);

			ColorValue border = store->color(QStringLiteral("window"), QStringLiteral("border"));
			if (!border.ok)
			{
				border = store->color(QStringLiteral("frame"), QStringLiteral("border"));
			}
			if (!border.ok)
			{
				border = store->color(QStringLiteral("palette"), QStringLiteral("stroke"), QColor(0xd1, 0xd1, 0xd1));
			}
			app->setProperty(kWindowBorderProperty, border.ok ? border.value : QColor(0xd1, 0xd1, 0xd1));
		}
	} // namespace

	Engine::Engine(QObject* parent)
		: QObject(parent)
		, m_store(std::make_shared<ThemeStore>())
		, m_packs(std::make_unique<PackRegistry>())
	{
		Error err = Error::None;
		if (!m_packs->registerBuiltinFluentPacks(&err))
		{
			m_lastError = err;
		}
		m_accentFollowSystem = true;
		m_followOsHighContrast = true;
		m_accent = AccentResolver::systemAccent();
		(void)rebuildStore(QString::fromUtf8(kPackFluentLight), true);
		m_currentPack = QString::fromUtf8(kPackFluentLight);
		m_colorScheme = ColorScheme::Light;
	}

	Engine::~Engine()
	{
		if (g_default == this)
		{
			g_default = nullptr;
		}
	}

	void Engine::setDefault(Engine* engine)
	{
		g_default = engine;
	}

	Engine* Engine::defaultEngine()
	{
		return g_default;
	}

	void Engine::installOsHooks(QGuiApplication* app)
	{
		if (!app || m_osHooksInstalled)
		{
			return;
		}
		m_osHooksInstalled = true;
		connect(app, &QGuiApplication::paletteChanged, this, &Engine::onOsPaletteChanged);
		if (QStyleHints* hints = app->styleHints())
		{
			connect(hints, &QStyleHints::colorSchemeChanged, this, &Engine::onOsColorSchemeChanged);
		}
		if (QScreen* screen = app->primaryScreen())
		{
			connect(screen, &QScreen::logicalDotsPerInchChanged, this,
					[this](qreal)
					{
						updateDpiScale();
						refreshUi();
					});
		}
	}

	void Engine::updateDpiScale()
	{
		qreal scale = 1.0;
		if (auto* app = qobject_cast<QGuiApplication*>(QCoreApplication::instance()))
		{
			if (QScreen* screen = app->primaryScreen())
			{
				scale = screen->logicalDotsPerInch() / 96.0;
			}
		}
		if (!m_style)
		{
			return;
		}
		const qreal previous = m_style->dpiScale();
		m_style->setDpiScale(scale);
		if (!qFuzzyCompare(previous, scale))
		{
			emit dpiScaleChanged(scale);
		}
	}

	void Engine::syncFromOsAppearance(bool forcePackReload)
	{
		if (m_syncingFromOs)
		{
			return;
		}
		m_syncingFromOs = true;

		bool changed = false;

		if (m_colorScheme == ColorScheme::System)
		{
			const QString packId = packIdForScheme(ColorScheme::System);
			if (forcePackReload || packId != m_currentPack)
			{
				if (applyPack(packId, ColorScheme::System, true))
				{
					changed = true;
				}
			}
		}

		if (m_accentFollowSystem)
		{
			const QColor next = AccentResolver::systemAccent();
			if (next.isValid() && next != m_accent)
			{
				m_accent = next;
				applyAccentToStore();
				changed = true;
				emit accentChanged(m_accent);
			}
		}

		if (changed)
		{
			refreshUi();
		}

		m_syncingFromOs = false;
	}

	void Engine::onOsPaletteChanged(const QPalette& /*palette*/)
	{
		// Accent follow-system, and System scheme (incl. OS HC ↔ fluent.hc).
		if (m_accentFollowSystem || m_colorScheme == ColorScheme::System)
		{
			syncFromOsAppearance(false);
		}
	}

	void Engine::onOsColorSchemeChanged(Qt::ColorScheme /*scheme*/)
	{
		if (m_colorScheme == ColorScheme::System)
		{
			syncFromOsAppearance(true);
		}
	}

	void Engine::refreshUi()
	{
		updateDpiScale();
		if (m_style)
		{
			m_style->setStore(m_store);
		}
		if (auto* app = qobject_cast<QApplication*>(QCoreApplication::instance()))
		{
			if (m_style)
			{
				app->setPalette(m_style->standardPalette());
			}
			publishWindowChromeProperties(app, m_store.get());
			for (QWidget* w : app->topLevelWidgets())
			{
				if (w)
				{
					w->update();
				}
			}
		}
	}

	void Engine::applyAccentToStore()
	{
		if (!m_store)
		{
			return;
		}
		QColor accent = m_accent;
		if (m_accentFollowSystem)
		{
			accent = AccentResolver::systemAccent();
			m_accent = accent;
		}
		AccentResolver::applyAccentPatch(m_store.get(), accent);
	}

	bool Engine::rebuildStore(const QString& packId, bool force)
	{
		Q_UNUSED(force);
		auto next = std::make_shared<ThemeStore>();
		Error err = Error::None;
		if (!m_packs->materialize(packId, next.get(), &err))
		{
			m_lastError = err;
			return false;
		}
		m_store = std::move(next);
		applyAccentToStore();
		m_lastError = Error::None;
		return true;
	}

	QString Engine::packIdForScheme(ColorScheme scheme) const
	{
		switch (scheme)
		{
		case ColorScheme::Dark:
			return QString::fromUtf8(kPackFluentDark);
		case ColorScheme::HighContrast:
			return QString::fromUtf8(kPackFluentHc);
		case ColorScheme::System:
		{
			if (m_followOsHighContrast && AccentResolver::systemHighContrast())
			{
				return QString::fromUtf8(kPackFluentHc);
			}
			if (auto* app = qobject_cast<QGuiApplication*>(QCoreApplication::instance()))
			{
				if (app->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
				{
					return QString::fromUtf8(kPackFluentDark);
				}
			}
			return QString::fromUtf8(kPackFluentLight);
		}
		case ColorScheme::Light:
		default:
			return QString::fromUtf8(kPackFluentLight);
		}
	}

	void Engine::apply(QApplication* app, bool clearStyleSheets)
	{
		if (!app || !m_store)
		{
			return;
		}

		if (clearStyleSheets)
		{
			app->setStyleSheet(QString());
			const auto widgets = app->allWidgets();
			for (QWidget* w : widgets)
			{
				if (w && !w->styleSheet().isEmpty())
				{
					w->setStyleSheet(QString());
				}
			}
		}

		installOsHooks(app);

		if (m_accentFollowSystem)
		{
			m_accent = AccentResolver::systemAccent(app);
			applyAccentToStore();
		}

		if (m_colorScheme == ColorScheme::System)
		{
			(void)rebuildStore(packIdForScheme(ColorScheme::System), true);
			m_currentPack = packIdForScheme(ColorScheme::System);
		}

		if (!m_style)
		{
			m_style = new QThemeStyle(m_store);
		}
		else
		{
			m_style->setStore(m_store);
		}

		updateDpiScale();
		app->setStyle(m_style);
		app->setPalette(m_style->standardPalette());
		publishWindowChromeProperties(app, m_store.get());
		setDefault(this);
		m_inited = true;
		refreshUi();
	}

	bool Engine::applyPack(const QString& packId, ColorScheme scheme, bool force)
	{
		if (!force && packId == m_currentPack && scheme == m_colorScheme && m_inited)
		{
			return true;
		}

		if (!rebuildStore(packId, force))
		{
			return false;
		}

		const QString previousPack = m_currentPack;
		const ColorScheme previousScheme = m_colorScheme;
		m_currentPack = packId;
		m_colorScheme = scheme;

		refreshUi();
		if (previousPack != m_currentPack)
		{
			emit packChanged(previousPack, m_currentPack);
		}
		if (previousScheme != m_colorScheme)
		{
			emit colorSchemeChanged();
		}
		onAutoSavePreferences();
		return true;
	}

	bool Engine::switchPack(const QString& name, bool force)
	{
		QString resolved = name;
		if (name == QLatin1String("light"))
		{
			resolved = QString::fromUtf8(kPackFluentLight);
		}
		else if (name == QLatin1String("dark"))
		{
			resolved = QString::fromUtf8(kPackFluentDark);
		}
		else if (name == QLatin1String("hc") || name == QLatin1String("highcontrast"))
		{
			resolved = QString::fromUtf8(kPackFluentHc);
		}

		ColorScheme scheme = m_colorScheme;
		if (resolved == QLatin1String(kPackFluentLight))
		{
			scheme = ColorScheme::Light;
		}
		else if (resolved == QLatin1String(kPackFluentDark))
		{
			scheme = ColorScheme::Dark;
		}
		else if (resolved == QLatin1String(kPackFluentHc))
		{
			scheme = ColorScheme::HighContrast;
		}

		return applyPack(resolved, scheme, force);
	}

	bool Engine::setColorScheme(ColorScheme scheme, bool force)
	{
		return applyPack(packIdForScheme(scheme), scheme, force);
	}

	void Engine::setFollowOsHighContrast(bool follow)
	{
		m_followOsHighContrast = follow;
		if (m_colorScheme == ColorScheme::System)
		{
			(void)setColorScheme(ColorScheme::System, true);
		}
		onAutoSavePreferences();
	}

	bool Engine::setAccent(const QColor& accent)
	{
		if (!accent.isValid())
		{
			m_lastError = Error::PackInvalid;
			return false;
		}
		m_accentFollowSystem = false;
		m_accent = accent;
		applyAccentToStore();
		refreshUi();
		emit accentChanged(m_accent);
		onAutoSavePreferences();
		return true;
	}

	bool Engine::setAccentFollowSystem(bool follow)
	{
		m_accentFollowSystem = follow;
		if (follow)
		{
			m_accent = AccentResolver::systemAccent();
		}
		applyAccentToStore();
		refreshUi();
		emit accentChanged(m_accent);
		onAutoSavePreferences();
		return true;
	}

	bool Engine::registerPack(const QString& pathOrQrc)
	{
		Error err = Error::None;
		if (!m_packs->registerPackFile(pathOrQrc, &err))
		{
			m_lastError = err;
			return false;
		}
		if (!pathOrQrc.startsWith(QLatin1Char(':')))
		{
			if (!m_extraPackFiles.contains(pathOrQrc))
			{
				m_extraPackFiles.append(pathOrQrc);
			}
		}
		m_lastError = Error::None;
		onAutoSavePreferences();
		return true;
	}

	void Engine::addPackSearchPath(const QString& dir)
	{
		if (dir.isEmpty() || m_packSearchPaths.contains(dir))
		{
			return;
		}
		m_packSearchPaths.append(dir);
		onAutoSavePreferences();
	}

	int Engine::scanPackSearchPaths()
	{
		if (!m_packs)
		{
			return 0;
		}
		int total = 0;
		for (const QString& dir : m_packSearchPaths)
		{
			total += m_packs->registerPacksInDirectory(dir, nullptr);
		}
		return total;
	}

	QStringList Engine::registeredPacks() const
	{
		return m_packs ? m_packs->packIds() : QStringList{};
	}

	AppearancePrefs Engine::appearancePrefs() const
	{
		AppearancePrefs prefs;
		prefs.packId = m_currentPack;
		prefs.colorScheme = m_colorScheme;
		prefs.accentFollowSystem = m_accentFollowSystem;
		prefs.accent = m_accent;
		prefs.followOsHighContrast = m_followOsHighContrast;
		prefs.packSearchPaths = m_packSearchPaths;
		prefs.extraPackFiles = m_extraPackFiles;
		return prefs;
	}

	bool Engine::applyAppearancePrefs(const AppearancePrefs& prefs)
	{
		m_loadingPreferences = true;

		m_packSearchPaths = prefs.packSearchPaths;
		m_extraPackFiles = prefs.extraPackFiles;
		(void)scanPackSearchPaths();
		for (const QString& file : m_extraPackFiles)
		{
			Error err = Error::None;
			(void)m_packs->registerPackFile(file, &err);
		}

		m_followOsHighContrast = prefs.followOsHighContrast;

		if (prefs.accentFollowSystem)
		{
			(void)setAccentFollowSystem(true);
		}
		else
		{
			(void)setAccent(prefs.accent);
		}

		bool ok = false;
		if (prefs.colorScheme == ColorScheme::System)
		{
			ok = setColorScheme(ColorScheme::System, true);
		}
		else
		{
			ok = applyPack(prefs.packId.isEmpty() ? packIdForScheme(prefs.colorScheme) : prefs.packId, prefs.colorScheme, true);
		}

		m_loadingPreferences = false;
		return ok;
	}

	bool Engine::savePreferences(QSettings* settings) const
	{
		if (settings)
		{
			return saveAppearancePrefs(settings, appearancePrefs());
		}
		QSettings local;
		return saveAppearancePrefs(&local, appearancePrefs());
	}

	bool Engine::loadPreferences(QSettings* settings)
	{
		AppearancePrefs prefs;
		bool ok = false;
		if (settings)
		{
			ok = loadAppearancePrefs(settings, &prefs);
		}
		else
		{
			QSettings local;
			ok = loadAppearancePrefs(&local, &prefs);
		}
		if (!ok)
		{
			return false;
		}
		return applyAppearancePrefs(prefs);
	}

	void Engine::setAutoSavePreferences(bool enable)
	{
		m_autoSavePreferences = enable;
	}

	void Engine::onAutoSavePreferences()
	{
		if (!m_autoSavePreferences || m_loadingPreferences)
		{
			return;
		}
		(void)savePreferences(nullptr);
	}
} // namespace qtheme