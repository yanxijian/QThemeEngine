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
	listView->setAlternatingRowColors(true);
	listView->setMouseTracking(true);
	listView->setMaximumHeight(90);
	layout->addWidget(new QLabel(QStringLiteral("QListView"), root));
	layout->addWidget(listView);

	auto* treeModel = detail::makeTreeModel(root);
	auto* treeView = new QTreeView(root);
	treeView->setModel(treeModel);
	treeView->expandAll();
	treeView->setAlternatingRowColors(true);
	treeView->setMouseTracking(true);
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
	tableView->setAlternatingRowColors(true);
	tableView->setMouseTracking(true);
	tableView->setShowGrid(true);
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
	return detail::wrapScroll(root, parent);
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
		auto* box = detail::makeGroup(QStringLiteral("Standalone QTabBar (T0)"), root);
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
	return detail::wrapScroll(root, parent);
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
	return detail::wrapScroll(root, parent);
}


} // namespace gallery
