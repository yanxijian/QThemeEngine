#ifndef __QTHEME_ENGINE_SETTINGS_H__
#define __QTHEME_ENGINE_SETTINGS_H__

#include "types.hpp"

#include <QColor>
#include <QString>
#include <QStringList>
#include <QStringView>
#include <qtheme/qtheme_export.hpp>

class QSettings;

namespace qtheme
{
	/// Serializable appearance preferences (pack / scheme / accent / pack paths).
	struct AppearancePrefs
	{
		QString packId = QString::fromUtf8(kPackFluentLight);
		ColorScheme colorScheme = ColorScheme::Light;
		bool accentFollowSystem = true;
		QColor accent = QColor(QStringLiteral("#0078D4"));
		bool followOsHighContrast = true;
		QStringList packSearchPaths;
		QStringList extraPackFiles;
	};

	namespace SettingsKeys
	{
		inline constexpr char kGroup[] = "qtheme";
		inline constexpr char kPackId[] = "packId";
		inline constexpr char kColorScheme[] = "colorScheme";
		inline constexpr char kAccentFollowSystem[] = "accentFollowSystem";
		inline constexpr char kAccent[] = "accent";
		inline constexpr char kFollowOsHighContrast[] = "followOsHighContrast";
		inline constexpr char kPackSearchPaths[] = "packSearchPaths";
		inline constexpr char kExtraPackFiles[] = "extraPackFiles";
	} // namespace SettingsKeys

	[[nodiscard]] QTE_EXPORT bool saveAppearancePrefs(QSettings* settings, const AppearancePrefs& prefs);
	[[nodiscard]] QTE_EXPORT bool loadAppearancePrefs(QSettings* settings, AppearancePrefs* out);

	[[nodiscard]] QTE_EXPORT QString colorSchemeToString(ColorScheme scheme);
	[[nodiscard]] QTE_EXPORT ColorScheme colorSchemeFromString(QStringView s, ColorScheme fallback = ColorScheme::Light);
} // namespace qtheme

#endif // __QTHEME_ENGINE_SETTINGS_H__
