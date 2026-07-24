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

QWidget* pageButtons(QWidget* parent)
{
	auto* root = new QWidget;
	auto* layout = new QVBoxLayout(root);

	{
		auto* box = detail::makeGroup(QStringLiteral("Push / Tool / Command"), root);
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
		auto* box = detail::makeGroup(QStringLiteral("Check / Radio"), root);
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
		auto* box = detail::makeGroup(QStringLiteral("Dialog button box"), root);
		auto* buttons = new QDialogButtonBox(
			QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply
				| QDialogButtonBox::Help,
			box);
		static_cast<QVBoxLayout*>(box->layout())->addWidget(buttons);
		layout->addWidget(box);
	}

	layout->addStretch(1);
	return detail::wrapScroll(root, parent);
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

	auto* plainDis = new QPlainTextEdit(root);
	plainDis->setPlainText(QStringLiteral("Disabled multi-line"));
	plainDis->setEnabled(false);
	plainDis->setMaximumHeight(60);
	form->addRow(QStringLiteral("QPlainTextEdit disabled"), plainDis);

	auto* rich = new QTextEdit(root);
	rich->setHtml(QStringLiteral("<b>QTextEdit</b> with <i>rich</i> text"));
	rich->setMaximumHeight(80);
	form->addRow(QStringLiteral("QTextEdit"), rich);

	return detail::wrapScroll(root, parent);
}

QWidget* pageStates(QWidget* parent)
{
	auto* root = new QWidget;
	auto* layout = new QVBoxLayout(root);

	layout->addWidget(new QLabel(
		QStringLiteral("Enabled vs Disabled / Default — Fluent state acceptance"), root));

	{
		auto* box = detail::makeGroup(QStringLiteral("Buttons"), root);
		auto* grid = new QGridLayout;
		grid->addWidget(new QLabel(QStringLiteral("Enabled"), box), 0, 1, Qt::AlignCenter);
		grid->addWidget(new QLabel(QStringLiteral("Disabled"), box), 0, 2, Qt::AlignCenter);

		grid->addWidget(new QLabel(QStringLiteral("QPushButton"), box), 1, 0);
		grid->addWidget(new QPushButton(QStringLiteral("Normal"), box), 1, 1);
		auto* disBtn = new QPushButton(QStringLiteral("Normal"), box);
		disBtn->setEnabled(false);
		grid->addWidget(disBtn, 1, 2);

		grid->addWidget(new QLabel(QStringLiteral("DefaultButton"), box), 2, 0);
		auto* def = new QPushButton(QStringLiteral("OK"), box);
		def->setDefault(true);
		grid->addWidget(def, 2, 1);
		auto* defDis = new QPushButton(QStringLiteral("OK"), box);
		defDis->setDefault(true);
		defDis->setEnabled(false);
		grid->addWidget(defDis, 2, 2);

		grid->addWidget(new QLabel(QStringLiteral("QToolButton"), box), 3, 0);
		auto* tool = new QToolButton(box);
		tool->setText(QStringLiteral("Tool"));
		grid->addWidget(tool, 3, 1);
		auto* toolDis = new QToolButton(box);
		toolDis->setText(QStringLiteral("Tool"));
		toolDis->setEnabled(false);
		grid->addWidget(toolDis, 3, 2);

		static_cast<QVBoxLayout*>(box->layout())->addLayout(grid);
		layout->addWidget(box);
	}

	{
		auto* box = detail::makeGroup(QStringLiteral("Check / Radio"), root);
		auto* row = new QHBoxLayout;
		auto* cOn = new QCheckBox(QStringLiteral("Checked"), box);
		cOn->setChecked(true);
		auto* cOff = new QCheckBox(QStringLiteral("Unchecked"), box);
		auto* cDis = new QCheckBox(QStringLiteral("Checked+Disabled"), box);
		cDis->setChecked(true);
		cDis->setEnabled(false);
		auto* rOn = new QRadioButton(QStringLiteral("Selected"), box);
		rOn->setChecked(true);
		auto* rDis = new QRadioButton(QStringLiteral("Disabled"), box);
		rDis->setEnabled(false);
		row->addWidget(cOn);
		row->addWidget(cOff);
		row->addWidget(cDis);
		row->addWidget(rOn);
		row->addWidget(rDis);
		row->addStretch(1);
		static_cast<QVBoxLayout*>(box->layout())->addLayout(row);
		layout->addWidget(box);
	}

	{
		auto* box = detail::makeGroup(QStringLiteral("Input"), root);
		auto* form = new QFormLayout;
		auto* line = new QLineEdit(QStringLiteral("Editable"), box);
		auto* lineDis = new QLineEdit(QStringLiteral("Disabled"), box);
		lineDis->setEnabled(false);
		form->addRow(QStringLiteral("QLineEdit"), line);
		form->addRow(QStringLiteral("QLineEdit disabled"), lineDis);

		auto* combo = new QComboBox(box);
		combo->addItems({QStringLiteral("One"), QStringLiteral("Two")});
		auto* comboDis = new QComboBox(box);
		comboDis->addItems({QStringLiteral("One"), QStringLiteral("Two")});
		comboDis->setEnabled(false);
		form->addRow(QStringLiteral("QComboBox"), combo);
		form->addRow(QStringLiteral("QComboBox disabled"), comboDis);

		auto* spin = new QSpinBox(box);
		spin->setValue(7);
		auto* spinDis = new QSpinBox(box);
		spinDis->setValue(7);
		spinDis->setEnabled(false);
		form->addRow(QStringLiteral("QSpinBox"), spin);
		form->addRow(QStringLiteral("QSpinBox disabled"), spinDis);

		auto* dspin = new QDoubleSpinBox(box);
		dspin->setValue(1.25);
		auto* dspinDis = new QDoubleSpinBox(box);
		dspinDis->setValue(1.25);
		dspinDis->setEnabled(false);
		form->addRow(QStringLiteral("QDoubleSpinBox"), dspin);
		form->addRow(QStringLiteral("QDoubleSpinBox disabled"), dspinDis);

		static_cast<QVBoxLayout*>(box->layout())->addLayout(form);
		layout->addWidget(box);
	}

	{
		auto* box = detail::makeGroup(QStringLiteral("TabBar"), root);
		auto* bar = new QTabBar(box);
		bar->addTab(QStringLiteral("Active"));
		bar->addTab(QStringLiteral("Other"));
		const int dis = bar->addTab(QStringLiteral("Disabled"));
		bar->setTabEnabled(dis, false);
		static_cast<QVBoxLayout*>(box->layout())->addWidget(bar);
		layout->addWidget(box);
	}

	layout->addStretch(1);
	return detail::wrapScroll(root, parent);
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
		auto* box = detail::makeGroup(QStringLiteral("Progress"), root);
		auto* row = new QHBoxLayout;
		auto* p1 = new QProgressBar(box);
		p1->setValue(35);
		auto* p2 = new QProgressBar(box);
		p2->setRange(0, 0);
		auto* pPaused = new QProgressBar(box);
		pPaused->setValue(55);
		pPaused->setFormat(QStringLiteral("Paused %p%"));
		pPaused->setProperty("qtheme.progressState", QStringLiteral("paused"));
		auto* pError = new QProgressBar(box);
		pError->setValue(40);
		pError->setFormat(QStringLiteral("Error %p%"));
		pError->setProperty("qtheme.progressState", QStringLiteral("error"));
		auto* p3 = new QProgressBar(box);
		p3->setOrientation(Qt::Vertical);
		p3->setValue(70);
		p3->setMinimumHeight(80);
		row->addWidget(p1, 1);
		row->addWidget(p2, 1);
		row->addWidget(pPaused, 1);
		row->addWidget(pError, 1);
		row->addWidget(p3);
		static_cast<QVBoxLayout*>(box->layout())->addLayout(row);
		layout->addWidget(box);
	}

	{
		auto* box = detail::makeGroup(QStringLiteral("Slider / Dial / ScrollBar"), root);
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
		auto* box = detail::makeGroup(QStringLiteral("QUndoView"), root);
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
		auto* box = detail::makeGroup(QStringLiteral("QGraphicsView"), root);
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
		auto* box = detail::makeGroup(QStringLiteral("GPU canvases"), root);
		auto* row = new QHBoxLayout;
#if defined(QTE_HAS_OPENGLWIDGETS)
		auto* gl = detail::createSampleGlWidget(box);
		gl->setMinimumSize(160, 100);
		auto* rhi = detail::createSampleRhiWidget(box);
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
		auto* box = detail::makeGroup(QStringLiteral("QPrintPreviewWidget (PrintSupport)"), root);
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
		auto* box = detail::makeGroup(QStringLiteral("Frames"), root);
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
	return detail::wrapScroll(root, parent);
}


} // namespace gallery
