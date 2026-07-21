#pragma once

#include <QEnterEvent>
#include <QEvent>
#include <QMouseEvent>
#include <QShowEvent>
#include <QString>
#include <QWidget>

namespace theme
{
	/// L0 self-painted button (docs/zh/style.md T4). Implementation pending full ThemeLoader.
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

		QString text_;
		bool pressed_ = false;
		bool skinConnected_ = false;
	};
} // namespace theme
