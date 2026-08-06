#ifndef __QTHEME_DEMO_GALLERY_H__
#define __QTHEME_DEMO_GALLERY_H__

#include <QString>
#include <QWidget>

class QApplication;
class QMainWindow;

namespace gallery
{
	[[nodiscard]] QWidget* tabButtons(QWidget* parent = nullptr);
	[[nodiscard]] QWidget* tabInput(QWidget* parent = nullptr);
	[[nodiscard]] QWidget* tabStates(QWidget* parent = nullptr);
	[[nodiscard]] QWidget* tabDisplay(QWidget* parent = nullptr);
	[[nodiscard]] QWidget* tabContainers(QWidget* parent = nullptr);
	[[nodiscard]] QWidget* tabItemViews(QWidget* parent = nullptr);
	[[nodiscard]] QWidget* tabMdi(QWidget* parent = nullptr);
	[[nodiscard]] QWidget* tabNavigation(QWidget* parent = nullptr);
	[[nodiscard]] QWidget* tabDialogs(QWidget* parent = nullptr);
	[[nodiscard]] QWidget* tabOwnerDraw(QWidget* parent = nullptr);
	[[nodiscard]] QWidget* tabCoverage(QWidget* parent = nullptr);

	void attachDockWidgets(QMainWindow* window);

	/// Runtime self-check vs docs/zh/qt-widgets-inventory.md (T0 presence + no QSS).
	[[nodiscard]] bool verifyGalleryCoverage(QApplication* app, QWidget* root, QString* report);
} // namespace gallery

#endif // __QTHEME_DEMO_GALLERY_H__
