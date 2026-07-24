#include "qtheme/engine.hpp"

#include "qtheme/accent.hpp"
#include "qtheme/pack.hpp"
#include "qtheme/settings.hpp"

#include <QApplication>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QPalette>
#include <QScreen>
#include <QSettings>
#include <QStyleHints>
#include <QWidget>

namespace qtheme {

namespace {
Engine* g_default = nullptr;
} // namespace

Engine::Engine(QObject* parent)
	: QObject(parent)
	, store_(std::make_shared<ThemeStore>())
	, packs_(std::make_unique<PackRegistry>())
{
	Error err = Error::None;
	if (!packs_->registerBuiltinFluentPacks(&err))
	{
		lastError_ = err;
	}
	accentFollowSystem_ = true;
	followOsHighContrast_ = true;
	accent_ = AccentResolver::systemAccent();
	(void)rebuildStore(QString::fromUtf8(kPackFluentLight), true);
	currentSkin_ = QString::fromUtf8(kPackFluentLight);
	colorScheme_ = ColorScheme::Light;
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
	if (!app || osHooksInstalled_)
	{
		return;
	}
	osHooksInstalled_ = true;
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
	if (style_)
	{
		style_->setDpiScale(scale);
	}
}

void Engine::syncFromOsAppearance(bool forcePackReload)
{
	if (syncingFromOs_)
	{
		return;
	}
	syncingFromOs_ = true;

	bool changed = false;

	if (colorScheme_ == ColorScheme::System)
	{
		const QString packId = packIdForScheme(ColorScheme::System);
		if (forcePackReload || packId != currentSkin_)
		{
			if (applyPack(packId, ColorScheme::System, true))
			{
				changed = true;
			}
		}
	}

	if (accentFollowSystem_)
	{
		const QColor next = AccentResolver::systemAccent();
		if (next.isValid() && next != accent_)
		{
			accent_ = next;
			applyAccentToStore();
			changed = true;
			emit accentChanged(accent_);
		}
	}

	if (changed)
	{
		refreshUi();
	}

	syncingFromOs_ = false;
}

void Engine::onOsPaletteChanged(const QPalette& /*palette*/)
{
	// Accent follow-system, and System scheme (incl. OS HC ↔ fluent.hc).
	if (accentFollowSystem_ || colorScheme_ == ColorScheme::System)
	{
		syncFromOsAppearance(false);
	}
}

void Engine::onOsColorSchemeChanged(Qt::ColorScheme /*scheme*/)
{
	if (colorScheme_ == ColorScheme::System)
	{
		syncFromOsAppearance(true);
	}
}

void Engine::refreshUi()
{
	updateDpiScale();
	if (style_)
	{
		style_->setStore(store_);
	}
	if (auto* app = qobject_cast<QApplication*>(QCoreApplication::instance()))
	{
		if (style_)
		{
			app->setPalette(style_->standardPalette());
		}
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
	if (!store_)
	{
		return;
	}
	QColor accent = accent_;
	if (accentFollowSystem_)
	{
		accent = AccentResolver::systemAccent();
		accent_ = accent;
	}
	AccentResolver::applyAccentPatch(store_.get(), accent);
}

bool Engine::rebuildStore(const QString& packId, bool force)
{
	Q_UNUSED(force);
	auto next = std::make_shared<ThemeStore>();
	Error err = Error::None;
	if (!packs_->materialize(packId, next.get(), &err))
	{
		lastError_ = err;
		return false;
	}
	store_ = std::move(next);
	applyAccentToStore();
	lastError_ = Error::None;
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
		if (followOsHighContrast_ && AccentResolver::systemHighContrast())
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
	if (!app || !store_)
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

	if (accentFollowSystem_)
	{
		accent_ = AccentResolver::systemAccent(app);
		applyAccentToStore();
	}

	if (colorScheme_ == ColorScheme::System)
	{
		(void)rebuildStore(packIdForScheme(ColorScheme::System), true);
		currentSkin_ = packIdForScheme(ColorScheme::System);
	}

	if (!style_)
	{
		style_ = new QThemeStyle(store_);
	}
	else
	{
		style_->setStore(store_);
	}

	updateDpiScale();
	app->setStyle(style_);
	app->setPalette(style_->standardPalette());
	setDefault(this);
	inited_ = true;
	refreshUi();
}

bool Engine::applyPack(const QString& packId, ColorScheme scheme, bool force)
{
	if (!force && packId == currentSkin_ && scheme == colorScheme_ && inited_)
	{
		return true;
	}

	if (!rebuildStore(packId, force))
	{
		return false;
	}

	const QString previousSkin = currentSkin_;
	const ColorScheme previousScheme = colorScheme_;
	currentSkin_ = packId;
	colorScheme_ = scheme;

	refreshUi();
	if (previousSkin != currentSkin_)
	{
		emit skinChanged(previousSkin, currentSkin_);
	}
	if (previousScheme != colorScheme_)
	{
		emit colorSchemeChanged();
	}
	onAutoSavePreferences();
	return true;
}

bool Engine::switchSkin(const QString& name, bool force)
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

	ColorScheme scheme = colorScheme_;
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
	followOsHighContrast_ = follow;
	if (colorScheme_ == ColorScheme::System)
	{
		(void)setColorScheme(ColorScheme::System, true);
	}
	onAutoSavePreferences();
}

bool Engine::setAccent(const QColor& accent)
{
	if (!accent.isValid())
	{
		lastError_ = Error::PackInvalid;
		return false;
	}
	accentFollowSystem_ = false;
	accent_ = accent;
	applyAccentToStore();
	refreshUi();
	emit accentChanged(accent_);
	onAutoSavePreferences();
	return true;
}

bool Engine::setAccentFollowSystem(bool follow)
{
	accentFollowSystem_ = follow;
	if (follow)
	{
		accent_ = AccentResolver::systemAccent();
	}
	applyAccentToStore();
	refreshUi();
	emit accentChanged(accent_);
	onAutoSavePreferences();
	return true;
}

bool Engine::registerPack(const QString& pathOrQrc)
{
	Error err = Error::None;
	if (!packs_->registerPackFile(pathOrQrc, &err))
	{
		lastError_ = err;
		return false;
	}
	if (!pathOrQrc.startsWith(QLatin1Char(':')))
	{
		if (!extraPackFiles_.contains(pathOrQrc))
		{
			extraPackFiles_.append(pathOrQrc);
		}
	}
	lastError_ = Error::None;
	onAutoSavePreferences();
	return true;
}

void Engine::addPackSearchPath(const QString& dir)
{
	if (dir.isEmpty() || packSearchPaths_.contains(dir))
	{
		return;
	}
	packSearchPaths_.append(dir);
	onAutoSavePreferences();
}

int Engine::scanPackSearchPaths()
{
	if (!packs_)
	{
		return 0;
	}
	int total = 0;
	for (const QString& dir : packSearchPaths_)
	{
		total += packs_->registerPacksInDirectory(dir, nullptr);
	}
	return total;
}

QStringList Engine::registeredPacks() const
{
	return packs_ ? packs_->packIds() : QStringList{};
}

AppearancePrefs Engine::appearancePrefs() const
{
	AppearancePrefs prefs;
	prefs.skinId = currentSkin_;
	prefs.colorScheme = colorScheme_;
	prefs.accentFollowSystem = accentFollowSystem_;
	prefs.accent = accent_;
	prefs.followOsHighContrast = followOsHighContrast_;
	prefs.packSearchPaths = packSearchPaths_;
	prefs.extraPackFiles = extraPackFiles_;
	return prefs;
}

bool Engine::applyAppearancePrefs(const AppearancePrefs& prefs)
{
	loadingPreferences_ = true;

	packSearchPaths_ = prefs.packSearchPaths;
	extraPackFiles_ = prefs.extraPackFiles;
	(void)scanPackSearchPaths();
	for (const QString& file : extraPackFiles_)
	{
		Error err = Error::None;
		(void)packs_->registerPackFile(file, &err);
	}

	followOsHighContrast_ = prefs.followOsHighContrast;

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
		ok = applyPack(prefs.skinId.isEmpty() ? packIdForScheme(prefs.colorScheme) : prefs.skinId,
					   prefs.colorScheme, true);
	}

	loadingPreferences_ = false;
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
	autoSavePreferences_ = enable;
}

void Engine::onAutoSavePreferences()
{
	if (!autoSavePreferences_ || loadingPreferences_)
	{
		return;
	}
	(void)savePreferences(nullptr);
}

} // namespace qtheme