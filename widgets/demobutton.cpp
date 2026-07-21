#include "demobutton.hpp"

#include "theme/themeapi.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QShowEvent>
#include <QStyleOption>

namespace theme
{
	DemoButton::DemoButton(QWidget* parent)
		: QWidget(parent)
	{
		ThemeApi::setThemeClassName(this, QStringLiteral("DemoButton"));
		setMouseTracking(true);
		setAttribute(Qt::WA_Hover, true);
		ensureSkinConnection();
	}

	void DemoButton::setText(const QString& text)
	{
		text_ = text;
		update();
	}

	QString DemoButton::text() const
	{
		return text_;
	}

	void DemoButton::ensureSkinConnection()
	{
		if (skinConnected_)
		{
			return;
		}
		if (auto* obj = ThemeApi::skinObject())
		{
			connect(obj, SIGNAL(skinChanged(QString, QString)), this, SLOT(update()), Qt::QueuedConnection);
			skinConnected_ = true;
		}
	}

	void DemoButton::showEvent(QShowEvent* event)
	{
		ensureSkinConnection();
		QWidget::showEvent(event);
	}

	void DemoButton::enterEvent(QEnterEvent* event)
	{
		QWidget::enterEvent(event);
		update();
	}

	void DemoButton::leaveEvent(QEvent* event)
	{
		QWidget::leaveEvent(event);
		update();
	}

	void DemoButton::mousePressEvent(QMouseEvent* event)
	{
		pressed_ = true;
		update();
		QWidget::mousePressEvent(event);
	}

	void DemoButton::mouseReleaseEvent(QMouseEvent* event)
	{
		pressed_ = false;
		update();
		QWidget::mouseReleaseEvent(event);
	}

	void DemoButton::paintEvent(QPaintEvent* /*event*/)
	{
		QStyleOption opt;
		opt.initFrom(this);
		if (underMouse())
		{
			opt.state |= QStyle::State_MouseOver;
		}
		if (pressed_)
		{
			opt.state |= QStyle::State_Sunken;
		}
		if (!isEnabled())
		{
			opt.state &= ~QStyle::State_Enabled;
		}

		const QString cls = ThemeApi::themeClassName(this);
		const QColor bg = ThemeApi::color(cls, ThemeApi::propWithState(QStringLiteral("background"), &opt));
		const QColor fg = ThemeApi::color(cls, ThemeApi::propWithState(QStringLiteral("text"), &opt));
		const QColor border = ThemeApi::color(cls, QStringLiteral("border"));
		const int radius = ThemeApi::hint(cls, QStringLiteral("borderRadius"), 6);

		QPainter p(this);
		p.setRenderHint(QPainter::Antialiasing);
		QPainterPath path;
		path.addRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
		p.fillPath(path, bg.isValid() ? bg : palette().button().color());
		p.setPen(QPen(border.isValid() ? border : palette().mid().color(), 1));
		p.drawPath(path);
		p.setPen(fg.isValid() ? fg : palette().buttonText().color());
		p.drawText(rect(), Qt::AlignCenter, text_);
	}
} // namespace theme
