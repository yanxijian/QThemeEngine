#include "qtheme/settings.hpp"

#include <QSettings>

namespace qtheme {

QString colorSchemeToString(ColorScheme scheme)
{
	switch (scheme)
	{
	case ColorScheme::Dark:
		return QStringLiteral("dark");
	case ColorScheme::HighContrast:
		return QStringLiteral("highcontrast");
	case ColorScheme::System:
		return QStringLiteral("system");
	case ColorScheme::Light:
	default:
		return QStringLiteral("light");
	}
}

ColorScheme colorSchemeFromString(const QString& s, ColorScheme fallback)
{
	const QString v = s.trimmed().toLower();
	if (v == QLatin1String("dark"))
	{
		return ColorScheme::Dark;
	}
	if (v == QLatin1String("highcontrast") || v == QLatin1String("hc"))
	{
		return ColorScheme::HighContrast;
	}
	if (v == QLatin1String("system"))
	{
		return ColorScheme::System;
	}
	if (v == QLatin1String("light"))
	{
		return ColorScheme::Light;
	}
	return fallback;
}

bool saveAppearancePrefs(QSettings* settings, const AppearancePrefs& prefs)
{
	if (!settings)
	{
		return false;
	}
	settings->beginGroup(QLatin1String(SettingsKeys::kGroup));
	settings->setValue(QLatin1String(SettingsKeys::kSkinId), prefs.skinId);
	settings->setValue(QLatin1String(SettingsKeys::kColorScheme), colorSchemeToString(prefs.colorScheme));
	settings->setValue(QLatin1String(SettingsKeys::kAccentFollowSystem), prefs.accentFollowSystem);
	settings->setValue(QLatin1String(SettingsKeys::kAccent), prefs.accent.name(QColor::HexArgb));
	settings->setValue(QLatin1String(SettingsKeys::kFollowOsHighContrast), prefs.followOsHighContrast);
	settings->setValue(QLatin1String(SettingsKeys::kPackSearchPaths), prefs.packSearchPaths);
	settings->setValue(QLatin1String(SettingsKeys::kExtraPackFiles), prefs.extraPackFiles);
	settings->endGroup();
	settings->sync();
	return settings->status() == QSettings::NoError;
}

bool loadAppearancePrefs(QSettings* settings, AppearancePrefs* out)
{
	if (!settings || !out)
	{
		return false;
	}
	settings->beginGroup(QLatin1String(SettingsKeys::kGroup));
	if (!settings->contains(QLatin1String(SettingsKeys::kSkinId))
		&& !settings->contains(QLatin1String(SettingsKeys::kColorScheme)))
	{
		settings->endGroup();
		return false;
	}

	AppearancePrefs prefs;
	prefs.skinId = settings->value(QLatin1String(SettingsKeys::kSkinId), prefs.skinId).toString();
	prefs.colorScheme = colorSchemeFromString(
		settings->value(QLatin1String(SettingsKeys::kColorScheme), colorSchemeToString(prefs.colorScheme))
			.toString(),
		prefs.colorScheme);
	prefs.accentFollowSystem =
		settings->value(QLatin1String(SettingsKeys::kAccentFollowSystem), prefs.accentFollowSystem).toBool();
	const QString accentName =
		settings->value(QLatin1String(SettingsKeys::kAccent), prefs.accent.name(QColor::HexArgb)).toString();
	const QColor accent(accentName);
	if (accent.isValid())
	{
		prefs.accent = accent;
	}
	prefs.followOsHighContrast =
		settings->value(QLatin1String(SettingsKeys::kFollowOsHighContrast), prefs.followOsHighContrast).toBool();
	prefs.packSearchPaths =
		settings->value(QLatin1String(SettingsKeys::kPackSearchPaths), prefs.packSearchPaths).toStringList();
	prefs.extraPackFiles =
		settings->value(QLatin1String(SettingsKeys::kExtraPackFiles), prefs.extraPackFiles).toStringList();
	settings->endGroup();

	*out = prefs;
	return true;
}

} // namespace qtheme
