#include "theme/themeapi.hpp"

#include "theme/iskinmanager.hpp"
#include "theme/ithemeloader.hpp"

#include <QStyle>

namespace theme
{
	namespace
	{
		ISkinManager* g_skin = nullptr;
		QObject* g_skinObject = nullptr;
	} // namespace

	namespace ThemeApi
	{
		void bind(ISkinManager* skin)
		{
			g_skin = skin;
			g_skinObject = skin ? dynamic_cast<QObject*>(skin) : nullptr;
		}

		ISkinManager* skin()
		{
			return g_skin;
		}

		QObject* skinObject()
		{
			return g_skinObject;
		}

		QColor color(const QString& className, const QString& propName, const QColor& def)
		{
			if (!g_skin || !g_skin->theme())
			{
				return def;
			}
			return g_skin->theme()->color(className, propName, def).value;
		}

		int hint(const QString& className, const QString& propName, int def)
		{
			if (!g_skin || !g_skin->theme())
			{
				return def;
			}
			return g_skin->theme()->hint(className, propName, def);
		}

		bool isDarkSkin()
		{
			if (!g_skin || !g_skin->theme())
			{
				return false;
			}
			bool ok = false;
			const int h = g_skin->theme()->hint(QStringLiteral("AppPalette"), QStringLiteral("is-dark-skin"), 0, &ok);
			if (ok)
			{
				return h == 1;
			}
			const QString name = g_skin->current().name;
			return name.compare(QStringLiteral("dark"), Qt::CaseInsensitive) == 0
				   || name.endsWith(QStringLiteral("_dark"), Qt::CaseInsensitive);
		}

		QString themeClassName(const QObject* obj)
		{
			if (!obj)
			{
				return {};
			}
			return obj->property("themeClassName").toString();
		}

		void setThemeClassName(QObject* obj, const QString& className)
		{
			if (obj)
			{
				obj->setProperty("themeClassName", className);
			}
		}

		QString propWithState(const QString& baseProp, const QStyleOption* option)
		{
			if (!option)
			{
				return baseProp;
			}
			const auto st = option->state;
			if (!(st & QStyle::State_Enabled))
			{
				return baseProp + QStringLiteral("-disabled");
			}
			if (st & QStyle::State_Sunken)
			{
				return baseProp + QStringLiteral("-down");
			}
			if (st & QStyle::State_MouseOver)
			{
				return baseProp + QStringLiteral("-hover");
			}
			if (st & QStyle::State_Selected)
			{
				return baseProp + QStringLiteral("-selected");
			}
			return baseProp;
		}
	} // namespace ThemeApi
} // namespace theme
