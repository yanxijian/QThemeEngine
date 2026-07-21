#pragma once

#include "store.hpp"
#include "style.hpp"
#include "types.hpp"

#include <QObject>
#include <memory>

class QApplication;

namespace qtheme {

/// Process-facing session: load/seed skins, apply QThemeStyle, notify listeners.
class Engine final : public QObject
{
	Q_OBJECT
public:
	explicit Engine(QObject* parent = nullptr);
	~Engine() override;

	/// Install style (+ palette) on the application. Clears stylesheets when clearStyleSheets is true.
	void apply(QApplication* app, bool clearStyleSheets = true);

	bool switchSkin(const QString& name, bool force = false);
	[[nodiscard]] QString currentSkin() const { return currentSkin_; }
	[[nodiscard]] Error lastError() const { return lastError_; }

	[[nodiscard]] ThemeStore* store() const { return store_.get(); }
	[[nodiscard]] QThemeStyle* style() const { return style_; }

	/// Bind default engine for ThemeApi (tests may bind nullptr).
	static void setDefault(Engine* engine);
	static Engine* defaultEngine();

signals:
	void skinChanged(const QString& previous, const QString& current);

private:
	std::shared_ptr<ThemeStore> store_;
	QThemeStyle* style_ = nullptr; // owned by QApplication after apply()
	QString currentSkin_;
	Error lastError_ = Error::None;
	bool inited_ = false;
};

} // namespace qtheme
