#pragma once

#include "store.hpp"

#include <QProxyStyle>
#include <memory>

namespace qtheme {

/// Theme-table-driven style. Proxies Fusion (or base) where roles are not covered yet.
class QThemeStyle final : public QProxyStyle
{
	Q_OBJECT
public:
	explicit QThemeStyle(std::shared_ptr<ThemeStore> store, QStyle* base = nullptr);

	void setStore(std::shared_ptr<ThemeStore> store);
	[[nodiscard]] std::shared_ptr<ThemeStore> store() const { return store_; }

	void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter,
					 const QWidget* widget = nullptr) const override;
	int pixelMetric(PixelMetric metric, const QStyleOption* option = nullptr,
					const QWidget* widget = nullptr) const override;
	QPalette standardPalette() const override;

private:
	[[nodiscard]] QColor roleColor(const QString& group, const QString& role,
								   const QColor& fallback) const;
	[[nodiscard]] int roleMetric(const QString& group, const QString& role, int fallback) const;

	std::shared_ptr<ThemeStore> store_;
};

} // namespace qtheme
