#ifndef __QTHEME_ENGINE_QTHEME_COLOR_UTIL_H__
#define __QTHEME_ENGINE_QTHEME_COLOR_UTIL_H__

#include <QColor>
#include <QString>

namespace qtheme {

[[nodiscard]] QColor parseColorLiteral(const QString& s);
[[nodiscard]] QColor colorFromRgbaHex(const QString& hex);
[[nodiscard]] bool hasUtf8Bom(const QByteArray& bytes);

} // namespace qtheme

#endif  // __QTHEME_ENGINE_QTHEME_COLOR_UTIL_H__
