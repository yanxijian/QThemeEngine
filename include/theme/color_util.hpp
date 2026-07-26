#ifndef __QTHEME_ENGINE_THEME_COLOR_UTIL_H__
#define __QTHEME_ENGINE_THEME_COLOR_UTIL_H__

#include <QColor>
#include <QStringView>
#include <qtheme/qtheme_export.hpp>

namespace theme
{
	/// Parse #RRGGBB or #RRGGBBAA (not Qt's #AARRGGBB).
	[[nodiscard]] QTE_EXPORT QColor parseColorLiteral(QStringView s);

	/// Same rules as parseColorLiteral; for Golden tests.
	[[nodiscard]] QTE_EXPORT QColor themeColorFromRgbaHex(QStringView hex);

	[[nodiscard]] QTE_EXPORT bool hasUtf8Bom(const QByteArray& bytes);
} // namespace theme

#endif // __QTHEME_ENGINE_THEME_COLOR_UTIL_H__
