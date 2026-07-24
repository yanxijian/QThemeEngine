#include "qtheme/accent.hpp"
#include "qtheme/color_util.hpp"
#include "qtheme/engine.hpp"
#include "qtheme/pack.hpp"
#include "qtheme/store.hpp"
#include "qtheme/style.hpp"
#include "qtheme/types.hpp"
#include "theme/themeloader.hpp"

#include <QCoreApplication>
#include <QtTest>

class ThemeSmokeTest : public QObject
{
	Q_OBJECT
private slots:
	void colorLiteral_rrggbbaa();
	void store_fluentLightHasRequiredTokens();
	void store_fluentDarkDiffersFromLight();
	void store_fluentHcHasZeroRadius();
	void store_beginUpdateBatchesGeneration();
	void pack_materializeLightFromJson();
	void pack_userSampleDerivesFromLight();
	void pack_t0ChromeTokensPresent();
	void style_dpiScaleAffectsMetrics();
	void accent_patchUpdatesHighlight();
	void accent_systemHighContrastApi();
	void engine_switchFluentSkins();
	void engine_userPackKeepsColorScheme();
	void engine_setColorSchemeSystemPreserved();
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

void ThemeSmokeTest::store_fluentLightHasRequiredTokens()
{
	qtheme::ThemeStore s;
	QVERIFY(qtheme::ThemeStore::loadBuiltinPack(QString::fromUtf8(qtheme::kPackFluentLight), &s));
	QVERIFY(s.missingRequiredColors().isEmpty());
	QVERIFY(s.hasColor(QStringLiteral("button"), QStringLiteral("bg")));
	QVERIFY(s.hasColor(QStringLiteral("palette"), QStringLiteral("accent")));
	QCOMPARE(s.metric(QStringLiteral("button"), QStringLiteral("radius"), -1), 4);
}

void ThemeSmokeTest::store_fluentDarkDiffersFromLight()
{
	const auto light = qtheme::ThemeStore::seedLight();
	const auto dark = qtheme::ThemeStore::seedDark();
	const auto lb = light.color(QStringLiteral("button"), QStringLiteral("bg"));
	const auto db = dark.color(QStringLiteral("button"), QStringLiteral("bg"));
	QVERIFY(lb.ok && db.ok);
	QVERIFY(lb.value != db.value);
}

void ThemeSmokeTest::store_fluentHcHasZeroRadius()
{
	qtheme::ThemeStore hc;
	QVERIFY(qtheme::ThemeStore::loadBuiltinPack(QString::fromUtf8(qtheme::kPackFluentHc), &hc));
	QCOMPARE(hc.metric(QStringLiteral("button"), QStringLiteral("radius"), -1), 0);
	QVERIFY(hc.hasColor(QStringLiteral("palette"), QStringLiteral("accent")));
}

void ThemeSmokeTest::store_beginUpdateBatchesGeneration()
{
	qtheme::ThemeStore s;
	const auto g0 = s.generation();
	s.beginUpdate();
	s.setColor(QStringLiteral("palette"), QStringLiteral("accent"), QColor(Qt::red));
	s.setColor(QStringLiteral("palette"), QStringLiteral("highlight"), QColor(Qt::blue));
	s.setMetric(QStringLiteral("button"), QStringLiteral("radius"), 2);
	QCOMPARE(s.generation(), g0);
	s.endUpdate();
	QCOMPARE(s.generation(), g0 + 1);
}

void ThemeSmokeTest::pack_materializeLightFromJson()
{
	qtheme::PackRegistry reg;
	QVERIFY(reg.registerBuiltinFluentPacks());
	qtheme::ThemeStore store;
	qtheme::Error err = qtheme::Error::None;
	QVERIFY(reg.materialize(QString::fromUtf8(qtheme::kPackFluentLight), &store, &err));
	QCOMPARE(static_cast<int>(err), static_cast<int>(qtheme::Error::None));
	QVERIFY(store.missingRequiredColors().isEmpty());
}

void ThemeSmokeTest::pack_userSampleDerivesFromLight()
{
	qtheme::PackRegistry reg;
	QVERIFY(reg.registerBuiltinFluentPacks());
	qtheme::ThemeStore store;
	QVERIFY(reg.materialize(QStringLiteral("user.sample"), &store, nullptr));
	const auto accent = store.color(QStringLiteral("palette"), QStringLiteral("accent"));
	QVERIFY(accent.ok);
	QCOMPARE(accent.value, QColor(QStringLiteral("#D83B01")));
	QVERIFY(store.hasColor(QStringLiteral("button"), QStringLiteral("bg")));
}

void ThemeSmokeTest::pack_t0ChromeTokensPresent()
{
	qtheme::ThemeStore s;
	QVERIFY(qtheme::ThemeStore::loadBuiltinPack(QString::fromUtf8(qtheme::kPackFluentLight), &s));
	QVERIFY(s.hasColor(QStringLiteral("menu"), QStringLiteral("bg")));
	QVERIFY(s.hasColor(QStringLiteral("menu"), QStringLiteral("bg.hover")));
	QVERIFY(s.hasColor(QStringLiteral("menu"), QStringLiteral("fg.disabled")));
	QVERIFY(s.hasColor(QStringLiteral("toolbar"), QStringLiteral("bg")));
	QVERIFY(s.hasColor(QStringLiteral("combo"), QStringLiteral("bg.hover")));
	QVERIFY(s.hasColor(QStringLiteral("combo"), QStringLiteral("border.focus")));
	QVERIFY(s.hasColor(QStringLiteral("header"), QStringLiteral("bg.pressed")));
	QVERIFY(s.hasColor(QStringLiteral("check"), QStringLiteral("bg.disabled")));
	QVERIFY(s.hasColor(QStringLiteral("tab"), QStringLiteral("fg.disabled")));
	QVERIFY(s.hasColor(QStringLiteral("scroll"), QStringLiteral("handle.disabled")));
	QVERIFY(s.hasColor(QStringLiteral("spin"), QStringLiteral("button.hover")));
	QVERIFY(s.hasColor(QStringLiteral("spin"), QStringLiteral("border.focus")));
	QCOMPARE(s.metric(QStringLiteral("spin"), QStringLiteral("buttonWidth"), -1), 20);
	QCOMPARE(s.metric(QStringLiteral("button"), QStringLiteral("height"), -1), 32);
	QVERIFY(s.hasColor(QStringLiteral("slider"), QStringLiteral("fill")));
	QVERIFY(s.hasColor(QStringLiteral("progress"), QStringLiteral("chunk")));
	QVERIFY(s.hasColor(QStringLiteral("groupbox"), QStringLiteral("border")));
	QVERIFY(s.hasColor(QStringLiteral("tooltip"), QStringLiteral("bg")));
	QCOMPARE(s.metric(QStringLiteral("slider"), QStringLiteral("handle"), -1), 16);

	qtheme::ThemeStore dark;
	QVERIFY(qtheme::ThemeStore::loadBuiltinPack(QString::fromUtf8(qtheme::kPackFluentDark), &dark));
	QVERIFY(dark.hasColor(QStringLiteral("combo"), QStringLiteral("arrow")));
	qtheme::ThemeStore hc;
	QVERIFY(qtheme::ThemeStore::loadBuiltinPack(QString::fromUtf8(qtheme::kPackFluentHc), &hc));
	QCOMPARE(hc.metric(QStringLiteral("combo"), QStringLiteral("radius"), -1), 0);
	QCOMPARE(hc.metric(QStringLiteral("menu"), QStringLiteral("radius"), -1), 0);
	QCOMPARE(hc.metric(QStringLiteral("slider"), QStringLiteral("radius"), -1), 0);
}

void ThemeSmokeTest::style_dpiScaleAffectsMetrics()
{
	auto store = std::make_shared<qtheme::ThemeStore>();
	QVERIFY(qtheme::ThemeStore::loadBuiltinPack(QString::fromUtf8(qtheme::kPackFluentLight), store.get()));
	qtheme::QThemeStyle style(store);
	style.setDpiScale(1.0);
	QCOMPARE(style.pixelMetric(QStyle::PM_SliderLength), 16);
	style.setDpiScale(2.0);
	QCOMPARE(style.pixelMetric(QStyle::PM_SliderLength), 32);
	style.setDpiScale(1.0);
	QCOMPARE(style.pixelMetric(QStyle::PM_ButtonMargin), 10);
	style.setDpiScale(1.5);
	QCOMPARE(style.pixelMetric(QStyle::PM_ButtonMargin), 15);
}

void ThemeSmokeTest::accent_patchUpdatesHighlight()
{
	qtheme::ThemeStore store = qtheme::ThemeStore::seedLight();
	const auto g0 = store.generation();
	const QColor accent(QStringLiteral("#AA00AA"));
	qtheme::AccentResolver::applyAccentPatch(&store, accent);
	QCOMPARE(store.color(QStringLiteral("palette"), QStringLiteral("highlight")).value, accent);
	QCOMPARE(store.color(QStringLiteral("button"), QStringLiteral("border.focus")).value, accent);
	// Accent patch is one generation bump (batched), not one per role.
	QCOMPARE(store.generation(), g0 + 1);
}

void ThemeSmokeTest::accent_systemHighContrastApi()
{
	// API must be callable; value is OS-dependent.
	(void)qtheme::AccentResolver::systemHighContrast();
	QVERIFY(true);
}

void ThemeSmokeTest::engine_switchFluentSkins()
{
	qtheme::Engine engine;
	QVERIFY(engine.switchSkin(QStringLiteral("fluent.dark")));
	QCOMPARE(engine.currentSkin(), QString::fromUtf8(qtheme::kPackFluentDark));
	QCOMPARE(static_cast<int>(engine.colorScheme()), static_cast<int>(qtheme::ColorScheme::Dark));
	QVERIFY(engine.switchSkin(QStringLiteral("fluent.hc")));
	QCOMPARE(engine.currentSkin(), QString::fromUtf8(qtheme::kPackFluentHc));
	QCOMPARE(static_cast<int>(engine.colorScheme()), static_cast<int>(qtheme::ColorScheme::HighContrast));
	QVERIFY(engine.setAccent(QColor(QStringLiteral("#112233"))));
	QCOMPARE(engine.accent(), QColor(QStringLiteral("#112233")));
	QVERIFY(engine.store()->color(QStringLiteral("palette"), QStringLiteral("accent")).value
			== QColor(QStringLiteral("#112233")));
}

void ThemeSmokeTest::engine_userPackKeepsColorScheme()
{
	qtheme::Engine engine;
	QVERIFY(engine.setColorScheme(qtheme::ColorScheme::Dark));
	QCOMPARE(static_cast<int>(engine.colorScheme()), static_cast<int>(qtheme::ColorScheme::Dark));
	QVERIFY(engine.switchSkin(QStringLiteral("user.sample")));
	QCOMPARE(engine.currentSkin(), QStringLiteral("user.sample"));
	// Derived/user packs must not clobber the prior ColorScheme intent.
	QCOMPARE(static_cast<int>(engine.colorScheme()), static_cast<int>(qtheme::ColorScheme::Dark));
}

void ThemeSmokeTest::engine_setColorSchemeSystemPreserved()
{
	qtheme::Engine engine;
	int schemeEmits = 0;
	QObject::connect(&engine, &qtheme::Engine::colorSchemeChanged, &engine,
					 [&schemeEmits]
					 {
						 ++schemeEmits;
					 });
	QVERIFY(engine.setColorScheme(qtheme::ColorScheme::System));
	QCOMPARE(static_cast<int>(engine.colorScheme()), static_cast<int>(qtheme::ColorScheme::System));
	QCOMPARE(schemeEmits, 1);
	// Idempotent: same scheme + resolved pack should not re-emit.
	QVERIFY(engine.setColorScheme(qtheme::ColorScheme::System));
	QCOMPARE(schemeEmits, 1);
}

void ThemeSmokeTest::setupXml_deferredToM1()
{
	theme::ThemeLoader loader;
	theme::ThemeError err = theme::ThemeError::None;
	const bool ok = loader.setupXml(QStringLiteral(":/theme/app.theme.xml"), QStringLiteral("light"), {}, &err);
	QVERIFY(!ok);
}

QTEST_MAIN(ThemeSmokeTest)
#include "test_smoke.moc"
