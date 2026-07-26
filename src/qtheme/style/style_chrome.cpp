#include "style/style_families.hpp"
#include "style/style_paint_util.hpp"

#include <QAbstractScrollArea>
#include <QCalendarWidget>
#include <QFrame>
#include <QPainter>
#include <QStyleOptionDockWidget>
#include <QStyleOptionFrame>

namespace qtheme::style_detail
{
	bool tryChromePrimitive(const StyleCtx& ctx, QStyle::PrimitiveElement element, const QStyleOption* option, QPainter* painter,
							const QWidget* widget)
	{
		if (element == QStyle::PE_FrameGroupBox)
		{
			const bool enabled = option->state & QStyle::State_Enabled;
			const QColor bg = ctx.roleColor(QStringLiteral("groupbox"), QStringLiteral("bg"), option->palette.color(QPalette::Window));
			const QColor border = ctx.roleColor(QStringLiteral("groupbox"), QStringLiteral("border"), option->palette.mid().color());
			const int radius = ctx.roleMetric(QStringLiteral("groupbox"), QStringLiteral("radius"), 4);
			Q_UNUSED(enabled);
			drawRounded(painter, option->rect, radius, bg, border);
			return true;
		}

		if (element == QStyle::PE_PanelTipLabel)
		{
			const QColor bg = ctx.roleColor(QStringLiteral("tooltip"), QStringLiteral("bg"), option->palette.color(QPalette::ToolTipBase));
			const QColor border = ctx.roleColor(QStringLiteral("tooltip"), QStringLiteral("border"), option->palette.mid().color());
			const int radius = ctx.roleMetric(QStringLiteral("tooltip"), QStringLiteral("radius"), 4);
			if (radius > 0)
			{
				clearPopupToTransparent(painter, widget ? widget->rect() : option->rect);
			}
			drawRounded(painter, option->rect, radius, bg, border);
			return true;
		}

		if (element == QStyle::PE_IndicatorToolBarSeparator)
		{
			const QColor sep = ctx.roleColor(QStringLiteral("toolbar"), QStringLiteral("separator"), option->palette.mid().color());
			painter->save();
			painter->setPen(sep);
			const QRect r = option->rect;
			if (option->state & QStyle::State_Horizontal)
			{
				const int x = r.center().x();
				painter->drawLine(x, r.top() + 4, x, r.bottom() - 4);
			}
			else
			{
				const int y = r.center().y();
				painter->drawLine(r.left() + 4, y, r.right() - 4, y);
			}
			painter->restore();
			return true;
		}

		if (element == QStyle::PE_IndicatorToolBarHandle)
		{
			const QColor handle = ctx.roleColor(QStringLiteral("toolbar"), QStringLiteral("handle"), option->palette.mid().color());
			painter->save();
			painter->setPen(handle);
			painter->setBrush(handle);
			const QRect r = option->rect.adjusted(2, 2, -2, -2);
			const bool horiz = option->state & QStyle::State_Horizontal;
			for (int i = 0; i < 3; ++i)
			{
				if (horiz)
				{
					const int x = r.left() + i * 3;
					painter->drawRect(x, r.top() + 2, 1, r.height() - 4);
				}
				else
				{
					const int y = r.top() + i * 3;
					painter->drawRect(r.left() + 2, y, r.width() - 4, 1);
				}
			}
			painter->restore();
			return true;
		}

		if (element == QStyle::PE_Frame || element == QStyle::PE_FrameWindow)
		{
			const bool textEdit = isTextEditLike(widget);
			const auto* scroll = qobject_cast<const QAbstractScrollArea*>(widget);
			if (textEdit || (scroll && scroll->frameWidth() > 0 && !widget->inherits("QAbstractItemView")))
			{
				const QString group = QStringLiteral("textedit");
				const bool enabled = option->state & QStyle::State_Enabled;
				const QString borderRole = stateBorderRole(option);
				const QColor bg = ctx.roleColor(group, enabled ? QStringLiteral("bg") : QStringLiteral("bg.disabled"),
												option->palette.color(QPalette::Base));
				const QColor border = ctx.roleColor(group, borderRole, option->palette.mid().color());
				const int radius = ctx.roleMetric(group, QStringLiteral("radius"), 4);
				const qreal bw = (borderRole == QStringLiteral("border.focus") && enabled) ? 2.0 : 1.0;
				drawRounded(painter, option->rect, radius, bg, border, bw);
				return true;
			}

			if (qobject_cast<const QCalendarWidget*>(widget)
				|| (widget && widget->parentWidget() && qobject_cast<const QCalendarWidget*>(widget->parentWidget())))
			{
				const QColor bg = ctx.roleColor(QStringLiteral("calendar"), QStringLiteral("bg"), option->palette.color(QPalette::Base));
				const QColor border = ctx.roleColor(QStringLiteral("calendar"), QStringLiteral("border"), option->palette.mid().color());
				painter->fillRect(option->rect, bg);
				painter->setPen(border);
				painter->drawRect(option->rect.adjusted(0, 0, -1, -1));
				return true;
			}

			const auto* frameOpt = qstyleoption_cast<const QStyleOptionFrame*>(option);
			const QFrame* frameWidget = qobject_cast<const QFrame*>(widget);
			const QFrame::Shape shape = frameOpt ? frameOpt->frameShape : (frameWidget ? frameWidget->frameShape() : QFrame::NoFrame);
			if (shape == QFrame::HLine || shape == QFrame::VLine)
			{
				const QColor line = ctx.roleColor(QStringLiteral("frame"), QStringLiteral("line"), option->palette.mid().color());
				painter->save();
				painter->setPen(QPen(line, ctx.roleMetric(QStringLiteral("frame"), QStringLiteral("lineWidth"), 1)));
				if (shape == QFrame::HLine)
				{
					const int y = option->rect.center().y();
					painter->drawLine(option->rect.left(), y, option->rect.right(), y);
				}
				else
				{
					const int x = option->rect.center().x();
					painter->drawLine(x, option->rect.top(), x, option->rect.bottom());
				}
				painter->restore();
				return true;
			}

			if (shape != QFrame::NoFrame)
			{
				const QColor bg = ctx.roleColor(QStringLiteral("frame"), QStringLiteral("bg"), option->palette.color(QPalette::Window));
				const QColor border = ctx.roleColor(QStringLiteral("frame"), QStringLiteral("border"), option->palette.mid().color());
				const int radius = ctx.roleMetric(QStringLiteral("frame"), QStringLiteral("radius"), 4);
				drawRounded(painter, option->rect, radius, bg, border);
				return true;
			}
		}

		if (element == QStyle::PE_FrameDockWidget)
		{
			const QColor bg = ctx.roleColor(QStringLiteral("dock"), QStringLiteral("bg"), option->palette.color(QPalette::Window));
			const QColor border = ctx.roleColor(QStringLiteral("dock"), QStringLiteral("border"), option->palette.mid().color());
			painter->fillRect(option->rect, bg);
			painter->setPen(border);
			painter->drawRect(option->rect.adjusted(0, 0, -1, -1));
			return true;
		}

		if (element == QStyle::PE_FrameStatusBarItem || element == QStyle::PE_PanelStatusBar)
		{
			const QColor bg = ctx.roleColor(QStringLiteral("status"), QStringLiteral("bg"), option->palette.color(QPalette::Window));
			painter->fillRect(option->rect, bg);
			if (element == QStyle::PE_PanelStatusBar)
			{
				const QColor border = ctx.roleColor(QStringLiteral("status"), QStringLiteral("border"), option->palette.mid().color());
				painter->setPen(border);
				painter->drawLine(option->rect.topLeft(), option->rect.topRight());
			}
			return true;
		}

		if (element == QStyle::PE_IndicatorDockWidgetResizeHandle)
		{
			const QColor handle = ctx.roleColor(QStringLiteral("dock"), QStringLiteral("border"), option->palette.mid().color());
			painter->fillRect(option->rect, handle);
			return true;
		}
		return false;
	}

	bool tryChromeControl(const StyleCtx& ctx, QStyle::ControlElement element, const QStyleOption* option, QPainter* painter,
						  const QWidget* widget, const QProxyStyle* style)
	{
		if (element == QStyle::CE_ToolBar)
		{
			const QColor bg = ctx.roleColor(QStringLiteral("toolbar"), QStringLiteral("bg"), option->palette.color(QPalette::Window));
			const QColor border = ctx.roleColor(QStringLiteral("toolbar"), QStringLiteral("border"), option->palette.mid().color());
			painter->fillRect(option->rect, bg);
			painter->setPen(border);
			painter->drawLine(option->rect.bottomLeft(), option->rect.bottomRight());
			return true;
		}

		if (element == QStyle::CE_Splitter)
		{
			const bool hover = option->state & QStyle::State_MouseOver;
			const QColor bg = ctx.roleColor(QStringLiteral("splitter"), QStringLiteral("bg"), option->palette.window().color());
			const QColor handle =
				ctx.roleColor(QStringLiteral("splitter"), hover ? QStringLiteral("handle.hover") : QStringLiteral("handle"),
							  option->palette.mid().color());
			painter->fillRect(option->rect, bg);
			painter->save();
			painter->setPen(Qt::NoPen);
			painter->setBrush(handle);
			const QRect r = option->rect;
			if (option->state & QStyle::State_Horizontal)
			{
				const int x = r.center().x();
				painter->drawRect(x - 1, r.center().y() - 12, 2, 24);
			}
			else
			{
				const int y = r.center().y();
				painter->drawRect(r.center().x() - 12, y - 1, 24, 2);
			}
			painter->restore();
			return true;
		}

		if (element == QStyle::CE_DockWidgetTitle)
		{
			const auto* dock = qstyleoption_cast<const QStyleOptionDockWidget*>(option);
			if (dock && painter)
			{
				const bool active = dock->state & QStyle::State_Active;
				const bool enabled = dock->state & QStyle::State_Enabled;
				const QColor bg =
					ctx.roleColor(QStringLiteral("dock"), active ? QStringLiteral("title.bg.active") : QStringLiteral("title.bg"),
								  dock->palette.color(QPalette::Window));
				const QColor fg =
					ctx.roleColor(QStringLiteral("dock"), enabled ? QStringLiteral("title.fg") : QStringLiteral("title.fg.disabled"),
								  dock->palette.color(QPalette::WindowText));
				painter->fillRect(dock->rect, bg);
				painter->setPen(fg);
				QRect textRect = dock->rect.adjusted(8, 0, -8, 0);
				painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, dock->title);
				return true;
			}
		}
		return false;
	}
} // namespace qtheme::style_detail
