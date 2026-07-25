#include "qtheme/style.hpp"

#include "style/style_ctx.hpp"
#include "style/style_families.hpp"

#include <QMenu>
#include <QStyleFactory>
#include <QStyleOption>
#include <QStyleOptionComboBox>
#include <QStyleOptionComplex>
#include <QStyleOptionSpinBox>
#include <QVariant>
#include <QtMath>

namespace qtheme
{
	namespace
	{
		QStyle* makeFusionBase()
		{
			QStyle* fusion = QStyleFactory::create(QStringLiteral("Fusion"));
			return fusion ? fusion : QStyleFactory::create(QStringLiteral("windows"));
		}

		void polishRoundedPopup(QWidget* widget, int radius)
		{
			if (!widget || radius <= 0)
			{
				return;
			}
			// Must be set before the native window is created (before base polish / first show).
			widget->setProperty("qtheme.popupRadius", radius);
			widget->clearMask();
			widget->setAutoFillBackground(false);
			widget->setAttribute(Qt::WA_TranslucentBackground, true);
			widget->setAttribute(Qt::WA_NoSystemBackground, true);
			widget->setWindowFlag(Qt::FramelessWindowHint, true);
			widget->setWindowFlag(Qt::NoDropShadowWindowHint, true);
		}

		[[nodiscard]] style_detail::StyleCtx makeCtx(const ThemeStore* store, qreal dpiScale)
		{
			return style_detail::StyleCtx{store, dpiScale};
		}
	} // namespace

	QThemeStyle::QThemeStyle(std::shared_ptr<ThemeStore> store, QStyle* base)
		: QProxyStyle(base ? base : makeFusionBase())
		, m_store(std::move(store))
	{
		setObjectName(QStringLiteral("QThemeStyle"));
	}

	void QThemeStyle::setStore(std::shared_ptr<ThemeStore> store)
	{
		m_store = std::move(store);
	}

	void QThemeStyle::setDpiScale(qreal scale)
	{
		if (scale <= 0.0)
		{
			scale = 1.0;
		}
		m_dpiScale = scale;
	}

	void QThemeStyle::polish(QWidget* widget)
	{
		if (widget && m_store && (qobject_cast<QMenu*>(widget) || widget->inherits("QTipLabel")))
		{
			const QString group = qobject_cast<QMenu*>(widget) ? QStringLiteral("menu") : QStringLiteral("tooltip");
			polishRoundedPopup(widget, roleMetric(group, QStringLiteral("radius"), 4));
		}
		QProxyStyle::polish(widget);
	}

	void QThemeStyle::unpolish(QWidget* widget)
	{
		if (widget && (qobject_cast<QMenu*>(widget) || widget->inherits("QTipLabel")))
		{
			widget->clearMask();
			widget->setProperty("qtheme.popupRadius", QVariant());
			widget->setAttribute(Qt::WA_TranslucentBackground, false);
			widget->setAttribute(Qt::WA_NoSystemBackground, false);
		}
		QProxyStyle::unpolish(widget);
	}

	QColor QThemeStyle::roleColor(const QString& group, const QString& role, const QColor& fallback) const
	{
		return makeCtx(m_store.get(), m_dpiScale).roleColor(group, role, fallback);
	}

	int QThemeStyle::scaleMetric(int logicalPx) const
	{
		return makeCtx(m_store.get(), m_dpiScale).scaleMetric(logicalPx);
	}

	int QThemeStyle::roleMetric(const QString& group, const QString& role, int fallback) const
	{
		return makeCtx(m_store.get(), m_dpiScale).roleMetric(group, role, fallback);
	}

	void QThemeStyle::focusStrokeColors(const QColor& ambient, QColor* outer, QColor* inner) const
	{
		makeCtx(m_store.get(), m_dpiScale).focusStrokeColors(ambient, outer, inner);
	}

	QPalette QThemeStyle::standardPalette() const
	{
		QPalette pal = QProxyStyle::standardPalette();
		if (!m_store)
		{
			return pal;
		}

		auto set = [&](QPalette::ColorRole role, const QString& name)
		{
			const ColorValue cv = m_store->color(QStringLiteral("palette"), name);
			if (cv.ok)
			{
				pal.setColor(QPalette::Active, role, cv.value);
				pal.setColor(QPalette::Inactive, role, cv.value);
				pal.setColor(QPalette::Disabled, role, cv.value);
			}
		};

		set(QPalette::Window, QStringLiteral("window"));
		set(QPalette::WindowText, QStringLiteral("windowText"));
		set(QPalette::Base, QStringLiteral("base"));
		set(QPalette::Text, QStringLiteral("text"));
		set(QPalette::Button, QStringLiteral("button"));
		set(QPalette::ButtonText, QStringLiteral("buttonText"));
		set(QPalette::Highlight, QStringLiteral("highlight"));
		set(QPalette::HighlightedText, QStringLiteral("highlightedText"));
		set(QPalette::Mid, QStringLiteral("mid"));
		set(QPalette::Light, QStringLiteral("light"));
		set(QPalette::Dark, QStringLiteral("dark"));

		const ColorValue disabledText = m_store->color(QStringLiteral("button"), QStringLiteral("fg.disabled"));
		if (disabledText.ok)
		{
			pal.setColor(QPalette::Disabled, QPalette::ButtonText, disabledText.value);
			pal.setColor(QPalette::Disabled, QPalette::WindowText, disabledText.value);
			pal.setColor(QPalette::Disabled, QPalette::Text, disabledText.value);
		}

		const ColorValue editFg = m_store->color(QStringLiteral("edit"), QStringLiteral("fg"));
		if (editFg.ok)
		{
			pal.setColor(QPalette::Active, QPalette::Text, editFg.value);
			pal.setColor(QPalette::Inactive, QPalette::Text, editFg.value);
		}
		const ColorValue editFgDis = m_store->color(QStringLiteral("edit"), QStringLiteral("fg.disabled"));
		if (editFgDis.ok)
		{
			pal.setColor(QPalette::Disabled, QPalette::Text, editFgDis.value);
		}
		ColorValue placeholder = m_store->color(QStringLiteral("combo"), QStringLiteral("fg.placeholder"));
		if (!placeholder.ok)
		{
			placeholder = m_store->color(QStringLiteral("edit"), QStringLiteral("fg.placeholder"));
		}
		if (placeholder.ok)
		{
			pal.setColor(QPalette::Active, QPalette::PlaceholderText, placeholder.value);
			pal.setColor(QPalette::Inactive, QPalette::PlaceholderText, placeholder.value);
			pal.setColor(QPalette::Disabled, QPalette::PlaceholderText, editFgDis.ok ? editFgDis.value : placeholder.value);
		}

		const ColorValue textTertiary = m_store->color(QStringLiteral("palette"), QStringLiteral("text.tertiary"));
		if (textTertiary.ok)
		{
			pal.setColor(QPalette::Active, QPalette::BrightText, textTertiary.value);
			pal.setColor(QPalette::Inactive, QPalette::BrightText, textTertiary.value);
		}

		const ColorValue tipBg = m_store->color(QStringLiteral("tooltip"), QStringLiteral("bg"));
		if (tipBg.ok)
		{
			pal.setColor(QPalette::Active, QPalette::ToolTipBase, tipBg.value);
			pal.setColor(QPalette::Inactive, QPalette::ToolTipBase, tipBg.value);
		}
		const ColorValue tipFg = m_store->color(QStringLiteral("tooltip"), QStringLiteral("fg"));
		if (tipFg.ok)
		{
			pal.setColor(QPalette::Active, QPalette::ToolTipText, tipFg.value);
			pal.setColor(QPalette::Inactive, QPalette::ToolTipText, tipFg.value);
		}

		const ColorValue viewBg = m_store->color(QStringLiteral("view"), QStringLiteral("bg"));
		if (viewBg.ok)
		{
			pal.setColor(QPalette::Active, QPalette::Base, viewBg.value);
			pal.setColor(QPalette::Inactive, QPalette::Base, viewBg.value);
		}
		const ColorValue viewAlt = m_store->color(QStringLiteral("view"), QStringLiteral("bg.alternate"));
		if (viewAlt.ok)
		{
			pal.setColor(QPalette::Active, QPalette::AlternateBase, viewAlt.value);
			pal.setColor(QPalette::Inactive, QPalette::AlternateBase, viewAlt.value);
		}
		const ColorValue viewFg = m_store->color(QStringLiteral("view"), QStringLiteral("fg"));
		if (viewFg.ok)
		{
			pal.setColor(QPalette::Active, QPalette::Text, viewFg.value);
			pal.setColor(QPalette::Inactive, QPalette::Text, viewFg.value);
		}
		const ColorValue viewSel = m_store->color(QStringLiteral("view"), QStringLiteral("bg.selected"));
		if (viewSel.ok)
		{
			pal.setColor(QPalette::Active, QPalette::Highlight, viewSel.value);
		}
		const ColorValue viewSelFg = m_store->color(QStringLiteral("view"), QStringLiteral("fg.selected"));
		if (viewSelFg.ok)
		{
			pal.setColor(QPalette::Active, QPalette::HighlightedText, viewSelFg.value);
		}
		const ColorValue viewGrid = m_store->color(QStringLiteral("view"), QStringLiteral("grid"));
		if (viewGrid.ok)
		{
			pal.setColor(QPalette::Active, QPalette::Mid, viewGrid.value);
			pal.setColor(QPalette::Inactive, QPalette::Mid, viewGrid.value);
		}
		return pal;
	}

	int QThemeStyle::pixelMetric(PixelMetric metric, const QStyleOption* option, const QWidget* widget) const
	{
		switch (metric)
		{
		case PM_ButtonMargin:
			return roleMetric(QStringLiteral("button"), QStringLiteral("padding"), QProxyStyle::pixelMetric(metric, option, widget));
		case PM_IndicatorWidth:
		case PM_IndicatorHeight:
		case PM_ExclusiveIndicatorWidth:
		case PM_ExclusiveIndicatorHeight:
			return roleMetric(QStringLiteral("check"), QStringLiteral("indicator"), QProxyStyle::pixelMetric(metric, option, widget));
		case PM_ScrollBarExtent:
			return roleMetric(QStringLiteral("scroll"), QStringLiteral("thickness"), QProxyStyle::pixelMetric(metric, option, widget));
		case PM_TabBarTabHSpace:
			return 16;
		case PM_DefaultFrameWidth:
			return 1;
		case PM_MenuBarHMargin:
		case PM_MenuBarVMargin:
			return 4;
		case PM_MenuHMargin:
			return roleMetric(QStringLiteral("menu"), QStringLiteral("itemInset"), 2);
		case PM_MenuVMargin:
			return roleMetric(QStringLiteral("menu"), QStringLiteral("itemInset"), 2);
		case PM_MenuPanelWidth:
			return 1;
		case PM_HeaderMargin:
			return 4;
		case PM_ToolBarHandleExtent:
			return roleMetric(QStringLiteral("toolbar"), QStringLiteral("handleExtent"), 8);
		case PM_ToolBarItemSpacing:
			return roleMetric(QStringLiteral("toolbar"), QStringLiteral("itemSpacing"), 4);
		case PM_ToolBarItemMargin:
			return roleMetric(QStringLiteral("toolbar"), QStringLiteral("padding"), 4);
		case PM_ToolBarFrameWidth:
			return roleMetric(QStringLiteral("toolbar"), QStringLiteral("frameWidth"), 1);
		case PM_ToolTipLabelFrameWidth:
			return roleMetric(QStringLiteral("tooltip"), QStringLiteral("padding"), 6);
		case PM_SpinBoxFrameWidth:
			return 1;
		case PM_TabBarTabVSpace:
			return 6;
		case PM_ButtonDefaultIndicator:
			return 0;
		case PM_SliderThickness:
			return roleMetric(QStringLiteral("slider"), QStringLiteral("handle"), 16);
		case PM_SliderLength:
			return roleMetric(QStringLiteral("slider"), QStringLiteral("handle"), 16);
		case PM_SliderControlThickness:
			return roleMetric(QStringLiteral("slider"), QStringLiteral("handle"), 16);
		case PM_ProgressBarChunkWidth:
			return roleMetric(QStringLiteral("progress"), QStringLiteral("height"), 6);
		case PM_TreeViewIndentation:
			return roleMetric(QStringLiteral("view"), QStringLiteral("indent"), 20);
		case PM_SplitterWidth:
			return roleMetric(QStringLiteral("splitter"), QStringLiteral("width"), 6);
		case PM_DockWidgetTitleBarButtonMargin:
			return 4;
		case PM_DockWidgetTitleMargin:
			return 4;
		case PM_DockWidgetFrameWidth:
			return roleMetric(QStringLiteral("dock"), QStringLiteral("frameWidth"), 1);
		case PM_DockWidgetSeparatorExtent:
			return roleMetric(QStringLiteral("dock"), QStringLiteral("separator"), 4);
		case PM_DockWidgetHandleExtent:
			return roleMetric(QStringLiteral("dock"), QStringLiteral("handle"), 10);
		case PM_FocusFrameVMargin:
		case PM_FocusFrameHMargin:
			return roleMetric(QStringLiteral("focus"), QStringLiteral("margin"), 1);
		default:
			return QProxyStyle::pixelMetric(metric, option, widget);
		}
	}

	QSize QThemeStyle::sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& contentsSize,
										const QWidget* widget) const
	{
		QSize sz = QProxyStyle::sizeFromContents(type, option, contentsSize, widget);
		switch (type)
		{
		case CT_PushButton:
		case CT_ToolButton: {
			const int h = roleMetric(QStringLiteral("button"), QStringLiteral("height"), sz.height());
			sz.setHeight(qMax(sz.height(), h));
			break;
		}
		case CT_LineEdit: {
			const int pad = roleMetric(QStringLiteral("edit"), QStringLiteral("padding"), 6);
			const int h = roleMetric(QStringLiteral("button"), QStringLiteral("height"), sz.height());
			sz.setHeight(qMax(sz.height(), h));
			sz.setWidth(sz.width() + pad);
			break;
		}
		case CT_ComboBox:
		case CT_SpinBox: {
			const int h = roleMetric(QStringLiteral("button"), QStringLiteral("height"), sz.height());
			sz.setHeight(qMax(sz.height(), h));
			break;
		}
		case CT_TabBarTab: {
			const int h = roleMetric(QStringLiteral("tab"), QStringLiteral("height"), sz.height());
			sz.setHeight(qMax(sz.height(), h));
			break;
		}
		case CT_HeaderSection: {
			const int h = roleMetric(QStringLiteral("header"), QStringLiteral("height"), sz.height());
			sz.setHeight(qMax(sz.height(), h));
			break;
		}
		case CT_MenuItem: {
			const int h = roleMetric(QStringLiteral("menu"), QStringLiteral("itemHeight"), sz.height());
			sz.setHeight(qMax(sz.height(), h));
			break;
		}
		case CT_ItemViewItem: {
			const int h = roleMetric(QStringLiteral("view"), QStringLiteral("itemHeight"), sz.height());
			sz.setHeight(qMax(sz.height(), h));
			break;
		}
		case CT_GroupBox: {
			const int pad = roleMetric(QStringLiteral("groupbox"), QStringLiteral("padding"), 8);
			sz += QSize(pad * 2, pad);
			break;
		}
		default:
			break;
		}
		return sz;
	}

	QRect QThemeStyle::subControlRect(ComplexControl control, const QStyleOptionComplex* option, SubControl subControl,
									  const QWidget* widget) const
	{
		if (control == CC_SpinBox)
		{
			const auto* spin = qstyleoption_cast<const QStyleOptionSpinBox*>(option);
			if (spin)
			{
				QRect r = spin->rect;
				const int bw = roleMetric(QStringLiteral("spin"), QStringLiteral("buttonWidth"), 20);
				const int frame = pixelMetric(PM_SpinBoxFrameWidth, spin, widget);
				switch (subControl)
				{
				case SC_SpinBoxFrame:
					return r;
				case SC_SpinBoxEditField:
					return r.adjusted(frame, frame, -(bw + frame), -frame);
				case SC_SpinBoxUp:
					return QRect(r.right() - bw - frame + 1, r.top() + frame, bw, (r.height() - 2 * frame) / 2);
				case SC_SpinBoxDown:
					return QRect(r.right() - bw - frame + 1, r.top() + frame + (r.height() - 2 * frame) / 2, bw,
								 (r.height() - 2 * frame) - (r.height() - 2 * frame) / 2);
				default:
					break;
				}
			}
		}
		return QProxyStyle::subControlRect(control, option, subControl, widget);
	}

	QRect QThemeStyle::subElementRect(SubElement element, const QStyleOption* option, const QWidget* widget) const
	{
		if (element == SE_CheckBoxFocusRect || element == SE_RadioButtonFocusRect)
		{
			const int margin = roleMetric(QStringLiteral("focus"), QStringLiteral("margin"), 1);
			if (widget)
			{
				return widget->rect().adjusted(margin, margin, -margin, -margin);
			}
			if (option)
			{
				return option->rect.adjusted(margin, margin, -margin, -margin);
			}
		}
		if (element == SE_PushButtonFocusRect)
		{
			const int margin = roleMetric(QStringLiteral("focus"), QStringLiteral("margin"), 1);
			if (option)
			{
				return option->rect.adjusted(margin, margin, -margin, -margin);
			}
		}
		if (element == SE_ItemViewItemFocusRect)
		{
			return QRect();
		}
		if (element == SE_ItemViewItemText || element == SE_ItemViewItemDecoration || element == SE_ItemViewItemCheckIndicator)
		{
			QRect r = QProxyStyle::subElementRect(element, option, widget);
			if (!option || r.isEmpty())
			{
				return r;
			}
			const int padL = roleMetric(QStringLiteral("view"), QStringLiteral("contentPadLeft"), 8);
			const int padR = roleMetric(QStringLiteral("view"), QStringLiteral("contentPadRight"), 8);
			if (element == SE_ItemViewItemText)
			{
				r.setLeft(qMax(r.left(), option->rect.left() + padL));
				r.setRight(qMin(r.right(), option->rect.right() - padR));
			}
			else
			{
				r.translate(padL, 0);
				if (r.right() > option->rect.right() - padR)
				{
					r.moveRight(option->rect.right() - padR);
				}
			}
			return r;
		}
		if (element == SE_ComboBoxFocusRect || element == SE_SliderFocusRect)
		{
			return QRect();
		}
		return QProxyStyle::subElementRect(element, option, widget);
	}

	void QThemeStyle::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
	{
		if (!painter || !option)
		{
			QProxyStyle::drawPrimitive(element, option, painter, widget);
			return;
		}
		const auto ctx = makeCtx(m_store.get(), m_dpiScale);
		if (style_detail::tryEditPrimitive(ctx, element, option, painter, widget)
			|| style_detail::tryCheckPrimitive(ctx, element, option, painter, widget)
			|| style_detail::tryMenuPrimitive(ctx, element, option, painter, widget)
			|| style_detail::tryFocusPrimitive(ctx, element, option, painter, widget)
			|| style_detail::tryComplexPrimitive(ctx, element, option, painter, widget)
			|| style_detail::tryButtonPrimitive(ctx, element, option, painter, widget)
			|| style_detail::tryChromePrimitive(ctx, element, option, painter, widget)
			|| style_detail::tryItemViewPrimitive(ctx, element, option, painter, widget))
		{
			return;
		}
		QProxyStyle::drawPrimitive(element, option, painter, widget);
	}

	void QThemeStyle::drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
	{
		const auto ctx = makeCtx(m_store.get(), m_dpiScale);
		if (style_detail::tryButtonControl(ctx, element, option, painter, widget, this)
			|| style_detail::tryTabHeaderControl(ctx, element, option, painter, widget, this)
			|| style_detail::tryMenuControl(ctx, element, option, painter, widget, this)
			|| style_detail::tryChromeControl(ctx, element, option, painter, widget, this)
			|| style_detail::tryComplexControlLabel(ctx, element, option, painter, widget, this)
			|| style_detail::tryProgressControl(ctx, element, option, painter, widget, this)
			|| style_detail::tryItemViewControl(ctx, element, option, painter, widget, this))
		{
			return;
		}
		QProxyStyle::drawControl(element, option, painter, widget);
	}

	void QThemeStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex* option, QPainter* painter,
										 const QWidget* widget) const
	{
		const auto ctx = makeCtx(m_store.get(), m_dpiScale);
		if (style_detail::tryComplexControl(ctx, control, option, painter, widget, this))
		{
			return;
		}
		QProxyStyle::drawComplexControl(control, option, painter, widget);
	}
} // namespace qtheme
