#pragma once

#include <QColor>
#include <QString>

namespace theme
{
	/// Parse #RRGGBB or #RRGGBBAA (not Qt's #AARRGGBB).
	[[nodiscard]] QColor parseColorLiteral(const QString& s);

	/// Same rules as parseColorLiteral; for Golden tests.
	[[nodiscard]] QColor themeColorFromRgbaHex(const QString& hex);

	[[nodiscard]] bool hasUtf8Bom(const QByteArray& bytes);
} // namespace theme
