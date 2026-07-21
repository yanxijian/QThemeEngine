#ifndef __QTHEME_ENGINE_ENGINE_H__
#define __QTHEME_ENGINE_ENGINE_H__

#include "store.hpp"
#include "style.hpp"
#include "types.hpp"

#include <QColor>
#include <QObject>
#include <QStringList>
#include <memory>

class QApplication;
class QGuiApplication;

namespace qtheme {

class PackRegistry;

/// Process-facing session: packs, accent, color scheme, QThemeStyle.
class Engine final : public QObject
{
	Q_OBJECT
public:
	explicit Engine(QObject* parent = nullptr);
	~Engine() override;

	void apply(QApplication* app, bool clearStyleSheets = true);

	bool switchSkin(const QString& name, bool force = false);
	[[nodiscard]] QString currentSkin() const { return currentSkin_; }
	[[nodiscard]] Error lastError() const { return lastError_; }

	bool setColorScheme(ColorScheme scheme, bool force = false);
	[[nodiscard]] ColorScheme colorScheme() const { return colorScheme_; }

	bool setAccent(const QColor& accent);
	bool setAccentFollowSystem(bool follow);
	[[nodiscard]] QColor accent() const { return accent_; }
	[[nodiscard]] bool accentFollowsSystem() const { return accentFollowSystem_; }

	/// When true (default), ColorScheme::System prefers fluent.hc if the OS high-contrast mode is on.
	void setFollowOsHighContrast(bool follow) { followOsHighContrast_ = follow; }
	[[nodiscard]] bool followOsHighContrast() const { return followOsHighContrast_; }

	bool registerPack(const QString& pathOrQrc);
	[[nodiscard]] QStringList registeredPacks() const;

	[[nodiscard]] ThemeStore* store() const { return store_.get(); }
	[[nodiscard]] QThemeStyle* style() const { return style_; }
	[[nodiscard]] PackRegistry* packs() const { return packs_.get(); }

	static void setDefault(Engine* engine);
	static Engine* defaultEngine();

signals:
	void skinChanged(const QString& previous, const QString& current);
	void accentChanged(const QColor& accent);
	void colorSchemeChanged();

private slots:
	void onOsPaletteChanged(const QPalette& palette);
	void onOsColorSchemeChanged(Qt::ColorScheme scheme);

private:
	void refreshUi();
	bool rebuildStore(const QString& packId, bool force);
	bool applyPack(const QString& packId, ColorScheme scheme, bool force);
	[[nodiscard]] QString packIdForScheme(ColorScheme scheme) const;
	void applyAccentToStore();
	void installOsHooks(QGuiApplication* app);
	void syncFromOsAppearance(bool forcePackReload);

	std::shared_ptr<ThemeStore> store_;
	std::unique_ptr<PackRegistry> packs_;
	QThemeStyle* style_ = nullptr;
	QString currentSkin_;
	ColorScheme colorScheme_ = ColorScheme::Light;
	QColor accent_;
	bool accentFollowSystem_ = true;
	bool followOsHighContrast_ = true;
	bool osHooksInstalled_ = false;
	bool syncingFromOs_ = false;
	Error lastError_ = Error::None;
	bool inited_ = false;
};

} // namespace qtheme

#endif  // __QTHEME_ENGINE_ENGINE_H__
