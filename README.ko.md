# QThemeEngine

**한국어** · [English](README.md) · [简体中文](README.zh-CN.md) · [日本語](README.ja.md)

[![CI](https://github.com/yanxijian/QThemeEngine/actions/workflows/ci.yml/badge.svg)](https://github.com/yanxijian/QThemeEngine/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Qt](https://img.shields.io/badge/Qt-6.8%2B-green.svg)](https://www.qt.io/)

<!-- translated-from: README.md sha256:f87638758811 -->


업무용 Qt Widgets 테마 런타임: **ThemeStore + 자체 `QThemeStyle`** 로 **QSS를 대체**합니다(스타일시트를 생성하지는 않습니다).

## 기능

- **QSS 없는 그리기 경로**: `Engine::apply`가 스타일시트를 지우고, 그리기는 `QThemeStyle`(`QProxyStyle` + Fusion)을 거칩니다
- **ThemeStore**: 그룹/역할로 색과 치수를 조회. Fluent 팩(라이트/다크/고대비)이 단일 기준(`*.theme.json`)
- **OS 연동**: 강조색/색 구성표, QSettings 기반 설정 저장, 팩 검색 경로
- **제품으로 배포**: `find_package(QThemeEngine)` → `QThemeEngine::engine`
- **갤러리 데모**: `examples/native_controls`의 Fluent 컨트롤 갤러리

## 요구 사항

| 항목 | 내용 |
|------|------|
| Qt | **6.8 이상**(Widgets. 단위 테스트에는 Test) |
| 툴체인 | CMake 3.21 이상, Ninja. Windows에서는 MSVC x64(`vcvars`) |
| 선택 | 로컬 서식 검사용 `clang-format` 20 |

Windows 시스템 라이브러리는 `#pragma comment(lib, …)` 대신 `CMakeLists.txt`의 `target_link_libraries`로 연결하세요. `.cursor/rules/cmake-windows-libs.mdc`를 참고하세요.

## 빠른 시작(Windows)

최소 사용 예:

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

이 엔진이 테마를 입히는 UI에 `setStyleSheet`를 섞지 마세요. 색 리터럴은 `#RRGGBB` / `#RRGGBBAA`입니다(Qt의 `#AARRGGBB`가 아님).

로컬 공유 라이브러리 관례는 **`build-shared`** 를 씁니다(CI는 `-B build`일 수 있음).

```bat
:: 먼저 vcvars x64. 기본은 공유 라이브러리이며 로컬 프리픽스에 설치
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

| CMake 옵션 | 역할 |
|--------------|------|
| `QTE_BUILD_SHARED` | 공유 라이브러리(기본 ON). 출력 이름 `qte_engine` |
| `QTE_BUILD_EXAMPLES` | 데모(`qte_demo`) |
| `QTE_BUILD_TESTS` | 단위 테스트(`qte_tests`) |
| `QTE_BUILD_WIDGETS` | 예제 위젯 타깃 `qte_demowidgets` |
| `QTE_INSTALL` | 설치와 Config 패키지(`QThemeEngine::engine`) |

[ci.md](docs/en/ci.md)도 보세요. 로컬 서식 확인은 `python scripts/format_source.py --check`.

## 저장소 구조

```text
include/qtheme/           공개 API(Store / Style / Engine / Pack / Settings)
src/qtheme/               구현(style/*는 컨트롤 종류별)
resources/themes/fluent/  Fluent 테마 팩 JSON(단일 기준)
examples/native_controls/ Fluent 갤러리
widgets/                  직접 그리는 DemoButton(보조)
cmake/                    QThemeEngineConfig.cmake.in
docs/zh|en/               중국어와 영어 문서
```

## 문서

| 주제 | English | 简体中文 |
|------|---------|----------|
| 아키텍처(기준) | [architecture.md](docs/en/architecture.md) | [architecture.md](docs/zh/architecture.md) |
| 개발 계획 | [dev-plan.md](docs/en/dev-plan.md) | [dev-plan.md](docs/zh/dev-plan.md) |
| 커버리지 표 | — | [coverage-matrix.md](docs/zh/coverage-matrix.md) |
| 위젯 목록 | [qt-widgets-inventory.md](docs/en/qt-widgets-inventory.md) | [qt-widgets-inventory.md](docs/zh/qt-widgets-inventory.md) |
| 옛 형식 사양(보관) | — | [theme-engine-spec.md](docs/zh/theme-engine-spec.md) |
| CI | [ci.md](docs/en/ci.md) | [ci.md](docs/zh/ci.md) |

긴 설계 문서는 영어와 중국어 간체 두 언어로만 관리합니다. 이 README가 네 언어 사이에서 동기화를 유지하는 문서입니다.

**방침**: 제품 경계는 [architecture.md](docs/zh/architecture.md)를 따릅니다.

## 현재 상태

| 기능 | 상태 |
|------|------|
| Store 초기화 + `Engine::apply` + QThemeStyle(M0) | 완료 |
| Fluent 팩 + 강조색/색 구성표 + 팩 병합(M0.5) | 완료 |
| 주요 Fluent 컨트롤 그리기(M2~M4) | 완료 |
| 설정 저장, 팩 검색 경로, `find_package` 설치(M5) | 완료 |
| 적용 범위 확대(TextEdit / Frame / Splitter / Dock / Dial, M6) | 완료 |
| 마무리 | [dev-plan](docs/en/dev-plan.md) 참조 |

> 이전 **M1**(`.theme.xml` 형식)은 취소했습니다. 지금은 JSON 팩만 씁니다.

## 라이선스

[MIT License](LICENSE)로 배포합니다.
