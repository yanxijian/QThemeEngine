#include "style/style_families.hpp"
#include "style/style_paint_util.hpp"

#include <QPainter>
#include <QStyleOptionComboBox>
#include <QStyleOptionComplex>
#include <QStyleOptionSlider>
#include <QStyleOptionSpinBox>
#include <QStyleOptionToolButton>
#include <QtMath>

namespace qtheme::style_detail
{
	bool tryComplexPrimitive(const StyleCtx& ctx, QStyle::PrimitiveElement element, const QStyleOption* option, QPainter* painter,
							 const QWidget* widget)
	{
		if (element == QStyle::PE_IndicatorArrowDown || element == QStyle::PE_IndicatorArrowUp || element == QStyle::PE_IndicatorArrowLeft
			|| element == QStyle::PE_IndicatorArrowRight || element == QStyle::PE_IndicatorSpinUp || element == QStyle::PE_IndicatorSpinDown
			|| element == QStyle::PE_IndicatorSpinPlus || element == QStyle::PE_IndicatorSpinMinus)
		{
			Qt::ArrowType type = Qt::DownArrow;
			if (element == QStyle::PE_IndicatorArrowUp || element == QStyle::PE_IndicatorSpinUp || element == QStyle::PE_IndicatorSpinPlus)
			{
				type = Qt::UpArrow;
			}
			else if (element == QStyle::PE_IndicatorArrowLeft)
			{
				type = Qt::LeftArrow;
			}
			else if (element == QStyle::PE_IndicatorArrowRight)
			{
				type = Qt::RightArrow;
			}
			const bool enabled = option->state & QStyle::State_Enabled;
			const bool fromSpin = element == QStyle::PE_IndicatorSpinUp || element == QStyle::PE_IndicatorSpinDown
								  || element == QStyle::PE_IndicatorSpinPlus || element == QStyle::PE_IndicatorSpinMinus;
			const QString group = fromSpin ? QStringLiteral("spin") : QStringLiteral("combo");
			const QColor color = ctx.roleColor(group, enabled ? QStringLiteral("arrow") : QStringLiteral("arrow.disabled"),
											   option->palette.color(QPalette::WindowText));
			drawArrow(painter, option->rect, type, color);
			return true;
		}
		return false;
	}

	bool tryComplexControlLabel(const StyleCtx& ctx, QStyle::ControlElement element, const QStyleOption* option, QPainter* painter,
								const QWidget* widget, const QProxyStyle* style)
	{
		if (element == QStyle::CE_ComboBoxLabel)
		{
			const auto* combo = qstyleoption_cast<const QStyleOptionComboBox*>(option);
			if (combo && painter)
			{
				const bool enabled = combo->state & QStyle::State_Enabled;
				const QColor fg = ctx.roleColor(QStringLiteral("combo"), enabled ? QStringLiteral("fg") : QStringLiteral("fg.disabled"),
												combo->palette.color(QPalette::ButtonText));
				QStyleOptionComboBox copy = *combo;
				copy.palette.setColor(QPalette::ButtonText, fg);
				copy.palette.setColor(QPalette::Text, fg);
				style->QProxyStyle::drawControl(element, &copy, painter, widget);
				return true;
			}
		}
		return false;
	}

	bool tryComplexControl(const StyleCtx& ctx, QStyle::ComplexControl control, const QStyleOptionComplex* option, QPainter* painter,
						   const QWidget* widget, const QProxyStyle* style)
	{
		if (control == QStyle::CC_ScrollBar)
		{
			const auto* sb = qstyleoption_cast<const QStyleOptionSlider*>(option);
			if (sb && painter)
			{
				const bool enabled = sb->state & QStyle::State_Enabled;
				const QColor groove = ctx.roleColor(QStringLiteral("scroll"), QStringLiteral("groove"), sb->palette.window().color());
				painter->fillRect(sb->rect, groove);

				const QRect handle = style->subControlRect(QStyle::CC_ScrollBar, sb, QStyle::SC_ScrollBarSlider, widget);
				QString handleRole = QStringLiteral("handle");
				if (!enabled)
				{
					handleRole = QStringLiteral("handle.disabled");
				}
				else if (sb->state & QStyle::State_Sunken)
				{
					handleRole = QStringLiteral("handle.pressed");
				}
				else if (sb->state & QStyle::State_MouseOver)
				{
					handleRole = QStringLiteral("handle.hover");
				}
				const QColor hc = ctx.roleColor(QStringLiteral("scroll"), handleRole, sb->palette.mid().color());
				const int radius = ctx.roleMetric(QStringLiteral("scroll"), QStringLiteral("radius"), 4);
				drawRounded(painter, handle.adjusted(1, 1, -1, -1), radius, hc, hc);

				const QColor arrow =
					ctx.roleColor(QStringLiteral("scroll"), enabled ? QStringLiteral("arrow") : QStringLiteral("arrow.disabled"),
								  sb->palette.color(QPalette::WindowText));
				const QRect sub = style->subControlRect(QStyle::CC_ScrollBar, sb, QStyle::SC_ScrollBarSubLine, widget);
				const QRect add = style->subControlRect(QStyle::CC_ScrollBar, sb, QStyle::SC_ScrollBarAddLine, widget);
				const bool horiz = sb->orientation == Qt::Horizontal;
				drawArrow(painter, sub, horiz ? Qt::LeftArrow : Qt::UpArrow, arrow);
				drawArrow(painter, add, horiz ? Qt::RightArrow : Qt::DownArrow, arrow);
				return true;
			}
		}

		if (control == QStyle::CC_Slider)
		{
			const auto* slider = qstyleoption_cast<const QStyleOptionSlider*>(option);
			if (slider && painter)
			{
				const bool enabled = slider->state & QStyle::State_Enabled;
				const QRect grooveRect = style->subControlRect(QStyle::CC_Slider, slider, QStyle::SC_SliderGroove, widget);
				const QRect handleRect = style->subControlRect(QStyle::CC_Slider, slider, QStyle::SC_SliderHandle, widget);
				const int grooveThickness = ctx.roleMetric(QStringLiteral("slider"), QStringLiteral("groove"), 4);
				QRect groove = grooveRect;
				if (slider->orientation == Qt::Horizontal)
				{
					groove.setTop(grooveRect.center().y() - grooveThickness / 2);
					groove.setHeight(grooveThickness);
				}
				else
				{
					groove.setLeft(grooveRect.center().x() - grooveThickness / 2);
					groove.setWidth(grooveThickness);
				}

				const QColor grooveColor =
					ctx.roleColor(QStringLiteral("slider"), enabled ? QStringLiteral("groove") : QStringLiteral("groove.disabled"),
								  slider->palette.mid().color());
				const int radius = ctx.roleMetric(QStringLiteral("slider"), QStringLiteral("radius"), 8);
				drawRounded(painter, groove, qMin(radius, grooveThickness), grooveColor, grooveColor);

				QRect fill = groove;
				if (slider->orientation == Qt::Horizontal)
				{
					if (slider->upsideDown)
					{
						fill.setLeft(handleRect.center().x());
					}
					else
					{
						fill.setRight(handleRect.center().x());
					}
				}
				else
				{
					if (slider->upsideDown)
					{
						fill.setTop(handleRect.center().y());
					}
					else
					{
						fill.setBottom(handleRect.center().y());
					}
				}
				QString fillRole = QStringLiteral("fill");
				if (!enabled)
				{
					fillRole = QStringLiteral("fill.disabled");
				}
				else if (slider->state & QStyle::State_MouseOver)
				{
					fillRole = QStringLiteral("fill.hover");
				}
				const QColor fillColor = ctx.roleColor(QStringLiteral("slider"), fillRole, slider->palette.color(QPalette::Highlight));
				drawRounded(painter, fill, qMin(radius, grooveThickness), fillColor, fillColor);

				QString handleRole = QStringLiteral("handle");
				if (!enabled)
				{
					handleRole = QStringLiteral("handle.disabled");
				}
				else if (slider->state & QStyle::State_Sunken)
				{
					handleRole = QStringLiteral("handle.pressed");
				}
				else if (slider->state & QStyle::State_MouseOver)
				{
					handleRole = QStringLiteral("handle.hover");
				}
				const QColor handleBg = ctx.roleColor(QStringLiteral("slider"), handleRole, slider->palette.button().color());
				const bool focused = (slider->state & QStyle::State_HasFocus) && enabled;
				const QColor handleBorder = ctx.roleColor(QStringLiteral("slider"),
														  focused ? QStringLiteral("handle.border.focus") : QStringLiteral("handle.border"),
														  slider->palette.mid().color());
				drawRounded(painter, handleRect, radius, handleBg, handleBorder, focused ? 2.0 : 1.0);
				return true;
			}
		}

		if (control == QStyle::CC_ComboBox)
		{
			const auto* combo = qstyleoption_cast<const QStyleOptionComboBox*>(option);
			if (combo && painter)
			{
				const bool enabled = combo->state & QStyle::State_Enabled;
				QString bgRole = QStringLiteral("bg");
				QString borderRole = QStringLiteral("border");
				if (!enabled)
				{
					bgRole = QStringLiteral("bg.disabled");
				}
				else if (combo->state & QStyle::State_Sunken)
				{
					bgRole = QStringLiteral("bg.pressed");
				}
				else if (combo->state & QStyle::State_MouseOver)
				{
					bgRole = QStringLiteral("bg.hover");
					borderRole = QStringLiteral("border.hover");
				}
				if (enabled && (combo->state & QStyle::State_HasFocus))
				{
					borderRole = QStringLiteral("border.focus");
				}
				const QColor bg = ctx.roleColor(QStringLiteral("combo"), bgRole, combo->palette.color(QPalette::Button));
				const QColor border = ctx.roleColor(QStringLiteral("combo"), borderRole, combo->palette.mid().color());
				const int radius = ctx.roleMetric(QStringLiteral("combo"), QStringLiteral("radius"), 4);
				const qreal bw = (borderRole == QStringLiteral("border.focus")) ? 2.0 : 1.0;
				drawRounded(painter, combo->rect, radius, bg, border, bw);

				const QRect arrowRect = style->subControlRect(QStyle::CC_ComboBox, combo, QStyle::SC_ComboBoxArrow, widget);
				const QColor arrow =
					ctx.roleColor(QStringLiteral("combo"), enabled ? QStringLiteral("arrow") : QStringLiteral("arrow.disabled"),
								  combo->palette.color(QPalette::WindowText));
				drawArrow(painter, arrowRect, Qt::DownArrow, arrow);
				return true;
			}
		}

		if (control == QStyle::CC_SpinBox)
		{
			const auto* spin = qstyleoption_cast<const QStyleOptionSpinBox*>(option);
			if (spin && painter)
			{
				const bool enabled = spin->state & QStyle::State_Enabled;
				QString borderRole = QStringLiteral("border");
				if (!enabled)
				{
					borderRole = QStringLiteral("border.disabled");
				}
				else if (spin->state & QStyle::State_HasFocus)
				{
					borderRole = QStringLiteral("border.focus");
				}
				else if (spin->state & QStyle::State_MouseOver)
				{
					borderRole = QStringLiteral("border.hover");
				}
				const QColor bg = ctx.roleColor(QStringLiteral("spin"), enabled ? QStringLiteral("bg") : QStringLiteral("bg.disabled"),
												spin->palette.color(QPalette::Base));
				const QColor border = ctx.roleColor(QStringLiteral("spin"), borderRole, spin->palette.mid().color());
				const int radius = ctx.roleMetric(QStringLiteral("spin"), QStringLiteral("radius"), 4);
				const qreal bw = (borderRole == QStringLiteral("border.focus")) ? 2.0 : 1.0;
				drawRounded(painter, spin->rect, radius, bg, border, bw);

				auto paintButton = [&](QStyle::SubControl sc, bool up)
				{
					if (!(spin->subControls & sc))
					{
						return;
					}
					QRect br = style->subControlRect(QStyle::CC_SpinBox, spin, sc, widget);
					QString btnRole = QStringLiteral("button");
					const bool active = spin->activeSubControls & sc;
					if (!enabled)
					{
						btnRole = QStringLiteral("button.disabled");
					}
					else if (active && (spin->state & QStyle::State_Sunken))
					{
						btnRole = QStringLiteral("button.pressed");
					}
					else if (active || (spin->state & QStyle::State_MouseOver))
					{
						btnRole = QStringLiteral("button.hover");
					}
					const QColor btnBg = ctx.roleColor(QStringLiteral("spin"), btnRole, spin->palette.button().color());
					painter->fillRect(br.adjusted(1, 1, -1, -1), btnBg);
					const QColor arrow =
						ctx.roleColor(QStringLiteral("spin"), enabled ? QStringLiteral("arrow") : QStringLiteral("arrow.disabled"),
									  spin->palette.color(QPalette::WindowText));
					drawArrow(painter, br, up ? Qt::UpArrow : Qt::DownArrow, arrow);
				};
				paintButton(QStyle::SC_SpinBoxUp, true);
				paintButton(QStyle::SC_SpinBoxDown, false);
				return true;
			}
		}

		if (control == QStyle::CC_ToolButton)
		{
			const auto* tb = qstyleoption_cast<const QStyleOptionToolButton*>(option);
			if (tb && painter)
			{
				QStyleOptionToolButton copy = *tb;
				if (tb->subControls & QStyle::SC_ToolButton)
				{
					(void)tryButtonPrimitive(ctx, QStyle::PE_PanelButtonTool, &copy, painter, widget);
				}
				if (tb->subControls & QStyle::SC_ToolButtonMenu)
				{
					const QRect menuRect = style->subControlRect(QStyle::CC_ToolButton, tb, QStyle::SC_ToolButtonMenu, widget);
					QStyleOptionToolButton menuOpt = *tb;
					menuOpt.rect = menuRect;
					const QColor arrow =
						ctx.roleColor(QStringLiteral("combo"),
									  (tb->state & QStyle::State_Enabled) ? QStringLiteral("arrow") : QStringLiteral("arrow.disabled"),
									  tb->palette.color(QPalette::WindowText));
					drawArrow(painter, menuRect, Qt::DownArrow, arrow);
				}
				style->QProxyStyle::drawControl(QStyle::CE_ToolButtonLabel, &copy, painter, widget);
				return true;
			}
		}

		if (control == QStyle::CC_Dial)
		{
			const auto* dial = qstyleoption_cast<const QStyleOptionSlider*>(option);
			if (dial && painter)
			{
				const bool enabled = dial->state & QStyle::State_Enabled;
				const bool focused = (dial->state & QStyle::State_HasFocus) && enabled;
				const QRect r = dial->rect.adjusted(4, 4, -4, -4);
				const int thickness = ctx.roleMetric(QStringLiteral("dial"), QStringLiteral("thickness"), 8);
				const QColor groove =
					ctx.roleColor(QStringLiteral("dial"), enabled ? QStringLiteral("groove") : QStringLiteral("groove.disabled"),
								  dial->palette.mid().color());
				const QColor fill =
					ctx.roleColor(QStringLiteral("dial"), enabled ? QStringLiteral("fill") : QStringLiteral("fill.disabled"),
								  dial->palette.color(QPalette::Highlight));
				const QColor handleBg = ctx.roleColor(QStringLiteral("dial"), QStringLiteral("handle"), dial->palette.button().color());
				const QColor handleBorder =
					ctx.roleColor(QStringLiteral("dial"), focused ? QStringLiteral("handle.border.focus") : QStringLiteral("handle.border"),
								  dial->palette.mid().color());

				painter->save();
				painter->setRenderHint(QPainter::Antialiasing, true);
				const QPen groovePen(groove, thickness, Qt::SolidLine, Qt::RoundCap);
				painter->setPen(groovePen);
				painter->setBrush(Qt::NoBrush);
				painter->drawEllipse(r);

				const int span = dial->maximum - dial->minimum;
				const qreal ratio = span > 0 ? qreal(dial->sliderPosition - dial->minimum) / qreal(span) : 0.0;
				const int startAngle = 225 * 16;
				const int arcSpan = -int(270.0 * 16.0 * ratio);
				painter->setPen(QPen(fill, thickness, Qt::SolidLine, Qt::RoundCap));
				painter->drawArc(r, startAngle, arcSpan);

				const qreal angleRad = qDegreesToRadians(225.0 - 270.0 * ratio);
				const QPointF center = r.center();
				const qreal radius = qMin(r.width(), r.height()) / 2.0 - thickness / 2.0;
				const QPointF tip(center.x() + radius * qCos(angleRad), center.y() - radius * qSin(angleRad));
				const int hs = qMax(6, thickness + 2);
				painter->setPen(QPen(handleBorder, focused ? 2.0 : 1.0));
				painter->setBrush(handleBg);
				painter->drawEllipse(tip, hs / 2.0, hs / 2.0);
				painter->restore();
				return true;
			}
		}
		return false;
	}
} // namespace qtheme::style_detail
