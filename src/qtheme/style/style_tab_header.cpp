#include "style/style_families.hpp"
#include "style/style_paint_util.hpp"

#include <QPainter>
#include <QStyleOptionHeader>
#include <QStyleOptionTab>

namespace qtheme::style_detail
{
	bool tryTabHeaderControl(const StyleCtx& ctx, QStyle::ControlElement element, const QStyleOption* option, QPainter* painter,
							 const QWidget* widget, const QProxyStyle* style)
	{
		if (element == QStyle::CE_TabBarTabShape)
		{
			const auto* tab = qstyleoption_cast<const QStyleOptionTab*>(option);
			if (tab && painter)
			{
				const bool enabled = tab->state & QStyle::State_Enabled;
				const bool selected = tab->state & QStyle::State_Selected;
				const bool hover = tab->state & QStyle::State_MouseOver;
				QString bgRole = QStringLiteral("bg");
				if (!enabled)
				{
					bgRole = QStringLiteral("bg.disabled");
				}
				else if (selected)
				{
					bgRole = QStringLiteral("bg.selected");
				}
				else if (hover)
				{
					bgRole = QStringLiteral("bg.hover");
				}
				const QColor bg = ctx.roleColor(QStringLiteral("tab"), bgRole, tab->palette.color(QPalette::Window));
				const QColor border = ctx.roleColor(QStringLiteral("tab"), QStringLiteral("border"), tab->palette.mid().color());
				const int radius = ctx.roleMetric(QStringLiteral("tab"), QStringLiteral("radius"), 4);
				drawRounded(painter, tab->rect.adjusted(1, 1, -1, -1), radius, bg, border);
				if (selected && enabled)
				{
					const QColor ind =
						ctx.roleColor(QStringLiteral("tab"), QStringLiteral("indicator"), tab->palette.color(QPalette::Highlight));
					painter->save();
					painter->setPen(Qt::NoPen);
					painter->setBrush(ind);
					painter->drawRect(tab->rect.left() + 6, tab->rect.bottom() - 2, tab->rect.width() - 12, 2);
					painter->restore();
				}
				return true;
			}
		}

		if (element == QStyle::CE_TabBarTabLabel)
		{
			const auto* tab = qstyleoption_cast<const QStyleOptionTab*>(option);
			if (tab && painter)
			{
				const bool enabled = tab->state & QStyle::State_Enabled;
				const bool selected = tab->state & QStyle::State_Selected;
				QString fgRole = QStringLiteral("fg");
				if (!enabled)
				{
					fgRole = QStringLiteral("fg.disabled");
				}
				else if (selected)
				{
					fgRole = QStringLiteral("fg.selected");
				}
				const QColor fg = ctx.roleColor(QStringLiteral("tab"), fgRole, tab->palette.color(QPalette::WindowText));
				QStyleOptionTab copy = *tab;
				copy.palette.setColor(QPalette::WindowText, fg);
				style->QProxyStyle::drawControl(element, &copy, painter, widget);
				return true;
			}
		}

		if (element == QStyle::CE_HeaderSection)
		{
			const auto* header = qstyleoption_cast<const QStyleOptionHeader*>(option);
			if (header && painter)
			{
				const bool enabled = header->state & QStyle::State_Enabled;
				QString bgRole = QStringLiteral("bg");
				if (enabled)
				{
					if (header->state & QStyle::State_Sunken)
					{
						bgRole = QStringLiteral("bg.pressed");
					}
					else if (header->state & QStyle::State_MouseOver)
					{
						bgRole = QStringLiteral("bg.hover");
					}
				}
				const QColor bg = ctx.roleColor(QStringLiteral("header"), bgRole, header->palette.color(QPalette::Button));
				const QColor border = ctx.roleColor(QStringLiteral("header"), QStringLiteral("border"), header->palette.mid().color());
				painter->fillRect(header->rect, bg);
				painter->setPen(border);
				painter->drawLine(header->rect.topRight(), header->rect.bottomRight());
				painter->drawLine(header->rect.bottomLeft(), header->rect.bottomRight());
				return true;
			}
		}

		if (element == QStyle::CE_HeaderLabel)
		{
			const auto* header = qstyleoption_cast<const QStyleOptionHeader*>(option);
			if (header && painter)
			{
				const bool enabled = header->state & QStyle::State_Enabled;
				const QColor fg = ctx.roleColor(QStringLiteral("header"), enabled ? QStringLiteral("fg") : QStringLiteral("fg.disabled"),
												header->palette.color(QPalette::ButtonText));
				QStyleOptionHeader copy = *header;
				copy.palette.setColor(QPalette::ButtonText, fg);
				copy.palette.setColor(QPalette::WindowText, fg);
				style->QProxyStyle::drawControl(element, &copy, painter, widget);
				return true;
			}
		}
		return false;
	}
} // namespace qtheme::style_detail
