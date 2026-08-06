#include "qtheme/api.hpp"

#include "qtheme/engine.hpp"
#include "qtheme/theme_style.hpp"

#include <QCoreApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QStyle>
#include <QtMath>

namespace qtheme
{
	namespace api
	{
		namespace
		{
			Engine* g_bound = nullptr;
		} // namespace

		void bind(Engine* engine)
		{
			g_bound = engine;
			Engine::setDefault(engine);
		}

		Engine* engine()
		{
			return g_bound ? g_bound : Engine::defaultEngine();
		}

		QObject* engineObject()
		{
			return engine();
		}

		QColor color(const QString& group, const QString& role, const QColor& def)
		{
			Engine* e = engine();
			if (!e || !e->store())
			{
				return def;
			}
			const ColorValue cv = e->store()->color(group, role, def);
			return cv.ok ? cv.value : def;
		}

		int metric(const QString& group, const QString& role, int def)
		{
			Engine* e = engine();
			if (!e || !e->store())
			{
				return def;
			}
			return e->store()->metric(group, role, def);
		}

		qreal dpiScale()
		{
			Engine* e = engine();
			if (e && e->style())
			{
				return e->style()->dpiScale();
			}
			if (auto* app = qobject_cast<QGuiApplication*>(QCoreApplication::instance()))
			{
				if (QScreen* screen = app->primaryScreen())
				{
					return screen->logicalDotsPerInch() / 96.0;
				}
			}
			return 1.0;
		}

		int scaledMetric(const QString& group, const QString& role, int def)
		{
			return qRound(static_cast<qreal>(metric(group, role, def)) * dpiScale());
		}

		QString themeClass(const QObject* obj)
		{
			if (!obj)
			{
				return {};
			}
			return obj->property(kThemeClassProperty).toString();
		}

		void setThemeClass(QObject* obj, const QString& name)
		{
			if (obj)
			{
				obj->setProperty(kThemeClassProperty, name);
			}
		}

		QString roleWithState(const QString& baseRole, const QStyleOption* option)
		{
			if (!option)
			{
				return baseRole;
			}
			if (!(option->state & QStyle::State_Enabled))
			{
				return baseRole + QStringLiteral(".disabled");
			}
			if (option->state & QStyle::State_Sunken)
			{
				return baseRole + QStringLiteral(".pressed");
			}
			if (option->state & QStyle::State_MouseOver)
			{
				return baseRole + QStringLiteral(".hover");
			}
			if (option->state & QStyle::State_HasFocus)
			{
				return baseRole + QStringLiteral(".focus");
			}
			return baseRole;
		}
	} // namespace api
} // namespace qtheme
