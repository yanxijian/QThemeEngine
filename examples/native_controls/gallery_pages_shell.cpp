#include "gallery.hpp"

#include "demobutton.hpp"

#include "qtheme/engine.hpp"

#include <QButtonGroup>
#include <QCalendarWidget>
#include <QCheckBox>
#include <QColorDialog>
#include <QColumnView>
#include <QComboBox>
#include <QCommandLinkButton>
#include <QDate>
#include <QDateEdit>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QDial>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDockWidget>
#include <QDoubleSpinBox>
#include <QErrorMessage>
#include <QFileDialog>
#include <QCompleter>
#include <QFocusFrame>
#include <QFontComboBox>
#include <QFontDialog>
#include <QFormLayout>
#include <QFrame>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QKeySequenceEdit>
#include <QLabel>
#include <QLCDNumber>
#include <QLineEdit>
#include <QListView>
#include <QListWidget>
#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QProgressDialog>
#include <QPushButton>
#include <QRadioButton>
#include <QRubberBand>
#include <QScrollArea>
#include <QScrollBar>
#include <QSizeGrip>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QStringListModel>
#include <QTabBar>
#include <QTabWidget>
#include <QTableView>
#include <QTableWidget>
#include <QTextBrowser>
#include <QTextEdit>
#include <QTime>
#include <QTimeEdit>
#include <QToolBar>
#include <QToolBox>
#include <QToolButton>
#include <QToolTip>
#include <QTreeView>
#include <QTreeWidget>
#include <QUndoCommand>
#include <QUndoStack>
#include <QUndoView>
#include <QVBoxLayout>
#include <QWhatsThis>
#include <QWizard>
#include <QWizardPage>

#if defined(QTE_HAS_OPENGLWIDGETS)
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QRhiWidget>
#endif

#if defined(QTE_HAS_PRINTSUPPORT)
#include <QPageSetupDialog>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrintPreviewWidget>
#include <QPrinter>
#endif

#include <QApplication>
#include <QStyle>

#include "gallery.hpp"
#include "gallery_internal.hpp"

namespace gallery {

QWidget* pageContainers(QWidget* parent)
{
	auto* root = new QWidget;
	auto* layout = new QVBoxLayout(root);

	layout->addWidget(new QLabel(
		QStringLiteral("QDockWidget samples are attached to the main window (left / right). "
					   "QScrollArea wraps each gallery page."),
		root));

	{
		auto* box = detail::makeGroup(QStringLiteral("Nested QTabWidget"), root);
		auto* tabs = new QTabWidget(box);
		tabs->addTab(new QLabel(QStringLiteral("Nested tab A"), tabs), QStringLiteral("A"));
		tabs->addTab(new QLabel(QStringLiteral("Nested tab B"), tabs), QStringLiteral("B"));
		tabs->addTab(new QLabel(QStringLiteral("Disabled"), tabs), QStringLiteral("C"));
		tabs->setTabEnabled(2, false);
		static_cast<QVBoxLayout*>(box->layout())->addWidget(tabs);
		layout->addWidget(box);
	}

	{
		auto* box = detail::makeGroup(QStringLiteral("QToolBox"), root);
		auto* toolbox = new QToolBox(box);
		toolbox->addItem(new QLabel(QStringLiteral("Page one content"), toolbox),
						 QStringLiteral("Section 1"));
		toolbox->addItem(new QLabel(QStringLiteral("Page two content"), toolbox),
						 QStringLiteral("Section 2"));
		toolbox->addItem(new QLabel(QStringLiteral("Page three content"), toolbox),
						 QStringLiteral("Section 3"));
		static_cast<QVBoxLayout*>(box->layout())->addWidget(toolbox);
		layout->addWidget(box);
	}

	{
		auto* box = detail::makeGroup(QStringLiteral("QStackedWidget + switcher"), root);
		auto* stack = new QStackedWidget(box);
		stack->addWidget(new QLabel(QStringLiteral("Stack page 0"), stack));
		stack->addWidget(new QLabel(QStringLiteral("Stack page 1"), stack));
		stack->addWidget(new QLabel(QStringLiteral("Stack page 2"), stack));
		auto* switcher = new QComboBox(box);
		switcher->addItems(
			{QStringLiteral("Page 0"), QStringLiteral("Page 1"), QStringLiteral("Page 2")});
		QObject::connect(switcher, QOverload<int>::of(&QComboBox::currentIndexChanged), stack,
						 &QStackedWidget::setCurrentIndex);
		static_cast<QVBoxLayout*>(box->layout())->addWidget(switcher);
		static_cast<QVBoxLayout*>(box->layout())->addWidget(stack);
		layout->addWidget(box);
	}

	{
		auto* box = detail::makeGroup(QStringLiteral("QSplitter"), root);
		auto* split = new QSplitter(Qt::Horizontal, box);
		split->addWidget(new QLabel(QStringLiteral("Left pane"), split));
		split->addWidget(new QLabel(QStringLiteral("Right pane"), split));
		split->setSizes({180, 180});
		split->setMinimumHeight(80);
		static_cast<QVBoxLayout*>(box->layout())->addWidget(split);
		layout->addWidget(box);
	}

	{
		auto* checkable = new QGroupBox(QStringLiteral("Checkable group"), root);
		checkable->setCheckable(true);
		checkable->setChecked(true);
		auto* gl = new QVBoxLayout(checkable);
		gl->addWidget(new QLabel(QStringLiteral("Content inside checkable QGroupBox"), checkable));
		layout->addWidget(checkable);
	}

	{
		auto* box = detail::makeGroup(QStringLiteral("QRubberBand / QSizeGrip / QFocusFrame"), root);
		auto* host = detail::createRubberBandHost(box);
		static_cast<QVBoxLayout*>(box->layout())->addWidget(host);

		auto* gripPanel = new QWidget(box);
		gripPanel->setMinimumHeight(56);
		auto* gripLayout = new QGridLayout(gripPanel);
		gripLayout->setContentsMargins(0, 0, 0, 0);
		gripLayout->addWidget(new QLabel(QStringLiteral("QSizeGrip →"), gripPanel), 0, 0);
		gripLayout->addWidget(new QSizeGrip(gripPanel), 0, 1, Qt::AlignBottom | Qt::AlignRight);
		static_cast<QVBoxLayout*>(box->layout())->addWidget(gripPanel);

		auto* focusTarget = new QPushButton(QStringLiteral("Focus target (QFocusFrame)"), box);
		auto* focusFrame = new QFocusFrame(box);
		focusFrame->setWidget(focusTarget);
		static_cast<QVBoxLayout*>(box->layout())->addWidget(focusTarget);
		layout->addWidget(box);
	}

	layout->addStretch(1);
	return detail::wrapScroll(root, parent);
}

QWidget* pageMdi(QWidget* parent)
{
	auto* root = new QWidget;
	auto* layout = new QVBoxLayout(root);
	layout->addWidget(new QLabel(QStringLiteral("QMdiArea / QMdiSubWindow"), root));

	auto* mdi = new QMdiArea(root);
	mdi->setMinimumHeight(320);
	mdi->setViewMode(QMdiArea::SubWindowView);

	auto addSub = [mdi](const QString& title)
	{
		auto* w = new QTextEdit;
		w->setPlainText(QStringLiteral("Sub-window: %1").arg(title));
		auto* sub = mdi->addSubWindow(w);
		sub->setWindowTitle(title);
		sub->resize(220, 140);
		sub->show();
	};
	addSub(QStringLiteral("Document 1"));
	addSub(QStringLiteral("Document 2"));
	addSub(QStringLiteral("Document 3"));

	auto* row = new QHBoxLayout;
	auto* tile = new QPushButton(QStringLiteral("Tile"), root);
	auto* cascade = new QPushButton(QStringLiteral("Cascade"), root);
	QObject::connect(tile, &QPushButton::clicked, mdi, &QMdiArea::tileSubWindows);
	QObject::connect(cascade, &QPushButton::clicked, mdi, &QMdiArea::cascadeSubWindows);
	row->addWidget(tile);
	row->addWidget(cascade);
	row->addStretch(1);

	layout->addLayout(row);
	layout->addWidget(mdi);
	return root;
}


} // namespace gallery
