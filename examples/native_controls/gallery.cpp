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

namespace gallery {
namespace {

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

} // namespace

QWidget* pageButtons(QWidget* parent)
{
	auto* root = new QWidget;
	auto* layout = new QVBoxLayout(root);

	{
		auto* box = makeGroup(QStringLiteral("Push / Tool / Command"), root);
		auto* row = new QHBoxLayout;
		row->addWidget(new QPushButton(QStringLiteral("Default"), box));
		auto* primary = new QPushButton(QStringLiteral("Default+"), box);
		primary->setDefault(true);
		row->addWidget(primary);
		auto* flat = new QPushButton(QStringLiteral("Flat"), box);
		flat->setFlat(true);
		row->addWidget(flat);
		auto* disabled = new QPushButton(QStringLiteral("Disabled"), box);
		disabled->setEnabled(false);
		row->addWidget(disabled);
		auto* tool = new QToolButton(box);
		tool->setText(QStringLiteral("Tool"));
		tool->setToolButtonStyle(Qt::ToolButtonTextOnly);
		tool->setPopupMode(QToolButton::MenuButtonPopup);
		auto* toolMenu = new QMenu(tool);
		toolMenu->addAction(QStringLiteral("Action 1"));
		toolMenu->addAction(QStringLiteral("Action 2"));
		tool->setMenu(toolMenu);
		row->addWidget(tool);
		auto* link = new QCommandLinkButton(QStringLiteral("Command link"),
											QStringLiteral("Secondary description"), box);
		row->addWidget(link);
		row->addStretch(1);
		static_cast<QVBoxLayout*>(box->layout())->addLayout(row);
		layout->addWidget(box);
	}

	{
		auto* box = makeGroup(QStringLiteral("Check / Radio"), root);
		auto* row = new QHBoxLayout;
		auto* c1 = new QCheckBox(QStringLiteral("Unchecked"), box);
		auto* c2 = new QCheckBox(QStringLiteral("Checked"), box);
		c2->setChecked(true);
		auto* c3 = new QCheckBox(QStringLiteral("Partial"), box);
		c3->setTristate(true);
		c3->setCheckState(Qt::PartiallyChecked);
		auto* c4 = new QCheckBox(QStringLiteral("Disabled"), box);
		c4->setEnabled(false);
		row->addWidget(c1);
		row->addWidget(c2);
		row->addWidget(c3);
		row->addWidget(c4);

		auto* radios = new QButtonGroup(box);
		auto* r1 = new QRadioButton(QStringLiteral("Option A"), box);
		auto* r2 = new QRadioButton(QStringLiteral("Option B"), box);
		r2->setChecked(true);
		auto* r3 = new QRadioButton(QStringLiteral("Disabled"), box);
		r3->setEnabled(false);
		radios->addButton(r1);
		radios->addButton(r2);
		radios->addButton(r3);
		row->addWidget(r1);
		row->addWidget(r2);
		row->addWidget(r3);
		row->addStretch(1);
		static_cast<QVBoxLayout*>(box->layout())->addLayout(row);
		layout->addWidget(box);
	}

	{
		auto* box = makeGroup(QStringLiteral("Dialog button box"), root);
		auto* buttons = new QDialogButtonBox(
			QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply
				| QDialogButtonBox::Help,
			box);
		static_cast<QVBoxLayout*>(box->layout())->addWidget(buttons);
		layout->addWidget(box);
	}

	layout->addStretch(1);
	return wrapScroll(root, parent);
}

QWidget* pageInput(QWidget* parent)
{
	auto* root = new QWidget;
	auto* form = new QFormLayout(root);
	form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

	auto* line = new QLineEdit(root);
	line->setPlaceholderText(QStringLiteral("Type a… / b… for QCompleter"));
	line->setToolTip(QStringLiteral("QToolTip on QLineEdit"));
	auto* completer = new QCompleter(
		QStringList{QStringLiteral("alpha"), QStringLiteral("amber"), QStringLiteral("beta"),
					QStringLiteral("browser"), QStringLiteral("button")},
		line);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	line->setCompleter(completer);
	form->addRow(QStringLiteral("QLineEdit + QCompleter"), line);

	auto* password = new QLineEdit(root);
	password->setEchoMode(QLineEdit::Password);
	password->setText(QStringLiteral("secret"));
	form->addRow(QStringLiteral("Password"), password);

	auto* readOnly = new QLineEdit(QStringLiteral("Read-only"), root);
	readOnly->setReadOnly(true);
	form->addRow(QStringLiteral("Read-only"), readOnly);

	auto* combo = new QComboBox(root);
	combo->addItems({QStringLiteral("Alpha"), QStringLiteral("Beta"), QStringLiteral("Gamma")});
	combo->setEditable(true);
	form->addRow(QStringLiteral("QComboBox"), combo);

	auto* fonts = new QFontComboBox(root);
	form->addRow(QStringLiteral("QFontComboBox"), fonts);

	auto* spin = new QSpinBox(root);
	spin->setRange(0, 100);
	spin->setValue(42);
	spin->setSuffix(QStringLiteral(" %"));
	form->addRow(QStringLiteral("QSpinBox"), spin);

	auto* dspin = new QDoubleSpinBox(root);
	dspin->setDecimals(2);
	dspin->setValue(3.14);
	form->addRow(QStringLiteral("QDoubleSpinBox"), dspin);

	auto* keys = new QKeySequenceEdit(QKeySequence(QStringLiteral("Ctrl+S")), root);
	form->addRow(QStringLiteral("QKeySequenceEdit"), keys);

	auto* date = new QDateEdit(QDate::currentDate(), root);
	date->setCalendarPopup(true);
	form->addRow(QStringLiteral("QDateEdit"), date);

	auto* time = new QTimeEdit(QTime::currentTime(), root);
	form->addRow(QStringLiteral("QTimeEdit"), time);

	auto* dt = new QDateTimeEdit(QDateTime::currentDateTime(), root);
	dt->setCalendarPopup(true);
	form->addRow(QStringLiteral("QDateTimeEdit"), dt);

	auto* plain = new QPlainTextEdit(root);
	plain->setPlaceholderText(QStringLiteral("QPlainTextEdit…"));
	plain->setMaximumHeight(80);
	form->addRow(QStringLiteral("QPlainTextEdit"), plain);

	auto* rich = new QTextEdit(root);
	rich->setHtml(QStringLiteral("<b>QTextEdit</b> with <i>rich</i> text"));
	rich->setMaximumHeight(80);
	form->addRow(QStringLiteral("QTextEdit"), rich);

	return wrapScroll(root, parent);
}

QWidget* pageDisplay(QWidget* parent)
{
	auto* root = new QWidget;
	auto* layout = new QVBoxLayout(root);

	layout->addWidget(new QLabel(QStringLiteral("QLabel — primary text"), root));
	auto* muted = new QLabel(QStringLiteral("Disabled label"), root);
	muted->setEnabled(false);
	layout->addWidget(muted);

	auto* browser = new QTextBrowser(root);
	browser->setHtml(
		QStringLiteral("<h3>QTextBrowser</h3><p>Read-only rich text with "
					   "<a href=\"https://www.qt.io\">hyperlink</a>.</p>"));
	browser->setOpenExternalLinks(true);
	browser->setMaximumHeight(100);
	layout->addWidget(browser);

	auto* lcd = new QLCDNumber(root);
	lcd->display(1234);
	lcd->setSegmentStyle(QLCDNumber::Flat);
	lcd->setMinimumHeight(48);
	layout->addWidget(lcd);

	{
		auto* box = makeGroup(QStringLiteral("Progress"), root);
		auto* row = new QHBoxLayout;
		auto* p1 = new QProgressBar(box);
		p1->setValue(35);
		auto* p2 = new QProgressBar(box);
		p2->setRange(0, 0);
		auto* p3 = new QProgressBar(box);
		p3->setOrientation(Qt::Vertical);
		p3->setValue(70);
		p3->setMinimumHeight(80);
		row->addWidget(p1, 1);
		row->addWidget(p2, 1);
		row->addWidget(p3);
		static_cast<QVBoxLayout*>(box->layout())->addLayout(row);
		layout->addWidget(box);
	}

	{
		auto* box = makeGroup(QStringLiteral("Slider / Dial / ScrollBar"), root);
		auto* grid = new QGridLayout;
		auto* h = new QSlider(Qt::Horizontal, box);
		h->setRange(0, 100);
		h->setValue(40);
		auto* v = new QSlider(Qt::Vertical, box);
		v->setRange(0, 100);
		v->setValue(60);
		v->setMinimumHeight(100);
		auto* dial = new QDial(box);
		dial->setRange(0, 100);
		dial->setValue(25);
		dial->setNotchesVisible(true);
		auto* hbar = new QScrollBar(Qt::Horizontal, box);
		hbar->setRange(0, 100);
		hbar->setValue(30);
		auto* vbar = new QScrollBar(Qt::Vertical, box);
		vbar->setRange(0, 100);
		vbar->setValue(55);
		vbar->setMinimumHeight(100);
		grid->addWidget(new QLabel(QStringLiteral("QSlider H"), box), 0, 0);
		grid->addWidget(h, 0, 1);
		grid->addWidget(new QLabel(QStringLiteral("QSlider V"), box), 1, 0);
		grid->addWidget(v, 1, 1);
		grid->addWidget(new QLabel(QStringLiteral("QDial"), box), 0, 2);
		grid->addWidget(dial, 0, 3, 2, 1);
		grid->addWidget(new QLabel(QStringLiteral("QScrollBar H"), box), 2, 0);
		grid->addWidget(hbar, 2, 1, 1, 3);
		grid->addWidget(new QLabel(QStringLiteral("QScrollBar V"), box), 0, 4);
		grid->addWidget(vbar, 0, 5, 3, 1);
		static_cast<QVBoxLayout*>(box->layout())->addLayout(grid);
		layout->addWidget(box);
	}

	{
		auto* box = makeGroup(QStringLiteral("QUndoView"), root);
		auto* stack = new QUndoStack(box);
		stack->push(new QUndoCommand(QStringLiteral("Insert row")));
		stack->push(new QUndoCommand(QStringLiteral("Edit cell")));
		stack->push(new QUndoCommand(QStringLiteral("Delete row")));
		auto* undoView = new QUndoView(stack, box);
		undoView->setMaximumHeight(100);
		static_cast<QVBoxLayout*>(box->layout())->addWidget(undoView);
		layout->addWidget(box);
	}

	{
		auto* box = makeGroup(QStringLiteral("QGraphicsView"), root);
		auto* scene = new QGraphicsScene(box);
		scene->setSceneRect(0, 0, 280, 120);
		scene->addEllipse(20, 20, 80, 80, QPen(Qt::darkCyan), QBrush(QColor(100, 180, 200)));
		scene->addRect(140, 30, 100, 60, QPen(Qt::darkMagenta), QBrush(QColor(200, 140, 180)));
		scene->addText(QStringLiteral("Scene"));
		auto* view = new QGraphicsView(scene, box);
		view->setMinimumHeight(140);
		static_cast<QVBoxLayout*>(box->layout())->addWidget(view);
		layout->addWidget(box);
	}

	{
		auto* box = makeGroup(QStringLiteral("GPU canvases"), root);
		auto* row = new QHBoxLayout;
#if defined(QTE_HAS_OPENGLWIDGETS)
		auto* gl = new SampleGlWidget(box);
		gl->setMinimumSize(160, 100);
		auto* rhi = new SampleRhiWidget(box);
		rhi->setMinimumSize(160, 100);
		row->addWidget(new QLabel(QStringLiteral("QOpenGLWidget"), box));
		row->addWidget(gl);
		row->addWidget(new QLabel(QStringLiteral("QRhiWidget"), box));
		row->addWidget(rhi);
#else
		row->addWidget(new QLabel(QStringLiteral("QOpenGLWidget / QRhiWidget (Qt OpenGLWidgets not linked)"), box));
#endif
		row->addStretch(1);
		static_cast<QVBoxLayout*>(box->layout())->addLayout(row);
		layout->addWidget(box);
	}

#if defined(QTE_HAS_PRINTSUPPORT)
	{
		auto* box = makeGroup(QStringLiteral("QPrintPreviewWidget (PrintSupport)"), root);
		auto* printer = new QPrinter(QPrinter::ScreenResolution);
		auto* preview = new QPrintPreviewWidget(printer, box);
		preview->setMinimumHeight(160);
		QObject::connect(box, &QObject::destroyed, box,
						 [printer]
						 {
							 delete printer;
						 });
		QObject::connect(preview, &QPrintPreviewWidget::paintRequested, preview,
						 [](QPrinter* p)
						 {
							 QPainter painter(p);
							 painter.drawText(80, 80, QStringLiteral("QPrintPreviewWidget sample"));
						 });
		preview->updatePreview();
		static_cast<QVBoxLayout*>(box->layout())->addWidget(preview);
		layout->addWidget(box);
	}
#endif

	{
		auto* box = makeGroup(QStringLiteral("Frames"), root);
		auto* row = new QHBoxLayout;
		for (auto shape :
			 {QFrame::Box, QFrame::Panel, QFrame::StyledPanel, QFrame::HLine, QFrame::VLine})
		{
			auto* f = new QFrame(box);
			f->setFrameShape(shape);
			f->setFrameShadow(QFrame::Sunken);
			f->setMinimumSize(48, 36);
			if (shape == QFrame::HLine || shape == QFrame::VLine)
			{
				f->setMinimumSize(shape == QFrame::HLine ? 80 : 8, shape == QFrame::HLine ? 8 : 48);
			}
			row->addWidget(f);
		}
		row->addStretch(1);
		static_cast<QVBoxLayout*>(box->layout())->addLayout(row);
		layout->addWidget(box);
	}

	layout->addStretch(1);
	return wrapScroll(root, parent);
}

QWidget* pageContainers(QWidget* parent)
{
	auto* root = new QWidget;
	auto* layout = new QVBoxLayout(root);

	layout->addWidget(new QLabel(
		QStringLiteral("QDockWidget samples are attached to the main window (left / right). "
					   "QScrollArea wraps each gallery page."),
		root));

	{
		auto* box = makeGroup(QStringLiteral("Nested QTabWidget"), root);
		auto* tabs = new QTabWidget(box);
		tabs->addTab(new QLabel(QStringLiteral("Nested tab A"), tabs), QStringLiteral("A"));
		tabs->addTab(new QLabel(QStringLiteral("Nested tab B"), tabs), QStringLiteral("B"));
		tabs->addTab(new QLabel(QStringLiteral("Disabled"), tabs), QStringLiteral("C"));
		tabs->setTabEnabled(2, false);
		static_cast<QVBoxLayout*>(box->layout())->addWidget(tabs);
		layout->addWidget(box);
	}

	{
		auto* box = makeGroup(QStringLiteral("QToolBox"), root);
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
		auto* box = makeGroup(QStringLiteral("QStackedWidget + switcher"), root);
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
		auto* box = makeGroup(QStringLiteral("QSplitter"), root);
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
		auto* box = makeGroup(QStringLiteral("QRubberBand / QSizeGrip / QFocusFrame"), root);
		auto* host = new RubberBandHost(box);
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
	return wrapScroll(root, parent);
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

QWidget* pageItemViews(QWidget* parent)
{
	auto* root = new QWidget;
	auto* layout = new QVBoxLayout(root);

	layout->addWidget(new QLabel(QStringLiteral("Item-based (self-contained)"), root));

	auto* list = new QListWidget(root);
	list->addItems({QStringLiteral("List item 1"), QStringLiteral("List item 2"),
					QStringLiteral("List item 3"), QStringLiteral("Disabled item")});
	list->item(3)->setFlags(list->item(3)->flags() & ~Qt::ItemIsEnabled);
	list->setMaximumHeight(100);
	layout->addWidget(new QLabel(QStringLiteral("QListWidget"), root));
	layout->addWidget(list);

	auto* tree = new QTreeWidget(root);
	tree->setHeaderLabels({QStringLiteral("Name"), QStringLiteral("Value")});
	auto* t1 = new QTreeWidgetItem(tree, {QStringLiteral("Root A"), QStringLiteral("1")});
	new QTreeWidgetItem(t1, {QStringLiteral("Child A1"), QStringLiteral("1.1")});
	new QTreeWidgetItem(t1, {QStringLiteral("Child A2"), QStringLiteral("1.2")});
	auto* t2 = new QTreeWidgetItem(tree, {QStringLiteral("Root B"), QStringLiteral("2")});
	new QTreeWidgetItem(t2, {QStringLiteral("Child B1"), QStringLiteral("2.1")});
	tree->expandAll();
	tree->setMaximumHeight(120);
	layout->addWidget(new QLabel(QStringLiteral("QTreeWidget"), root));
	layout->addWidget(tree);

	auto* table = new QTableWidget(4, 3, root);
	table->setHorizontalHeaderLabels(
		{QStringLiteral("Col A"), QStringLiteral("Col B"), QStringLiteral("Col C")});
	for (int r = 0; r < 4; ++r)
	{
		for (int c = 0; c < 3; ++c)
		{
			table->setItem(r, c, new QTableWidgetItem(QStringLiteral("%1,%2").arg(r).arg(c)));
		}
	}
	table->horizontalHeader()->setStretchLastSection(true);
	table->setMaximumHeight(130);
	layout->addWidget(new QLabel(QStringLiteral("QTableWidget (+ QHeaderView)"), root));
	layout->addWidget(table);

	layout->addWidget(new QLabel(QStringLiteral("Model-based (QAbstractItemView)"), root));

	auto* listModel = new QStringListModel(
		QStringList{QStringLiteral("Model row 1"), QStringLiteral("Model row 2"),
					QStringLiteral("Model row 3")},
		root);
	auto* listView = new QListView(root);
	listView->setModel(listModel);
	listView->setMaximumHeight(90);
	layout->addWidget(new QLabel(QStringLiteral("QListView"), root));
	layout->addWidget(listView);

	auto* treeModel = makeTreeModel(root);
	auto* treeView = new QTreeView(root);
	treeView->setModel(treeModel);
	treeView->expandAll();
	treeView->setMaximumHeight(120);
	layout->addWidget(new QLabel(QStringLiteral("QTreeView"), root));
	layout->addWidget(treeView);

	auto* tableModel = new QStandardItemModel(3, 3, root);
	tableModel->setHorizontalHeaderLabels(
		{QStringLiteral("A"), QStringLiteral("B"), QStringLiteral("C")});
	for (int r = 0; r < 3; ++r)
	{
		for (int c = 0; c < 3; ++c)
		{
			tableModel->setItem(r, c, new QStandardItem(QStringLiteral("m%1%2").arg(r).arg(c)));
		}
	}
	auto* tableView = new QTableView(root);
	tableView->setModel(tableModel);
	tableView->horizontalHeader()->setStretchLastSection(true);
	tableView->setMaximumHeight(120);
	layout->addWidget(new QLabel(QStringLiteral("QTableView"), root));
	layout->addWidget(tableView);

	auto* colModel = new QStandardItemModel(root);
	colModel->setHorizontalHeaderLabels({QStringLiteral("Folder")});
	auto* folder = new QStandardItem(QStringLiteral("Documents"));
	folder->appendRow(new QStandardItem(QStringLiteral("Notes.txt")));
	folder->appendRow(new QStandardItem(QStringLiteral("Todo.md")));
	colModel->invisibleRootItem()->appendRow(folder);
	colModel->invisibleRootItem()->appendRow(new QStandardItem(QStringLiteral("Downloads")));
	auto* columns = new QColumnView(root);
	columns->setModel(colModel);
	columns->setMaximumHeight(110);
	layout->addWidget(new QLabel(QStringLiteral("QColumnView"), root));
	layout->addWidget(columns);

	layout->addStretch(1);
	return wrapScroll(root, parent);
}

QWidget* pageNavigation(QWidget* parent)
{
	auto* root = new QWidget;
	auto* layout = new QVBoxLayout(root);
	layout->addWidget(new QLabel(
		QStringLiteral("Shell chrome (QMainWindow / QMenuBar / QMenu / QToolBar / QStatusBar / "
					   "QWidgetAction) lives on the main window. Right-click for QMenu. "
					   "Hover controls for QToolTip."),
		root));

	root->setContextMenuPolicy(Qt::CustomContextMenu);
	QObject::connect(root, &QWidget::customContextMenuRequested, root,
					 [root](const QPoint& pos)
					 {
						 QMenu menu(root);
						 menu.addAction(QStringLiteral("Context action 1"));
						 menu.addAction(QStringLiteral("Context action 2"));
						 menu.addSeparator();
						 menu.addMenu(QStringLiteral("Submenu"))->addAction(QStringLiteral("Nested"));
						 menu.exec(root->mapToGlobal(pos));
					 });

	{
		auto* box = makeGroup(QStringLiteral("Standalone QTabBar (T0)"), root);
		auto* bar = new QTabBar(box);
		bar->setObjectName(QStringLiteral("gallery.standaloneTabBar"));
		bar->addTab(QStringLiteral("Tab A"));
		bar->addTab(QStringLiteral("Tab B"));
		bar->addTab(QStringLiteral("Tab C"));
		bar->setTabsClosable(true);
		bar->setToolTip(QStringLiteral("Independent QTabBar — Style CE_TabBarTab"));
		static_cast<QVBoxLayout*>(box->layout())->addWidget(bar);

		auto* eastBar = new QTabBar(box);
		eastBar->setShape(QTabBar::RoundedEast);
		eastBar->addTab(QStringLiteral("E1"));
		eastBar->addTab(QStringLiteral("E2"));
		static_cast<QVBoxLayout*>(box->layout())->addWidget(eastBar);
		layout->addWidget(box);
	}

	auto* tipBtn = new QPushButton(QStringLiteral("Show QToolTip::showText"), root);
	QObject::connect(tipBtn, &QPushButton::clicked, tipBtn,
					 [tipBtn]
					 {
						 QToolTip::showText(tipBtn->mapToGlobal(QPoint(8, tipBtn->height())),
											QStringLiteral("QToolTip::showText sample"), tipBtn);
					 });
	layout->addWidget(tipBtn);

	auto* tabs = new QTabWidget(root);
	tabs->setTabPosition(QTabWidget::North);
	tabs->addTab(new QWidget, QStringLiteral("North 1"));
	tabs->addTab(new QWidget, QStringLiteral("North 2"));
	tabs->setMaximumHeight(80);
	layout->addWidget(new QLabel(QStringLiteral("QTabWidget (North)"), root));
	layout->addWidget(tabs);

	auto* south = new QTabWidget(root);
	south->setTabPosition(QTabWidget::South);
	south->addTab(new QLabel(QStringLiteral("South page"), south), QStringLiteral("S1"));
	south->addTab(new QLabel(QStringLiteral("South page 2"), south), QStringLiteral("S2"));
	south->setMaximumHeight(90);
	layout->addWidget(new QLabel(QStringLiteral("QTabWidget (South)"), root));
	layout->addWidget(south);

	auto* west = new QTabWidget(root);
	west->setTabPosition(QTabWidget::West);
	west->addTab(new QLabel(QStringLiteral("West page"), west), QStringLiteral("W1"));
	west->addTab(new QLabel(QStringLiteral("West page 2"), west), QStringLiteral("W2"));
	west->setMinimumHeight(120);
	layout->addWidget(new QLabel(QStringLiteral("QTabWidget (West)"), root));
	layout->addWidget(west);

	auto* east = new QTabWidget(root);
	east->setTabPosition(QTabWidget::East);
	east->addTab(new QLabel(QStringLiteral("East page"), east), QStringLiteral("E1"));
	east->addTab(new QLabel(QStringLiteral("East page 2"), east), QStringLiteral("E2"));
	east->setMinimumHeight(120);
	layout->addWidget(new QLabel(QStringLiteral("QTabWidget (East)"), root));
	layout->addWidget(east);

	layout->addStretch(1);
	return wrapScroll(root, parent);
}

QWidget* pageDialogs(QWidget* parent)
{
	auto* root = new QWidget;
	auto* layout = new QVBoxLayout(root);
	layout->addWidget(new QLabel(
		QStringLiteral("Open stock dialogs to preview modal chrome (palette / buttons)."), root));

	auto addBtn = [&](const QString& text, auto handler)
	{
		auto* b = new QPushButton(text, root);
		QObject::connect(b, &QPushButton::clicked, root, handler);
		return b;
	};

	auto* grid = new QGridLayout;
	int i = 0;
	auto place = [&](QWidget* w)
	{
		grid->addWidget(w, i / 4, i % 4);
		++i;
	};

	place(addBtn(QStringLiteral("QDialog"),
				 [root]
				 {
					 QDialog dlg(root);
					 dlg.setWindowTitle(QStringLiteral("Custom QDialog"));
					 auto* vl = new QVBoxLayout(&dlg);
					 vl->addWidget(new QLabel(QStringLiteral("Sample custom dialog body"), &dlg));
					 auto* box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
					 QObject::connect(box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
					 QObject::connect(box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
					 vl->addWidget(box);
					 dlg.exec();
				 }));
	place(addBtn(QStringLiteral("MessageBox"),
				 [root]
				 {
					 QMessageBox::information(root, QStringLiteral("Info"),
											  QStringLiteral("Sample information message."));
				 }));
	place(addBtn(QStringLiteral("Question"),
				 [root]
				 {
					 QMessageBox::question(root, QStringLiteral("Confirm"),
										   QStringLiteral("Proceed with sample action?"));
				 }));
	place(addBtn(QStringLiteral("Warning"),
				 [root]
				 {
					 QMessageBox::warning(root, QStringLiteral("Warning"),
										  QStringLiteral("Sample warning."));
				 }));
	place(addBtn(QStringLiteral("Critical"),
				 [root]
				 {
					 QMessageBox::critical(root, QStringLiteral("Error"),
										   QStringLiteral("Sample critical error."));
				 }));
	place(addBtn(QStringLiteral("InputDialog"),
				 [root]
				 {
					 QInputDialog::getText(root, QStringLiteral("Input"), QStringLiteral("Name:"));
				 }));
	place(addBtn(QStringLiteral("FileDialog"),
				 [root]
				 {
					 QFileDialog::getOpenFileName(root, QStringLiteral("Open"));
				 }));
	place(addBtn(QStringLiteral("FontDialog"),
				 [root]
				 {
					 bool ok = false;
					 QFontDialog::getFont(&ok, root->font(), root);
				 }));
	place(addBtn(QStringLiteral("ColorDialog"),
				 [root]
				 {
					 QColorDialog::getColor(Qt::white, root, QStringLiteral("Pick color"));
				 }));
	place(addBtn(QStringLiteral("ProgressDialog"),
				 [root]
				 {
					 QProgressDialog dlg(QStringLiteral("Working…"), QStringLiteral("Cancel"), 0, 100,
										 root);
					 dlg.setWindowModality(Qt::WindowModal);
					 dlg.setValue(40);
					 dlg.exec();
				 }));
	place(addBtn(QStringLiteral("ErrorMessage"),
				 [root]
				 {
					 auto* err = new QErrorMessage(root);
					 err->setAttribute(Qt::WA_DeleteOnClose);
					 err->showMessage(QStringLiteral("Sample QErrorMessage (with “Do not show again”)."));
				 }));
	place(addBtn(QStringLiteral("QWizard"),
				 [root]
				 {
					 QWizard wiz(root);
					 wiz.setWindowTitle(QStringLiteral("Sample Wizard"));
					 auto* p1 = new QWizardPage;
					 p1->setTitle(QStringLiteral("Step 1"));
					 auto* l1 = new QVBoxLayout(p1);
					 l1->addWidget(new QLabel(QStringLiteral("First wizard page"), p1));
					 auto* p2 = new QWizardPage;
					 p2->setTitle(QStringLiteral("Step 2"));
					 auto* l2 = new QVBoxLayout(p2);
					 l2->addWidget(new QLabel(QStringLiteral("Second wizard page"), p2));
					 wiz.addPage(p1);
					 wiz.addPage(p2);
					 wiz.exec();
				 }));

#if defined(QTE_HAS_PRINTSUPPORT)
	place(addBtn(QStringLiteral("PrintDialog"),
				 [root]
				 {
					 QPrinter printer;
					 QPrintDialog dlg(&printer, root);
					 dlg.exec();
				 }));
	place(addBtn(QStringLiteral("PageSetup"),
				 [root]
				 {
					 QPrinter printer;
					 QPageSetupDialog dlg(&printer, root);
					 dlg.exec();
				 }));
	place(addBtn(QStringLiteral("PrintPreview"),
				 [root]
				 {
					 QPrinter printer;
					 QPrintPreviewDialog dlg(&printer, root);
					 QObject::connect(&dlg, &QPrintPreviewDialog::paintRequested,
									  [](QPrinter* p)
									  {
										  QPainter painter(p);
										  painter.drawText(100, 100, QStringLiteral("Preview sample"));
									  });
					 dlg.exec();
				 }));
#endif

	layout->addLayout(grid);

	auto* cal = new QCalendarWidget(root);
	cal->setMaximumHeight(220);
	layout->addWidget(new QLabel(QStringLiteral("QCalendarWidget"), root));
	layout->addWidget(cal);

	layout->addStretch(1);
	return wrapScroll(root, parent);
}

QWidget* pageOwnerDraw(QWidget* parent)
{
	auto* root = new QWidget;
	auto* layout = new QVBoxLayout(root);
	layout->addWidget(new QLabel(
		QStringLiteral("Secondary path: owner-drawn widgets read the same ThemeStore via "
					   "qtheme::api (not QStyle)."),
		root));

	auto* drawn = new qtheme::DemoButton(root);
	drawn->setText(QStringLiteral("Owner-draw DemoButton"));
	drawn->setMinimumHeight(40);
	layout->addWidget(drawn);

	auto* native = new QPushButton(QStringLiteral("Native QPushButton (QThemeStyle)"), root);
	native->setMinimumHeight(40);
	layout->addWidget(native);

	layout->addStretch(1);
	return wrapScroll(root, parent);
}

QWidget* pageCoverage(QWidget* parent)
{
	auto* root = new QWidget;
	auto* layout = new QVBoxLayout(root);
	auto* text = new QTextBrowser(root);
	text->setOpenExternalLinks(false);

	QString html = QStringLiteral(
		"<h3>对齐仓库文档（Fluent Pack 架构）</h3>"
		"<p>权威：<code>docs/zh/architecture.md</code> §4.3–4.6 · "
		"进度：<code>coverage-matrix.md</code> · "
		"清单：<code>qt-widgets-inventory.md</code>。</p>"
		"<p><b>主路径</b>：单一 <code>QThemeStyle</code> + 多套 Theme Pack"
		"（<code>fluent.light</code> / <code>fluent.dark</code> / <code>fluent.hc</code>）；"
		"Accent / ColorScheme 为 Engine 策略。禁止 QSS。</p>"
		"<h4>Theme 菜单</h4>"
		"<ul>"
		"<li>Fluent Light / Dark / High Contrast / Follow System</li>"
		"<li>User Sample Pack（<code>base: fluent.light</code> 派生）</li>"
		"<li>Accent: System 或自定义取色</li>"
		"</ul>"
		"<h4>启动自检</h4>"
		"<p><code>gallery::verifyGallerySession</code>：无 stylesheet、QThemeStyle、"
		"必需 Fluent Token、T0 代表控件。命令行：<code>--self-check</code>。</p>");
#if defined(QTE_HAS_OPENGLWIDGETS)
	html += QStringLiteral("<p>已链接 OpenGLWidgets（QOpenGLWidget / QRhiWidget）。</p>");
#endif
#if defined(QTE_HAS_PRINTSUPPORT)
	html += QStringLiteral(
		"<p>已链接 PrintSupport（打印对话框 + QPrintPreviewWidget）。</p>");
#endif

	text->setHtml(html);
	layout->addWidget(text);
	return root;
}

void attachDockWidgets(QMainWindow* window)
{
	if (!window)
	{
		return;
	}

	auto* left = new QDockWidget(QStringLiteral("Left Dock"), window);
	left->setObjectName(QStringLiteral("leftDock"));
	left->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	left->setWidget(new QLabel(QStringLiteral("QDockWidget\n(left)"), left));
	window->addDockWidget(Qt::LeftDockWidgetArea, left);

	auto* right = new QDockWidget(QStringLiteral("Right Dock"), window);
	right->setObjectName(QStringLiteral("rightDock"));
	right->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	auto* list = new QListWidget(right);
	list->addItems({QStringLiteral("Dock item 1"), QStringLiteral("Dock item 2"),
					QStringLiteral("Dock item 3")});
	right->setWidget(list);
	window->addDockWidget(Qt::RightDockWidgetArea, right);
}

bool verifyGallerySession(QApplication* app, QWidget* root, QString* report)
{
	QStringList problems;
	QStringList ok;

	if (!app)
	{
		problems << QStringLiteral("QApplication is null");
	}
	else
	{
		if (!app->styleSheet().isEmpty())
		{
			problems << QStringLiteral("QApplication stylesheet not cleared");
		}
		else
		{
			ok << QStringLiteral("no app stylesheet");
		}

		if (!app->style() || app->style()->objectName() != QLatin1String("QThemeStyle"))
		{
			problems << QStringLiteral("style is not QThemeStyle (got '%1')")
							.arg(app->style() ? app->style()->objectName() : QStringLiteral("<null>"));
		}
		else
		{
			ok << QStringLiteral("QThemeStyle installed");
		}

		if (auto* eng = qtheme::Engine::defaultEngine())
		{
			if (eng->store())
			{
				const QStringList missing = eng->store()->missingRequiredColors();
				if (!missing.isEmpty())
				{
					problems << QStringLiteral("missing tokens: %1").arg(missing.join(QLatin1Char(',')));
				}
				else
				{
					ok << QStringLiteral("required Fluent tokens");
				}
				ok << QStringLiteral("skin=%1").arg(eng->currentSkin());
			}
		}
		else
		{
			problems << QStringLiteral("Engine::defaultEngine is null");
		}
	}

	if (!root)
	{
		problems << QStringLiteral("root widget is null");
	}
	else
	{
		const auto require = [&](const char* label, bool present)
		{
			if (present)
			{
				ok << QString::fromUtf8(label);
			}
			else
			{
				problems << QStringLiteral("missing %1").arg(QString::fromUtf8(label));
			}
		};

		require("QPushButton", !root->findChildren<QPushButton*>().isEmpty());
		require("QToolButton", !root->findChildren<QToolButton*>().isEmpty());
		require("QCheckBox", !root->findChildren<QCheckBox*>().isEmpty());
		require("QRadioButton", !root->findChildren<QRadioButton*>().isEmpty());
		require("QLineEdit", !root->findChildren<QLineEdit*>().isEmpty());
		require("QComboBox", !root->findChildren<QComboBox*>().isEmpty());
		require("QScrollBar", !root->findChildren<QScrollBar*>().isEmpty());
		require("QTabBar", !root->findChildren<QTabBar*>().isEmpty());
		require("QTabWidget", !root->findChildren<QTabWidget*>().isEmpty());
		require("QHeaderView", !root->findChildren<QHeaderView*>().isEmpty());
		require("QMenuBar", !root->findChildren<QMenuBar*>().isEmpty());
		require("QToolBar", !root->findChildren<QToolBar*>().isEmpty());
		require("QStatusBar", !root->findChildren<QStatusBar*>().isEmpty());
		require("QDockWidget", !root->findChildren<QDockWidget*>().isEmpty());
		require("standalone QTabBar",
				root->findChild<QTabBar*>(QStringLiteral("gallery.standaloneTabBar")) != nullptr);

		bool dirtySs = false;
		for (QWidget* w : root->findChildren<QWidget*>())
		{
			if (w && !w->styleSheet().isEmpty())
			{
				dirtySs = true;
				break;
			}
		}
		if (dirtySs)
		{
			problems << QStringLiteral("widget stylesheet present under root");
		}
		else
		{
			ok << QStringLiteral("no widget stylesheets under root");
		}
	}

	if (report)
	{
		if (problems.isEmpty())
		{
			*report = QStringLiteral("OK (%1 checks)").arg(ok.size());
		}
		else
		{
			*report = QStringLiteral("FAIL: %1").arg(problems.join(QStringLiteral("; ")));
		}
	}
	return problems.isEmpty();
}

} // namespace gallery
