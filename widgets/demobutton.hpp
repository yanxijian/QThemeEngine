#ifndef __QTHEME_ENGINE_DEMOBUTTON_H__
#define __QTHEME_ENGINE_DEMOBUTTON_H__

#include <QEnterEvent>
#include <QEvent>
#include <QMouseEvent>
#include <QShowEvent>
#include <QString>
#include <QWidget>

namespace qtheme {

/// Owner-drawn sample control (secondary path). Prefers ThemeStore via api::.
class DemoButton final : public QWidget
{
	Q_OBJECT
public:
	explicit DemoButton(QWidget* parent = nullptr);

	void setText(const QString& text);
	[[nodiscard]] QString text() const;

protected:
	void paintEvent(QPaintEvent* event) override;
	void enterEvent(QEnterEvent* event) override;
	void leaveEvent(QEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void showEvent(QShowEvent* event) override;

private:
	void ensureSkinConnection();

	QString m_text;
	bool m_pressed = false;
	bool m_skinConnected = false;
};

} // namespace qtheme

#endif  // __QTHEME_ENGINE_DEMOBUTTON_H__
