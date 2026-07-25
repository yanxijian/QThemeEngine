#include "style/style_families.hpp"
#include "style/style_paint_util.hpp"

#include <QPainter>
#include <QStyleOptionViewItem>
#include <QtMath>

namespace qtheme::style_detail
{
	namespace
	{
		/// When CE_ItemViewItem paints panel first, skip panel pass inside QCommonStyle::drawControl.
		thread_local int g_skipItemViewPanel = 0;
	} // namespace

	void paintItemViewPanel(const StyleCtx& ctx, const QStyleOption* option, QPainter* painter)
	{
		if (!option || !painter)
		{
			return;
		}
		if (g_skipItemViewPanel > 0)
		{
			return;
		}
		const auto* item = qstyleoption_cast<const QStyleOptionViewItem*>(option);
		const bool enabled = option->state & QStyle::State_Enabled;
		const bool selected = option->state & QStyle::State_Selected;
		const bool hover = option->state & QStyle::State_MouseOver;
		const bool active = option->state & QStyle::State_Active;
		const bool alternate = item && (item->features & QStyleOptionViewItem::Alternate);

		QString bgRole = QStringLiteral("bg");
		if (selected && hover && enabled && active)
		{
			bgRole = QStringLiteral("bg.selected.hover");
		}
		else if (selected)
		{
			bgRole = active ? QStringLiteral("bg.selected") : QStringLiteral("bg.selected.inactive");
		}
		else if (hover && enabled)
		{
			bgRole = QStringLiteral("bg.hover");
		}
		else if (alternate)
		{
			bgRole = QStringLiteral("bg.alternate");
		}

		const QColor bg = ctx.roleColor(QStringLiteral("view"), bgRole,
										selected ? option->palette.color(QPalette::Highlight) : option->palette.color(QPalette::Base));
		// Focused: same corner radius family as the focus ring so fill cannot poke past the arc.
		if ((option->state & QStyle::State_HasFocus) && enabled)
		{
			const qreal outerW = qreal(ctx.roleMetric(QStringLiteral("view"), QStringLiteral("focusWidth"), 1));
			const int radius = ctx.roleMetric(QStringLiteral("focus"), QStringLiteral("radius"), 4);
			const QRectF fillRect = QRectF(option->rect).adjusted(outerW, outerW, -outerW, -outerW);
			// Stroke centerline uses `radius`; inner contour ≈ radius - outerW/2.
			const qreal fillRadius = qMax(0.0, qreal(radius) - outerW * 0.5);
			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);
			painter->setPen(Qt::NoPen);
			painter->setBrush(bg);
			painter->drawRoundedRect(fillRect, fillRadius, fillRadius);
			painter->restore();
		}
		else
		{
			painter->fillRect(option->rect, bg);
		}
	}

	bool tryItemViewPrimitive(const StyleCtx& ctx, QStyle::PrimitiveElement element, const QStyleOption* option, QPainter* painter,
							  const QWidget* widget)
	{
		if (element == QStyle::PE_PanelItemViewItem || element == QStyle::PE_PanelItemViewRow)
		{
			paintItemViewPanel(ctx, option, painter);
			return true;
		}
		if (element == QStyle::PE_IndicatorBranch)
		{
			const bool hasChildren = option->state & QStyle::State_Children;
			const bool open = option->state & QStyle::State_Open;
			if (!hasChildren)
			{
				return true;
			}
			const QColor branch =
				ctx.roleColor(QStringLiteral("view"), QStringLiteral("branch"), option->palette.color(QPalette::WindowText));
			drawArrow(painter, option->rect, open ? Qt::DownArrow : Qt::RightArrow, branch);
			return true;
		}
		return false;
	}

	bool tryItemViewControl(const StyleCtx& ctx, QStyle::ControlElement element, const QStyleOption* option, QPainter* painter,
							const QWidget* widget, const QProxyStyle* style)
	{
		if (element == QStyle::CE_ItemViewItem)
		{
			const auto* item = qstyleoption_cast<const QStyleOptionViewItem*>(option);
			if (item && painter)
			{
				const bool enabled = item->state & QStyle::State_Enabled;
				const bool selected = item->state & QStyle::State_Selected;
				const bool active = item->state & QStyle::State_Active;

				QString fgRole = QStringLiteral("fg");
				if (!enabled)
				{
					fgRole = QStringLiteral("fg.disabled");
				}
				else if (selected)
				{
					fgRole = QStringLiteral("fg.selected");
				}
				const QColor fg = ctx.roleColor(QStringLiteral("view"), fgRole, item->palette.color(QPalette::Text));
				const QColor selBg =
					ctx.roleColor(QStringLiteral("view"), active ? QStringLiteral("bg.selected") : QStringLiteral("bg.selected.inactive"),
								  item->palette.color(QPalette::Highlight));
				const QColor base = ctx.roleColor(QStringLiteral("view"), QStringLiteral("bg"), item->palette.color(QPalette::Base));
				const QColor alt =
					ctx.roleColor(QStringLiteral("view"), QStringLiteral("bg.alternate"), item->palette.color(QPalette::AlternateBase));

				QStyleOptionViewItem copy = *item;
				copy.palette.setColor(QPalette::Text, fg);
				copy.palette.setColor(QPalette::WindowText, fg);
				copy.palette.setColor(QPalette::HighlightedText, fg);
				copy.palette.setColor(QPalette::Highlight, selBg);
				copy.palette.setColor(QPalette::Base, base);
				copy.palette.setColor(QPalette::AlternateBase, alt);
				// Full item rect for selection fill; contentPad is applied in subElementRect (text/icon only).
				const bool focused = (item->state & QStyle::State_HasFocus) && enabled;
				// Paint panel once with real Selected/HasFocus (rounded when focused). Skip the
				// panel inside QCommonStyle so it cannot cover blue with a square Base/Highlight fill.
				paintItemViewPanel(ctx, item, painter);
				copy.state &= ~(QStyle::State_HasFocus | QStyle::State_KeyboardFocusChange);
				++g_skipItemViewPanel;
				style->QProxyStyle::drawControl(element, &copy, painter, widget);
				--g_skipItemViewPanel;
				if (focused)
				{
					// Ambient = the fill behind the ring (selected accent / inactive / base).
					const QColor ambient = selected ? selBg : base;
					QColor outer;
					ctx.focusStrokeColors(ambient, &outer, nullptr);
					const int radius = ctx.roleMetric(QStringLiteral("focus"), QStringLiteral("radius"), 4);
					const qreal outerW = qreal(ctx.roleMetric(QStringLiteral("view"), QStringLiteral("focusWidth"), 1));
					drawFluentFocusRing(painter, item->rect, radius, outer, QColor(), outerW, 0.0);
				}
				return true;
			}
		}
		return false;
	}
} // namespace qtheme::style_detail
