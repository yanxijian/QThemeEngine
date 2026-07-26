#ifndef __QTHEME_ENGINE_THEMEAPI_H__
#define __QTHEME_ENGINE_THEMEAPI_H__

#include <QColor>
#include <QObject>
#include <QString>
#include <QStyleOption>
#include <qtheme/qtheme_export.hpp>

namespace theme
{
	class ISkinManager;

	namespace ThemeApi
	{
		QTE_EXPORT void bind(ISkinManager* skin); // nullptr unbinds
		QTE_EXPORT ISkinManager* skin();
		QTE_EXPORT QObject* skinObject(); // for connect(skinChanged…); nullptr if unbound

		QTE_EXPORT QColor color(const QString& className, const QString& propName, const QColor& def = QColor());
		QTE_EXPORT int hint(const QString& className, const QString& propName, int def = 0);
		QTE_EXPORT bool isDarkSkin();

		QTE_EXPORT QString themeClassName(const QObject* obj);
		QTE_EXPORT void setThemeClassName(QObject* obj, const QString& className);

		QTE_EXPORT QString propWithState(const QString& baseProp, const QStyleOption* option);
	} // namespace ThemeApi
} // namespace theme

#endif // __QTHEME_ENGINE_THEMEAPI_H__
