#include "style/style_ctx.hpp"

#include "qtheme/color_util.hpp"

#include <QtMath>

namespace qtheme::style_detail
{
	QColor StyleCtx::roleColor(const QString& group, const QString& role, const QColor& fallback) const
	{
		if (!store)
		{
			return fallback;
		}
		if (cache)
		{
			cache->ensure(store);
			QColor hit;
			if (cache->tryColor(group, role, &hit))
			{
				return hit;
			}
		}
		const ColorValue cv = store->color(group, role, fallback);
		const QColor out = cv.ok ? cv.value : fallback;
		if (cache)
		{
			cache->putColor(group, role, out);
		}
		return out;
	}

	int StyleCtx::scaleMetric(int logicalPx) const
	{
		if (logicalPx <= 0)
		{
			return logicalPx;
		}
		return qRound(logicalPx * dpiScale);
	}

	int StyleCtx::roleMetric(const QString& group, const QString& role, int fallback) const
	{
		if (!store)
		{
			return scaleMetric(fallback);
		}
		if (cache)
		{
			cache->ensure(store);
			int hit = 0;
			if (cache->tryMetric(group, role, &hit))
			{
				return scaleMetric(hit);
			}
		}
		bool ok = false;
		const int v = store->metric(group, role, fallback, &ok);
		const int logical = ok ? v : fallback;
		if (cache)
		{
			cache->putMetric(group, role, logical);
		}
		return scaleMetric(logical);
	}

	void StyleCtx::focusStrokeColors(const QColor& ambient, QColor* outer, QColor* inner) const
	{
		if (!outer)
		{
			return;
		}
		const QColor packOuter = roleColor(QStringLiteral("palette"), QStringLiteral("focus.outer"), QColor(Qt::black));
		const QColor packInner = roleColor(QStringLiteral("palette"), QStringLiteral("focus.inner"), QColor(Qt::white));
		// focus.derive / lightnessOffset are logical tokens — do not DPI-scale.
		int derive = 1;
		int offset = 165;
		if (store)
		{
			bool ok = false;
			derive = store->metric(QStringLiteral("focus"), QStringLiteral("derive"), 1, &ok);
			offset = store->metric(QStringLiteral("focus"), QStringLiteral("lightnessOffset"), 165, &ok);
		}
		if (derive == 0 || !ambient.isValid())
		{
			*outer = packOuter;
			if (inner)
			{
				*inner = packInner;
			}
			return;
		}
		*outer = focusOuterFromAmbient(ambient, offset);
		if (inner)
		{
			*inner = focusInnerFromOuter(*outer);
		}
	}
} // namespace qtheme::style_detail
