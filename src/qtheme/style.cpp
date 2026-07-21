#include "qtheme/style.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QStyleFactory>
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QStyleOptionComplex>
#include <QStyleOptionSlider>
#include <QStyleOptionTab>

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

	if (element == PE_FrameLineEdit)
	{
		const auto* frame = qstyleoption_cast<const QStyleOption*>(option);
		QString borderRole = QStringLiteral("border");
		if (option->state & State_HasFocus)
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
		const qreal bw = (option->state & State_HasFocus) ? 2.0 : 1.0;
		drawRounded(painter, option->rect, radius, bg, border, bw);
		Q_UNUSED(frame);
		return;
	}

	if (element == PE_IndicatorCheckBox || element == PE_IndicatorRadioButton)
	{
		const bool radio = (element == PE_IndicatorRadioButton);
		const bool on = option->state & State_On;
		const bool partial = option->state & State_NoChange;
		const bool hover = option->state & State_MouseOver;
		QColor bg = roleColor(QStringLiteral("check"), QStringLiteral("bg"),
							  option->palette.color(QPalette::Base));
		QColor border = roleColor(QStringLiteral("check"), QStringLiteral("border"),
								  option->palette.color(QPalette::Mid));
		if (on || partial)
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
			const QColor ind = roleColor(QStringLiteral("check"), QStringLiteral("indicator"),
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
			QString bgRole = QStringLiteral("bg");
			if (!(btn->state & State_Enabled))
			{
				bgRole = QStringLiteral("bg.disabled");
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
			const bool focused = (btn->state & State_HasFocus) && (btn->state & State_Enabled);
			const QColor border =
				roleColor(QStringLiteral("button"),
						  focused ? QStringLiteral("border.focus") : QStringLiteral("border"),
						  btn->palette.color(QPalette::Mid));
			const int radius = roleMetric(QStringLiteral("button"), QStringLiteral("radius"), 4);
			drawRounded(painter, btn->rect, radius, bg, border, focused ? 2.0 : 1.0);
			return;
		}
	}

	if (element == CE_PushButtonLabel)
	{
		const auto* btn = qstyleoption_cast<const QStyleOptionButton*>(option);
		if (btn && painter)
		{
			QString fgRole = QStringLiteral("fg");
			if (!(btn->state & State_Enabled))
			{
				fgRole = QStringLiteral("fg.disabled");
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
			const bool selected = tab->state & State_Selected;
			const bool hover = tab->state & State_MouseOver;
			QString bgRole = QStringLiteral("bg");
			if (selected)
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
			if (selected)
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
			const bool selected = tab->state & State_Selected;
			const QColor fg =
				roleColor(QStringLiteral("tab"),
						  selected ? QStringLiteral("fg.selected") : QStringLiteral("fg"),
						  tab->palette.color(QPalette::WindowText));
			QStyleOptionTab copy = *tab;
			copy.palette.setColor(QPalette::WindowText, fg);
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
			const QColor groove =
				roleColor(QStringLiteral("scroll"), QStringLiteral("groove"), sb->palette.window().color());
			painter->fillRect(sb->rect, groove);

			QStyleOptionSlider copy = *sb;
			const QRect handle = subControlRect(CC_ScrollBar, sb, SC_ScrollBarSlider, widget);
			QString handleRole = QStringLiteral("handle");
			if (sb->state & State_Sunken)
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
			Q_UNUSED(copy);
			return;
		}
	}

	QProxyStyle::drawComplexControl(control, option, painter, widget);
}

} // namespace qtheme
