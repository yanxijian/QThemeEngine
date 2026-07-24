#ifndef __QTHEME_ENGINE_STYLE_H__
#define __QTHEME_ENGINE_STYLE_H__

#include "store.hpp"

#include <QProxyStyle>
#include <memory>

namespace qtheme {

/// Theme-table-driven style (Fluent-inspired metrics/drawing via ThemeStore).
class QThemeStyle final : public QProxyStyle
{
	Q_OBJECT
public:
	explicit QThemeStyle(std::shared_ptr<ThemeStore> store, QStyle* base = nullptr);

	void setStore(std::shared_ptr<ThemeStore> store);
	[[nodiscard]] std::shared_ptr<ThemeStore> store() const { return m_store; }

	/// Logical-pixel scale (1.0 = 96 DPI). Metrics from ThemeStore are multiplied by this.
	void setDpiScale(qreal scale);
	[[nodiscard]] qreal dpiScale() const { return m_dpiScale; }

	void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter,
					   const QWidget* widget = nullptr) const override;
	void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter,
					 const QWidget* widget = nullptr) const override;
	void drawComplexControl(ComplexControl control, const QStyleOptionComplex* option, QPainter* painter,
							const QWidget* widget = nullptr) const override;
	int pixelMetric(PixelMetric metric, const QStyleOption* option = nullptr,
					const QWidget* widget = nullptr) const override;
	QSize sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& contentsSize,
						   const QWidget* widget = nullptr) const override;
	QRect subControlRect(ComplexControl control, const QStyleOptionComplex* option, SubControl subControl,
						 const QWidget* widget = nullptr) const override;
	QPalette standardPalette() const override;

	void polish(QWidget* widget) override;
	void unpolish(QWidget* widget) override;

private:
	[[nodiscard]] QColor roleColor(const QString& group, const QString& role,
								   const QColor& fallback) const;
	[[nodiscard]] int roleMetric(const QString& group, const QString& role, int fallback) const;
	[[nodiscard]] int scaleMetric(int logicalPx) const;

	std::shared_ptr<ThemeStore> m_store;
	qreal m_dpiScale = 1.0;
};

} // namespace qtheme

#endif  // __QTHEME_ENGINE_STYLE_H__
