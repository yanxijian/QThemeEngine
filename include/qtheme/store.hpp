#ifndef __QTHEME_ENGINE_STORE_H__
#define __QTHEME_ENGINE_STORE_H__

#include "types.hpp"

#include <QHash>
#include <QString>
#include <QStringList>
#include <cstdint>

namespace qtheme {

/// Table: (group, role) → color / metric. Prefer batching mutations via beginUpdate/endUpdate.
class ThemeStore
{
public:
	void clear();

	/// Coalesce generation bumps across many setColor/setMetric/mergeFrom calls.
	void beginUpdate();
	void endUpdate();

	void setColor(const QString& group, const QString& role, const QColor& color);
	void setMetric(const QString& group, const QString& role, int value);

	/// Merge another store into this one (overwrite existing keys when overwrite is true).
	void mergeFrom(const ThemeStore& other, bool overwrite = true);

	[[nodiscard]] ColorValue color(const QString& group, const QString& role,
								   const QColor& def = QColor()) const;
	[[nodiscard]] int metric(const QString& group, const QString& role, int def = 0,
							 bool* ok = nullptr) const;

	[[nodiscard]] bool hasColor(const QString& group, const QString& role) const;
	[[nodiscard]] bool hasMetric(const QString& group, const QString& role) const;
	[[nodiscard]] std::uint64_t generation() const { return generation_; }

	/// Required Fluent tokens for pack validation / self-check.
	[[nodiscard]] static QStringList requiredColorKeys();
	[[nodiscard]] QStringList missingRequiredColors() const;

	/// Load built-in Fluent pack from JSON (qrc). Empty store + false if pack missing.
	[[nodiscard]] static bool loadBuiltinPack(const QString& packId, ThemeStore* out);

	/// Compatibility helpers (JSON-backed).
	[[nodiscard]] static ThemeStore seedLight();
	[[nodiscard]] static ThemeStore seedDark();

private:
	void markDirty();
	static QString key(const QString& group, const QString& role);

	QHash<QString, QColor> colors_;
	QHash<QString, int> metrics_;
	std::uint64_t generation_ = 0;
	int updateDepth_ = 0;
	bool dirty_ = false;
};

} // namespace qtheme

#endif  // __QTHEME_ENGINE_STORE_H__
