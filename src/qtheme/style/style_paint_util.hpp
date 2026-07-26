#ifndef __QTHEME_ENGINE_STYLE_PAINT_UTIL_H__
#define __QTHEME_ENGINE_STYLE_PAINT_UTIL_H__

#include <QColor>
#include <QPainter>
#include <QRect>
#include <QStyleOption>
#include <QWidget>

namespace qtheme::style_detail
{
	void drawRounded(QPainter* painter, const QRect& rect, int radius, const QColor& fill, const QColor& border, qreal borderWidth = 1.0);

	/// WinUI Focus Visual: primary (outer) + secondary (inner). Pass innerWidth <= 0 for outer only.
	void drawFluentFocusRing(QPainter* painter, const QRect& rect, int radius, const QColor& outer, const QColor& inner, qreal outerWidth,
							 qreal innerWidth);

	void drawArrow(QPainter* painter, const QRect& rect, Qt::ArrowType type, const QColor& color);

	[[nodiscard]] QString stateBorderRole(const QStyleOption* option);

	[[nodiscard]] bool isTextEditLike(const QWidget* widget);

	/// Input chrome already paints focus via border.focus; skip PE_FrameFocusRect dual ring.
	[[nodiscard]] bool chromeOwnsFocusStroke(const QWidget* widget);

	void clearPopupToTransparent(QPainter* painter, const QRect& rect);
} // namespace qtheme::style_detail

#endif // __QTHEME_ENGINE_STYLE_PAINT_UTIL_H__
