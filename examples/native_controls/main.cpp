#include "gallery.hpp"

#include "qtheme/engine.hpp"
#include "qtheme/types.hpp"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QColor>
#include <QColorDialog>
#include <QDockWidget>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QPixmap>
#include <QSplashScreen>
#include <QStatusBar>
#include <QTabWidget>
#include <QToolBar>
#include <QWhatsThis>
#include <QWidgetAction>
#include <QtGlobal>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	QPixmap splashPm(420, 180);
	splashPm.fill(QColor(QStringLiteral("#0078D4")));
	QSplashScreen splash(splashPm);
	splash.showMessage(QStringLiteral("QThemeEngine — Fluent packs\nThemeStore + QThemeStyle (no QSS)"),
					   Qt::AlignCenter | Qt::AlignBottom, Qt::white);
	splash.show();
	app.processEvents();

	qtheme::Engine engine;
	engine.apply(&app);

	const bool selfCheckOnly = app.arguments().contains(QStringLiteral("--self-check"));

	QMainWindow window;
	window.setWindowTitle(QStringLiteral("QThemeEngine — Fluent Control Gallery"));

	auto* themeMenu = window.menuBar()->addMenu(QStringLiteral("&Theme"));
	auto* schemeGroup = new QActionGroup(&window);
	schemeGroup->setExclusive(true);

	auto* actLight = themeMenu->addAction(QStringLiteral("Fluent &Light"));
	actLight->setCheckable(true);
	actLight->setChecked(true);
	schemeGroup->addAction(actLight);
	auto* actDark = themeMenu->addAction(QStringLiteral("Fluent &Dark"));
	actDark->setCheckable(true);
	schemeGroup->addAction(actDark);
	auto* actHc = themeMenu->addAction(QStringLiteral("Fluent &High Contrast"));
	actHc->setCheckable(true);
	schemeGroup->addAction(actHc);
	auto* actSystem = themeMenu->addAction(QStringLiteral("Follow &System"));
	actSystem->setCheckable(true);
	schemeGroup->addAction(actSystem);

	themeMenu->addSeparator();
	auto* actUser = themeMenu->addAction(QStringLiteral("User &Sample Pack"));
	themeMenu->addSeparator();
	auto* actAccentSys = themeMenu->addAction(QStringLiteral("Accent: &System"));
	auto* actAccentPick = themeMenu->addAction(QStringLiteral("Accent: &Custom…"));

	QObject::connect(actLight, &QAction::triggered, &engine,
					 [&engine]
					 {
						 engine.setColorScheme(qtheme::ColorScheme::Light);
					 });
	QObject::connect(actDark, &QAction::triggered, &engine,
					 [&engine]
					 {
						 engine.setColorScheme(qtheme::ColorScheme::Dark);
					 });
	QObject::connect(actHc, &QAction::triggered, &engine,
					 [&engine]
					 {
						 engine.setColorScheme(qtheme::ColorScheme::HighContrast);
					 });
	QObject::connect(actSystem, &QAction::triggered, &engine,
					 [&engine]
					 {
						 engine.setColorScheme(qtheme::ColorScheme::System);
					 });
	QObject::connect(actUser, &QAction::triggered, &engine,
					 [&engine]
					 {
						 engine.switchSkin(QStringLiteral("user.sample"));
					 });
	QObject::connect(actAccentSys, &QAction::triggered, &engine,
					 [&engine]
					 {
						 engine.setAccentFollowSystem(true);
					 });
	QObject::connect(actAccentPick, &QAction::triggered, &window,
					 [&engine, &window]
					 {
						 const QColor c = QColorDialog::getColor(engine.accent(), &window,
																QStringLiteral("Accent color"));
						 if (c.isValid())
						 {
							 engine.setAccent(c);
						 }
					 });

	auto* fileMenu = window.menuBar()->addMenu(QStringLiteral("&File"));
	fileMenu->addAction(QStringLiteral("E&xit"), &window, &QWidget::close);

	auto* viewMenu = window.menuBar()->addMenu(QStringLiteral("&View"));

	auto* helpMenu = window.menuBar()->addMenu(QStringLiteral("&Help"));
	helpMenu->addAction(QWhatsThis::createAction(&window));
	helpMenu->addAction(QStringLiteral("About Qt"), &app, &QApplication::aboutQt);

	auto* toolbar = window.addToolBar(QStringLiteral("Theme"));
	toolbar->setMovable(true);
	toolbar->addAction(actLight);
	toolbar->addAction(actDark);
	toolbar->addAction(actHc);
	toolbar->addSeparator();
	toolbar->addAction(actUser);
	toolbar->addSeparator();
	toolbar->addAction(actAccentSys);
	toolbar->addAction(actAccentPick);

	auto* search = new QLineEdit;
	search->setPlaceholderText(QStringLiteral("QWidgetAction…"));
	search->setClearButtonEnabled(true);
	search->setMaximumWidth(160);
	auto* widgetAction = new QWidgetAction(toolbar);
	widgetAction->setDefaultWidget(search);
	toolbar->addAction(widgetAction);

	gallery::attachDockWidgets(&window);
	for (auto* dock : window.findChildren<QDockWidget*>())
	{
		viewMenu->addAction(dock->toggleViewAction());
	}

	auto* tabs = new QTabWidget(&window);
	tabs->addTab(gallery::pageButtons(tabs), QStringLiteral("Buttons"));
	tabs->addTab(gallery::pageInput(tabs), QStringLiteral("Input"));
	tabs->addTab(gallery::pageStates(tabs), QStringLiteral("States"));
	tabs->addTab(gallery::pageDisplay(tabs), QStringLiteral("Display"));
	tabs->addTab(gallery::pageContainers(tabs), QStringLiteral("Containers"));
	tabs->addTab(gallery::pageMdi(tabs), QStringLiteral("MDI"));
	tabs->addTab(gallery::pageItemViews(tabs), QStringLiteral("Item Views"));
	tabs->addTab(gallery::pageNavigation(tabs), QStringLiteral("Tabs / Nav"));
	tabs->addTab(gallery::pageDialogs(tabs), QStringLiteral("Dialogs"));
	tabs->addTab(gallery::pageOwnerDraw(tabs), QStringLiteral("Owner-draw"));
	tabs->addTab(gallery::pageCoverage(tabs), QStringLiteral("Coverage"));
	window.setCentralWidget(tabs);

	auto* permanent = new QLabel(QStringLiteral("Fluent"), &window);
	window.statusBar()->addPermanentWidget(permanent);

	QString checkReport;
	const bool checkOk = gallery::verifyGallerySession(&app, &window, &checkReport);
	window.statusBar()->showMessage(
		QStringLiteral("Skin: %1 | %2").arg(engine.currentSkin(), checkReport));
	if (!checkOk)
	{
		qWarning("gallery self-check failed: %s", qPrintable(checkReport));
	}

	if (selfCheckOnly)
	{
		splash.hide();
		return checkOk ? 0 : 1;
	}

	auto syncStatus = [&](const QString&)
	{
		QString report;
		const bool ok = gallery::verifyGallerySession(&app, &window, &report);
		Q_UNUSED(ok);
		window.statusBar()->showMessage(
			QStringLiteral("Skin: %1 | accent: %2 | %3")
				.arg(engine.currentSkin(), engine.accent().name(), report));
		permanent->setText(engine.currentSkin());
	};

	QObject::connect(&engine, &qtheme::Engine::skinChanged, &window,
					 [&](const QString&, const QString& cur)
					 {
						 syncStatus(cur);
					 });
	QObject::connect(&engine, &qtheme::Engine::accentChanged, &window,
					 [&](const QColor&)
					 {
						 syncStatus(engine.currentSkin());
					 });

	window.resize(1100, 720);
	window.show();
	splash.finish(&window);
	return app.exec();
}
