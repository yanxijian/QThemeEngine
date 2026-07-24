#ifndef __QTHEME_DEMO_GALLERY_INTERNAL_H__
#define __QTHEME_DEMO_GALLERY_INTERNAL_H__

#include <QGroupBox>
#include <QStandardItemModel>
#include <QString>
#include <QWidget>

namespace gallery {
namespace detail {

[[nodiscard]] QWidget* wrapScroll(QWidget* content, QWidget* parent);
[[nodiscard]] QGroupBox* makeGroup(const QString& title, QWidget* parent);
[[nodiscard]] QStandardItemModel* makeTreeModel(QObject* parent);

[[nodiscard]] QWidget* createRubberBandHost(QWidget* parent);

#if defined(QTE_HAS_OPENGLWIDGETS)
[[nodiscard]] QWidget* createSampleGlWidget(QWidget* parent);
[[nodiscard]] QWidget* createSampleRhiWidget(QWidget* parent);
#endif

} // namespace detail
} // namespace gallery

#endif
