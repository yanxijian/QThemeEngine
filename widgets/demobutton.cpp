#include "demobutton.hpp"

#include "qtheme/api.hpp"
#include "qtheme/engine.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QShowEvent>
#include <QStyleOption>

namespace qtheme::demo
{
	DemoButton::DemoButton(QWidget* parent)
		: QWidget(parent)
	{
		api::setThemeClass(this, QStringLiteral("DemoButton"));
		setMouseTracking(true);
		setAttribute(Qt::WA_Hover, true);
		ensurePackConnection();
	}

	void DemoButton::setText(const QString& text)
	{
		m_text = text;
		update();
	}

	QString DemoButton::text() const
	{
		return m_text;
	}

	void DemoButton::ensurePackConnection()
	{
		if (m_packConnected)
		{
			return;
		}
		if (auto* eng = api::engine())
		{
			connect(
				eng, &Engine::packChanged, this,
				[this](const QString&, const QString&)
				{
					update();
				},
				Qt::QueuedConnection);
			m_packConnected = true;
		}
	}

	void DemoButton::showEvent(QShowEvent* event)
	{
		ensurePackConnection();
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
		m_pressed = true;
		update();
		QWidget::mousePressEvent(event);
	}

	void DemoButton::mouseReleaseEvent(QMouseEvent* event)
	{
		m_pressed = false;
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
		if (m_pressed)
		{
			opt.state |= QStyle::State_Sunken;
		}
		if (!isEnabled())
		{
			opt.state &= ~QStyle::State_Enabled;
		}

		const QString cls = api::themeClass(this);
		const QColor bg = api::color(cls, api::roleWithState(QStringLiteral("background"), &opt));
		const QColor fg = api::color(cls, api::roleWithState(QStringLiteral("text"), &opt));
		const QColor border = api::color(cls, QStringLiteral("border"));
		const int radius = api::metric(cls, QStringLiteral("borderRadius"), 6);

		QPainter p(this);
		p.setRenderHint(QPainter::Antialiasing);
		QPainterPath path;
		path.addRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
		p.fillPath(path, bg.isValid() ? bg : palette().button().color());
		p.setPen(QPen(border.isValid() ? border : palette().mid().color(), 1));
		p.drawPath(path);
		p.setPen(fg.isValid() ? fg : palette().buttonText().color());
		p.drawText(rect(), Qt::AlignCenter, m_text);
	}
} // namespace qtheme::demo
