#pragma once

#include <QColor>
#include <QObject>
#include <QString>
#include <QStyleOption>

namespace theme
{
	class ISkinManager;

	namespace ThemeApi
	{
		void bind(ISkinManager* skin); // nullptr unbinds
		ISkinManager* skin();
		QObject* skinObject(); // for connect(skinChanged…); nullptr if unbound

		QColor color(const QString& className, const QString& propName, const QColor& def = QColor());
		int hint(const QString& className, const QString& propName, int def = 0);
		bool isDarkSkin();

		QString themeClassName(const QObject* obj);
		void setThemeClassName(QObject* obj, const QString& className);

		QString propWithState(const QString& baseProp, const QStyleOption* option);
	} // namespace ThemeApi
} // namespace theme
