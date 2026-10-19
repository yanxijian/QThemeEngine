# QThemeEngine

**日本語** · [English](README.md) · [简体中文](README.zh-CN.md) · [한국어](README.ko.md)

[![CI](https://github.com/yanxijian/QThemeEngine/actions/workflows/ci.yml/badge.svg)](https://github.com/yanxijian/QThemeEngine/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

業務用 Qt Widgets 向けのテーマランタイム：**ThemeStore + 独自の `QThemeStyle`** で **QSS を置き換えます**（スタイルシートを生成するわけではありません）。

## 特長

- **QSS を使わない描画経路**：`Engine::apply` がスタイルシートを消去し、描画は `QThemeStyle`（`QProxyStyle` + Fusion）を通ります
- **ThemeStore**：グループ／ロールから色と寸法を引く仕組み。Fluent パック（ライト／ダーク／ハイコントラスト）が唯一の正（`*.theme.json`）
- **OS との連携**：アクセントカラー／カラースキーム、QSettings による設定保存、パックの検索パス
- **製品としての配布**：`find_package(QThemeEngine)` → `QThemeEngine::engine`
- **ギャラリーのデモ**：`examples/native_controls` に Fluent のコントロール一覧

## 動作要件

| 項目 | 内容 |
|------|------|
| Qt | **6.8 以降**（Widgets。単体テストには Test） |
| ツールチェーン | CMake 3.21 以降、Ninja。Windows では MSVC x64（`vcvars`） |
| 任意 | ローカルの書式チェック用に `clang-format` 20 |

Windows のシステムライブラリは `#pragma comment(lib, …)` ではなく、`CMakeLists.txt` の `target_link_libraries` でリンクしてください。`.cursor/rules/cmake-windows-libs.mdc` を参照。

## クイックスタート（Windows）

最小の使い方：

```cpp
QApplication app(argc, argv);
QCoreApplication::setOrganizationName(QStringLiteral("MyOrg"));
QCoreApplication::setApplicationName(QStringLiteral("MyApp"));

qtheme::Engine engine;
engine.apply(&app);
engine.loadPreferences();
engine.setAutoSavePreferences(true);
engine.switchPack(QString::fromUtf8(qtheme::kPackFluentDark));
```

このエンジンでテーマを当てている UI に `setStyleSheet` を混ぜないでください。色リテラルは `#RRGGBB` / `#RRGGBBAA` です（Qt の `#AARRGGBB` ではありません）。

ローカルの共有ライブラリ運用は **`build-shared`** を使います（CI は `-B build` のこともあります）。

```bat
:: 先に vcvars x64。既定は共有ライブラリで、ローカルのプレフィックスへインストール
set QTDIR=<Qt-6.8+-prefix>
set PREFIX=<install-prefix>
cmake -S . -B build-shared -G Ninja ^
  -DCMAKE_PREFIX_PATH=%QTDIR% ^
  -DCMAKE_INSTALL_PREFIX=%PREFIX% ^
  -DQTE_BUILD_SHARED=ON -DQTE_INSTALL=ON
cmake --build build-shared
cmake --install build-shared
ctest --test-dir build-shared --output-on-failure
build-shared\qte_demo.exe
```

| CMake オプション | 役割 |
|--------------|------|
| `QTE_BUILD_SHARED` | 共有ライブラリ（既定 ON）。出力名は `qte_engine` |
| `QTE_BUILD_EXAMPLES` | デモ（`qte_demo`） |
| `QTE_BUILD_TESTS` | 単体テスト（`qte_tests`） |
| `QTE_BUILD_WIDGETS` | サンプルウィジェットのターゲット `qte_demowidgets` |
| `QTE_INSTALL` | インストールと Config パッケージ（`QThemeEngine::engine`） |

[ci.md](docs/en/ci.md) も参照。ローカルの整形確認は `python scripts/format_source.py --check`。

## リポジトリ構成

```text
include/qtheme/           公開 API（Store / Style / Engine / Pack / Settings）
src/qtheme/               実装（style/* はコントロール種別ごと）
resources/themes/fluent/  Fluent テーマパックの JSON（唯一の正）
examples/native_controls/ Fluent ギャラリー
widgets/                  自前描画の DemoButton（補助）
cmake/                    QThemeEngineConfig.cmake.in
docs/zh|en/               中国語と英語のドキュメント
```

## ドキュメント

| 内容 | English | 简体中文 |
|------|---------|----------|
| アーキテクチャ（正） | [architecture.md](docs/en/architecture.md) | [architecture.md](docs/zh/architecture.md) |
| 開発計画 | [dev-plan.md](docs/en/dev-plan.md) | [dev-plan.md](docs/zh/dev-plan.md) |
| カバレッジ表 | — | [coverage-matrix.md](docs/zh/coverage-matrix.md) |
| ウィジェット一覧 | [qt-widgets-inventory.md](docs/en/qt-widgets-inventory.md) | [qt-widgets-inventory.md](docs/zh/qt-widgets-inventory.md) |
| 旧フォーマット仕様（保管） | — | [theme-engine-spec.md](docs/zh/theme-engine-spec.md) |
| CI | [ci.md](docs/en/ci.md) | [ci.md](docs/zh/ci.md) |

長文の設計メモは英語と簡体字中国語の 2 言語のみで管理しています。この README が 4 言語で同期を保つ文書です。

**方針**：製品としての境界は [architecture.md](docs/zh/architecture.md) に従います。

## ステータス

| 機能 | 状態 |
|------|------|
| Store の初期化 + `Engine::apply` + QThemeStyle（M0） | 完了 |
| Fluent パック + アクセント／カラースキーム + パックの合成（M0.5） | 完了 |
| 主要な Fluent コントロールの描画（M2〜M4） | 完了 |
| 設定保存、パック検索パス、`find_package` による導入（M5） | 完了 |
| 対応範囲の拡大（TextEdit / Frame / Splitter / Dock / Dial、M6） | 完了 |
| 仕上げ | [dev-plan](docs/en/dev-plan.md) を参照 |

> 以前の **M1**（`.theme.xml` フォーマット）は中止しました。現在は JSON パックのみです。

## ライセンス

[MIT License](LICENSE) で公開しています。
