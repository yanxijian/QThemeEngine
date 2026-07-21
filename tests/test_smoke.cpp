#include "qtheme/color_util.hpp"
#include "qtheme/store.hpp"
#include "theme/themeloader.hpp"

#include <QCoreApplication>
#include <QtTest>

class ThemeSmokeTest : public QObject
{
	Q_OBJECT
private slots:
	void colorLiteral_rrggbbaa();
	void store_seedLightHasButtonRoles();
	void store_seedDarkDiffersFromLight();
	void setupXml_deferredToM1();
};

void ThemeSmokeTest::colorLiteral_rrggbbaa()
{
	const QColor c = qtheme::colorFromRgbaHex(QStringLiteral("#0D0D0DE5"));
	QVERIFY(c.isValid());
	QCOMPARE(c.red(), 0x0D);
	QCOMPARE(c.green(), 0x0D);
	QCOMPARE(c.blue(), 0x0D);
	QCOMPARE(c.alpha(), 0xE5);
}

void ThemeSmokeTest::store_seedLightHasButtonRoles()
{
	const qtheme::ThemeStore s = qtheme::ThemeStore::seedLight();
	QVERIFY(s.hasColor(QStringLiteral("button"), QStringLiteral("bg")));
	QVERIFY(s.hasColor(QStringLiteral("button"), QStringLiteral("bg.hover")));
	const auto bg = s.color(QStringLiteral("button"), QStringLiteral("bg"));
	QVERIFY(bg.ok);
	QCOMPARE(bg.value, QColor(QStringLiteral("#E8E8E8")));
	QCOMPARE(s.metric(QStringLiteral("button"), QStringLiteral("radius"), -1), 6);
}

void ThemeSmokeTest::store_seedDarkDiffersFromLight()
{
	const auto light = qtheme::ThemeStore::seedLight();
	const auto dark = qtheme::ThemeStore::seedDark();
	const auto lb = light.color(QStringLiteral("button"), QStringLiteral("bg"));
	const auto db = dark.color(QStringLiteral("button"), QStringLiteral("bg"));
	QVERIFY(lb.ok && db.ok);
	QVERIFY(lb.value != db.value);
}

void ThemeSmokeTest::setupXml_deferredToM1()
{
	theme::ThemeLoader loader;
	theme::ThemeError err = theme::ThemeError::None;
	const bool ok = loader.setupXml(QStringLiteral(":/theme/app.theme.xml"), QStringLiteral("light"), {}, &err);
	// Format loader is M1; M0 uses programmatic ThemeStore seeds.
	QVERIFY(!ok);
}

QTEST_MAIN(ThemeSmokeTest)
#include "test_smoke.moc"
