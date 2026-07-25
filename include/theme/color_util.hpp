#ifndef __QTHEME_ENGINE_THEME_COLOR_UTIL_H__
#define __QTHEME_ENGINE_THEME_COLOR_UTIL_H__

#include <QColor>
#include <QStringView>

namespace theme
{
	/// Parse #RRGGBB or #RRGGBBAA (not Qt's #AARRGGBB).
	[[nodiscard]] QColor parseColorLiteral(QStringView s);

	/// Same rules as parseColorLiteral; for Golden tests.
	[[nodiscard]] QColor themeColorFromRgbaHex(QStringView hex);

	[[nodiscard]] bool hasUtf8Bom(const QByteArray& bytes);
} // namespace theme

#endif // __QTHEME_ENGINE_THEME_COLOR_UTIL_H__
