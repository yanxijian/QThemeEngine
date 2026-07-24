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
