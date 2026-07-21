#include "qtheme/store.hpp"

namespace qtheme {

namespace {

void fillCommonMetrics(ThemeStore& s)
{
	s.setMetric(QStringLiteral("button"), QStringLiteral("radius"), 6);
	s.setMetric(QStringLiteral("button"), QStringLiteral("padding"), 8);
	s.setMetric(QStringLiteral("button"), QStringLiteral("height"), 32);
}

} // namespace

void ThemeStore::clear()
{
	colors_.clear();
	metrics_.clear();
	++generation_;
}

QString ThemeStore::key(const QString& group, const QString& role)
{
	return group + QLatin1Char('/') + role;
}

void ThemeStore::setColor(const QString& group, const QString& role, const QColor& color)
{
	colors_.insert(key(group, role), color);
	++generation_;
}

void ThemeStore::setMetric(const QString& group, const QString& role, int value)
{
	metrics_.insert(key(group, role), value);
	++generation_;
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

ThemeStore ThemeStore::seedLight()
{
	ThemeStore s;
	// Palette tokens (also feed standardPalette).
	s.setColor(QStringLiteral("palette"), QStringLiteral("window"), QColor(QStringLiteral("#F5F5F5")));
	s.setColor(QStringLiteral("palette"), QStringLiteral("windowText"), QColor(QStringLiteral("#1A1A1A")));
	s.setColor(QStringLiteral("palette"), QStringLiteral("base"), QColor(QStringLiteral("#FFFFFF")));
	s.setColor(QStringLiteral("palette"), QStringLiteral("text"), QColor(QStringLiteral("#1A1A1A")));
	s.setColor(QStringLiteral("palette"), QStringLiteral("button"), QColor(QStringLiteral("#E8E8E8")));
	s.setColor(QStringLiteral("palette"), QStringLiteral("buttonText"), QColor(QStringLiteral("#1A1A1A")));
	s.setColor(QStringLiteral("palette"), QStringLiteral("highlight"), QColor(QStringLiteral("#2B6DE5")));
	s.setColor(QStringLiteral("palette"), QStringLiteral("highlightedText"), QColor(QStringLiteral("#FFFFFF")));
	s.setColor(QStringLiteral("palette"), QStringLiteral("mid"), QColor(QStringLiteral("#C8C8C8")));
	s.setColor(QStringLiteral("palette"), QStringLiteral("light"), QColor(QStringLiteral("#FFFFFF")));
	s.setColor(QStringLiteral("palette"), QStringLiteral("dark"), QColor(QStringLiteral("#8A8A8A")));

	// Button control roles.
	s.setColor(QStringLiteral("button"), QStringLiteral("bg"), QColor(QStringLiteral("#E8E8E8")));
	s.setColor(QStringLiteral("button"), QStringLiteral("bg.hover"), QColor(QStringLiteral("#D8D8D8")));
	s.setColor(QStringLiteral("button"), QStringLiteral("bg.pressed"), QColor(QStringLiteral("#C8C8C8")));
	s.setColor(QStringLiteral("button"), QStringLiteral("bg.disabled"), QColor(QStringLiteral("#F0F0F0")));
	s.setColor(QStringLiteral("button"), QStringLiteral("fg"), QColor(QStringLiteral("#1A1A1A")));
	s.setColor(QStringLiteral("button"), QStringLiteral("fg.disabled"), QColor(QStringLiteral("#A0A0A0")));
	s.setColor(QStringLiteral("button"), QStringLiteral("border"), QColor(QStringLiteral("#B0B0B0")));
	s.setColor(QStringLiteral("button"), QStringLiteral("border.focus"), QColor(QStringLiteral("#2B6DE5")));

	// Owner-draw DemoButton roles (secondary path; same store).
	s.setColor(QStringLiteral("DemoButton"), QStringLiteral("background"), QColor(QStringLiteral("#E8E8E8")));
	s.setColor(QStringLiteral("DemoButton"), QStringLiteral("background.hover"), QColor(QStringLiteral("#D8D8D8")));
	s.setColor(QStringLiteral("DemoButton"), QStringLiteral("background.pressed"), QColor(QStringLiteral("#C8C8C8")));
	s.setColor(QStringLiteral("DemoButton"), QStringLiteral("text"), QColor(QStringLiteral("#1A1A1A")));
	s.setColor(QStringLiteral("DemoButton"), QStringLiteral("border"), QColor(QStringLiteral("#B0B0B0")));
	s.setMetric(QStringLiteral("DemoButton"), QStringLiteral("borderRadius"), 6);

	fillCommonMetrics(s);
	return s;
}

ThemeStore ThemeStore::seedDark()
{
	ThemeStore s;
	s.setColor(QStringLiteral("palette"), QStringLiteral("window"), QColor(QStringLiteral("#2B2B2B")));
	s.setColor(QStringLiteral("palette"), QStringLiteral("windowText"), QColor(QStringLiteral("#E8E8E8")));
	s.setColor(QStringLiteral("palette"), QStringLiteral("base"), QColor(QStringLiteral("#1E1E1E")));
	s.setColor(QStringLiteral("palette"), QStringLiteral("text"), QColor(QStringLiteral("#E8E8E8")));
	s.setColor(QStringLiteral("palette"), QStringLiteral("button"), QColor(QStringLiteral("#3A3A3A")));
	s.setColor(QStringLiteral("palette"), QStringLiteral("buttonText"), QColor(QStringLiteral("#E8E8E8")));
	s.setColor(QStringLiteral("palette"), QStringLiteral("highlight"), QColor(QStringLiteral("#3D7EFF")));
	s.setColor(QStringLiteral("palette"), QStringLiteral("highlightedText"), QColor(QStringLiteral("#FFFFFF")));
	s.setColor(QStringLiteral("palette"), QStringLiteral("mid"), QColor(QStringLiteral("#555555")));
	s.setColor(QStringLiteral("palette"), QStringLiteral("light"), QColor(QStringLiteral("#4A4A4A")));
	s.setColor(QStringLiteral("palette"), QStringLiteral("dark"), QColor(QStringLiteral("#1A1A1A")));

	s.setColor(QStringLiteral("button"), QStringLiteral("bg"), QColor(QStringLiteral("#3A3A3A")));
	s.setColor(QStringLiteral("button"), QStringLiteral("bg.hover"), QColor(QStringLiteral("#484848")));
	s.setColor(QStringLiteral("button"), QStringLiteral("bg.pressed"), QColor(QStringLiteral("#2E2E2E")));
	s.setColor(QStringLiteral("button"), QStringLiteral("bg.disabled"), QColor(QStringLiteral("#333333")));
	s.setColor(QStringLiteral("button"), QStringLiteral("fg"), QColor(QStringLiteral("#E8E8E8")));
	s.setColor(QStringLiteral("button"), QStringLiteral("fg.disabled"), QColor(QStringLiteral("#777777")));
	s.setColor(QStringLiteral("button"), QStringLiteral("border"), QColor(QStringLiteral("#5A5A5A")));
	s.setColor(QStringLiteral("button"), QStringLiteral("border.focus"), QColor(QStringLiteral("#3D7EFF")));

	s.setColor(QStringLiteral("DemoButton"), QStringLiteral("background"), QColor(QStringLiteral("#3A3A3A")));
	s.setColor(QStringLiteral("DemoButton"), QStringLiteral("background.hover"), QColor(QStringLiteral("#484848")));
	s.setColor(QStringLiteral("DemoButton"), QStringLiteral("background.pressed"), QColor(QStringLiteral("#2E2E2E")));
	s.setColor(QStringLiteral("DemoButton"), QStringLiteral("text"), QColor(QStringLiteral("#E8E8E8")));
	s.setColor(QStringLiteral("DemoButton"), QStringLiteral("border"), QColor(QStringLiteral("#5A5A5A")));
	s.setMetric(QStringLiteral("DemoButton"), QStringLiteral("borderRadius"), 6);

	fillCommonMetrics(s);
	return s;
}

} // namespace qtheme
