#include "qtheme/engine.hpp"

#include "qtheme/accent.hpp"
#include "qtheme/pack.hpp"

#include <QApplication>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QPalette>
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
	lastError_ = Error::None;
	return true;
}

QStringList Engine::registeredPacks() const
{
	return packs_ ? packs_->packIds() : QStringList{};
}

} // namespace qtheme
