#ifndef __QTHEME_ENGINE_ACCENT_H__
#define __QTHEME_ENGINE_ACCENT_H__

#include <QColor>
#include <qtheme/qtheme_export.hpp>

class QGuiApplication;

namespace qtheme
{
	class ThemeStore;

	/// Resolve OS accent / high-contrast and patch ThemeStore accent-related roles.
	namespace AccentResolver
	{
		[[nodiscard]] QTE_EXPORT QColor systemAccent(const QGuiApplication* app = nullptr);
		[[nodiscard]] QTE_EXPORT bool systemHighContrast();
		[[nodiscard]] QTE_EXPORT QColor lighten(const QColor& c, int delta = 24);
		[[nodiscard]] QTE_EXPORT QColor darken(const QColor& c, int delta = 24);

		/// Patch accent / highlight / focus borders / checked indicators derived from accent.
		QTE_EXPORT void applyAccentPatch(ThemeStore* store, const QColor& accent);
	} // namespace AccentResolver
} // namespace qtheme

#endif // __QTHEME_ENGINE_ACCENT_H__
