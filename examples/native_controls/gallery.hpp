#pragma once

#include <QString>
#include <QWidget>

class QApplication;
class QMainWindow;

namespace gallery {

[[nodiscard]] QWidget* pageButtons(QWidget* parent = nullptr);
[[nodiscard]] QWidget* pageInput(QWidget* parent = nullptr);
[[nodiscard]] QWidget* pageDisplay(QWidget* parent = nullptr);
[[nodiscard]] QWidget* pageContainers(QWidget* parent = nullptr);
[[nodiscard]] QWidget* pageItemViews(QWidget* parent = nullptr);
[[nodiscard]] QWidget* pageMdi(QWidget* parent = nullptr);
[[nodiscard]] QWidget* pageNavigation(QWidget* parent = nullptr);
[[nodiscard]] QWidget* pageDialogs(QWidget* parent = nullptr);
[[nodiscard]] QWidget* pageOwnerDraw(QWidget* parent = nullptr);
[[nodiscard]] QWidget* pageCoverage(QWidget* parent = nullptr);

void attachDockWidgets(QMainWindow* window);

/// Runtime self-check vs docs/zh/qt-widgets-inventory.md (T0 presence + no QSS).
[[nodiscard]] bool verifyGallerySession(QApplication* app, QWidget* root, QString* report);

} // namespace gallery
