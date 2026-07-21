#include "qtheme/engine.hpp"

#include <QApplication>
#include <QCoreApplication>
#include <QWidget>

namespace qtheme {

namespace {
Engine* g_default = nullptr;
} // namespace

Engine::Engine(QObject* parent)
	: QObject(parent)
	, store_(std::make_shared<ThemeStore>(ThemeStore::seedLight()))
{
	currentSkin_ = QStringLiteral("light");
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

	if (!style_)
	{
		// QApplication::setStyle takes ownership of the style object.
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

	for (QWidget* w : app->topLevelWidgets())
	{
		if (w)
		{
			w->update();
		}
	}
}

bool Engine::switchSkin(const QString& name, bool force)
{
	if (!force && name == currentSkin_ && inited_)
	{
		return true;
	}

	auto next = std::make_shared<ThemeStore>();
	if (name == QLatin1String("light"))
	{
		*next = ThemeStore::seedLight();
	}
	else if (name == QLatin1String("dark"))
	{
		*next = ThemeStore::seedDark();
	}
	else
	{
		lastError_ = Error::SkinNotFound;
		return false;
	}

	const QString previous = currentSkin_;
	store_ = std::move(next);
	currentSkin_ = name;
	lastError_ = Error::None;

	if (style_)
	{
		style_->setStore(store_);
		if (auto* app = qobject_cast<QApplication*>(QCoreApplication::instance()))
		{
			app->setPalette(style_->standardPalette());
			for (QWidget* w : app->topLevelWidgets())
			{
				if (w)
				{
					w->update();
				}
			}
		}
	}

	emit skinChanged(previous, currentSkin_);
	return true;
}

} // namespace qtheme
