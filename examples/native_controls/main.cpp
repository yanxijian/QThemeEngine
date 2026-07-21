#include "demobutton.hpp"

#include "qtheme/engine.hpp"

#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	qtheme::Engine engine;
	engine.apply(&app); // installs QThemeStyle, clears stylesheets, seed light

	QWidget w;
	auto* layout = new QVBoxLayout(&w);
	auto* hint = new QLabel(
		QStringLiteral("Native QPushButton via QThemeStyle (no QSS).\nOwner-drawn DemoButton uses the same ThemeStore."),
		&w);
	hint->setWordWrap(true);

	auto* native = new QPushButton(QStringLiteral("Native OK"), &w);
	native->setMinimumHeight(36);

	auto* drawn = new qtheme::DemoButton(&w);
	drawn->setText(QStringLiteral("Owner-draw OK"));
	drawn->setMinimumHeight(36);

	auto* light = new QPushButton(QStringLiteral("Light"), &w);
	auto* dark = new QPushButton(QStringLiteral("Dark"), &w);
	QObject::connect(light, &QPushButton::clicked, &engine,
					 [&engine]
					 {
						 engine.switchSkin(QStringLiteral("light"));
					 });
	QObject::connect(dark, &QPushButton::clicked, &engine,
					 [&engine]
					 {
						 engine.switchSkin(QStringLiteral("dark"));
					 });

	layout->addWidget(hint);
	layout->addWidget(native);
	layout->addWidget(drawn);
	layout->addWidget(light);
	layout->addWidget(dark);
	w.setWindowTitle(QStringLiteral("QThemeEngine M0 — Native Controls"));
	w.resize(360, 220);
	w.show();
	return app.exec();
}
