#include "style/style_families.hpp"
#include "style/style_paint_util.hpp"

#include <QPainter>
#include <QStyleOption>
#include <QStyleOptionMenuItem>
#include <QWidget>

namespace qtheme::style_detail
{
	bool tryMenuPrimitive(const StyleCtx& ctx, QStyle::PrimitiveElement element, const QStyleOption* option, QPainter* painter,
						  const QWidget* widget)
	{
		if (element == QStyle::PE_PanelMenu || element == QStyle::PE_FrameMenu)
		{
			const int radius = ctx.roleMetric(QStringLiteral("menu"), QStringLiteral("radius"), 4);
			// Rounded menus: Panel draws fill+border after a transparent clear. Frame would
			// either double-draw or (if it clears) wipe the panel — skip it.
			if (element == QStyle::PE_FrameMenu && radius > 0)
			{
				return true;
			}
			const QColor solid = ctx.roleColor(QStringLiteral("menu"), QStringLiteral("bg"), option->palette.color(QPalette::Window));
			const QColor bg = ctx.roleColor(QStringLiteral("menu"), QStringLiteral("bg.acrylic"), solid);
			const QColor border = ctx.roleColor(QStringLiteral("menu"), QStringLiteral("border"), option->palette.color(QPalette::Mid));
			if (radius > 0)
			{
				// Without this, WA_TranslucentBackground leaves uncleared corners black on Windows.
				clearPopupToTransparent(painter, widget ? widget->rect() : option->rect);
			}
			drawRounded(painter, option->rect, radius, bg, border);
			return true;
		}
		return false;
	}

	bool tryMenuControl(const StyleCtx& ctx, QStyle::ControlElement element, const QStyleOption* option, QPainter* painter,
						const QWidget* widget, const QProxyStyle* style)
	{
		if (element == QStyle::CE_MenuItem)
		{
			const auto* mi = qstyleoption_cast<const QStyleOptionMenuItem*>(option);
			if (mi && painter)
			{
				if (mi->menuItemType == QStyleOptionMenuItem::Separator)
				{
					const QColor sep = ctx.roleColor(QStringLiteral("menu"), QStringLiteral("separator"), mi->palette.mid().color());
					const int inset = ctx.roleMetric(QStringLiteral("menu"), QStringLiteral("itemInset"), 2);
					const int y = mi->rect.center().y();
					painter->fillRect(mi->rect.left() + inset, y, mi->rect.width() - 2 * inset, 1, sep);
					return true;
				}

				const bool enabled = mi->state & QStyle::State_Enabled;
				const QColor panelBg = ctx.roleColor(QStringLiteral("menu"), QStringLiteral("bg"), mi->palette.color(QPalette::Window));
				const QColor hoverBg = ctx.roleColor(QStringLiteral("menu"), QStringLiteral("bg.hover"), panelBg);
				const QColor pressedBg = ctx.roleColor(QStringLiteral("menu"), QStringLiteral("bg.pressed"), hoverBg);

				const bool selected = enabled && (mi->state & QStyle::State_Selected);
				const bool pressed = enabled && (mi->state & QStyle::State_Sunken);
				const bool hovered = enabled && (mi->state & QStyle::State_MouseOver);
				if (selected || pressed || hovered)
				{
					const int inset = ctx.roleMetric(QStringLiteral("menu"), QStringLiteral("itemInset"), 2);
					const int itemRadius = ctx.roleMetric(QStringLiteral("menu"), QStringLiteral("itemRadius"), 4);
					const QRect pill = mi->rect.adjusted(inset, inset, -inset, -inset);
					const QColor fill = pressed ? pressedBg : hoverBg;
					drawRounded(painter, pill, itemRadius, fill, fill);
				}

				QString fgRole = QStringLiteral("fg");
				if (!enabled)
				{
					fgRole = QStringLiteral("fg.disabled");
				}
				else if (selected || pressed || hovered)
				{
					fgRole = QStringLiteral("fg.selected");
				}
				const QColor fg = ctx.roleColor(QStringLiteral("menu"), fgRole, mi->palette.color(QPalette::WindowText));

				// Clear selection/hover so Fusion does not paint a square Highlight behind our pill.
				QStyleOptionMenuItem copy = *mi;
				copy.state &= ~(QStyle::State_Selected | QStyle::State_MouseOver | QStyle::State_Sunken);
				copy.palette.setColor(QPalette::Window, panelBg);
				copy.palette.setColor(QPalette::Base, panelBg);
				copy.palette.setColor(QPalette::Button, panelBg);
				copy.palette.setColor(QPalette::Highlight, panelBg);
				copy.palette.setColor(QPalette::HighlightedText, fg);
				copy.palette.setColor(QPalette::WindowText, fg);
				copy.palette.setColor(QPalette::Text, fg);
				copy.palette.setColor(QPalette::ButtonText, fg);
				style->QProxyStyle::drawControl(element, &copy, painter, widget);
				return true;
			}
		}

		if (element == QStyle::CE_MenuBarItem)
		{
			const auto* mi = qstyleoption_cast<const QStyleOptionMenuItem*>(option);
			if (mi && painter)
			{
				const bool enabled = mi->state & QStyle::State_Enabled;
				QString bgRole = QStringLiteral("bar.bg");
				if ((mi->state & QStyle::State_Sunken) && enabled)
				{
					bgRole = QStringLiteral("bar.bg.pressed");
				}
				else if ((mi->state & (QStyle::State_Selected | QStyle::State_MouseOver)) && enabled)
				{
					bgRole = QStringLiteral("bar.bg.hover");
				}
				const QColor bg = ctx.roleColor(QStringLiteral("menu"), bgRole, mi->palette.color(QPalette::Window));
				const QColor fg = ctx.roleColor(QStringLiteral("menu"), enabled ? QStringLiteral("bar.fg") : QStringLiteral("fg.disabled"),
												mi->palette.color(QPalette::WindowText));
				const int itemRadius = ctx.roleMetric(QStringLiteral("menu"), QStringLiteral("itemRadius"), 4);
				if (itemRadius > 0 && bgRole != QStringLiteral("bar.bg"))
				{
					drawRounded(painter, mi->rect.adjusted(2, 2, -2, -2), itemRadius, bg, bg);
				}
				else
				{
					painter->fillRect(mi->rect, bg);
				}
				painter->setPen(fg);
				painter->drawText(mi->rect, Qt::AlignCenter | Qt::TextShowMnemonic, mi->text);
				return true;
			}
		}

		if (element == QStyle::CE_MenuBarEmptyArea)
		{
			const QColor bg = ctx.roleColor(QStringLiteral("menu"), QStringLiteral("bar.bg"), option->palette.color(QPalette::Window));
			painter->fillRect(option->rect, bg);
			return true;
		}
		return false;
	}
} // namespace qtheme::style_detail
