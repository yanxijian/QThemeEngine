#ifndef __QTHEME_ENGINE_STYLE_FAMILIES_H__
#define __QTHEME_ENGINE_STYLE_FAMILIES_H__

#include "style/style_ctx.hpp"

#include <QProxyStyle>
#include <QStyle>

class QPainter;
class QStyleOption;
class QStyleOptionComplex;
class QWidget;

namespace qtheme::style_detail
{
	[[nodiscard]] bool tryEditPrimitive(const StyleCtx& ctx, QStyle::PrimitiveElement element, const QStyleOption* option,
										QPainter* painter, const QWidget* widget);
	[[nodiscard]] bool tryCheckPrimitive(const StyleCtx& ctx, QStyle::PrimitiveElement element, const QStyleOption* option,
										 QPainter* painter, const QWidget* widget);
	[[nodiscard]] bool tryMenuPrimitive(const StyleCtx& ctx, QStyle::PrimitiveElement element, const QStyleOption* option,
										QPainter* painter, const QWidget* widget);
	[[nodiscard]] bool tryFocusPrimitive(const StyleCtx& ctx, QStyle::PrimitiveElement element, const QStyleOption* option,
										 QPainter* painter, const QWidget* widget);
	[[nodiscard]] bool tryButtonPrimitive(const StyleCtx& ctx, QStyle::PrimitiveElement element, const QStyleOption* option,
										  QPainter* painter, const QWidget* widget);
	[[nodiscard]] bool tryChromePrimitive(const StyleCtx& ctx, QStyle::PrimitiveElement element, const QStyleOption* option,
										  QPainter* painter, const QWidget* widget);
	[[nodiscard]] bool tryItemViewPrimitive(const StyleCtx& ctx, QStyle::PrimitiveElement element, const QStyleOption* option,
											QPainter* painter, const QWidget* widget);
	[[nodiscard]] bool tryComplexPrimitive(const StyleCtx& ctx, QStyle::PrimitiveElement element, const QStyleOption* option,
										   QPainter* painter, const QWidget* widget);

	/// Shared panel paint used by PE_PanelItemView* and CE_ItemViewItem (direct call).
	void paintItemViewPanel(const StyleCtx& ctx, const QStyleOption* option, QPainter* painter);

	[[nodiscard]] bool tryButtonControl(const StyleCtx& ctx, QStyle::ControlElement element, const QStyleOption* option, QPainter* painter,
										const QWidget* widget, const QProxyStyle* style);
	[[nodiscard]] bool tryTabHeaderControl(const StyleCtx& ctx, QStyle::ControlElement element, const QStyleOption* option,
										   QPainter* painter, const QWidget* widget, const QProxyStyle* style);
	[[nodiscard]] bool tryMenuControl(const StyleCtx& ctx, QStyle::ControlElement element, const QStyleOption* option, QPainter* painter,
									  const QWidget* widget, const QProxyStyle* style);
	[[nodiscard]] bool tryChromeControl(const StyleCtx& ctx, QStyle::ControlElement element, const QStyleOption* option, QPainter* painter,
										const QWidget* widget, const QProxyStyle* style);
	[[nodiscard]] bool tryProgressControl(const StyleCtx& ctx, QStyle::ControlElement element, const QStyleOption* option,
										  QPainter* painter, const QWidget* widget, const QProxyStyle* style);
	[[nodiscard]] bool tryItemViewControl(const StyleCtx& ctx, QStyle::ControlElement element, const QStyleOption* option,
										  QPainter* painter, const QWidget* widget, const QProxyStyle* style);
	[[nodiscard]] bool tryComplexControlLabel(const StyleCtx& ctx, QStyle::ControlElement element, const QStyleOption* option,
											  QPainter* painter, const QWidget* widget, const QProxyStyle* style);

	[[nodiscard]] bool tryComplexControl(const StyleCtx& ctx, QStyle::ComplexControl control, const QStyleOptionComplex* option,
										 QPainter* painter, const QWidget* widget, const QProxyStyle* style);
} // namespace qtheme::style_detail

#endif // __QTHEME_ENGINE_STYLE_FAMILIES_H__
