#include "style/style_families.hpp"
#include "style/style_paint_util.hpp"

#include <QPainter>
#include <QStyleOption>

namespace qtheme::style_detail
{
	bool tryEditPrimitive(const StyleCtx& ctx, QStyle::PrimitiveElement element, const QStyleOption* option, QPainter* painter,
						  const QWidget* widget)
	{
		if (element == QStyle::PE_FrameLineEdit || element == QStyle::PE_PanelLineEdit)
		{
			QString borderRole = QStringLiteral("border");
			if (!(option->state & QStyle::State_Enabled))
			{
				borderRole = QStringLiteral("border.disabled");
			}
			else if (option->state & QStyle::State_HasFocus)
			{
				borderRole = QStringLiteral("border.focus");
			}
			else if (option->state & QStyle::State_MouseOver)
			{
				borderRole = QStringLiteral("border.hover");
			}
			const QColor bg = ctx.roleColor(QStringLiteral("edit"),
											(option->state & QStyle::State_Enabled) ? QStringLiteral("bg") : QStringLiteral("bg.disabled"),
											option->palette.color(QPalette::Base));
			const QColor border = ctx.roleColor(QStringLiteral("edit"), borderRole, option->palette.color(QPalette::Mid));
			const int radius = ctx.roleMetric(QStringLiteral("edit"), QStringLiteral("radius"), 4);
			const qreal bw = (option->state & QStyle::State_HasFocus) && (option->state & QStyle::State_Enabled) ? 2.0 : 1.0;
			drawRounded(painter, option->rect, radius, bg, border, bw);
			return true;
		}
		return false;
	}
} // namespace qtheme::style_detail
