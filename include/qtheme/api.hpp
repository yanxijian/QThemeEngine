#ifndef __QTHEME_ENGINE_API_H__
#define __QTHEME_ENGINE_API_H__

#include "types.hpp"

#include <QColor>
#include <QObject>
#include <QString>
#include <QStyleOption>

namespace qtheme {

class Engine;

/// Thin facade for owner-drawn widgets (secondary path). Reads Engine::defaultEngine().
namespace api {

void bind(Engine* engine);
Engine* engine();
QObject* engineObject();

QColor color(const QString& group, const QString& role, const QColor& def = QColor());
int metric(const QString& group, const QString& role, int def = 0);

QString className(const QObject* obj);
void setClassName(QObject* obj, const QString& name);

QString roleWithState(const QString& baseRole, const QStyleOption* option);

} // namespace api

} // namespace qtheme

#endif  // __QTHEME_ENGINE_API_H__
