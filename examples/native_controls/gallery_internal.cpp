#include "gallery_internal.hpp"

#include <QMouseEvent>
#include <QPainter>
#include <QPoint>
#include <QRubberBand>
#include <QScrollArea>
#include <QStandardItem>
#include <QVBoxLayout>

#if defined(QTE_HAS_OPENGLWIDGETS)
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QRhiWidget>
#endif

namespace gallery {
namespace detail {

QWidget* wrapScroll(QWidget* content, QWidget* parent)
{
	auto* area = new QScrollArea(parent);
	area->setWidgetResizable(true);
	area->setFrameShape(QFrame::NoFrame);
	area->setWidget(content);
	return area;
}

QGroupBox* makeGroup(const QString& title, QWidget* parent)
{
	auto* box = new QGroupBox(title, parent);
	box->setLayout(new QVBoxLayout);
	return box;
}

namespace {

class RubberBandHost final : public QWidget
{
public:
	explicit RubberBandHost(QWidget* parent = nullptr)
		: QWidget(parent)
		, band_(new QRubberBand(QRubberBand::Rectangle, this))
	{
		setMinimumHeight(100);
		setAutoFillBackground(true);
		setToolTip(QStringLiteral("Drag to preview QRubberBand"));
	}

protected:
	void mousePressEvent(QMouseEvent* event) override
	{
		origin_ = event->pos();
		band_->setGeometry(QRect(origin_, QSize()));
		band_->show();
	}
	void mouseMoveEvent(QMouseEvent* event) override
	{
		band_->setGeometry(QRect(origin_, event->pos()).normalized());
	}
	void mouseReleaseEvent(QMouseEvent* event) override
	{
		Q_UNUSED(event);
		band_->hide();
	}
	void paintEvent(QPaintEvent*) override
	{
		QPainter p(this);
		p.setPen(palette().mid().color());
		p.drawText(rect(), Qt::AlignCenter, QStringLiteral("Drag here — QRubberBand"));
	}

private:
	QPoint origin_;
	QRubberBand* band_ = nullptr;
};

#if defined(QTE_HAS_OPENGLWIDGETS)
class SampleRhiWidget final : public QRhiWidget
{
public:
	using QRhiWidget::QRhiWidget;

protected:
	void initialize(QRhiCommandBuffer* /*cb*/) override {}
	void render(QRhiCommandBuffer* /*cb*/) override {}
};

class SampleGlWidget final : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
	using QOpenGLWidget::QOpenGLWidget;

protected:
	void initializeGL() override { initializeOpenGLFunctions(); }
	void paintGL() override
	{
		glClearColor(0.22f, 0.35f, 0.48f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
};
#endif

} // namespace

QWidget* createRubberBandHost(QWidget* parent)
{
	return new RubberBandHost(parent);
}

#if defined(QTE_HAS_OPENGLWIDGETS)
QWidget* createSampleGlWidget(QWidget* parent)
{
	return new SampleGlWidget(parent);
}

QWidget* createSampleRhiWidget(QWidget* parent)
{
	return new SampleRhiWidget(parent);
}
#endif

QStandardItemModel* makeTreeModel(QObject* parent)
{
	auto* model = new QStandardItemModel(parent);
	model->setHorizontalHeaderLabels({QStringLiteral("Name"), QStringLiteral("Role")});
	auto* a = new QStandardItem(QStringLiteral("Department A"));
	a->appendRow({new QStandardItem(QStringLiteral("Alice")), new QStandardItem(QStringLiteral("Dev"))});
	a->appendRow({new QStandardItem(QStringLiteral("Bob")), new QStandardItem(QStringLiteral("QA"))});
	auto* b = new QStandardItem(QStringLiteral("Department B"));
	b->appendRow({new QStandardItem(QStringLiteral("Carol")), new QStandardItem(QStringLiteral("PM"))});
	model->appendRow(a);
	model->appendRow(b);
	return model;
}

} // namespace detail
} // namespace gallery
