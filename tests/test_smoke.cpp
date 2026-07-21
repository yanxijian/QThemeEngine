#include "theme/color_util.hpp"
#include "theme/themeloader.hpp"

#include <QCoreApplication>
#include <QtTest>

class ThemeSmokeTest : public QObject
{
	Q_OBJECT
private slots:
	void colorLiteral_rrggbbaa();
	void setupXml_notImplementedYet();
};

void ThemeSmokeTest::colorLiteral_rrggbbaa()
{
	const QColor c = theme::themeColorFromRgbaHex(QStringLiteral("#0D0D0DE5"));
	QVERIFY(c.isValid());
	QCOMPARE(c.red(), 0x0D);
	QCOMPARE(c.green(), 0x0D);
	QCOMPARE(c.blue(), 0x0D);
	QCOMPARE(c.alpha(), 0xE5);
}

void ThemeSmokeTest::setupXml_notImplementedYet()
{
	theme::ThemeLoader loader;
	theme::ThemeError err = theme::ThemeError::None;
	const bool ok = loader.setupXml(QStringLiteral(":/theme/app.theme.xml"), QStringLiteral("light"), {}, &err);
	// Placeholder until L0 ThemeLoader is implemented (docs/zh/style.md T2–T5).
	QVERIFY(!ok);
}

QTEST_MAIN(ThemeSmokeTest)
#include "test_smoke.moc"
