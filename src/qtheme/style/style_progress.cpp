#include "style/style_families.hpp"
#include "style/style_paint_util.hpp"

#include <QPainter>
#include <QStyleOptionProgressBar>
#include <QVariant>

namespace qtheme::style_detail
{
	bool tryProgressControl(const StyleCtx& ctx, QStyle::ControlElement element, const QStyleOption* option, QPainter* painter,
							const QWidget* widget, const QProxyStyle* style)
	{
		if (element == QStyle::CE_ProgressBarGroove)
		{
			const bool enabled = option->state & QStyle::State_Enabled;
			const QColor groove =
				ctx.roleColor(QStringLiteral("progress"), enabled ? QStringLiteral("groove") : QStringLiteral("groove.disabled"),
							  option->palette.mid().color());
			const QColor border = ctx.roleColor(QStringLiteral("progress"), QStringLiteral("border"), option->palette.mid().color());
			const int radius = ctx.roleMetric(QStringLiteral("progress"), QStringLiteral("radius"), 3);
			drawRounded(painter, option->rect, radius, groove, border);
			return true;
		}

		if (element == QStyle::CE_ProgressBarContents)
		{
			const auto* prog = qstyleoption_cast<const QStyleOptionProgressBar*>(option);
			if (prog && painter)
			{
				if (prog->minimum == prog->maximum || prog->progress < prog->minimum)
				{
					return true;
				}
				const bool enabled = prog->state & QStyle::State_Enabled;
				QRect full = prog->rect;
				const int range = qMax(1, prog->maximum - prog->minimum);
				const int value = qBound(0, prog->progress - prog->minimum, range);
				const bool horizontal = prog->state & QStyle::State_Horizontal;
				QRect chunk = full;
				if (horizontal)
				{
					chunk.setWidth(qRound(full.width() * (qreal(value) / range)));
				}
				else
				{
					const int h = qRound(full.height() * (qreal(value) / range));
					chunk.setTop(full.bottom() - h + 1);
					chunk.setHeight(h);
				}
				if (prog->invertedAppearance)
				{
					if (horizontal)
					{
						chunk.moveRight(full.right());
					}
					else
					{
						chunk.moveTop(full.top());
					}
				}
				QString chunkRole = enabled ? QStringLiteral("chunk") : QStringLiteral("chunk.disabled");
				if (enabled && widget)
				{
					const QString st = widget->property("qtheme.progressState").toString();
					if (st == QLatin1String("paused"))
					{
						chunkRole = QStringLiteral("chunk.paused");
					}
					else if (st == QLatin1String("error"))
					{
						chunkRole = QStringLiteral("chunk.error");
					}
				}
				const QColor fill = ctx.roleColor(QStringLiteral("progress"), chunkRole, prog->palette.color(QPalette::Highlight));
				const int radius = ctx.roleMetric(QStringLiteral("progress"), QStringLiteral("radius"), 3);
				if (chunk.isValid() && chunk.width() > 0 && chunk.height() > 0)
				{
					drawRounded(painter, chunk, radius, fill, fill);
				}
				return true;
			}
		}

		if (element == QStyle::CE_ProgressBarLabel)
		{
			const auto* prog = qstyleoption_cast<const QStyleOptionProgressBar*>(option);
			if (prog && painter)
			{
				const bool enabled = prog->state & QStyle::State_Enabled;
				const QColor fg = ctx.roleColor(QStringLiteral("progress"), enabled ? QStringLiteral("fg") : QStringLiteral("fg.disabled"),
												prog->palette.color(QPalette::WindowText));
				QStyleOptionProgressBar copy = *prog;
				copy.palette.setColor(QPalette::WindowText, fg);
				copy.palette.setColor(QPalette::Text, fg);
				style->QProxyStyle::drawControl(element, &copy, painter, widget);
				return true;
			}
		}
		return false;
	}
} // namespace qtheme::style_detail
