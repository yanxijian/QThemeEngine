#include "style/style_families.hpp"
#include "style/style_paint_util.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

namespace qtheme::style_detail
{
	bool tryCheckPrimitive(const StyleCtx& ctx, QStyle::PrimitiveElement element, const QStyleOption* option, QPainter* painter,
						   const QWidget* widget)
	{
		if (element == QStyle::PE_IndicatorCheckBox || element == QStyle::PE_IndicatorRadioButton)
		{
			const bool radio = (element == QStyle::PE_IndicatorRadioButton);
			const bool enabled = option->state & QStyle::State_Enabled;
			const bool on = option->state & QStyle::State_On;
			const bool partial = option->state & QStyle::State_NoChange;
			const bool hover = option->state & QStyle::State_MouseOver;
			QColor bg = ctx.roleColor(QStringLiteral("check"), QStringLiteral("bg"), option->palette.color(QPalette::Base));
			QColor border = ctx.roleColor(QStringLiteral("check"), QStringLiteral("border"), option->palette.color(QPalette::Mid));
			if (!enabled)
			{
				if (on || partial)
				{
					bg = ctx.roleColor(QStringLiteral("check"), QStringLiteral("bg.checked.disabled"), bg);
				}
				else
				{
					bg = ctx.roleColor(QStringLiteral("check"), QStringLiteral("bg.disabled"), bg);
				}
				border = ctx.roleColor(QStringLiteral("check"), QStringLiteral("border.disabled"), border);
			}
			else if (on || partial)
			{
				bg = ctx.roleColor(QStringLiteral("check"), hover ? QStringLiteral("bg.checked.hover") : QStringLiteral("bg.checked"), bg);
				border = ctx.roleColor(QStringLiteral("check"), QStringLiteral("border.checked"), border);
			}
			else if (hover)
			{
				bg = ctx.roleColor(QStringLiteral("check"), QStringLiteral("bg.hover"), bg);
			}
			const int radius = radio ? option->rect.width() / 2 : ctx.roleMetric(QStringLiteral("check"), QStringLiteral("radius"), 3);
			drawRounded(painter, option->rect.adjusted(1, 1, -1, -1), radius, bg, border, 1.5);

			if (on || partial)
			{
				painter->save();
				painter->setRenderHint(QPainter::Antialiasing, true);
				const QColor ind =
					ctx.roleColor(QStringLiteral("check"), enabled ? QStringLiteral("indicator") : QStringLiteral("indicator.disabled"),
								  option->palette.color(QPalette::HighlightedText));
				painter->setPen(QPen(ind, 2));
				const QRect r = option->rect.adjusted(4, 4, -4, -4);
				if (radio)
				{
					painter->setBrush(ind);
					painter->setPen(Qt::NoPen);
					painter->drawEllipse(r.adjusted(2, 2, -2, -2));
				}
				else if (partial)
				{
					painter->drawLine(r.left(), r.center().y(), r.right(), r.center().y());
				}
				else
				{
					QPainterPath tick;
					tick.moveTo(r.left(), r.center().y());
					tick.lineTo(r.center().x() - 1, r.bottom() - 1);
					tick.lineTo(r.right(), r.top());
					painter->drawPath(tick);
				}
				painter->restore();
			}
			return true;
		}
		return false;
	}
} // namespace qtheme::style_detail
