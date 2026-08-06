#include "gallery.hpp"
#include "qtheme/engine.hpp"
#include "qtheme/types.hpp"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QColor>
#include <QColorDialog>
#include <QCoreApplication>
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
	QCoreApplication::setOrganizationName(QStringLiteral("yanxijian"));
	QCoreApplication::setApplicationName(QStringLiteral("QThemeEngineGallery"));

	QPixmap splashPm(420, 180);
	splashPm.fill(QColor(QStringLiteral("#0078D4")));
	QSplashScreen splash(splashPm);
	splash.showMessage(QStringLiteral("QThemeEngine — Fluent packs\nThemeStore + QThemeStyle (no QSS)"), Qt::AlignCenter | Qt::AlignBottom,
					   Qt::white);
	splash.show();
	app.processEvents();

	qtheme::Engine engine;
	engine.apply(&app);
	(void)engine.loadPreferences();
	engine.setAutoSavePreferences(true);
	QObject::connect(&app, &QCoreApplication::aboutToQuit, &engine,
					 [&engine]
					 {
						 (void)engine.savePreferences();
					 });

	const bool selfCheckOnly = app.arguments().contains(QStringLiteral("--self-check"));

	QMainWindow window;
	window.setWindowTitle(QStringLiteral("QThemeEngine — Fluent Control Gallery"));

	auto* themeMenu = window.menuBar()->addMenu(QStringLiteral("&Theme"));
	auto* schemeGroup = new QActionGroup(&window);
	schemeGroup->setExclusive(true);

	auto* actLight = themeMenu->addAction(QStringLiteral("Fluent &Light"));
	actLight->setCheckable(true);
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

	auto syncSchemeActions = [&]()
	{
		switch (engine.colorScheme())
		{
		case qtheme::ColorScheme::Dark:
			actDark->setChecked(true);
			break;
		case qtheme::ColorScheme::HighContrast:
			actHc->setChecked(true);
			break;
		case qtheme::ColorScheme::System:
			actSystem->setChecked(true);
			break;
		case qtheme::ColorScheme::Light:
		default:
			actLight->setChecked(true);
			break;
		}
	};
	syncSchemeActions();
	QObject::connect(&engine, &qtheme::Engine::colorSchemeChanged, &window, syncSchemeActions);
	QObject::connect(&engine, &qtheme::Engine::packChanged, &window,
					 [&](const QString&, const QString&)
					 {
						 syncSchemeActions();
					 });

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
						 engine.switchPack(QStringLiteral("user.sample"));
					 });
	QObject::connect(actAccentSys, &QAction::triggered, &engine,
					 [&engine]
					 {
						 engine.setAccentFollowSystem(true);
					 });
	QObject::connect(actAccentPick, &QAction::triggered, &window,
					 [&engine, &window]
					 {
						 const QColor c = QColorDialog::getColor(engine.accent(), &window, QStringLiteral("Accent color"));
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
	tabs->addTab(gallery::tabButtons(tabs), QStringLiteral("Buttons"));
	tabs->addTab(gallery::tabInput(tabs), QStringLiteral("Input"));
	tabs->addTab(gallery::tabStates(tabs), QStringLiteral("States"));
	tabs->addTab(gallery::tabDisplay(tabs), QStringLiteral("Display"));
	tabs->addTab(gallery::tabContainers(tabs), QStringLiteral("Containers"));
	tabs->addTab(gallery::tabMdi(tabs), QStringLiteral("MDI"));
	tabs->addTab(gallery::tabItemViews(tabs), QStringLiteral("Item Views"));
	tabs->addTab(gallery::tabNavigation(tabs), QStringLiteral("Tabs / Nav"));
	tabs->addTab(gallery::tabDialogs(tabs), QStringLiteral("Dialogs"));
	tabs->addTab(gallery::tabOwnerDraw(tabs), QStringLiteral("Owner-draw"));
	tabs->addTab(gallery::tabCoverage(tabs), QStringLiteral("Coverage"));
	window.setCentralWidget(tabs);

	auto* permanent = new QLabel(QStringLiteral("Fluent"), &window);
	window.statusBar()->addPermanentWidget(permanent);

	QString checkReport;
	const bool checkOk = gallery::verifyGalleryCoverage(&app, &window, &checkReport);
	window.statusBar()->showMessage(QStringLiteral("Pack: %1 | %2").arg(engine.currentPack(), checkReport));
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
		const bool ok = gallery::verifyGalleryCoverage(&app, &window, &report);
		Q_UNUSED(ok);
		window.statusBar()->showMessage(
			QStringLiteral("Pack: %1 | accent: %2 | %3").arg(engine.currentPack(), engine.accent().name(), report));
		permanent->setText(engine.currentPack());
	};

	QObject::connect(&engine, &qtheme::Engine::packChanged, &window,
					 [&](const QString&, const QString& cur)
					 {
						 syncStatus(cur);
					 });
	QObject::connect(&engine, &qtheme::Engine::accentChanged, &window,
					 [&](const QColor&)
					 {
						 syncStatus(engine.currentPack());
					 });

	window.resize(1100, 720);
	window.show();
	splash.finish(&window);
	return app.exec();
}
