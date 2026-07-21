#include "demobutton.hpp"
#include "theme/skinmanager.hpp"
#include "theme/themeapi.hpp"

#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	auto* skin = new theme::SkinManager(&app);
	theme::ThemeApi::bind(skin);
	skin->switchSkin(QStringLiteral("light"));

	QWidget w;
	auto* layout = new QVBoxLayout(&w);
	auto* btn = new theme::DemoButton(&w);
	btn->setText(QStringLiteral("OK"));
	btn->setMinimumHeight(36);
	auto* light = new QPushButton(QStringLiteral("Light"), &w);
	auto* dark = new QPushButton(QStringLiteral("Dark"), &w);
	QObject::connect(light, &QPushButton::clicked,
					 [skin]
					 {
						 skin->switchSkin(QStringLiteral("light"));
					 });
	QObject::connect(dark, &QPushButton::clicked,
					 [skin]
					 {
						 skin->switchSkin(QStringLiteral("dark"));
					 });
	layout->addWidget(btn);
	layout->addWidget(light);
	layout->addWidget(dark);
	w.setWindowTitle(QStringLiteral("QThemeEngine L0 Demo"));
	w.resize(280, 160);
	w.show();
	return app.exec();
}
