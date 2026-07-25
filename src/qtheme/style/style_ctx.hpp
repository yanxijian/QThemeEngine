#ifndef __QTHEME_ENGINE_STYLE_CTX_H__
#define __QTHEME_ENGINE_STYLE_CTX_H__

#include "qtheme/store.hpp"

#include <QColor>
#include <QString>

namespace qtheme::style_detail
{
	/// Shared ThemeStore + DPI access for QThemeStyle paint/metrics helpers.
	struct StyleCtx
	{
		const ThemeStore* store = nullptr;
		qreal dpiScale = 1.0;

		[[nodiscard]] QColor roleColor(const QString& group, const QString& role, const QColor& fallback) const;
		[[nodiscard]] int scaleMetric(int logicalPx) const;
		[[nodiscard]] int roleMetric(const QString& group, const QString& role, int fallback) const;
		void focusStrokeColors(const QColor& ambient, QColor* outer, QColor* inner) const;
	};
} // namespace qtheme::style_detail

#endif // __QTHEME_ENGINE_STYLE_CTX_H__
