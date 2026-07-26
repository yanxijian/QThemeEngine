#ifndef __QTHEME_ENGINE_QTHEME_COLOR_UTIL_H__
#define __QTHEME_ENGINE_QTHEME_COLOR_UTIL_H__

#include <QColor>
#include <QStringView>
#include <qtheme/qtheme_export.hpp>

namespace qtheme
{
	[[nodiscard]] QTE_EXPORT QColor parseColorLiteral(QStringView s);
	[[nodiscard]] QTE_EXPORT QColor colorFromRgbaHex(QStringView hex);
	[[nodiscard]] QTE_EXPORT bool hasUtf8Bom(const QByteArray& bytes);

	/// WCAG relative luminance in [0, 1] (sRGB).
	[[nodiscard]] QTE_EXPORT qreal relativeLuminance(const QColor& c);

	/// Environment-aware focus contrast: light ambient → -1 (darken), dark ambient → +1 (lighten).
	[[nodiscard]] QTE_EXPORT int focusContrastDirection(const QColor& ambient, qreal lightThreshold = 0.4);

	/// Focus_Lightness = Base_Lightness + (Direction * Offset); achromatic outer stroke.
	/// @param lightnessOffset HSL L delta on Qt's 0–255 scale (clamped).
	[[nodiscard]] QTE_EXPORT QColor focusOuterFromAmbient(const QColor& ambient, int lightnessOffset = 165, qreal lightThreshold = 0.4);

	/// Secondary stroke opposite the outer (separates ring from chrome).
	[[nodiscard]] QTE_EXPORT QColor focusInnerFromOuter(const QColor& outer);
} // namespace qtheme

#endif // __QTHEME_ENGINE_QTHEME_COLOR_UTIL_H__
