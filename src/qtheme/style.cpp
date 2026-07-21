#include "qtheme/style.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QStyleFactory>
#include <QStyleOption>

namespace qtheme {

namespace {

QStyle* makeFusionBase()
{
	QStyle* fusion = QStyleFactory::create(QStringLiteral("Fusion"));
	return fusion ? fusion : QStyleFactory::create(QStringLiteral("windows"));
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
	case PM_DefaultFrameWidth:
		return 1;
	default:
		return QProxyStyle::pixelMetric(metric, option, widget);
	}
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

			const QColor bg = roleColor(QStringLiteral("button"), bgRole,
										btn->palette.color(QPalette::Button));
			const bool focused = (btn->state & State_HasFocus) && (btn->state & State_Enabled);
			const QColor border =
				roleColor(QStringLiteral("button"),
						  focused ? QStringLiteral("border.focus") : QStringLiteral("border"),
						  btn->palette.color(QPalette::Mid));
			const int radius = roleMetric(QStringLiteral("button"), QStringLiteral("radius"), 6);

			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);
			const QRectF r = QRectF(btn->rect).adjusted(0.5, 0.5, -0.5, -0.5);
			QPainterPath path;
			path.addRoundedRect(r, radius, radius);
			painter->fillPath(path, bg);
			painter->setPen(QPen(border, 1));
			painter->drawPath(path);
			painter->restore();
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
			const QColor fg = roleColor(QStringLiteral("button"), fgRole,
										btn->palette.color(QPalette::ButtonText));
			QStyleOptionButton copy = *btn;
			copy.palette.setColor(QPalette::ButtonText, fg);
			copy.palette.setColor(QPalette::WindowText, fg);
			QProxyStyle::drawControl(element, &copy, painter, widget);
			return;
		}
	}

	QProxyStyle::drawControl(element, option, painter, widget);
}

} // namespace qtheme
