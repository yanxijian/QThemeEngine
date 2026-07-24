#include "qtheme/style.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QStyleFactory>
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QStyleOptionComboBox>
#include <QStyleOptionComplex>
#include <QStyleOptionHeader>
#include <QStyleOptionMenuItem>
#include <QStyleOptionSlider>
#include <QStyleOptionTab>
#include <QStyleOptionToolButton>

namespace qtheme {

namespace {

QStyle* makeFusionBase()
{
	QStyle* fusion = QStyleFactory::create(QStringLiteral("Fusion"));
	return fusion ? fusion : QStyleFactory::create(QStringLiteral("windows"));
}

void drawRounded(QPainter* painter, const QRect& rect, int radius, const QColor& fill,
				 const QColor& border, qreal borderWidth = 1.0)
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);
	const QRectF r = QRectF(rect).adjusted(0.5, 0.5, -0.5, -0.5);
	QPainterPath path;
	path.addRoundedRect(r, radius, radius);
	painter->fillPath(path, fill);
	painter->setPen(QPen(border, borderWidth));
	painter->drawPath(path);
	painter->restore();
}

void drawArrow(QPainter* painter, const QRect& rect, Qt::ArrowType type, const QColor& color)
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->setPen(Qt::NoPen);
	painter->setBrush(color);
	const int s = qMin(rect.width(), rect.height()) / 3;
	const QPoint c = rect.center();
	QPolygon poly;
	switch (type)
	{
	case Qt::UpArrow:
		poly << QPoint(c.x(), c.y() - s) << QPoint(c.x() - s, c.y() + s / 2)
			 << QPoint(c.x() + s, c.y() + s / 2);
		break;
	case Qt::DownArrow:
		poly << QPoint(c.x(), c.y() + s) << QPoint(c.x() - s, c.y() - s / 2)
			 << QPoint(c.x() + s, c.y() - s / 2);
		break;
	case Qt::LeftArrow:
		poly << QPoint(c.x() - s, c.y()) << QPoint(c.x() + s / 2, c.y() - s)
			 << QPoint(c.x() + s / 2, c.y() + s);
		break;
	case Qt::RightArrow:
	default:
		poly << QPoint(c.x() + s, c.y()) << QPoint(c.x() - s / 2, c.y() - s)
			 << QPoint(c.x() - s / 2, c.y() + s);
		break;
	}
	painter->drawPolygon(poly);
	painter->restore();
}

} // namespace

QThemeStyle::QThemeStyle(std::shared_ptr<ThemeStore> store, QStyle* base)
	: QProxyStyle(base ? base : makeFusionBase())
	, store_(std::move(store))
{
	setObjectName(QStringLiteral("QThemeStyle"));
}

void QThemeStyle::setStore(std::shared_ptr<ThemeStore> store)
{
	store_ = std::move(store);
}

QColor QThemeStyle::roleColor(const QString& group, const QString& role, const QColor& fallback) const
{
	if (!store_)
	{
		return fallback;
	}
	const ColorValue cv = store_->color(group, role, fallback);
	return cv.ok ? cv.value : fallback;
}

int QThemeStyle::roleMetric(const QString& group, const QString& role, int fallback) const
{
	if (!store_)
	{
		return fallback;
	}
	return store_->metric(group, role, fallback);
}

QPalette QThemeStyle::standardPalette() const
{
	QPalette pal = QProxyStyle::standardPalette();
	if (!store_)
	{
		return pal;
	}

	auto set = [&](QPalette::ColorRole role, const QString& name)
	{
		const ColorValue cv = store_->color(QStringLiteral("palette"), name);
		if (cv.ok)
		{
			pal.setColor(QPalette::Active, role, cv.value);
			pal.setColor(QPalette::Inactive, role, cv.value);
			pal.setColor(QPalette::Disabled, role, cv.value);
		}
	};

	set(QPalette::Window, QStringLiteral("window"));
	set(QPalette::WindowText, QStringLiteral("windowText"));
	set(QPalette::Base, QStringLiteral("base"));
	set(QPalette::Text, QStringLiteral("text"));
	set(QPalette::Button, QStringLiteral("button"));
	set(QPalette::ButtonText, QStringLiteral("buttonText"));
	set(QPalette::Highlight, QStringLiteral("highlight"));
	set(QPalette::HighlightedText, QStringLiteral("highlightedText"));
	set(QPalette::Mid, QStringLiteral("mid"));
	set(QPalette::Light, QStringLiteral("light"));
	set(QPalette::Dark, QStringLiteral("dark"));

	const ColorValue disabledText = store_->color(QStringLiteral("button"), QStringLiteral("fg.disabled"));
	if (disabledText.ok)
	{
		pal.setColor(QPalette::Disabled, QPalette::ButtonText, disabledText.value);
		pal.setColor(QPalette::Disabled, QPalette::WindowText, disabledText.value);
		pal.setColor(QPalette::Disabled, QPalette::Text, disabledText.value);
	}

	const ColorValue editFg = store_->color(QStringLiteral("edit"), QStringLiteral("fg"));
	if (editFg.ok)
	{
		pal.setColor(QPalette::Active, QPalette::Text, editFg.value);
		pal.setColor(QPalette::Inactive, QPalette::Text, editFg.value);
	}
	const ColorValue editFgDis = store_->color(QStringLiteral("edit"), QStringLiteral("fg.disabled"));
	if (editFgDis.ok)
	{
		pal.setColor(QPalette::Disabled, QPalette::Text, editFgDis.value);
	}
	return pal;
}

int QThemeStyle::pixelMetric(PixelMetric metric, const QStyleOption* option, const QWidget* widget) const
{
	switch (metric)
	{
	case PM_ButtonMargin:
		return roleMetric(QStringLiteral("button"), QStringLiteral("padding"),
						  QProxyStyle::pixelMetric(metric, option, widget));
	case PM_IndicatorWidth:
	case PM_IndicatorHeight:
	case PM_ExclusiveIndicatorWidth:
	case PM_ExclusiveIndicatorHeight:
		return roleMetric(QStringLiteral("check"), QStringLiteral("indicator"),
						  QProxyStyle::pixelMetric(metric, option, widget));
	case PM_ScrollBarExtent:
		return roleMetric(QStringLiteral("scroll"), QStringLiteral("thickness"),
						  QProxyStyle::pixelMetric(metric, option, widget));
	case PM_TabBarTabHSpace:
		return 16;
	case PM_DefaultFrameWidth:
		return 1;
	case PM_MenuBarHMargin:
	case PM_MenuBarVMargin:
		return 4;
	case PM_MenuPanelWidth:
		return 1;
	case PM_HeaderMargin:
		return 4;
	case PM_ToolBarHandleExtent:
		return 8;
	case PM_ToolBarItemSpacing:
		return 4;
	case PM_ToolBarItemMargin:
		return roleMetric(QStringLiteral("toolbar"), QStringLiteral("padding"), 4);
	default:
		return QProxyStyle::pixelMetric(metric, option, widget);
	}
}

void QThemeStyle::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter,
								const QWidget* widget) const
{
	if (!painter || !option)
	{
		QProxyStyle::drawPrimitive(element, option, painter, widget);
		return;
	}

	if (element == PE_FrameLineEdit || element == PE_PanelLineEdit)
	{
		QString borderRole = QStringLiteral("border");
		if (!(option->state & State_Enabled))
		{
			borderRole = QStringLiteral("border.disabled");
		}
		else if (option->state & State_HasFocus)
		{
			borderRole = QStringLiteral("border.focus");
		}
		else if (option->state & State_MouseOver)
		{
			borderRole = QStringLiteral("border.hover");
		}
		const QColor bg = roleColor(QStringLiteral("edit"),
									(option->state & State_Enabled) ? QStringLiteral("bg")
																	: QStringLiteral("bg.disabled"),
									option->palette.color(QPalette::Base));
		const QColor border = roleColor(QStringLiteral("edit"), borderRole,
										option->palette.color(QPalette::Mid));
		const int radius = roleMetric(QStringLiteral("edit"), QStringLiteral("radius"), 4);
		const qreal bw = (option->state & State_HasFocus) && (option->state & State_Enabled) ? 2.0 : 1.0;
		drawRounded(painter, option->rect, radius, bg, border, bw);
		return;
	}

	if (element == PE_IndicatorCheckBox || element == PE_IndicatorRadioButton)
	{
		const bool radio = (element == PE_IndicatorRadioButton);
		const bool enabled = option->state & State_Enabled;
		const bool on = option->state & State_On;
		const bool partial = option->state & State_NoChange;
		const bool hover = option->state & State_MouseOver;
		QColor bg = roleColor(QStringLiteral("check"), QStringLiteral("bg"),
							  option->palette.color(QPalette::Base));
		QColor border = roleColor(QStringLiteral("check"), QStringLiteral("border"),
								  option->palette.color(QPalette::Mid));
		if (!enabled)
		{
			if (on || partial)
			{
				bg = roleColor(QStringLiteral("check"), QStringLiteral("bg.checked.disabled"), bg);
			}
			else
			{
				bg = roleColor(QStringLiteral("check"), QStringLiteral("bg.disabled"), bg);
			}
			border = roleColor(QStringLiteral("check"), QStringLiteral("border.disabled"), border);
		}
		else if (on || partial)
		{
			bg = roleColor(QStringLiteral("check"), QStringLiteral("bg.checked"), bg);
			border = roleColor(QStringLiteral("check"), QStringLiteral("border.checked"), border);
		}
		else if (hover)
		{
			bg = roleColor(QStringLiteral("check"), QStringLiteral("bg.hover"), bg);
		}
		const int radius =
			radio ? option->rect.width() / 2
				  : roleMetric(QStringLiteral("check"), QStringLiteral("radius"), 3);
		drawRounded(painter, option->rect.adjusted(1, 1, -1, -1), radius, bg, border, 1.5);

		if (on || partial)
		{
			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);
			const QColor ind =
				roleColor(QStringLiteral("check"),
						  enabled ? QStringLiteral("indicator") : QStringLiteral("indicator.disabled"),
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
		return;
	}

	if (element == PE_PanelMenu || element == PE_FrameMenu)
	{
		const QColor bg = roleColor(QStringLiteral("menu"), QStringLiteral("bg"),
									option->palette.color(QPalette::Window));
		const QColor border = roleColor(QStringLiteral("menu"), QStringLiteral("border"),
										option->palette.color(QPalette::Mid));
		const int radius = roleMetric(QStringLiteral("menu"), QStringLiteral("radius"), 4);
		drawRounded(painter, option->rect, radius, bg, border);
		return;
	}

	if (element == PE_IndicatorArrowDown || element == PE_IndicatorArrowUp
		|| element == PE_IndicatorArrowLeft || element == PE_IndicatorArrowRight)
	{
		Qt::ArrowType type = Qt::DownArrow;
		if (element == PE_IndicatorArrowUp)
		{
			type = Qt::UpArrow;
		}
		else if (element == PE_IndicatorArrowLeft)
		{
			type = Qt::LeftArrow;
		}
		else if (element == PE_IndicatorArrowRight)
		{
			type = Qt::RightArrow;
		}
		const bool enabled = option->state & State_Enabled;
		const QColor color =
			roleColor(QStringLiteral("combo"),
					  enabled ? QStringLiteral("arrow") : QStringLiteral("arrow.disabled"),
					  option->palette.color(QPalette::WindowText));
		drawArrow(painter, option->rect, type, color);
		return;
	}

	if (element == PE_PanelButtonTool)
	{
		const bool enabled = option->state & State_Enabled;
		QString bgRole = QStringLiteral("bg");
		if (!enabled)
		{
			bgRole = QStringLiteral("bg.disabled");
		}
		else if (option->state & (State_Sunken | State_On))
		{
			bgRole = QStringLiteral("bg.pressed");
		}
		else if (option->state & State_MouseOver)
		{
			bgRole = QStringLiteral("bg.hover");
		}
		const QColor bg =
			roleColor(QStringLiteral("button"), bgRole, option->palette.color(QPalette::Button));
		const QColor border =
			roleColor(QStringLiteral("button"), QStringLiteral("border"), option->palette.mid().color());
		const int radius = roleMetric(QStringLiteral("button"), QStringLiteral("radius"), 4);
		if (bgRole != QStringLiteral("bg") || (option->state & State_Raised))
		{
			drawRounded(painter, option->rect, radius, bg, border);
		}
		return;
	}

	QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void QThemeStyle::drawControl(ControlElement element, const QStyleOption* option, QPainter* painter,
							  const QWidget* widget) const
{
	if (element == CE_PushButtonBevel)
	{
		const auto* btn = qstyleoption_cast<const QStyleOptionButton*>(option);
		if (btn && painter)
		{
			const bool enabled = btn->state & State_Enabled;
			const bool isDefault =
				(btn->features & QStyleOptionButton::DefaultButton) && enabled;
			QString bgRole = QStringLiteral("bg");
			if (!enabled)
			{
				bgRole = QStringLiteral("bg.disabled");
			}
			else if (isDefault)
			{
				if (btn->state & State_Sunken)
				{
					bgRole = QStringLiteral("bg.accent.pressed");
				}
				else if (btn->state & State_MouseOver)
				{
					bgRole = QStringLiteral("bg.accent.hover");
				}
				else
				{
					bgRole = QStringLiteral("bg.accent");
				}
			}
			else if (btn->state & State_Sunken)
			{
				bgRole = QStringLiteral("bg.pressed");
			}
			else if (btn->state & State_MouseOver)
			{
				bgRole = QStringLiteral("bg.hover");
			}

			const QColor bg =
				roleColor(QStringLiteral("button"), bgRole, btn->palette.color(QPalette::Button));
			const bool focused = (btn->state & State_HasFocus) && enabled;
			const QColor border =
				roleColor(QStringLiteral("button"),
						  focused || isDefault ? QStringLiteral("border.focus") : QStringLiteral("border"),
						  btn->palette.color(QPalette::Mid));
			const int radius = roleMetric(QStringLiteral("button"), QStringLiteral("radius"), 4);
			drawRounded(painter, btn->rect, radius, bg, border, focused || isDefault ? 2.0 : 1.0);
			return;
		}
	}

	if (element == CE_PushButtonLabel)
	{
		const auto* btn = qstyleoption_cast<const QStyleOptionButton*>(option);
		if (btn && painter)
		{
			const bool enabled = btn->state & State_Enabled;
			const bool isDefault =
				(btn->features & QStyleOptionButton::DefaultButton) && enabled;
			QString fgRole = QStringLiteral("fg");
			if (!enabled)
			{
				fgRole = QStringLiteral("fg.disabled");
			}
			else if (isDefault)
			{
				fgRole = QStringLiteral("fg.accent");
			}
			const QColor fg =
				roleColor(QStringLiteral("button"), fgRole, btn->palette.color(QPalette::ButtonText));
			QStyleOptionButton copy = *btn;
			copy.palette.setColor(QPalette::ButtonText, fg);
			copy.palette.setColor(QPalette::WindowText, fg);
			QProxyStyle::drawControl(element, &copy, painter, widget);
			return;
		}
	}

	if (element == CE_TabBarTabShape)
	{
		const auto* tab = qstyleoption_cast<const QStyleOptionTab*>(option);
		if (tab && painter)
		{
			const bool enabled = tab->state & State_Enabled;
			const bool selected = tab->state & State_Selected;
			const bool hover = tab->state & State_MouseOver;
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
			const QColor bg =
				roleColor(QStringLiteral("tab"), bgRole, tab->palette.color(QPalette::Window));
			const QColor border =
				roleColor(QStringLiteral("tab"), QStringLiteral("border"), tab->palette.mid().color());
			const int radius = roleMetric(QStringLiteral("tab"), QStringLiteral("radius"), 4);
			drawRounded(painter, tab->rect.adjusted(1, 1, -1, -1), radius, bg, border);
			if (selected && enabled)
			{
				const QColor ind = roleColor(QStringLiteral("tab"), QStringLiteral("indicator"),
											 tab->palette.color(QPalette::Highlight));
				painter->save();
				painter->setPen(Qt::NoPen);
				painter->setBrush(ind);
				painter->drawRect(tab->rect.left() + 6, tab->rect.bottom() - 2, tab->rect.width() - 12,
								  2);
				painter->restore();
			}
			return;
		}
	}

	if (element == CE_TabBarTabLabel)
	{
		const auto* tab = qstyleoption_cast<const QStyleOptionTab*>(option);
		if (tab && painter)
		{
			const bool enabled = tab->state & State_Enabled;
			const bool selected = tab->state & State_Selected;
			QString fgRole = QStringLiteral("fg");
			if (!enabled)
			{
				fgRole = QStringLiteral("fg.disabled");
			}
			else if (selected)
			{
				fgRole = QStringLiteral("fg.selected");
			}
			const QColor fg =
				roleColor(QStringLiteral("tab"), fgRole, tab->palette.color(QPalette::WindowText));
			QStyleOptionTab copy = *tab;
			copy.palette.setColor(QPalette::WindowText, fg);
			QProxyStyle::drawControl(element, &copy, painter, widget);
			return;
		}
	}

	if (element == CE_MenuItem)
	{
		const auto* mi = qstyleoption_cast<const QStyleOptionMenuItem*>(option);
		if (mi && painter)
		{
			if (mi->menuItemType == QStyleOptionMenuItem::Separator)
			{
				const QColor sep = roleColor(QStringLiteral("menu"), QStringLiteral("separator"),
											 mi->palette.mid().color());
				const int y = mi->rect.center().y();
				painter->fillRect(mi->rect.left() + 8, y, mi->rect.width() - 16, 1, sep);
				return;
			}

			const bool enabled = mi->state & State_Enabled;
			const QColor bg = roleColor(QStringLiteral("menu"), QStringLiteral("bg"),
										mi->palette.color(QPalette::Window));
			const QColor hover = roleColor(QStringLiteral("menu"), QStringLiteral("bg.hover"), bg);
			QString fgRole = QStringLiteral("fg");
			if (!enabled)
			{
				fgRole = QStringLiteral("fg.disabled");
			}
			else if (mi->state & State_Selected)
			{
				fgRole = QStringLiteral("fg.selected");
			}
			const QColor fg =
				roleColor(QStringLiteral("menu"), fgRole, mi->palette.color(QPalette::WindowText));

			QStyleOptionMenuItem copy = *mi;
			copy.palette.setColor(QPalette::Window, bg);
			copy.palette.setColor(QPalette::Base, bg);
			copy.palette.setColor(QPalette::Button, bg);
			copy.palette.setColor(QPalette::Highlight, hover);
			copy.palette.setColor(QPalette::HighlightedText, fg);
			copy.palette.setColor(QPalette::WindowText, fg);
			copy.palette.setColor(QPalette::Text, fg);
			copy.palette.setColor(QPalette::ButtonText, fg);
			QProxyStyle::drawControl(element, &copy, painter, widget);
			return;
		}
	}

	if (element == CE_MenuBarItem)
	{
		const auto* mi = qstyleoption_cast<const QStyleOptionMenuItem*>(option);
		if (mi && painter)
		{
			const bool enabled = mi->state & State_Enabled;
			QString bgRole = QStringLiteral("bar.bg");
			if ((mi->state & State_Sunken) && enabled)
			{
				bgRole = QStringLiteral("bar.bg.pressed");
			}
			else if ((mi->state & (State_Selected | State_MouseOver)) && enabled)
			{
				bgRole = QStringLiteral("bar.bg.hover");
			}
			const QColor bg =
				roleColor(QStringLiteral("menu"), bgRole, mi->palette.color(QPalette::Window));
			const QColor fg =
				roleColor(QStringLiteral("menu"),
						  enabled ? QStringLiteral("bar.fg") : QStringLiteral("fg.disabled"),
						  mi->palette.color(QPalette::WindowText));
			painter->fillRect(mi->rect, bg);
			painter->setPen(fg);
			painter->drawText(mi->rect, Qt::AlignCenter | Qt::TextShowMnemonic, mi->text);
			return;
		}
	}

	if (element == CE_MenuBarEmptyArea)
	{
		const QColor bg = roleColor(QStringLiteral("menu"), QStringLiteral("bar.bg"),
									option->palette.color(QPalette::Window));
		painter->fillRect(option->rect, bg);
		return;
	}

	if (element == CE_ToolBar)
	{
		const QColor bg = roleColor(QStringLiteral("toolbar"), QStringLiteral("bg"),
									option->palette.color(QPalette::Window));
		const QColor border = roleColor(QStringLiteral("toolbar"), QStringLiteral("border"),
										option->palette.mid().color());
		painter->fillRect(option->rect, bg);
		painter->setPen(border);
		painter->drawLine(option->rect.bottomLeft(), option->rect.bottomRight());
		return;
	}

	if (element == CE_HeaderSection)
	{
		const auto* header = qstyleoption_cast<const QStyleOptionHeader*>(option);
		if (header && painter)
		{
			const bool enabled = header->state & State_Enabled;
			QString bgRole = QStringLiteral("bg");
			if (enabled)
			{
				if (header->state & State_Sunken)
				{
					bgRole = QStringLiteral("bg.pressed");
				}
				else if (header->state & State_MouseOver)
				{
					bgRole = QStringLiteral("bg.hover");
				}
			}
			const QColor bg =
				roleColor(QStringLiteral("header"), bgRole, header->palette.color(QPalette::Button));
			const QColor border =
				roleColor(QStringLiteral("header"), QStringLiteral("border"), header->palette.mid().color());
			painter->fillRect(header->rect, bg);
			painter->setPen(border);
			painter->drawLine(header->rect.topRight(), header->rect.bottomRight());
			painter->drawLine(header->rect.bottomLeft(), header->rect.bottomRight());
			return;
		}
	}

	if (element == CE_HeaderLabel)
	{
		const auto* header = qstyleoption_cast<const QStyleOptionHeader*>(option);
		if (header && painter)
		{
			const bool enabled = header->state & State_Enabled;
			const QColor fg =
				roleColor(QStringLiteral("header"),
						  enabled ? QStringLiteral("fg") : QStringLiteral("fg.disabled"),
						  header->palette.color(QPalette::ButtonText));
			QStyleOptionHeader copy = *header;
			copy.palette.setColor(QPalette::ButtonText, fg);
			copy.palette.setColor(QPalette::WindowText, fg);
			QProxyStyle::drawControl(element, &copy, painter, widget);
			return;
		}
	}

	if (element == CE_ComboBoxLabel)
	{
		const auto* combo = qstyleoption_cast<const QStyleOptionComboBox*>(option);
		if (combo && painter)
		{
			const bool enabled = combo->state & State_Enabled;
			const QColor fg =
				roleColor(QStringLiteral("combo"),
						  enabled ? QStringLiteral("fg") : QStringLiteral("fg.disabled"),
						  combo->palette.color(QPalette::ButtonText));
			QStyleOptionComboBox copy = *combo;
			copy.palette.setColor(QPalette::ButtonText, fg);
			copy.palette.setColor(QPalette::Text, fg);
			QProxyStyle::drawControl(element, &copy, painter, widget);
			return;
		}
	}

	QProxyStyle::drawControl(element, option, painter, widget);
}

void QThemeStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex* option,
									 QPainter* painter, const QWidget* widget) const
{
	if (control == CC_ScrollBar)
	{
		const auto* sb = qstyleoption_cast<const QStyleOptionSlider*>(option);
		if (sb && painter)
		{
			const bool enabled = sb->state & State_Enabled;
			const QColor groove =
				roleColor(QStringLiteral("scroll"), QStringLiteral("groove"), sb->palette.window().color());
			painter->fillRect(sb->rect, groove);

			const QRect handle = subControlRect(CC_ScrollBar, sb, SC_ScrollBarSlider, widget);
			QString handleRole = QStringLiteral("handle");
			if (!enabled)
			{
				handleRole = QStringLiteral("handle.disabled");
			}
			else if (sb->state & State_Sunken)
			{
				handleRole = QStringLiteral("handle.pressed");
			}
			else if (sb->state & State_MouseOver)
			{
				handleRole = QStringLiteral("handle.hover");
			}
			const QColor hc =
				roleColor(QStringLiteral("scroll"), handleRole, sb->palette.mid().color());
			const int radius = roleMetric(QStringLiteral("scroll"), QStringLiteral("radius"), 4);
			drawRounded(painter, handle.adjusted(1, 1, -1, -1), radius, hc, hc);

			const QColor arrow =
				roleColor(QStringLiteral("scroll"),
						  enabled ? QStringLiteral("arrow") : QStringLiteral("arrow.disabled"),
						  sb->palette.color(QPalette::WindowText));
			const QRect sub = subControlRect(CC_ScrollBar, sb, SC_ScrollBarSubLine, widget);
			const QRect add = subControlRect(CC_ScrollBar, sb, SC_ScrollBarAddLine, widget);
			const bool horiz = sb->orientation == Qt::Horizontal;
			drawArrow(painter, sub, horiz ? Qt::LeftArrow : Qt::UpArrow, arrow);
			drawArrow(painter, add, horiz ? Qt::RightArrow : Qt::DownArrow, arrow);
			return;
		}
	}

	if (control == CC_ComboBox)
	{
		const auto* combo = qstyleoption_cast<const QStyleOptionComboBox*>(option);
		if (combo && painter)
		{
			const bool enabled = combo->state & State_Enabled;
			QString bgRole = QStringLiteral("bg");
			QString borderRole = QStringLiteral("border");
			if (!enabled)
			{
				bgRole = QStringLiteral("bg.disabled");
			}
			else if (combo->state & State_Sunken)
			{
				bgRole = QStringLiteral("bg.pressed");
			}
			else if (combo->state & State_MouseOver)
			{
				bgRole = QStringLiteral("bg.hover");
				borderRole = QStringLiteral("border.hover");
			}
			if (enabled && (combo->state & State_HasFocus))
			{
				borderRole = QStringLiteral("border.focus");
			}
			const QColor bg =
				roleColor(QStringLiteral("combo"), bgRole, combo->palette.color(QPalette::Button));
			const QColor border =
				roleColor(QStringLiteral("combo"), borderRole, combo->palette.mid().color());
			const int radius = roleMetric(QStringLiteral("combo"), QStringLiteral("radius"), 4);
			const qreal bw = (borderRole == QStringLiteral("border.focus")) ? 2.0 : 1.0;
			drawRounded(painter, combo->rect, radius, bg, border, bw);

			const QRect arrowRect = subControlRect(CC_ComboBox, combo, SC_ComboBoxArrow, widget);
			const QColor arrow =
				roleColor(QStringLiteral("combo"),
						  enabled ? QStringLiteral("arrow") : QStringLiteral("arrow.disabled"),
						  combo->palette.color(QPalette::WindowText));
			drawArrow(painter, arrowRect, Qt::DownArrow, arrow);

			if (combo->editable)
			{
				// Frame already drawn; edit field uses PE_FrameLineEdit.
			}
			return;
		}
	}

	if (control == CC_ToolButton)
	{
		const auto* tb = qstyleoption_cast<const QStyleOptionToolButton*>(option);
		if (tb && painter)
		{
			QStyleOptionToolButton copy = *tb;
			if (tb->subControls & SC_ToolButton)
			{
				drawPrimitive(PE_PanelButtonTool, &copy, painter, widget);
			}
			if (tb->subControls & SC_ToolButtonMenu)
			{
				const QRect menuRect = subControlRect(CC_ToolButton, tb, SC_ToolButtonMenu, widget);
				QStyleOptionToolButton menuOpt = *tb;
				menuOpt.rect = menuRect;
				const QColor arrow =
					roleColor(QStringLiteral("combo"),
							  (tb->state & State_Enabled) ? QStringLiteral("arrow")
														  : QStringLiteral("arrow.disabled"),
							  tb->palette.color(QPalette::WindowText));
				drawArrow(painter, menuRect, Qt::DownArrow, arrow);
			}
			QProxyStyle::drawControl(CE_ToolButtonLabel, &copy, painter, widget);
			return;
		}
	}

	QProxyStyle::drawComplexControl(control, option, painter, widget);
}

} // namespace qtheme
