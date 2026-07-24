#ifndef __QTHEME_ENGINE_SETTINGS_H__
#define __QTHEME_ENGINE_SETTINGS_H__

#include "types.hpp"

#include <QColor>
#include <QString>
#include <QStringList>

class QSettings;

namespace qtheme {

/// Serializable appearance preferences (skin / scheme / accent / pack paths).
struct AppearancePrefs
{
	QString skinId = QString::fromUtf8(kPackFluentLight);
	ColorScheme colorScheme = ColorScheme::Light;
	bool accentFollowSystem = true;
	QColor accent = QColor(QStringLiteral("#0078D4"));
	bool followOsHighContrast = true;
	QStringList packSearchPaths;
	QStringList extraPackFiles;
};

namespace SettingsKeys {
inline constexpr char kGroup[] = "qtheme";
inline constexpr char kSkinId[] = "skinId";
inline constexpr char kColorScheme[] = "colorScheme";
inline constexpr char kAccentFollowSystem[] = "accentFollowSystem";
inline constexpr char kAccent[] = "accent";
inline constexpr char kFollowOsHighContrast[] = "followOsHighContrast";
inline constexpr char kPackSearchPaths[] = "packSearchPaths";
inline constexpr char kExtraPackFiles[] = "extraPackFiles";
} // namespace SettingsKeys

[[nodiscard]] bool saveAppearancePrefs(QSettings* settings, const AppearancePrefs& prefs);
[[nodiscard]] bool loadAppearancePrefs(QSettings* settings, AppearancePrefs* out);

[[nodiscard]] QString colorSchemeToString(ColorScheme scheme);
[[nodiscard]] ColorScheme colorSchemeFromString(const QString& s, ColorScheme fallback = ColorScheme::Light);

} // namespace qtheme

#endif  // __QTHEME_ENGINE_SETTINGS_H__
