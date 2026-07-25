#include "style/style_families.hpp"
#include "style/style_paint_util.hpp"

#include <QCheckBox>
#include <QPainter>
#include <QRadioButton>
#include <QStyleOptionFocusRect>

namespace qtheme::style_detail
{
	bool tryFocusPrimitive(const StyleCtx& ctx, QStyle::PrimitiveElement element, const QStyleOption* option, QPainter* painter,
						   const QWidget* widget)
	{
		if (element == QStyle::PE_FrameFocusRect)
		{
			QColor ambient;
			if (const auto* fr = qstyleoption_cast<const QStyleOptionFocusRect*>(option))
			{
				ambient = fr->backgroundColor;
			}
			if (!ambient.isValid() && widget)
			{
				ambient = widget->palette().color(widget->backgroundRole());
			}
			if (!ambient.isValid())
			{
				ambient = ctx.roleColor(QStringLiteral("palette"), QStringLiteral("canvas"), option->palette.color(QPalette::Window));
			}
			QColor outer;
			QColor inner;
			ctx.focusStrokeColors(ambient, &outer, &inner);
			const int radius = ctx.roleMetric(QStringLiteral("focus"), QStringLiteral("radius"), 4);
			// Check/Radio: 1px single stroke (minimum). Other controls keep dual-stroke metrics.
			const bool checkLike = qobject_cast<const QCheckBox*>(widget) || qobject_cast<const QRadioButton*>(widget);
			const qreal outerW = checkLike ? qreal(ctx.roleMetric(QStringLiteral("check"), QStringLiteral("focusWidth"), 1))
										   : qreal(ctx.roleMetric(QStringLiteral("focus"), QStringLiteral("outer"), 2));
			const qreal innerW = checkLike ? 0.0 : qreal(ctx.roleMetric(QStringLiteral("focus"), QStringLiteral("inner"), 1));
			drawFluentFocusRing(painter, option->rect, radius, outer, inner, outerW, innerW);
			return true;
		}
		return false;
	}
} // namespace qtheme::style_detail
