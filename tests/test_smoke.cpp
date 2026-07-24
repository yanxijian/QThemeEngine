#include "qtheme/accent.hpp"
#include "qtheme/color_util.hpp"
#include "qtheme/engine.hpp"
#include "qtheme/pack.hpp"
#include "qtheme/settings.hpp"
#include "qtheme/store.hpp"
#include "qtheme/style.hpp"
#include "qtheme/types.hpp"
#include "theme/themeloader.hpp"

#include <QApplication>
#include <QCheckBox>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QLineEdit>
#include <QPainter>
#include <QPixmap>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSettings>
#include <QStyle>
#include <QStyleOptionFrame>
#include <QTemporaryDir>
#include <QtTest>

#include <memory>

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
	void settings_appearancePrefsRoundTrip();
	void pack_registerDirectory();
	void style_pushButtonLightDiffersFromDark();
	void style_checkBoxCheckedNearAccent();
	void style_m6MetricsFromPack();
	void style_textEditFrameLightDiffersFromDark();
	void setupXml_deferredToM1_compatStub();
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
	QVERIFY(s.hasColor(QStringLiteral("view"), QStringLiteral("bg.selected")));
	QVERIFY(s.hasColor(QStringLiteral("view"), QStringLiteral("bg.alternate")));
	QVERIFY(s.hasColor(QStringLiteral("view"), QStringLiteral("bg.hover")));
	QVERIFY(s.hasColor(QStringLiteral("view"), QStringLiteral("fg.selected")));
	QCOMPARE(s.metric(QStringLiteral("view"), QStringLiteral("itemHeight"), -1), 28);
	QCOMPARE(s.metric(QStringLiteral("toolbar"), QStringLiteral("handleExtent"), -1), 8);
	QCOMPARE(s.metric(QStringLiteral("tooltip"), QStringLiteral("padding"), -1), 6);
	QCOMPARE(s.metric(QStringLiteral("splitter"), QStringLiteral("width"), -1), 6);
	QVERIFY(s.hasColor(QStringLiteral("textedit"), QStringLiteral("border.focus")));
	QVERIFY(s.hasColor(QStringLiteral("dock"), QStringLiteral("title.bg")));
	QVERIFY(s.hasColor(QStringLiteral("dial"), QStringLiteral("fill")));

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

void ThemeSmokeTest::settings_appearancePrefsRoundTrip()
{
	QTemporaryDir tmp;
	QVERIFY(tmp.isValid());
	const QString ini = tmp.filePath(QStringLiteral("prefs.ini"));
	QSettings settings(ini, QSettings::IniFormat);

	qtheme::AppearancePrefs prefs;
	prefs.skinId = QString::fromUtf8(qtheme::kPackFluentDark);
	prefs.colorScheme = qtheme::ColorScheme::Dark;
	prefs.accentFollowSystem = false;
	prefs.accent = QColor(QStringLiteral("#AABBCC"));
	prefs.followOsHighContrast = false;
	prefs.packSearchPaths = QStringList{QStringLiteral("/tmp/packs")};
	prefs.extraPackFiles = QStringList{QStringLiteral("/tmp/custom.theme.json")};
	QVERIFY(qtheme::saveAppearancePrefs(&settings, prefs));

	qtheme::AppearancePrefs loaded;
	QVERIFY(qtheme::loadAppearancePrefs(&settings, &loaded));
	QCOMPARE(loaded.skinId, prefs.skinId);
	QCOMPARE(static_cast<int>(loaded.colorScheme), static_cast<int>(prefs.colorScheme));
	QCOMPARE(loaded.accentFollowSystem, false);
	QCOMPARE(loaded.accent, prefs.accent);
	QCOMPARE(loaded.followOsHighContrast, false);
	QCOMPARE(loaded.packSearchPaths, prefs.packSearchPaths);
	QCOMPARE(loaded.extraPackFiles, prefs.extraPackFiles);

	qtheme::Engine engine;
	QVERIFY(engine.applyAppearancePrefs(loaded));
	QCOMPARE(engine.currentSkin(), QString::fromUtf8(qtheme::kPackFluentDark));
	QCOMPARE(static_cast<int>(engine.colorScheme()), static_cast<int>(qtheme::ColorScheme::Dark));
	QCOMPARE(engine.accent(), QColor(QStringLiteral("#AABBCC")));
	QVERIFY(!engine.accentFollowsSystem());
}

void ThemeSmokeTest::pack_registerDirectory()
{
	QTemporaryDir tmp;
	QVERIFY(tmp.isValid());
	const QString packPath = tmp.filePath(QStringLiteral("ext.brand.theme.json"));
	QFile f(packPath);
	QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
	f.write(R"({
  "id": "ext.brand",
  "displayName": "Ext Brand",
  "base": "fluent.light",
  "colors": { "palette": { "accent": "#FF00AA" } }
})");
	f.close();

	qtheme::PackRegistry reg;
	QVERIFY(reg.registerBuiltinFluentPacks());
	QCOMPARE(reg.registerPacksInDirectory(tmp.path()), 1);
	QVERIFY(reg.hasPack(QStringLiteral("ext.brand")));
	qtheme::ThemeStore store;
	QVERIFY(reg.materialize(QStringLiteral("ext.brand"), &store, nullptr));
	QCOMPARE(store.color(QStringLiteral("palette"), QStringLiteral("accent")).value,
			 QColor(QStringLiteral("#FF00AA")));
}

void ThemeSmokeTest::style_pushButtonLightDiffersFromDark()
{
	auto* app = qobject_cast<QApplication*>(QCoreApplication::instance());
	QVERIFY(app);
	qtheme::Engine engine;
	engine.apply(app);
	QVERIFY(engine.setAccentFollowSystem(false));
	QVERIFY(engine.setAccent(QColor(QStringLiteral("#0078D4"))));

	QPushButton btn(QStringLiteral("Sample"));
	btn.resize(160, 36);

	QVERIFY(engine.setColorScheme(qtheme::ColorScheme::Light));
	btn.ensurePolished();
	const QColor lightBg = btn.grab().toImage().pixelColor(80, 18);

	QVERIFY(engine.setColorScheme(qtheme::ColorScheme::Dark));
	btn.ensurePolished();
	const QColor darkBg = btn.grab().toImage().pixelColor(80, 18);

	QVERIFY(lightBg != darkBg);
	QVERIFY(lightBg.lightness() > darkBg.lightness());
}

void ThemeSmokeTest::style_checkBoxCheckedNearAccent()
{
	auto* app = qobject_cast<QApplication*>(QCoreApplication::instance());
	QVERIFY(app);
	qtheme::Engine engine;
	engine.apply(app);
	QVERIFY(engine.setColorScheme(qtheme::ColorScheme::Light));
	QVERIFY(engine.setAccentFollowSystem(false));
	const QColor accent(QStringLiteral("#0078D4"));
	QVERIFY(engine.setAccent(accent));

	QCheckBox box(QStringLiteral("On"));
	box.setChecked(true);
	box.resize(120, 28);
	box.ensurePolished();
	const QImage img = box.grab().toImage().convertToFormat(QImage::Format_ARGB32);

	bool found = false;
	for (int y = 0; y < img.height() && !found; ++y)
	{
		for (int x = 0; x < qMin(28, img.width()); ++x)
		{
			const QColor c = img.pixelColor(x, y);
			const int dist = qAbs(c.red() - accent.red()) + qAbs(c.green() - accent.green())
							 + qAbs(c.blue() - accent.blue());
			if (dist < 100)
			{
				found = true;
				break;
			}
		}
	}
	QVERIFY(found);
}

void ThemeSmokeTest::style_m6MetricsFromPack()
{
	auto store = std::make_shared<qtheme::ThemeStore>();
	QVERIFY(qtheme::ThemeStore::loadBuiltinPack(QString::fromUtf8(qtheme::kPackFluentLight), store.get()));
	qtheme::QThemeStyle style(store);
	style.setDpiScale(1.0);
	QCOMPARE(style.pixelMetric(QStyle::PM_ToolBarHandleExtent), 8);
	QCOMPARE(style.pixelMetric(QStyle::PM_ToolBarItemSpacing), 4);
	QCOMPARE(style.pixelMetric(QStyle::PM_ToolBarFrameWidth), 1);
	QCOMPARE(style.pixelMetric(QStyle::PM_ToolTipLabelFrameWidth), 6);
	QCOMPARE(style.pixelMetric(QStyle::PM_SplitterWidth), 6);
	QCOMPARE(style.pixelMetric(QStyle::PM_DockWidgetFrameWidth), 1);
	style.setDpiScale(2.0);
	QCOMPARE(style.pixelMetric(QStyle::PM_ToolTipLabelFrameWidth), 12);
	QCOMPARE(style.pixelMetric(QStyle::PM_SplitterWidth), 12);
}

void ThemeSmokeTest::style_textEditFrameLightDiffersFromDark()
{
	auto lightStore = std::make_shared<qtheme::ThemeStore>();
	auto darkStore = std::make_shared<qtheme::ThemeStore>();
	QVERIFY(qtheme::ThemeStore::loadBuiltinPack(QString::fromUtf8(qtheme::kPackFluentLight), lightStore.get()));
	QVERIFY(qtheme::ThemeStore::loadBuiltinPack(QString::fromUtf8(qtheme::kPackFluentDark), darkStore.get()));
	const QColor lightToken = lightStore->color(QStringLiteral("textedit"), QStringLiteral("bg")).value;
	const QColor darkToken = darkStore->color(QStringLiteral("textedit"), QStringLiteral("bg")).value;
	QVERIFY(lightToken != darkToken);

	auto paintFrame = [](const std::shared_ptr<qtheme::ThemeStore>& store) -> QColor
	{
		qtheme::QThemeStyle style(store);
		QPlainTextEdit edit;
		edit.resize(120, 48);
		QPixmap pm(120, 48);
		pm.fill(Qt::magenta);
		QPainter painter(&pm);
		QStyleOptionFrame opt;
		opt.initFrom(&edit);
		opt.rect = QRect(0, 0, 120, 48);
		opt.frameShape = QFrame::StyledPanel;
		opt.state |= QStyle::State_Enabled;
		style.drawPrimitive(QStyle::PE_Frame, &opt, &painter, &edit);
		painter.end();
		return pm.toImage().pixelColor(4, 4);
	};

	const QColor lightPx = paintFrame(lightStore);
	const QColor darkPx = paintFrame(darkStore);
	QVERIFY(lightPx != darkPx);
	QVERIFY(lightPx.lightness() > darkPx.lightness());
}

void ThemeSmokeTest::setupXml_deferredToM1_compatStub()
{
	// M1 XML Format remains an optional compatibility stub; JSON Theme Packs are SSOT.
	theme::ThemeLoader loader;
	theme::ThemeError err = theme::ThemeError::None;
	const bool ok = loader.setupXml(QStringLiteral(":/theme/app.theme.xml"), QStringLiteral("light"), {}, &err);
	QVERIFY(!ok);
}

QTEST_MAIN(ThemeSmokeTest)
#include "test_smoke.moc"
