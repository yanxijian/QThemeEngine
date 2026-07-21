#pragma once

#include <QColor>
#include <QString>

namespace qtheme {

/// Qt dynamic property for owner-drawn widgets (secondary path).
inline constexpr char kClassProperty[] = "qtheme.class";

struct ColorValue
{
	QColor value;
	bool ok = false;
};

enum class Error
{
	None = 0,
	FileNotFound,
	InvalidEncoding,
	XmlParseError,
	NoMatchingTheme,
	SkinNotFound,
};

} // namespace qtheme
