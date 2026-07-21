#pragma once

#include <QColor>
#include <QString>

namespace qtheme {

[[nodiscard]] QColor parseColorLiteral(const QString& s);
[[nodiscard]] QColor colorFromRgbaHex(const QString& hex);
[[nodiscard]] bool hasUtf8Bom(const QByteArray& bytes);

} // namespace qtheme
