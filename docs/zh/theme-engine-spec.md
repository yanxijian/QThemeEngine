# 桌面应用主题换肤与样式机制说明

> **English index**：[../en/README.md](../en/README.md)  
> 日期：2026-07

## 目录

**第一部分 · 机制说明**：§1 结论 · §2 配置→像素主链路 · §3 架构 · §4 主题 XML · §5 皮肤包 · §6 换肤 · §7 跨进程 · §8–§9 控件/扩展包 · §10–§15 其它

**第二部分 · 实现规格**：R0–R15

**第三部分 · L0 可生成规格**：T0 技术栈 · T1 接口 · T2 算法 · T3 Golden · T4 控件 · T5 验收 · T6 工程骨架

---

## 1. 一句话结论

桌面客户端（尤其是 Qt 自绘 UI）的换肤，通常**不是**靠 Qt StyleSheet「换一套 CSS」，而是：

1. 用 **主题 XML** 声明「样式类 → 颜色 / 字体 / 圆角 / 图片…」  
2. 用 **主题加载器** 解析进内存哈希表  
3. 用 **皮肤管理器** 整包切换主题数据 + 图标资源 +（可选）QStyle  
4. 控件通过 **`themeClassName` + 统一取色 API** 在绘制时按名取属性  

换肤 = 换数据源；控件代码只认 `className + propName`。

---

## 2. 核心流程：从主题配置到控件像素

真正连起来的是下面这条链。以标准组件库中的 `ThemedButton` 为例；自绘控件路径在本节末尾对照。

### 2.0 心智模型

```
【加载期，换肤时做一次】
  主题 XML
    → ThemeLoader 解析
    → styleMap["ThemedButton"] = { background→QColor, background-hover→QColor, ... }
    → Token「ComponentPalette::xxx」在解析 value 时展开成具体 QColor 存进 map

【运行期，每次 paint】
  控件知道自己的 themeClassName（如 "ThemedButton"）
    → 根据 hover/press 拼出 propName（如 "background-hover"）
    → ThemeApi::color("ThemedButton", "background-hover")
    → 从 styleMap 取出已解析好的 QColor
    → QPainter 填圆角矩形 / 画边框 / 画文字
```

**关键点**：主题 XML **不驱动**控件创建，也不挂样式表；它只是一份「按类名查表」的数据。控件在 `paintEvent` 里**主动查表再自己画**。

---

### 2.1 阶段 A：加载期 —— XML 如何进内存

#### A1. 换肤触发加载

`SkinManager::switchSkin` → `setupThemeXml` → `ThemeLoader::setupXml(主主题路径, …)`  
（还会先加载 `baseSkin` 链、调色板 import 等。）

#### A2. 解析 `<style class="ThemedButton">`

例如公共组件主题文件：

```xml
<style class="ThemedButton">
  <color name="background" value="ComponentPalette::fill-base"/>
  <color name="background-hover" value="ComponentPalette::fill-light"/>
  <color name="background-down" value="ComponentPalette::fill-regular"/>
  <color name="text" value="ComponentPalette::text-primary"/>
  <color name="border" value="ComponentPalette::line-regular"/>
</style>
```

解析时对每个 `<color>`：

1. 读 `name` → 属性键（如 `background`）  
2. 读 `value` → 交给颜色解析函数  
3. `#RRGGBB[AA]` → 直接变成 `QColor`  
4. 含 `::`（如 `ComponentPalette::fill-base`）：  
   - 拆成 `className=ComponentPalette`，`propName=fill-base`  
   - **立刻**再查表取色（因此调色板必须先于引用方加载）  
5. 写入该 style 的 `colorMap["background"]`  
6. 以 `styleMap["ThemedButton"] = ThemeStyle*` 挂表  

`base="ThemedButton"` 的派生 style（如 `ThemedButton_Danger`）只覆盖差异；查询时本级没有就沿父链向上找。

#### A3. 加载结束后内存里有什么

```
ThemeLoader::styleMap
  "AppPalette"         → { fill-base→#FFF..., text-primary→..., is-dark-skin→0, ... }
  "ComponentPalette"   → { fill-base→(已展开的色), text-primary→..., ... }
  "ThemedButton"       → { background→色, background-hover→色, text→色, border→色, ... }
  "ThemedButton_Primary" → { ... }
  "MyDialog"           → { ... }   // 扩展包注册后出现在子主题链
```

此后绘制期**不再读 XML**，只查这张表（换肤会清空再重建）。

---

### 2.2 阶段 B：绑定期 —— 控件如何挂上 class 名

```cpp
// 组件库按钮构造时
applyThemeClass("ThemedButton");   // 存成员或等价状态

// 也可改绑变体
applyThemeClass("ThemedButton_Danger");

// 自绘控件常见写法
widget->setProperty("themeClassName", "MyDialog");
```

**这一步只存字符串**，不读颜色。类名必须与主题 XML 里 `class="..."` **完全一致**。

---

### 2.3 阶段 C：绘制期 —— 从 paintEvent 到像素

以鼠标悬停触发重绘为例：

#### 步骤 1：控件 `paintEvent`

```cpp
QPainter painter(this);
ButtonStyleOption option;
initStyleOption(&option);   // 填状态、themeName、几何等
style()->drawComplexControl(CC_ThemedButton, &option, &painter, this);
// 或控件内直接自绘，不经过 QStyle
```

#### 步骤 2：把「用哪套 class」塞进 option

```cpp
option.themeName = themeClassName();   // 默认 "ThemedButton"
// option.state 含 MouseOver / Sunken / Enabled ...
```

#### 步骤 3：样式引擎分发给绘制器（若走 QStyle）

```cpp
case CC_ThemedButton:
  ThemedButtonDrawer drawer;
  drawer.draw(option, painter, widget);
```

#### 步骤 4：绘制器决定最终查表 class

```cpp
themeName = option.themeName;          // "ThemedButton"
if (Primary)  themeName += "_Primary"; // → "ThemedButton_Primary"
if (danger)   themeName += "_Danger";  // → "ThemedButton_Danger"
```

即：**控件类型/变体**映射到不同主题 `class`（须在主题文件中有定义）。

#### 步骤 5：按交互状态拼属性名，再取色

```cpp
switch (state) {
  case Hover:    prop = "background-hover"; break;
  case Down:     prop = "background-down"; break;
  case Disabled: prop = "background-disabled"; break;
  default:       prop = "background"; break;
}
brush = ThemeApi::brush(themeName, prop);
// 内部：ThemeApi::color(themeName, "background-hover")
```

#### 步骤 6：统一 API → 加载器查表

```cpp
// ThemeApi::color
return skinManager->theme()->color(className, propName, def);

// ThemeLoader::color 顺序（简化）：
//   1) hook 覆盖
//   2) 用户运行时覆盖表
//   3) 扩展子主题（后注册优先）
//   4) 主 styleMap[className]->color(prop)（含 base 继承）
```

取出的是 **加载期已展开的 `QColor`**，绘制期不再解析 `::` token。

#### 步骤 7：画到屏幕

```cpp
painter.setBrush(bgBrush);
painter.drawPath(roundedPath);     // 填背景
painter.setPen(borderColor);       // 描边
// 再画文字 / 图标 …
```

**到这一步，主题配置才变成像素。**

---

### 2.4 完整数据流对照

| 时刻 | 发生了什么 | 关键数据 |
|------|------------|----------|
| 写主题 XML | `<style class="ThemedButton"><color name="background-hover" value="ComponentPalette::…"/>` | 字符串配置 |
| switchSkin / setupXml | 解析并展开 token | `styleMap["ThemedButton"].colorMap["background-hover"]=QColor` |
| 构造按钮 | `applyThemeClass("ThemedButton")` | `themeClassName="ThemedButton"` |
| 鼠标进入 | `update()` → `paintEvent` | `state |= MouseOver` |
| initStyleOption | `themeName="ThemedButton"` | 查表键 class |
| 绘制器选色 | prop=`"background-hover"` | 查表键 prop |
| ThemeApi::color | map 查找 | `QColor` |
| drawBackground | `fillPath` / `drawPath` | 屏幕像素 |

---

### 2.5 自绘控件路径（同样原理）

不走自定义 QStyle，自己在 `paintEvent` 里查表：

```cpp
setProperty("themeClassName", "MyDialog");

void MyDialog::paintEvent(QPaintEvent*)
{
  QPainter p(this);
  QStyleOption opt; opt.initFrom(this);
  // 方式 1：复合绘制辅助（内部按 option 状态选 background / background-hover …）
  ThemeApi::drawBackground(&p, rect(), "MyDialog", &opt);
  // 方式 2：手写取色
  QColor bg = ThemeApi::color("MyDialog", "background");
  p.fillRect(rect(), bg);
}
```

与组件库路径的差别只在于：**谁负责拼状态后缀、谁调用 QPainter**；查表入口都是 `ThemeApi` → `ThemeLoader`。

也可封装：`ThemeSupports::colorWithState("background", option)`  
内部先拼出 `background-hover`，再 `ThemeApi::color(themeClassName, …)`。

---

### 2.6 换肤后为什么控件会变色

1. `switchSkin` → 清空主表  
2. 重新 `setupXml`，用新皮肤调色板 / 主题再填一遍（同一 class 名，颜色值不同）  
3. 发出 `skinChanged` → 控件 `update()`  
4. 下次 `paintEvent` 仍查 `"ThemedButton"+"background"`，但表里已是新色  

控件始终只认 **稳定的 class/prop 名字**；变的是表里的值。

---

### 2.7 最容易断的环节

| 断点 | 现象 | 原因 |
|------|------|------|
| 主题未加载 | 颜色总是默认/无效 | 未 `setupXml` / 未注册扩展包 |
| class 名不一致 | 取色失败走 default | `themeClassName` ≠ XML `class` |
| 状态后缀不一致 | hover 不变色 | XML 写 `background-hover`，代码查错后缀 |
| 调色板加载顺序错 | token 解析成无效色 | 引用 `ComponentPalette::` 时调色板尚未进表 |
| 派生 class 未定义 | Primary/Danger 外观不对 | 拼出 `ThemedButton_Primary` 但主题无此 style |
| 控件重建 | 样式丢失 | 未再次绑定 class |

---

## 3. 总体架构

```
设置界面 / 主题中心 / 宿主进程广播
            │
            ▼
   AppSkinFacade（可选：持久化、多组件维度）
   HostSkin / IPC（宿主 ↔ 子进程同步，可选）
            │
            ▼
   SkinManager                 ← 皮肤单例（通常挂在 qApp 下）
     ├── ThemeLoader           ← 主题 XML → style map
     ├── ResourceLoader        ← 图标资源包 + 搜索路径
     ├── StyleLoader           ← 可选：切换 QStyle
     ├── FontLoader
     └── 图标套色 / 固定外观表面 / 全局换肤通知…
            │
            ▼
   ThemeApi                    ← 统一查询与复合绘制入口
            │
            ▼
   组件库控件 / 自绘控件
   （ThemeSupports / paintEvent / 自定义 QStyle::drawControl）
```

### 3.1 类关系（概念）

```
ISkin
  └── SkinManager : QObject
        ├── 持有 Theme / Resource / Style / Font …
        └── 可扩展：子包管理、用户皮肤历史、在线皮肤、固定外观表面
```

实现时可按模块拆分，不必拘泥命名。

---

## 4. 主题 XML：样式描述层

### 4.1 角色

把视觉表现从控件逻辑中分离：代码通过 `className + propName` 查询外观，不在业务代码里硬编码颜色。  
同一控件逻辑可在亮色、暗色、活动主题之间切换。

### 4.2 基本结构

```xml
<?xml version="1.0" encoding="utf-8"?>
<shell>
  <import path="palette.theme.xml"/>
  <resource>...</resource>

  <theme relatedName="light">
    <style class="MyWidget" base="ThemedButton">
      <color name="background" value="AppPalette::dlg-background"/>
      <color name="background-hover" value="..."/>
      <font name="text" pixelSize="14"/>
      <hint name="borderRadius" value="8"/>
    </style>
  </theme>

  <theme relatedName="dark" baseRelated="light">
    <!-- 仅写暗色差异 -->
  </theme>

  <theme relatedName="*" baseRelated="light"/>
  <theme relatedName="*dark" baseRelated="dark"/>
</shell>
```

扩展名使用 `.theme.xml`，全项目保持一致。

### 4.3 属性类型与 API 对应

| XML 标签 | 内存 | 查询 API |
|----------|------|----------|
| `<color>` | 颜色项（可含 palette 引用） | `ThemeApi::color` |
| `<font>` | 字体项 | `ThemeApi::font` |
| `<hint>` | 整型提示 | `ThemeApi::hint` |
| `<linearGradient>` 等 | 渐变 | `ThemeApi::gradient` |
| `<image>` | 图片引用 | `ThemeApi::image` |

### 4.4 `relatedName` 匹配

加载时传入当前皮肤名：

1. **精确匹配**  
2. **暗色通配** `*dark`（皮肤名符合暗色规则时）  
3. **全通配** `*`  
4. 无 `relatedName` 的节点作为全局片段  

`baseRelated`：先应用被引用 theme，再叠加本节点。

### 4.5 `base` 样式继承

查询时本级没有则沿父 style 向上。父 class 必须已加载，否则继承静默失效（实现上应打告警）。

### 4.6 状态后缀

绘制侧按状态拼接，例如：`background` / `background-hover` / `background-down` / `background-disabled` / `text-selected` / `border-focus`。  
**建议统一禁用后缀为 `-disabled`。**

### 4.7 调色板 Token

| 典型 class | 作用 |
|------------|------|
| `AppPalette` | 产品级语义色 |
| `ComponentPalette` | 组件库语义色，多引用 `AppPalette::…` |

写法：`ComponentPalette::text-primary`，可选 `a="0.46"` 覆盖透明度。  
业务主题优先写 token，避免硬编码。

### 4.8 文件编码

建议：**UTF-8 带 BOM**，换行风格全项目统一（CRLF 或 LF）。无 BOM 文件视为非法并加载失败。

---

## 5. 皮肤包目录与查找

### 5.1 典型布局

```
skins/
  <skinName>/
    skin.ini                 # 元数据
    <appId>.theme.xml        # 按应用入口匹配的主主题
    <appId>_mac.theme.xml    # 可选平台后缀
    *.rcc / *.data           # 资源
    default/<appId>/...      # 可选散图
```

### 5.2 `skin.ini` 示意

```ini
[common]
style=AppStyle_Default
rccFile=main.rcc
rccSharedFile=shared.rcc
baseSkin=light
paletteOverride=0

[myapp]
style=AppStyle_Shell
rccFile=shell.rcc
```

`[<appId>]` 覆盖 `[common]`。`baseSkin` 用于主题与资源链合并。

### 5.3 查找顺序

1. 内置默认肤  
2. 扩展/插件肤  
3. 搜索路径下 `<path>/<skinName>/<appId>.theme.xml`  
4. 用户历史肤目录  
5. 回退默认  

主主题路径示意：`<skinPath>/<appId>.theme.xml`。

### 5.4 公共 themes 与皮肤包

- **公共 themes**：调色板、组件默认样式、各应用公共片段  
- **皮肤包**：该皮肤差异、资源、可选 palette 覆盖、按 app 的入口文件  

---

## 6. 换肤主流程

```
switchSkin(skinName, force)
  │
  ├─ 同名且非 force → 返回
  ├─ findSkin → SkinInfo
  ├─（可选）UI 配置体系不匹配 → 失败
  ├─ 主主题文件存在？
  │     ├─ resetXml（Soft）
  │     ├─ setupThemeXml（含 baseSkin 链）
  │     ├─ 重建图标搜索路径与资源
  │     ├─（可选）切换 QStyle / 应用字体
  │     ├─ 按新皮肤名重解析所有扩展子包
  │     ├─ emit skinChanged（建议 Queued）
  │     └─（可选）同步系统亮暗 Appearance
  └─ 失败则保持上一成功皮肤（或首次启动进 fallback）
```

---

## 7. 业务入口与跨进程同步

### 7.1 应用侧门面

可选封装：`switchSkin(name, save, allProducts)`、按应用维度持久化「当前皮肤」。

### 7.2 宿主进程

主题中心在宿主进程切换后，经 IPC 通知子进程跟随。

### 7.3 IPC 示意

```
宿主广播 skinName
  → 子进程 handler
  → switchSkin(name, force=true)   // 通常不再回写配置，避免抖动
```

### 7.4 全局事件

可用命名事件等机制通知同机其它实例；需去重（如 revision）。

### 7.5 固定外观表面（Clean Surface）

无法跟随宿主换肤的独立窗口：注册固定表面，走 `*_Clean` 样式或独立 `CleanPalette`。

---

## 8. 控件接入补充

### 8.1 绑定

```cpp
widget->setProperty("themeClassName", "MyWidget");
// 或
button->applyThemeClass("ThemedButton");
```

### 8.2 查询与绘制

```cpp
QColor bg = ThemeApi::color(className, "background");
int radius = ThemeApi::hint(className, "borderRadius");
ThemeApi::drawBackground(painter, rect, className, &option);
```

### 8.3 扩展包自定义对话框

扩展包主题中定义 `MySettingsDialog` 等 class，代码绑定同名，并提供 light / dark / `*` / `*dark` 兜底。

---

## 9. 扩展子主题

```cpp
SkinManager* skin = /* 从应用上下文取得 */;
skin->addExtension("myPlugin", themeDirPath, localStoragePath);
```

行为概要：

1. 加载 `themeDir/myPlugin.theme.xml`（文件名按项目约定）  
2. 以**当前皮肤名**为 `relatedName` 解析  
3. 挂到主加载器的子主题链  
4. 可附带资源包；换肤时由管理器按新皮肤名重 setup  

**时机**：主主题就绪之后。换肤后一般**不必**再 add，但必须重解析子包内容。

---

## 10. 图标与主题协同

| 能力 | 说明 |
|------|------|
| ResourceLoader | 注册资源包、按搜索路径 `loadIcon(name)` |
| 主题 `<resource><icon>` | 声明搜索路径 |
| 套色 | SVG 占位色名 + 主题中颜色映射 class |
| 区域绑定 | 窗口/控件绑定套色 class，换肤后缓存失效 |

---

## 11. 两类「主题」

| 概念 | 含义 |
|------|------|
| **应用 UI 主题 / 皮肤** | 窗口、控件、图标的外观 |
| **文档内容主题** | 文档内的主题色 / 字体 / 效果，属另一套体系 |

---

## 12. 排查清单

| 现象 | 检查 |
|------|------|
| 样式不生效 | 是否加载；class 拼写；relatedName；扩展包时机 |
| 暗色不对 | 是否有 `*dark`；是否硬编码色 |
| base 无效 | 父 style 是否已加载 |
| 换肤失败 | 入口主题文件、ini、搜索路径、UI 配置是否匹配 |
| 子进程未变 | IPC、是否实现 switchSkin、全局事件 |
| 图标空/不变色 | 搜索路径、资源注册、套色变量名 |
| 重建丢样式 | 是否重新绑定 class |

---

## 13. 新窗口 / 扩展最小步骤

1. 编写主题 XML，用 `AppPalette` / `ComponentPalette` token；补暗色。  
2. 启动后注册扩展包。  
3. 控件绑定 `themeClassName`。  
4. 绘制只用统一取色 API。  
5. 监听 `skinChanged` 清缓存并 `update`。  
6. 需要套色图标则绑定套色区域。  

---

## 14. API 速查

```cpp
skin->switchSkin(name, force);
theme = skin->theme();
skin->addExtension(id, dir, storage);

ThemeApi::color(className, propName);
ThemeApi::hint(className, propName);
ThemeApi::drawBackground(painter, rect, className, &option);
ThemeApi::loadIcon(iconName);
ThemeApi::isDarkSkin();

widget->setProperty("themeClassName", "ThemedButton");
// 或 applyThemeClass("ThemedButton");
```

信号：`skinChanged`（建议队列连接，避免重入）。

---

## 15. 设计要点小结

1. **数据驱动**：视觉在主题文件，逻辑在控件。  
2. **双层颜色**：Palette token + 控件 style。  
3. **继承与通配**：`base` / `baseRelated` / `*` / `*dark`。  
4. **皮肤包可组合**：`baseSkin` + 资源链 + 可选 palette 覆盖。  
5. **进程协作（可选）**：持久化 + 宿主广播 + IPC。  
6. **扩展点**：子主题包，不污染主 themes。  

---

## 附录 A. 加载器内部结构

```
ThemeLoader
  styleMap[className] → ThemeStyle
      colorMap / fontMap / hintMap / gradientMap / imageMap
      base → 父 style
  userStyleMap          → 运行时覆盖
  subThemes             → 扩展子加载器
  importSet             → 防重复 import
```

## 附录 B. 与本地化资源的交叉

语言包可带语言特定主题覆盖（如 RTL）与含文字图标资源。  
换语言后样式异常时，需同时检查语言目录下的主题文件。

---

# 第二部分：实现规格

以下约定主题格式、运行时行为、控件接入与发布要求。

---

## R0. 目标、范围与交付物

### R0.1 目标

1. 主题 XML 描述格式  
2. 主题加载器（查色表）  
3. 皮肤切换与皮肤包  
4. 绘制侧统一查询 API  
5. 控件按 `themeClassName + prop` 自绘  

### R0.2 范围内 / 范围外

| 范围内 | 范围外（可另立项） |
|--------|------------------|
| Format / Runtime / Widget 接入 | 主题商城、计费、运营后台 |
| 本地皮肤包、扩展子包 | 完整在线热更新协议 |
| 单进程完整方案；可选多进程规格 | 完整视觉组件库设计稿 |
| 图标按名加载 + 可选套色 | 文档内容级主题（版式/主题色另体系） |
| 最小可运行示例的设计说明 | — |

### R0.3 交付分层

| 层 | 内容 |
|----|------|
| L0 | Format + Loader + ThemeApi + 1 控件 Demo 逻辑 |
| L1 | 皮肤包、换肤、扩展包、持久化、DPI |
| L2 | 套色、固定外观表面、跨进程、版本迁移 |

---

## R1. Theme Format Spec

### R1.1 文件与编码

| 项 | 规范 |
|----|------|
| 扩展名 | `.theme.xml`（全项目一致） |
| 编码 | UTF-8 **带 BOM**（无 BOM 视为非法） |
| 换行 | 全项目统一 CRLF 或 LF |
| 根元素 | `<shell>`（全项目统一；若改名须同步解析器） |

### R1.2 结构（BNF 示意）

```
shell        := import* resource? theme+
import       := <import path="相对路径"/>
theme        := <theme relatedName? baseRelated? versionHint?> style* </theme>
style        := <style class="Name" base?> (color|font|hint|image|gradient)* </style>
```

未知标签：**忽略并告警**。未知属性：忽略并告警。

### R1.3 `import`

- 相对当前文件目录；深度优先；重复跳过；环检测失败。  
- 文件不存在：**默认整次 setup 失败**。

### R1.4 `relatedName` 算法

输入 `currentSkinName`（大小写不敏感）：

1. 精确匹配节点（多份则后者覆盖同名 style）  
2. 否则若暗色且存在 `*dark` → 用之  
3. 否则若存在 `*` → 用之  
4. 否则合并无 `relatedName` 的节点  
5. `baseRelated` 先递归再叠加；深度上限建议 16  

### R1.5 `style` / `base`

- `class` 必填；重复定义默认**按属性合并，后者覆盖**。  
- `base` 缺失 → 告警当无 base；禁止环。

### R1.6 属性类型

- **color**：`#RRGGBB` / `#RRGGBBAA` / `Class::prop` / 可选 `a=`；可选 HSV 相对色作 L2。  
- **hint**：整数或 `Class::prop`。  
- **font**：优先 `pixelSize`。  
- **image**：相对路径 / qrc / 搜索路径逻辑名。  
- **gradient**：至少支持 linear，value 走颜色解析。

## R1.7 Token 契约

颜色值若为 `Class::prop` 形式，在 **XML 加载时**解析为具体 `QColor` 写入 style 表（Resolve-on-Load）。  
因此调色板 style 必须先于引用方进入表中；换肤时清空并重建整表。

采用查询期再解析时，加载顺序约束可放宽，但每次取色成本更高，并需单独覆盖热更新与缓存失效测试。

### R1.8 标准属性名

填充 / 边框 / 文字的 `background*`、`border*`、`text*`；几何 `borderRadius`、`borderWidth`、`spacing`、margins；另有 `focus`、`shadow`、`opacity`。  
**禁用后缀统一 `-disabled`。**

### R1.9 暗色判定

1. 皮肤名符合约定（推荐 `endsWith("_dark")` 或皮肤名即为 `dark`）；或  
2. `AppPalette` 上 `is-dark-skin == 1`。  

对外 `isDarkSkin()`：**hint 优先**，再回退名字。

### R1.10 最小合法示例

```xml
<?xml version="1.0" encoding="utf-8"?>
<shell>
  <theme>
    <style class="AppPalette">
      <hint name="is-dark-skin" value="0"/>
      <color name="fill-base" value="#FFFFFFFF"/>
      <color name="text-primary" value="#0D0D0DE5"/>
    </style>
    <style class="DemoButton">
      <color name="background" value="AppPalette::fill-base"/>
      <color name="background-hover" value="#F0F0F0FF"/>
      <color name="text" value="AppPalette::text-primary"/>
      <hint name="borderRadius" value="6"/>
    </style>
  </theme>
  <theme relatedName="*dark" baseRelated="*">
    <style class="AppPalette">
      <hint name="is-dark-skin" value="1"/>
      <color name="fill-base" value="#2D2D2DFF"/>
      <color name="text-primary" value="#F5F5F5E5"/>
    </style>
  </theme>
</shell>
```

---

## R2. Runtime Spec

### R2.1 模块

```
ThemeEngine
  ├── ThemeLoader
  ├── SkinManager
  ├── ResourceLoader   // 可选
  ├── FontLoader       // 可选
  └── ThemeApi         // 门面
```

### R2.2 接口契约

```text
IThemeLoader:
  setupXml / resetXml(Soft|Final)
  addSubTheme / removeSubTheme
  color / hint / font / image / gradient
  hasClass / setUserColor|Hint|Font

ISkinManager:
  switchSkin(name, force=false)
  current() / theme()
  addExtension / removeExtension
  signal skinChanged(prev, current)   // 建议 Queued 到 UI 线程

ThemeApi:
  bind(ISkinManager*)
  color / hint / ... / isDarkSkin()
  findThemeClassName(QObject*)  // 读 "themeClassName"
```

线程：setup / switch / get* **仅 UI 线程**。

### R2.3 强制加载序

```
1. reset Soft（清主表；默认不清扩展注册表）
2. 沿 baseSkin 链加载
3. palette → component palette（或 import 保证顺序）
4. 目标主主题
5. 各扩展包：按新 skinName 重 setup + 切换资源
6. 可选 QStyle / 字体 / 图标路径
7. emit skinChanged
```

`addExtension` 须在主主题就绪后；换肤后由管理器重 setup 扩展包。

### R2.4 `switchSkin` 时序

时序同 §6。失败时保持上一成功皮肤；进程首次加载失败则进入内置 fallback。

### R2.5 查询优先级

Hook → 用户覆盖 → 扩展子主题（后注册优先） → 主表（含 base） → `def` + `ok=false`。

### R2.6 对象绑定

属性键：`themeClassName`。重建后必须重绑。

### R2.7 `skinChanged` 义务

丢弃缓存色/刷子/着色图标；`update` 可见控件；禁止在同步槽里再 `switchSkin`（需则投递下一事件循环）。

### R2.8 日志级别

文件/XML 失败 → Error；base/token 缺失 → Warning；查询 miss → Debug。

---

## R3. Widget Integration Spec

### R3.1 状态与后缀

| State | 后缀 |
|-------|------|
| Normal | （无） |
| Hover | `-hover` |
| Down | `-down` |
| Disabled | `-disabled` |
| Selected | `-selected` |
| Focus | `-focus` |

### R3.2 优先级（推荐单后缀）

`Disabled > Focus(边框可单独) > Down > Hover > Selected > Normal`  

统一函数拼后缀，禁止各控件手写。组合态（如 `selected-hover`）默认不做。

### R3.3 强制清单

1. 构造绑定稳定 class  
2. paint 只经 ThemeApi / ThemeSupports  
3. 不跨换肤缓存 `QColor`（或监听信号失效）  
4. 统一状态函数  
5. hint 按 DPI 规则缩放  
6. 变体用独立 class 或约定后缀，主题文件须有定义  
7. 缺 prop 不崩溃；换肤后颜色变化有测试  

### R3.4 DPI

颜色不缩放；半径/线宽/间距/尺寸 hint × `dpiScale`；无效 hint 用 `-1` 表示走代码默认。

### R3.5 与 QStyle

| 方案 | 适用 |
|------|------|
| A. 控件自绘 + 查表 | 新 UI 库首选 |
| B. 自定义 QStyle 内查表 | 替换标准控件观感 |
| C. StyleSheet | **不要**与本体系混用同一控件 |

---

## R4. 皮肤包发布

目录与 `skin.ini` 见 §5；必须内置不可删的 fallback 亮/暗（或单文件 `*` + `*dark`）。  
`baseSkin` 禁止环；`paletteOverride=1` 时允许皮肤目录覆盖全局调色板。

---

## R5. 图标与资源

- 搜索路径按注册顺序查找，**先命中先返回**。  
- 套色：主题 class 中的颜色名与 SVG 占位属性同名；换肤后使着色缓存失效。  
- 资源包：换肤时卸载旧包、加载新包及 `baseSkin` 链；扩展包资源按 `<res>/<skinName>/` 查找，缺失则回退 `baseSkin`；共享包注意引用计数。

---

## R6. 字体、渐变、图片

| 类型 | 要点 |
|------|------|
| font | style 内定义；可选全局 FontLoader；换肤可一次 `qApp->setFont` |
| gradient | 无渐变可退化纯色 |
| image | 懒加载；缩放缓存键含 path+size |

---

## R7. 固定外观表面

`regFixedSurface(obj, id)`；查询优先 `Original_Clean` 或 `CleanPalette`；主题提供 `_Clean` 后缀或独立固定调色板。

---

## R8. 跨进程与持久化（L2）

持久化键：`productVersion / appId / userId → skinName`。  
IPC 最小消息：`{ skinName, revision }`；子进程 `force` 切换且不回写配置。冲突时以宿主进程为准。

---

## R9. 版本与兼容

`versionHint` / `formatVersion`；未知标签忽略；主版本不兼容则拒绝并 fallback；旧皮肤名映射表可选。

---

## R10. 性能建议

冷启动默认主题 < 50ms 量级（千级 style）；`getColor` 禁止读盘/解析 XML；图标路径与套色结果缓存。

---

## R11. 测试矩阵

解析（BOM、坏 XML、环 import、缺 base）· Token · relatedName · 查询优先级 · switchSkin · 扩展包换肤重载 · 控件状态色 · DPI · 固定表面 ·（L2）IPC。  

陷阱：禁用后缀不统一、重建未绑 class、`skinChanged` 重入、共享资源过早卸载。

---

## R12. 最小示例设计

**目标**：双肤切换 + 一按钮悬停变色。  

**逻辑文件集**：`themes/app.theme.xml`（含 AppPalette + DemoButton + `*dark`）、可选 `skins/light|dark`、代码模块 ThemeLoader / SkinManager / ThemeApi / DemoButton。  

**脚本**：启动 light → hover → switch dark → 错 class 不崩溃 → 扩展包覆盖色优先。  

**验收**：无 StyleSheet；换肤不改按钮业务逻辑。

---

## R13. 范围边界与待决项

| 不在本规格交付内 | 说明 |
|------------------|------|
| 主题商城 / 运营后台 | 发行与运营另案 |
| StyleSheet 与查表混用同一控件 | 两套体系冲突，择一 |
| 文档内容级主题 | 版式与内容主题色另体系 |
| 非 UI 线程主题查询 | 加载与取色约定在 UI 线程 |

开工前确认（L0 见第三部分，下列已钉死：加载期 Token、单后缀、失败保持上一皮肤）：

- Token：加载期展开，或查询期解析  
- 状态后缀：仅单后缀，或允许组合态（如 `selected-hover`）  
- `switchSkin` 失败：保持上一皮肤，或落入 fallback  
- 模块与类型命名全项目一致

---

## R14. 实施顺序建议

```
阶段 1  Format 解析器 + ThemeLoader 查询 + 单测
阶段 2  ThemeApi + DemoButton + 双肤 switchSkin
阶段 3  skin.ini、baseSkin、扩展包、skinChanged
阶段 4  DPI、持久化、图标搜索路径
阶段 5+ 套色、固定表面、跨进程、性能与兼容
```

---

## R15. 开工检查清单

L0 已钉死项见 **第三部分**；下列用于 L1+ 扩展前确认：

- [ ] 第三部分 T0–T5 已作为实现与验收依据  
- [ ] `appId`、搜索路径、默认肤名已填入工程配置  
- [ ] 控件绘制路径不使用 StyleSheet  
- [ ] T5 用例全部通过后再进入 L1  

---

# 第三部分：L0 可生成规格

本部分把 L0（解析主题 → 查色 → 单控件换肤）钉到可直接生成代码的粒度。实现 L0 时以本节为准；与前文冲突时以本节为准。

---

## T0. 技术栈与约束

| 项 | 选定 |
|----|------|
| 语言 | C++17 |
| UI | Qt Widgets（Qt 5.15+ 或 Qt 6.x，API 按 Widgets 书写） |
| 外观体系 | 主题 XML 查表 + 控件自绘 |
| 禁止 | `setStyleSheet` / StyleSheet 参与同一套主题化控件 |
| Token | **加载期展开**；每次 `setupXml` 在合并完所有选中 `<theme>` 后，对带 token 引用的 color **再展开一遍**（见 T2.4） |
| 状态后缀 | **仅单后缀**（不生成 `selected-hover`） |
| `switchSkin` 失败 | 保持上一成功皮肤；进程内尚无成功皮肤则加载内置 fallback |
| 线程 | `setupXml` / `switchSkin` / `ThemeApi::*` 仅 UI 线程调用 |
| 根元素 | `<shell>` |
| 主题扩展名 | `.theme.xml` |
| 编码 | UTF-8 带 BOM |
| 对象属性键 | `"themeClassName"` |
| 默认肤名 | `light`；暗色通配匹配名：`dark`（或任意 `endsWith("_dark")` 的皮肤名） |

L0 **不实现**：扩展子包、skin.ini 多搜索路径、图标套色、IPC、Fixed Surface、自定义 QStyle 替换。

L0 **交付物**：

1. `ThemeLoader` + `SkinManager` + `ThemeApi`  
2. 内置 `app.theme.xml`（含 light 默认 theme + `*dark`）  
3. `DemoButton` 控件  
4. 可调用 `switchSkin("light"|"dark")` 的演示入口  
5. T5 用例通过  

---

## T1. 接口（头文件级）

以下为实现必须提供的 API；方法名可微调，语义不可缺。

```cpp
#pragma once
#include <QColor>
#include <QFont>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QStyleOption>

struct ThemeColor {
    QColor value;
    bool ok = false;
    QString paletteRef;    // 非空表示 value 来自 token，setupXml 末尾须 reexpand
};

enum class ThemeResetMode {
    Soft,   // 清空主 style 表；保留扩展注册元数据（L0 无扩展亦可实现）
    Final   // Soft + 清空扩展与 hook
};

enum class ThemeError {
    None = 0,
    FileNotFound,
    InvalidEncoding,   // 无 BOM 或无法按 UTF-8 解码
    XmlParseError,
    ImportCycle,
    ImportMissing,
    NoMatchingTheme,
    SkinNotFound
};

class IThemeLoader {
public:
    virtual ~IThemeLoader() = default;

    // searchPaths：import / image 相对路径的额外搜索根；L0 可传空
    // relatedSkinName：当前皮肤名，用于 relatedName 匹配
    virtual bool setupXml(const QString& filePath,
                          const QString& relatedSkinName,
                          const QStringList& searchPaths,
                          ThemeError* err = nullptr) = 0;

    virtual void resetXml(ThemeResetMode mode) = 0;

    virtual ThemeColor color(const QString& className,
                             const QString& propName,
                             const QColor& def = QColor()) const = 0;

    virtual int hint(const QString& className,
                     const QString& propName,
                     int def = 0,
                     bool* ok = nullptr) const = 0;

    virtual bool hasClass(const QString& className) const = 0;
};

struct SkinInfo {
    QString name;           // 如 "light" / "dark"
    QString themeFilePath;  // 主 .theme.xml
};

class ISkinManager {
public:
    virtual ~ISkinManager() = default;
    virtual bool switchSkin(const QString& name, bool force = false) = 0;
    virtual SkinInfo current() const = 0;
    virtual IThemeLoader* theme() const = 0;
    virtual ThemeError lastError() const = 0;
    // 信号由实现类 SkinManager(QObject) 提供：
    //   void skinChanged(const QString& previous, const QString& current);
};

// 进程内门面：bind 后供控件调用
namespace ThemeApi {
    void bind(ISkinManager* skin);           // 允许 nullptr 解绑
    ISkinManager* skin();
    QObject* skinObject();                   // 返回实现类 QObject*，供 connect(skinChanged…)；未 bind 则为 nullptr

    QColor color(const QString& className, const QString& propName,
                 const QColor& def = QColor());
    int hint(const QString& className, const QString& propName, int def = 0);
    bool isDarkSkin();

    QString themeClassName(const QObject* obj);
    void setThemeClassName(QObject* obj, const QString& className);

    QString propWithState(const QString& baseProp, const QStyleOption* option);
}
```

`SkinManager` 为 `QObject` 子类并实现 `ISkinManager`，提供  
`void skinChanged(const QString& previous, const QString& current);`  
构造后 `ThemeApi::bind(this)`，父对象建议为 `qApp`。  
`bind` 时须同时保存 `QObject*`，使 `skinObject()` 可连接信号。

测试比较 `#RRGGBBAA` 时**禁止**直接 `QColor("#RRGGBBAA")`（Qt 会按 AARRGGBB 解读）。使用与 `parseColorLiteral` 相同的工具函数，例如 `themeColorFromRgbaHex("#0D0D0DE5")`。

---

## T2. 核心算法伪代码

### T2.1 颜色字面量解析 `parseColorLiteral(s) -> QColor`

```
function parseColorLiteral(s):
  // 仅解析 # 字面量；token 由 ingestColor 走 resolveToken，不得进入本函数
  if s is empty: return invalid
  if s starts with '#':
    if length == 7:  // #RRGGBB
      return QColor(s) with alpha=255
    if length == 9:  // #RRGGBBAA（注意：不是 Qt 的 #AARRGGBB）
      rgb = "#" + s[1..6]     // #RRGGBB
      aa = hex(s[7..8])
      c = QColor(rgb); c.setAlpha(aa); return c
    return invalid
  return invalid
```

### T2.2 Token 展开 `resolveToken("A::b")`

```
function resolveToken(token):
  (cls, prop) = split(token, "::", max=2)
  c = lookupColorInStyleMap(cls, prop)  // 含 base 链，取已写入的 QColor
  if found: return c
  log warning; return invalid
```

解析单个 `<color>` 时：

```
function ingestColor(style, propName, valueAttr, aAttr):
  item.paletteRef = ""
  if valueAttr contains "::":
    item.paletteRef = valueAttr          // 保留 token，供合并后重展开
    item.value = resolveToken(valueAttr) // 当时能解析则先写入
  else:
    item.value = parseColorLiteral(valueAttr)  // 仅 #RRGGBB / #RRGGBBAA
  if aAttr present:
    item.value.setAlphaF(clamp(float(aAttr), 0.0, 1.0))
  style.colorMap[propName] = item
```

**换肤 / 多 theme 合并后必须重展开**（否则 `*dark` 只改 `AppPalette` 时，控件上已展开的旧色不会变）：

```
function reexpandAllTokenColors():
  for each style in styleMap:
    for each (prop, item) in style.colorMap:
      if item.paletteRef not empty:
        item.value = resolveToken(item.paletteRef)
        // 若仍有 a 覆盖需求，按首次 ingest 时缓存的 a 再应用；L0 Golden 无 a= 可省略
```

注意：`AppPalette` 的 value 必须是 `#...`，不能互相 token 环依赖。  
同一次 `setupXml` 内：先按文档顺序合并各 `<theme>` 的 style，**最后**调用 `reexpandAllTokenColors()`。

### T2.3 `relatedName` 匹配 `selectThemeNodes(doc, skinName)`

```
function isDarkName(skinName):
  return skinName equalsIgnoreCase "dark"
      OR skinName endsWithIgnoreCase "_dark"
      OR (after load) AppPalette hint is-dark-skin == 1
      // L0 匹配阶段：仅用名字规则；加载完成后 isDarkSkin() 可读 hint

function selectThemeNodes(doc, skinName):
  themes = all <theme> under <shell>

  // baseRelated 传入 "*" / "*dark" 时的字面匹配
  if skinName == "*":
    any = themes where relatedName == "*"
    if any not empty: return expandBaseRelated(any)
    bare = themes where relatedName missing or empty
    if bare not empty: return bare
    return error NoMatchingTheme
  if skinName == "*dark":
    d = themes where relatedName == "*dark"
    if d not empty: return expandBaseRelated(d)
    return error NoMatchingTheme

  exact = themes where relatedName equalsIgnoreCase skinName
  if exact not empty:
    return expandBaseRelated(exact)

  if isDarkName(skinName):
    darkAny = themes where relatedName == "*dark"
    if darkAny not empty:
      return expandBaseRelated(darkAny)

  any = themes where relatedName == "*"
  if any not empty:
    return expandBaseRelated(any)

  bare = themes where relatedName attribute missing or empty
  if bare not empty:
    return bare

  return error NoMatchingTheme

function expandBaseRelated(nodes):
  // 对每个 node，若有 baseRelated=X，先递归 selectThemeNodes(doc, X)，再追加本 node
  // 深度 > 16 → 失败
  // 同名 style 属性：后出现的 color/hint 覆盖先出现的
  result = []
  for n in nodes in document order:
    if n.has baseRelated:
      result.appendAll(selectThemeNodes(doc, n.baseRelated))
    result.append(n)
  return result
```

L0 内置文件使用：无 `relatedName` 的默认 `<theme>` + `relatedName="*dark" baseRelated="*"`，则：

- `skinName=light` → 命中 bare theme  
- `skinName=dark` → 命中 `*dark`，`baseRelated="*"` 先合并 bare，再叠 dark 差异；最后 `reexpandAllTokenColors()`

### T2.4 `setupXml`

```
function setupXml(path, relatedSkinName, searchPaths):
  if not QFile.exists(path): err=FileNotFound; return false   // qrc 路径 :/theme/... 同样适用
  bytes = readAll(path)   // QFile
  if not hasUtf8Bom(bytes): err=InvalidEncoding; return false
  doc = parseXml(stripBom(bytes))
  if parseFail: err=XmlParseError; return false

  // L0：忽略 <import>（Warning）；不实现 font/image 亦可，遇到则跳过

  nodes = selectThemeNodes(doc, relatedSkinName)
  if nodes failed: err=NoMatchingTheme; return false

  for each themeNode in nodes in order:
    for each style in themeNode:
      name = style@class
      ensure styleMap[name] exists
      if style@base: pendingBase[name] = base   // 后者覆盖
      for each child:
        if color: ingestColor(...)
        if hint:  hintMap[name] = toInt(value)  // L0 不解析 hint 的 Class::prop
        // font/image: skip
  link bases from pendingBase; break cycles with warn
  reexpandAllTokenColors()
  err=None; return true
```

`SkinManager::lastError()`：记录**最近一次** `switchSkin` / 其内部 `setupXml` 的错误码；成功则置 `None`。  
直接测 loader 时用 `setupXml(..., &err)` 出参（见 TC09），不必经过 `lastError()`。

**`baseRelated="*"` 语义（L0）：** 调用 `selectThemeNodes(doc, "*")` 时，若没有 `relatedName="*"` 的节点，则回退到 **无 relatedName 的 bare `<theme>`**（与 T3.1 一致）。

### T2.5 `getColor` / `propWithState`

```
function getColor(className, propName, def):
  s = styleMap[className]
  while s:
    if s.colorMap has propName:
      return { value: item.value, ok: true, paletteRef: "" }  // 查询结果不回传内部 ref
    s = s.base
  return { value: def, ok: false, paletteRef: "" }

function propWithState(baseProp, option):
  // 单后缀，优先级从高到低，只附加一个
  if not option.state & Enabled: return baseProp + "-disabled"
  if option.state & sunken:      return baseProp + "-down"
  if option.state & mouseOver:   return baseProp + "-hover"
  if option.state & selected:    return baseProp + "-selected"
  // focus 仅当 baseProp 为 border 或 focus 时可选附加；L0 DemoButton 背景忽略 focus
  return baseProp
```

### T2.6 `switchSkin`

```
function switchSkin(name, force):
  assertOnUiThread()
  if inited and name == current.name and not force: return false

  path = ":/theme/app.theme.xml"   // 见 T6 qrc；唯一内置主题文件

  prev = current.name
  theme.resetXml(Soft)
  ok = theme.setupXml(path, name, {})
  if not ok:
    if prev not empty:
      theme.setupXml(path, prev, {})
    else:
      theme.setupXml(path, "light", {})  // fallback
    return false

  current.name = name
  current.themeFilePath = path
  inited = true
  emit skinChanged(prev, name)   // QueuedConnection
  return true
```

### T2.7 `ThemeApi::isDarkSkin`

```
function isDarkSkin():
  h = theme.hint("AppPalette", "is-dark-skin", 0, &ok)
  if ok: return h == 1
  return isDarkName(current.name)
```

---

## T3. Golden 样例

### T3.1 文件 `app.theme.xml`

（须 UTF-8 BOM；下列正文）

```xml
<?xml version="1.0" encoding="utf-8"?>
<shell>
  <theme>
    <style class="AppPalette">
      <hint name="is-dark-skin" value="0"/>
      <color name="fill-base" value="#FFFFFFFF"/>
      <color name="fill-hover" value="#F0F0F0FF"/>
      <color name="fill-down" value="#E0E0E0FF"/>
      <color name="text-primary" value="#0D0D0DE5"/>
      <color name="line-regular" value="#0D0D0D26"/>
    </style>
    <style class="DemoButton">
      <color name="background" value="AppPalette::fill-base"/>
      <color name="background-hover" value="AppPalette::fill-hover"/>
      <color name="background-down" value="AppPalette::fill-down"/>
      <color name="background-disabled" value="#F5F5F5FF"/>
      <color name="text" value="AppPalette::text-primary"/>
      <color name="text-disabled" value="#0D0D0D66"/>
      <color name="border" value="AppPalette::line-regular"/>
      <hint name="borderRadius" value="6"/>
      <hint name="borderWidth" value="1"/>
    </style>
  </theme>
  <theme relatedName="*dark" baseRelated="*">
    <style class="AppPalette">
      <hint name="is-dark-skin" value="1"/>
      <color name="fill-base" value="#2D2D2DFF"/>
      <color name="fill-hover" value="#3A3A3AFF"/>
      <color name="fill-down" value="#242424FF"/>
      <color name="text-primary" value="#F5F5F5E5"/>
      <color name="line-regular" value="#F5F5F526"/>
    </style>
    <style class="DemoButton">
      <color name="background-disabled" value="#3A3A3AFF"/>
      <color name="text-disabled" value="#F5F5F566"/>
    </style>
  </theme>
</shell>
```

### T3.2 期望表（加载后 `getColor` / `getHint`）

颜色比较忽略 `QColor` 无效位差异；按 ARGB 8 位比对。

**当 `relatedSkinName = "light"`（或首次 `switchSkin("light")`）：**

| class | prop | 期望 |
|-------|------|------|
| AppPalette | is-dark-skin (hint) | 0 |
| AppPalette | fill-base | `#FFFFFFFF` |
| AppPalette | text-primary | `#0D0D0DE5` |
| DemoButton | background | `#FFFFFFFF`（由 token 展开） |
| DemoButton | background-hover | `#F0F0F0FF` |
| DemoButton | background-down | `#E0E0E0FF` |
| DemoButton | text | `#0D0D0DE5` |
| DemoButton | border | `#0D0D0D26` |
| DemoButton | borderRadius (hint) | 6 |
| DemoButton | missing-prop | ok=false |

**当 `relatedSkinName = "dark"`：**

| class | prop | 期望 |
|-------|------|------|
| AppPalette | is-dark-skin | 1 |
| AppPalette | fill-base | `#2D2D2DFF` |
| AppPalette | text-primary | `#F5F5F5E5` |
| DemoButton | background | `#2D2D2DFF` |
| DemoButton | background-hover | `#3A3A3AFF` |
| DemoButton | background-down | `#242424FF` |
| DemoButton | text | `#F5F5F5E5` |
| DemoButton | border | `#F5F5F526` |
| DemoButton | background-disabled | `#3A3A3AFF` |
| DemoButton | text-disabled | `#F5F5F566` |
| DemoButton | borderRadius | 6（来自 bare theme 合并，dark 未覆盖则保留） |

说明：dark 未重写 `DemoButton.background` 等 token 色时，依赖 T2.4 末尾 **`reexpandAllTokenColors()`**，在 `AppPalette` 更新后重算 `AppPalette::…` 引用，期望值才能成立。

**`ThemeApi::isDarkSkin()`：** light → false；dark → true。

---

## T4. `DemoButton` 伪代码

```
class DemoButton : QWidget
  text: QString
  pressed: bool = false

  setText(t): text = t; update()
  text(): return text

  constructor(parent):
    setThemeClassName(this, "DemoButton")
    setMouseTracking(true)
    setAttribute(WA_Hover, true)
    if auto* obj = ThemeApi::skinObject():
      connect(obj, SIGNAL(skinChanged(QString,QString)),
              this, SLOT(update()), Qt::QueuedConnection)

  enterEvent / leaveEvent:
    update()
  mousePressEvent:
    pressed = true; update()
  mouseReleaseEvent:
    pressed = false; update()

  paintEvent(e):
    QStyleOption opt
    opt.initFrom(this)
    if underMouse(): opt.state |= State_MouseOver
    if pressed:      opt.state |= State_Sunken
    if not isEnabled(): opt.state &= ~State_Enabled

    cls = themeClassName(this)
    bg = ThemeApi::color(cls, ThemeApi::propWithState("background", &opt))
    fg = ThemeApi::color(cls, ThemeApi::propWithState("text", &opt))
    border = ThemeApi::color(cls, "border")
    radius = ThemeApi::hint(cls, "borderRadius", 6)

    QPainter p(this)
    p.setRenderHint(Antialiasing)
    path = roundedRect(rect().adjusted(0,0,-1,-1), radius)
    p.fillPath(path, bg)
    p.setPen(QPen(border, 1))
    p.drawPath(path)
    p.setPen(fg)
    p.drawText(rect(), AlignCenter, text)
```

禁止：`setStyleSheet`。  
换肤后必须靠 `skinChanged → update()` 立即重绘，不得依赖鼠标再次进入。  
若构造时 `skinObject()` 为空，在首次 `showEvent` 再补连一次信号。

---

## T5. 验收用例（Given / When / Then）

实现 L0 后须全部通过。  
**颜色与 ok 断言一律通过 `IThemeLoader`：**

```cpp
auto r = loader->color(className, propName, def);
QCOMPARE(r.ok, expectedOk);
QCOMPARE(r.value.rgba(), themeColorFromRgbaHex(expectedHex).rgba());
```

`themeColorFromRgbaHex` 与 `parseColorLiteral` 规则相同（`#RRGGBB` / `#RRGGBBAA`）。  
`ThemeApi::color` 仅用于控件绘制；单测以 `skin->theme()->color(...)` 为准。  
`QSignalSpy spy(themeApiSkinObject, SIGNAL(skinChanged(QString,QString)))` 使用 `ThemeApi::skinObject()`。

### TC01 加载 light Golden

- **Given** 空 loader，文件为 `:/theme/app.theme.xml`（T3.1，带 BOM）  
- **When** `setupXml(path, "light", {})`  
- **Then** 返回 true；T3.2 light 表：`color(...).ok == true` 且 `value` 匹配；`hasClass("DemoButton") == true`

### TC02 加载 dark Golden

- **Given** 同文件  
- **When** `resetXml(Soft)` 后 `setupXml(path, "dark", {})`  
- **Then** T3.2 dark 表全部 `ok` 且色值匹配；`hint("AppPalette","is-dark-skin",0,&ok)==1 && ok`

### TC03 Token 展开

- **Given** light 已加载  
- **When** `c1 = color("DemoButton","background")`，`c2 = color("AppPalette","fill-base")`  
- **Then** `c1.ok && c2.ok && c1.value.rgba() == c2.value.rgba() == #FFFFFFFF`

### TC04 缺属性

- **Given** light 已加载  
- **When** `r = color("DemoButton", "no-such-prop", QColor("#FF0000FF"))`  
- **Then** `r.ok == false` 且 `r.value.rgba() == QColor("#FF0000FF").rgba()`；进程不崩溃

### TC05 switchSkin 往返

- **Given** `switchSkin("light")` 成功，`QSignalSpy spy(skin, skinChanged)`  
- **When** `switchSkin("dark")` 再 `switchSkin("light")`  
- **Then** `color("DemoButton","background")` 回到 light 期望色；`spy.count() >= 2`

### TC06 switchSkin 同名

- **Given** 当前为 light，`spy` 计数清零  
- **When** `switchSkin("light", force=false)`  
- **Then** 返回 false；`spy.count() == 0`

### TC07 悬停属性名与取色

- **Given** light 已加载  
- **When** `opt.state = Enabled | MouseOver`；`p = propWithState("background", &opt)`；`r = color("DemoButton", p)`  
- **Then** `p == "background-hover"`；`r.ok` 且色为 `#F0F0F0FF`

### TC08 禁用态取色

- **Given** light 已加载  
- **When** `opt.state = 0`（无 Enabled）；`p = propWithState("background", &opt)`；`r = color("DemoButton", p)`  
- **Then** `p == "background-disabled"`；`r.ok` 且色为 `#F5F5F5FF`

### TC09 无 BOM 失败

- **Given** 与 T3.1 正文相同但无 UTF-8 BOM 的临时文件  
- **When** `ThemeError err = None; ok = loader->setupXml(tempPath, "light", {}, &err)`  
- **Then** `ok == false` 且 `err == InvalidEncoding`

### TC10 错误 class 名不崩溃

- **Given** light 已加载  
- **When** `r = color("NotExist", "background", QColor("#00FF00FF"))`  
- **Then** `r.ok == false` 且返回 def；无崩溃

### TC11 换肤触发重绘约定

- **Given** DemoButton 已按 T4 连接 `skinChanged`  
- **When** `switchSkin("dark")`  
- **Then** 控件在下一事件循环后外观为 dark 色（可对 `grab()` 主色抽样，或至少断言连接存在且 `update` 被调度）；不得要求鼠标再次 enter

---

## T6. L0 工程骨架

### T6.1 目录

```
theme_engine/
  CMakeLists.txt
  include/theme/
    ithemeloader.h
    iskinmanager.h
    themeapi.h
    themeloader.h          // 实现类
    skinmanager.h
  src/
    themeloader.cpp
    skinmanager.cpp
    themeapi.cpp
    color_util.cpp
  resources/
    theme.qrc
    app.theme.xml           // T3.1，UTF-8 BOM
  widgets/
    demobutton.h
    demobutton.cpp
  app/
    main.cpp
  tests/
    CMakeLists.txt
    test_golden.cpp         // TC01–TC04, TC09–TC10
    test_switchskin.cpp     // TC05–TC06, TC11
    test_propstate.cpp      // TC07–TC08
```

### T6.2 `resources/theme.qrc`

```xml
<RCC>
  <qresource prefix="/theme">
    <file alias="app.theme.xml">app.theme.xml</file>
  </qresource>
</RCC>
```

运行时主题路径固定为：`:/theme/app.theme.xml`。

### T6.3 最小 `CMakeLists.txt`（示意）

```cmake
cmake_minimum_required(VERSION 3.16)
project(theme_engine LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets Test QUIET)
if(NOT Qt6_FOUND)
  find_package(Qt5 5.15 REQUIRED COMPONENTS Widgets Test)
  set(QT_PKG Qt5)
else()
  set(QT_PKG Qt6)
endif()

add_library(theme_lib
  src/themeloader.cpp src/skinmanager.cpp src/themeapi.cpp src/color_util.cpp
  widgets/demobutton.cpp
  resources/theme.qrc
)
target_include_directories(theme_lib PUBLIC include)
target_link_libraries(theme_lib PUBLIC ${QT_PKG}::Widgets)

add_executable(theme_demo app/main.cpp)
target_link_libraries(theme_demo PRIVATE theme_lib)

enable_testing()
add_executable(theme_tests
  tests/test_golden.cpp tests/test_switchskin.cpp tests/test_propstate.cpp)
target_link_libraries(theme_tests PRIVATE theme_lib ${QT_PKG}::Test)
add_test(NAME theme_tests COMMAND theme_tests)
```

### T6.4 最小 `app/main.cpp`（示意）

```cpp
int main(int argc, char** argv) {
  QApplication app(argc, argv);
  auto* skin = new SkinManager(&app);  // 具体类，实现 ISkinManager
  ThemeApi::bind(skin);
  skin->switchSkin(QStringLiteral("light"));

  QWidget w;
  auto* layout = new QVBoxLayout(&w);
  auto* btn = new DemoButton(&w);
  btn->setText(QStringLiteral("OK"));
  btn->setMinimumHeight(36);
  auto* light = new QPushButton(QStringLiteral("Light"), &w);
  auto* dark  = new QPushButton(QStringLiteral("Dark"), &w);
  // Light/Dark 仅用于切换皮肤，不参与主题查表绘制亦可
  QObject::connect(light, &QPushButton::clicked, [&]{ skin->switchSkin(QStringLiteral("light")); });
  QObject::connect(dark,  &QPushButton::clicked, [&]{ skin->switchSkin(QStringLiteral("dark")); });
  layout->addWidget(btn);
  layout->addWidget(light);
  layout->addWidget(dark);
  w.resize(280, 160);
  w.show();
  return app.exec();
}
```

说明：`ISkinManager` 可保留为纯接口（无 `Q_OBJECT`）；带信号的实现类为 `SkinManager : public QObject`（或 `public ISkinManager` 且仅实现类声明 `Q_OBJECT`）。

