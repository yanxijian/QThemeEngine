#pragma once

#include <QColor>
#include <QString>

namespace qtheme {

/// Qt dynamic property for owner-drawn widgets (secondary path).
inline constexpr char kClassProperty[] = "qtheme.class";

inline constexpr char kPackFluentLight[] = "fluent.light";
inline constexpr char kPackFluentDark[] = "fluent.dark";
inline constexpr char kPackFluentHc[] = "fluent.hc";

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
	JsonParseError,
	NoMatchingTheme,
	SkinNotFound,
	PackInvalid,
	PackMissingTokens,
};

enum class ColorScheme
{
	Light = 0,
	Dark,
	HighContrast,
	System,
};

} // namespace qtheme
