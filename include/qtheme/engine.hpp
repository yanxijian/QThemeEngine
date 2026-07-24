#ifndef __QTHEME_ENGINE_ENGINE_H__
#define __QTHEME_ENGINE_ENGINE_H__

#include "settings.hpp"
#include "store.hpp"
#include "style.hpp"
#include "types.hpp"

#include <QColor>
#include <QObject>
#include <QStringList>
#include <memory>

class QApplication;
class QGuiApplication;
class QSettings;

namespace qtheme {

class PackRegistry;

/// Process-facing session: packs, accent, color scheme, QThemeStyle, preferences.
class Engine final : public QObject
{
	Q_OBJECT
public:
	explicit Engine(QObject* parent = nullptr);
	~Engine() override;

	void apply(QApplication* app, bool clearStyleSheets = true);

	bool switchSkin(const QString& name, bool force = false);
	[[nodiscard]] QString currentSkin() const { return m_currentSkin; }
	[[nodiscard]] Error lastError() const { return m_lastError; }

	bool setColorScheme(ColorScheme scheme, bool force = false);
	[[nodiscard]] ColorScheme colorScheme() const { return m_colorScheme; }

	bool setAccent(const QColor& accent);
	bool setAccentFollowSystem(bool follow);
	[[nodiscard]] QColor accent() const { return m_accent; }
	[[nodiscard]] bool accentFollowsSystem() const { return m_accentFollowSystem; }

	/// When true (default), ColorScheme::System prefers fluent.hc if the OS high-contrast mode is on.
	void setFollowOsHighContrast(bool follow);
	[[nodiscard]] bool followOsHighContrast() const { return m_followOsHighContrast; }

	bool registerPack(const QString& pathOrQrc);
	/// Append a directory to scan for `*.theme.json` extension packs.
	void addPackSearchPath(const QString& dir);
	[[nodiscard]] QStringList packSearchPaths() const { return m_packSearchPaths; }
	/// Register packs from all search paths. Returns total newly attempted registrations.
	int scanPackSearchPaths();
	[[nodiscard]] QStringList registeredPacks() const;

	/// Snapshot / restore appearance (skin, scheme, accent, pack paths).
	[[nodiscard]] AppearancePrefs appearancePrefs() const;
	bool applyAppearancePrefs(const AppearancePrefs& prefs);
	bool savePreferences(QSettings* settings = nullptr) const;
	bool loadPreferences(QSettings* settings = nullptr);
	void setAutoSavePreferences(bool enable);
	[[nodiscard]] bool autoSavePreferences() const { return m_autoSavePreferences; }

	[[nodiscard]] ThemeStore* store() const { return m_store.get(); }
	[[nodiscard]] QThemeStyle* style() const { return m_style; }
	[[nodiscard]] PackRegistry* packs() const { return m_packs.get(); }

	static void setDefault(Engine* engine);
	static Engine* defaultEngine();

signals:
	void skinChanged(const QString& previous, const QString& current);
	void accentChanged(const QColor& accent);
	void colorSchemeChanged();

private slots:
	void onOsPaletteChanged(const QPalette& palette);
	void onOsColorSchemeChanged(Qt::ColorScheme scheme);
	void onAutoSavePreferences();

private:
	void refreshUi();
	bool rebuildStore(const QString& packId, bool force);
	bool applyPack(const QString& packId, ColorScheme scheme, bool force);
	[[nodiscard]] QString packIdForScheme(ColorScheme scheme) const;
	void applyAccentToStore();
	void installOsHooks(QGuiApplication* app);
	void syncFromOsAppearance(bool forcePackReload);
	void updateDpiScale();

	std::shared_ptr<ThemeStore> m_store;
	std::unique_ptr<PackRegistry> m_packs;
	QThemeStyle* m_style = nullptr;
	QString m_currentSkin;
	ColorScheme m_colorScheme = ColorScheme::Light;
	QColor m_accent;
	bool m_accentFollowSystem = true;
	bool m_followOsHighContrast = true;
	bool m_osHooksInstalled = false;
	bool m_syncingFromOs = false;
	bool m_autoSavePreferences = false;
	bool m_loadingPreferences = false;
	QStringList m_packSearchPaths;
	QStringList m_extraPackFiles;
	Error m_lastError = Error::None;
	bool m_inited = false;
};

} // namespace qtheme

#endif  // __QTHEME_ENGINE_ENGINE_H__
