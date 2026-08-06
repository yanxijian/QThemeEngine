#ifndef __QTHEME_ENGINE_API_H__
#define __QTHEME_ENGINE_API_H__

#include "types.hpp"

#include <QColor>
#include <QObject>
#include <QString>
#include <QStyleOption>
#include <qtheme/qtheme_export.hpp>

namespace qtheme
{
	class Engine;

	/// Thin facade for owner-drawn widgets (secondary path). Reads Engine::defaultEngine().
	namespace api
	{
		QTE_EXPORT void bind(Engine* engine);
		QTE_EXPORT Engine* engine();
		QTE_EXPORT QObject* engineObject();

		QTE_EXPORT QColor color(const QString& group, const QString& role, const QColor& def = QColor());
		QTE_EXPORT int metric(const QString& group, const QString& role, int def = 0);
		/// Logical-pixel scale from the bound Engine's QThemeStyle (1.0 = 96 DPI).
		[[nodiscard]] QTE_EXPORT qreal dpiScale();
		/// ThemeStore metric multiplied by dpiScale(); for owner-drawn chrome (e.g. Ribbon).
		[[nodiscard]] QTE_EXPORT int scaledMetric(const QString& group, const QString& role, int def = 0);

		QTE_EXPORT QString themeClass(const QObject* obj);
		QTE_EXPORT void setThemeClass(QObject* obj, const QString& name);

		QTE_EXPORT QString roleWithState(const QString& baseRole, const QStyleOption* option);
	} // namespace api
} // namespace qtheme

#endif // __QTHEME_ENGINE_API_H__
