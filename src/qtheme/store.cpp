#include "qtheme/store.hpp"

#include "qtheme/pack.hpp"

namespace qtheme {

void ThemeStore::markDirty()
{
	if (updateDepth_ > 0)
	{
		dirty_ = true;
	}
	else
	{
		++generation_;
	}
}

void ThemeStore::beginUpdate()
{
	++updateDepth_;
}

void ThemeStore::endUpdate()
{
	if (updateDepth_ <= 0)
	{
		return;
	}
	--updateDepth_;
	if (updateDepth_ == 0 && dirty_)
	{
		++generation_;
		dirty_ = false;
	}
}

void ThemeStore::clear()
{
	beginUpdate();
	colors_.clear();
	metrics_.clear();
	dirty_ = true;
	endUpdate();
}

QString ThemeStore::key(const QString& group, const QString& role)
{
	return group + QLatin1Char('/') + role;
}

void ThemeStore::setColor(const QString& group, const QString& role, const QColor& color)
{
	colors_.insert(key(group, role), color);
	markDirty();
}

void ThemeStore::setMetric(const QString& group, const QString& role, int value)
{
	metrics_.insert(key(group, role), value);
	markDirty();
}

void ThemeStore::mergeFrom(const ThemeStore& other, bool overwrite)
{
	beginUpdate();
	for (auto it = other.colors_.cbegin(); it != other.colors_.cend(); ++it)
	{
		if (overwrite || !colors_.contains(it.key()))
		{
			colors_.insert(it.key(), it.value());
			dirty_ = true;
		}
	}
	for (auto it = other.metrics_.cbegin(); it != other.metrics_.cend(); ++it)
	{
		if (overwrite || !metrics_.contains(it.key()))
		{
			metrics_.insert(it.key(), it.value());
			dirty_ = true;
		}
	}
	endUpdate();
}

ColorValue ThemeStore::color(const QString& group, const QString& role, const QColor& def) const
{
	ColorValue out;
	const auto it = colors_.constFind(key(group, role));
	if (it == colors_.cend())
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
	const auto it = metrics_.constFind(key(group, role));
	if (it == metrics_.cend())
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
	return colors_.contains(key(group, role));
}

bool ThemeStore::hasMetric(const QString& group, const QString& role) const
{
	return metrics_.contains(key(group, role));
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
		if (!colors_.contains(k))
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
