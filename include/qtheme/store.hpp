#pragma once

#include "types.hpp"

#include <QHash>
#include <QString>
#include <cstdint>

namespace qtheme {

/// Immutable-after-publish table: (group, role) → color / metric.
/// Built by Format/Loader or programmatic seed; swapped atomically on skin change.
class ThemeStore
{
public:
	void clear();

	void setColor(const QString& group, const QString& role, const QColor& color);
	void setMetric(const QString& group, const QString& role, int value);

	[[nodiscard]] ColorValue color(const QString& group, const QString& role,
								   const QColor& def = QColor()) const;
	[[nodiscard]] int metric(const QString& group, const QString& role, int def = 0,
							 bool* ok = nullptr) const;

	[[nodiscard]] bool hasColor(const QString& group, const QString& role) const;
	[[nodiscard]] std::uint64_t generation() const { return generation_; }

	/// Built-in industrial seed packs (M0, until XML loader lands).
	static ThemeStore seedLight();
	static ThemeStore seedDark();

private:
	static QString key(const QString& group, const QString& role);

	QHash<QString, QColor> colors_;
	QHash<QString, int> metrics_;
	std::uint64_t generation_ = 0;
};

} // namespace qtheme
