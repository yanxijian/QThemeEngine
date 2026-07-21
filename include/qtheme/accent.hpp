#pragma once

#include <QColor>

class QGuiApplication;

namespace qtheme {

class ThemeStore;

/// Resolve OS accent / high-contrast and patch ThemeStore accent-related roles.
namespace AccentResolver {

[[nodiscard]] QColor systemAccent(const QGuiApplication* app = nullptr);
[[nodiscard]] bool systemHighContrast();
[[nodiscard]] QColor lighten(const QColor& c, int delta = 24);
[[nodiscard]] QColor darken(const QColor& c, int delta = 24);

/// Patch accent / highlight / focus borders / checked indicators derived from accent.
void applyAccentPatch(ThemeStore* store, const QColor& accent);

} // namespace AccentResolver

} // namespace qtheme
