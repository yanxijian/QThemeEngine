#include "style/style_families.hpp"
#include "style/style_paint_util.hpp"

#include <QPainter>
#include <QStyleOptionButton>

namespace qtheme::style_detail
{
	bool tryButtonPrimitive(const StyleCtx& ctx, QStyle::PrimitiveElement element, const QStyleOption* option, QPainter* painter,
							const QWidget* widget)
	{
		Q_UNUSED(widget);
		if (element != QStyle::PE_PanelButtonTool)
		{
			return false;
		}
		const bool enabled = option->state & QStyle::State_Enabled;
		const bool on = option->state & QStyle::State_On;
		const bool sunken = option->state & QStyle::State_Sunken;
		const bool hover = option->state & QStyle::State_MouseOver;
		QString bgRole = QStringLiteral("bg");
		if (!enabled)
		{
			bgRole = QStringLiteral("bg.disabled");
		}
		else if (sunken)
		{
			bgRole = QStringLiteral("bg.pressed");
		}
		else if (on)
		{
			bgRole = hover ? QStringLiteral("bg.checked.hover") : QStringLiteral("bg.checked");
		}
		else if (hover)
		{
			bgRole = QStringLiteral("bg.hover");
		}
		const QColor bg = ctx.roleColor(QStringLiteral("button"), bgRole, option->palette.color(QPalette::Button));
		const QColor border = ctx.roleColor(QStringLiteral("button"), QStringLiteral("border"), option->palette.mid().color());
		const int radius = ctx.roleMetric(QStringLiteral("button"), QStringLiteral("radius"), 4);
		if (bgRole != QStringLiteral("bg") || (option->state & QStyle::State_Raised))
		{
			drawRounded(painter, option->rect, radius, bg, border);
		}
		return true;
	}

	bool tryButtonControl(const StyleCtx& ctx, QStyle::ControlElement element, const QStyleOption* option, QPainter* painter,
						  const QWidget* widget, const QProxyStyle* style)
	{
		if (element == QStyle::CE_PushButtonBevel)
		{
			const auto* btn = qstyleoption_cast<const QStyleOptionButton*>(option);
			if (!btn || !painter)
			{
				return false;
			}
			const bool enabled = btn->state & QStyle::State_Enabled;
			const bool isDefault = (btn->features & QStyleOptionButton::DefaultButton) && enabled;
			QString bgRole = QStringLiteral("bg");
			if (!enabled)
			{
				bgRole = QStringLiteral("bg.disabled");
			}
			else if (isDefault)
			{
				if (btn->state & QStyle::State_Sunken)
				{
					bgRole = QStringLiteral("bg.accent.pressed");
				}
				else if (btn->state & QStyle::State_MouseOver)
				{
					bgRole = QStringLiteral("bg.accent.hover");
				}
				else
				{
					bgRole = QStringLiteral("bg.accent");
				}
			}
			else if (btn->state & QStyle::State_Sunken)
			{
				bgRole = QStringLiteral("bg.pressed");
			}
			else if (btn->state & QStyle::State_MouseOver)
			{
				bgRole = QStringLiteral("bg.hover");
			}

			const QColor bg = ctx.roleColor(QStringLiteral("button"), bgRole, btn->palette.color(QPalette::Button));
			// Fluent: Default/Accent chrome uses accent stroke; keyboard focus uses PE_FrameFocusRect
			// (dual ring), not a thickened accent border on ordinary buttons.
			const QColor border =
				ctx.roleColor(QStringLiteral("button"), isDefault ? QStringLiteral("border.focus") : QStringLiteral("border"),
							  btn->palette.color(QPalette::Mid));
			const int radius = ctx.roleMetric(QStringLiteral("button"), QStringLiteral("radius"), 4);
			drawRounded(painter, btn->rect, radius, bg, border, isDefault ? 2.0 : 1.0);
			return true;
		}

		if (element == QStyle::CE_PushButtonLabel)
		{
			const auto* btn = qstyleoption_cast<const QStyleOptionButton*>(option);
			if (!btn || !painter)
			{
				return false;
			}
			const bool enabled = btn->state & QStyle::State_Enabled;
			QStyleOptionButton copy = *btn;
			if (btn->features & QStyleOptionButton::CommandLinkButton)
			{
				const QColor title =
					ctx.roleColor(QStringLiteral("commandlink"), enabled ? QStringLiteral("fg") : QStringLiteral("fg.disabled"),
								  btn->palette.color(QPalette::ButtonText));
				const QColor descFallback =
					ctx.roleColor(QStringLiteral("palette"), QStringLiteral("text.tertiary"), btn->palette.color(QPalette::WindowText));
				const QColor desc = ctx.roleColor(QStringLiteral("commandlink"),
												  enabled ? QStringLiteral("description") : QStringLiteral("fg.disabled"), descFallback);
				copy.palette.setColor(QPalette::ButtonText, title);
				copy.palette.setColor(QPalette::WindowText, desc);
				copy.palette.setColor(QPalette::Text, desc);
			}
			else
			{
				const bool isDefault = (btn->features & QStyleOptionButton::DefaultButton) && enabled;
				QString fgRole = QStringLiteral("fg");
				if (!enabled)
				{
					fgRole = QStringLiteral("fg.disabled");
				}
				else if (isDefault)
				{
					if (btn->state & QStyle::State_Sunken)
					{
						fgRole = QStringLiteral("fg.accent.pressed");
					}
					else
					{
						fgRole = QStringLiteral("fg.accent");
					}
				}
				const QColor fg = ctx.roleColor(QStringLiteral("button"), fgRole, btn->palette.color(QPalette::ButtonText));
				copy.palette.setColor(QPalette::ButtonText, fg);
				copy.palette.setColor(QPalette::WindowText, fg);
			}
			style->QProxyStyle::drawControl(element, &copy, painter, widget);
			return true;
		}
		return false;
	}
} // namespace qtheme::style_detail
