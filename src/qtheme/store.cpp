#include "qtheme/store.hpp"

#include "qtheme/pack.hpp"

namespace qtheme {

void ThemeStore::markDirty()
{
	if (m_updateDepth > 0)
	{
		m_dirty = true;
	}
	else
	{
		++m_generation;
	}
}

void ThemeStore::beginUpdate()
{
	++m_updateDepth;
}

void ThemeStore::endUpdate()
{
	if (m_updateDepth <= 0)
	{
		return;
	}
	--m_updateDepth;
	if (m_updateDepth == 0 && m_dirty)
	{
		++m_generation;
		m_dirty = false;
	}
}

void ThemeStore::clear()
{
	beginUpdate();
	m_colors.clear();
	m_metrics.clear();
	m_dirty = true;
	endUpdate();
}

QString ThemeStore::key(const QString& group, const QString& role)
{
	return group + QLatin1Char('/') + role;
}

void ThemeStore::setColor(const QString& group, const QString& role, const QColor& color)
{
	m_colors.insert(key(group, role), color);
	markDirty();
}

void ThemeStore::setMetric(const QString& group, const QString& role, int value)
{
	m_metrics.insert(key(group, role), value);
	markDirty();
}

void ThemeStore::mergeFrom(const ThemeStore& other, bool overwrite)
{
	beginUpdate();
	for (auto it = other.m_colors.cbegin(); it != other.m_colors.cend(); ++it)
	{
		if (overwrite || !m_colors.contains(it.key()))
		{
			m_colors.insert(it.key(), it.value());
			m_dirty = true;
		}
	}
	for (auto it = other.m_metrics.cbegin(); it != other.m_metrics.cend(); ++it)
	{
		if (overwrite || !m_metrics.contains(it.key()))
		{
			m_metrics.insert(it.key(), it.value());
			m_dirty = true;
		}
	}
	endUpdate();
}

ColorValue ThemeStore::color(const QString& group, const QString& role, const QColor& def) const
{
	ColorValue out;
	const auto it = m_colors.constFind(key(group, role));
	if (it == m_colors.cend())
	{
		out.value = def;
		out.ok = def.isValid();
		return out;
	}
	out.value = *it;
	out.ok = it->isValid();
	return out;
}

int ThemeStore::metric(const QString& group, const QString& role, int def, bool* ok) const
{
	const auto it = m_metrics.constFind(key(group, role));
	if (it == m_metrics.cend())
	{
		if (ok)
		{
			*ok = false;
		}
		return def;
	}
	if (ok)
	{
		*ok = true;
	}
	return *it;
}

bool ThemeStore::hasColor(const QString& group, const QString& role) const
{
	return m_colors.contains(key(group, role));
}

bool ThemeStore::hasMetric(const QString& group, const QString& role) const
{
	return m_metrics.contains(key(group, role));
}

QStringList ThemeStore::requiredColorKeys()
{
	return {
		QStringLiteral("palette/window"),	 QStringLiteral("palette/windowText"),
		QStringLiteral("palette/base"),		 QStringLiteral("palette/text"),
		QStringLiteral("palette/button"),	 QStringLiteral("palette/buttonText"),
		QStringLiteral("palette/highlight"), QStringLiteral("palette/accent"),
		QStringLiteral("button/bg"),		 QStringLiteral("button/fg"),
		QStringLiteral("button/border"),	 QStringLiteral("edit/bg"),
		QStringLiteral("edit/border"),		 QStringLiteral("check/bg"),
		QStringLiteral("tab/bg"),			 QStringLiteral("scroll/handle"),
	};
}

QStringList ThemeStore::missingRequiredColors() const
{
	QStringList missing;
	for (const QString& k : requiredColorKeys())
	{
		if (!m_colors.contains(k))
		{
			missing << k;
		}
	}
	return missing;
}

bool ThemeStore::loadBuiltinPack(const QString& packId, ThemeStore* out)
{
	if (!out)
	{
		return false;
	}
	PackRegistry reg;
	if (!reg.registerBuiltinFluentPacks(nullptr))
	{
		return false;
	}
	return reg.materialize(packId, out, nullptr);
}

ThemeStore ThemeStore::seedLight()
{
	ThemeStore s;
	(void)loadBuiltinPack(QString::fromUtf8(kPackFluentLight), &s);
	return s;
}

ThemeStore ThemeStore::seedDark()
{
	ThemeStore s;
	(void)loadBuiltinPack(QString::fromUtf8(kPackFluentDark), &s);
	return s;
}

} // namespace qtheme
