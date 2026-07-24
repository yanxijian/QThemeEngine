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
	return detail::wrapScroll(root, parent);
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


} // namespace gallery
