#ifndef __QTHEME_ENGINE_STYLE_COLOR_CACHE_H__
#define __QTHEME_ENGINE_STYLE_COLOR_CACHE_H__

#include "qtheme/store.hpp"

#include <QColor>
#include <QHash>
#include <QString>

#include <cstdint>

namespace qtheme::style_detail
{
	/// Per-QThemeStyle cache: one ThemeStore::key() concat + hash hit instead of
	/// repeated store lookups during a paint storm. Cleared when store generation
	/// changes (pack / accent apply).
	class ColorLookupCache
	{
	public:
		void clear()
		{
			m_colors.clear();
			m_metrics.clear();
			m_generation = 0;
			m_store = nullptr;
		}

		void ensure(const ThemeStore* store)
		{
			if (!store)
			{
				clear();
				return;
			}
			if (m_store != store || m_generation != store->generation())
			{
				m_colors.clear();
				m_metrics.clear();
				m_store = store;
				m_generation = store->generation();
			}
		}

		[[nodiscard]] bool tryColor(const QString& group, const QString& role, QColor* out) const
		{
			const auto it = m_colors.constFind(makeKey(group, role));
			if (it == m_colors.cend())
			{
				return false;
			}
			if (out)
			{
				*out = *it;
			}
			return true;
		}

		void putColor(const QString& group, const QString& role, const QColor& color)
		{
			m_colors.insert(makeKey(group, role), color);
		}

		[[nodiscard]] bool tryMetric(const QString& group, const QString& role, int* out) const
		{
			const auto it = m_metrics.constFind(makeKey(group, role));
			if (it == m_metrics.cend())
			{
				return false;
			}
			if (out)
			{
				*out = *it;
			}
			return true;
		}

		void putMetric(const QString& group, const QString& role, int value)
		{
			m_metrics.insert(makeKey(group, role), value);
		}

	private:
		[[nodiscard]] static QString makeKey(const QString& group, const QString& role)
		{
			return group + QLatin1Char('/') + role;
		}

		const ThemeStore* m_store = nullptr;
		std::uint64_t m_generation = 0;
		QHash<QString, QColor> m_colors;
		QHash<QString, int> m_metrics;
	};
} // namespace qtheme::style_detail

#endif // __QTHEME_ENGINE_STYLE_COLOR_CACHE_H__
