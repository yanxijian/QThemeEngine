#include "qtheme/style.hpp"

#include <QAbstractScrollArea>
#include <QCalendarWidget>
#include <QFrame>
#include <QtMath>
#include <QPainter>
#include <QPainterPath>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QStatusBar>
#include <QStyleFactory>
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QStyleOptionComboBox>
#include <QStyleOptionComplex>
#include <QStyleOptionDockWidget>
#include <QStyleOptionFrame>
#include <QStyleOptionHeader>
#include <QStyleOptionMenuItem>
#include <QStyleOptionProgressBar>
#include <QStyleOptionSlider>
#include <QStyleOptionSpinBox>
#include <QStyleOptionTab>
#include <QStyleOptionToolButton>
#include <QStyleOptionViewItem>
#include <QTextEdit>

namespace qtheme {

namespace {

QStyle* makeFusionBase()
{
	QStyle* fusion = QStyleFactory::create(QStringLiteral("Fusion"));
	return fusion ? fusion : QStyleFactory::create(QStringLiteral("windows"));
}

void drawRounded(QPainter* painter, const QRect& rect, int radius, const QColor& fill,
				 const QColor& border, qreal borderWidth = 1.0)
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);
	const QRectF r = QRectF(rect).adjusted(0.5, 0.5, -0.5, -0.5);
	QPainterPath path;
	path.addRoundedRect(r, radius, radius);
	painter->fillPath(path, fill);
	painter->setPen(QPen(border, borderWidth));
	painter->drawPath(path);
	painter->restore();
}

void drawArrow(QPainter* painter, const QRect& rect, Qt::ArrowType type, const QColor& color)
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->setPen(Qt::NoPen);
	painter->setBrush(color);
	const int s = qMin(rect.width(), rect.height()) / 3;
	const QPoint c = rect.center();
	QPolygon poly;
	switch (type)
	{
	case Qt::UpArrow:
		poly << QPoint(c.x(), c.y() - s) << QPoint(c.x() - s, c.y() + s / 2)
			 << QPoint(c.x() + s, c.y() + s / 2);
		break;
	case Qt::DownArrow:
		poly << QPoint(c.x(), c.y() + s) << QPoint(c.x() - s, c.y() - s / 2)
			 << QPoint(c.x() + s, c.y() - s / 2);
		break;
	case Qt::LeftArrow:
		poly << QPoint(c.x() - s, c.y()) << QPoint(c.x() + s / 2, c.y() - s)
			 << QPoint(c.x() + s / 2, c.y() + s);
		break;
	case Qt::RightArrow:
	default:
		poly << QPoint(c.x() + s, c.y()) << QPoint(c.x() - s / 2, c.y() - s)
			 << QPoint(c.x() - s / 2, c.y() + s);
		break;
	}
	painter->drawPolygon(poly);
	painter->restore();
}

QString stateBorderRole(const QStyleOption* option)
{
	if (!(option->state & QStyle::State_Enabled))
	{
		return QStringLiteral("border.disabled");
	}
	if (option->state & QStyle::State_HasFocus)
	{
		return QStringLiteral("border.focus");
	}
	if (option->state & QStyle::State_MouseOver)
	{
		return QStringLiteral("border.hover");
	}
	return QStringLiteral("border");
}

bool isTextEditLike(const QWidget* widget)
{
	return qobject_cast<const QTextEdit*>(widget) || qobject_cast<const QPlainTextEdit*>(widget);
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

QColor QThemeStyle::roleColor(const QString& group, const QString& role, const QColor& fallback) const
{
	if (!m_store)
	{
		return fallback;
	}
	const ColorValue cv = m_store->color(group, role, fallback);
	return cv.ok ? cv.value : fallback;
}

int QThemeStyle::scaleMetric(int logicalPx) const
{
	if (logicalPx <= 0)
	{
		return logicalPx;
	}
	return qRound(logicalPx * m_dpiScale);
}

int QThemeStyle::roleMetric(const QString& group, const QString& role, int fallback) const
{
	if (!m_store)
	{
		return scaleMetric(fallback);
	}
	bool ok = false;
	const int v = m_store->metric(group, role, fallback, &ok);
	return scaleMetric(ok ? v : fallback);
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
	// Fluent TextControlPlaceholderForeground ≈ TextFillColorSecondary/Tertiary (not primary Text).
	const ColorValue editPlaceholder =
		m_store->color(QStringLiteral("edit"), QStringLiteral("fg.placeholder"));
	if (editPlaceholder.ok)
	{
		pal.setColor(QPalette::Active, QPalette::PlaceholderText, editPlaceholder.value);
		pal.setColor(QPalette::Inactive, QPalette::PlaceholderText, editPlaceholder.value);
		pal.setColor(QPalette::Disabled, QPalette::PlaceholderText,
					 editFgDis.ok ? editFgDis.value : editPlaceholder.value);
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
		return roleMetric(QStringLiteral("button"), QStringLiteral("padding"),
						  QProxyStyle::pixelMetric(metric, option, widget));
	case PM_IndicatorWidth:
	case PM_IndicatorHeight:
	case PM_ExclusiveIndicatorWidth:
	case PM_ExclusiveIndicatorHeight:
		return roleMetric(QStringLiteral("check"), QStringLiteral("indicator"),
						  QProxyStyle::pixelMetric(metric, option, widget));
	case PM_ScrollBarExtent:
		return roleMetric(QStringLiteral("scroll"), QStringLiteral("thickness"),
						  QProxyStyle::pixelMetric(metric, option, widget));
	case PM_TabBarTabHSpace:
		return 16;
	case PM_DefaultFrameWidth:
		return 1;
	case PM_MenuBarHMargin:
	case PM_MenuBarVMargin:
		return 4;
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
		return 1;
	default:
		return QProxyStyle::pixelMetric(metric, option, widget);
	}
}

QSize QThemeStyle::sizeFromContents(ContentsType type, const QStyleOption* option,
									const QSize& contentsSize, const QWidget* widget) const
{
	QSize sz = QProxyStyle::sizeFromContents(type, option, contentsSize, widget);
	switch (type)
	{
	case CT_PushButton:
	case CT_ToolButton:
	{
		const int h = roleMetric(QStringLiteral("button"), QStringLiteral("height"), sz.height());
		sz.setHeight(qMax(sz.height(), h));
		break;
	}
	case CT_LineEdit:
	{
		const int pad = roleMetric(QStringLiteral("edit"), QStringLiteral("padding"), 6);
		const int h = roleMetric(QStringLiteral("button"), QStringLiteral("height"), sz.height());
		sz.setHeight(qMax(sz.height(), h));
		sz.setWidth(sz.width() + pad);
		break;
	}
	case CT_ComboBox:
	case CT_SpinBox:
	{
		const int h = roleMetric(QStringLiteral("button"), QStringLiteral("height"), sz.height());
		sz.setHeight(qMax(sz.height(), h));
		break;
	}
	case CT_TabBarTab:
	{
		const int h = roleMetric(QStringLiteral("tab"), QStringLiteral("height"), sz.height());
		sz.setHeight(qMax(sz.height(), h));
		break;
	}
	case CT_HeaderSection:
	{
		const int h = roleMetric(QStringLiteral("header"), QStringLiteral("height"), sz.height());
		sz.setHeight(qMax(sz.height(), h));
		break;
	}
	case CT_MenuItem:
	{
		const int h = roleMetric(QStringLiteral("menu"), QStringLiteral("itemHeight"), sz.height());
		sz.setHeight(qMax(sz.height(), h));
		break;
	}
	case CT_ItemViewItem:
	{
		const int h = roleMetric(QStringLiteral("view"), QStringLiteral("itemHeight"), sz.height());
		sz.setHeight(qMax(sz.height(), h));
		break;
	}
	case CT_GroupBox:
	{
		const int pad = roleMetric(QStringLiteral("groupbox"), QStringLiteral("padding"), 8);
		sz += QSize(pad * 2, pad);
		break;
	}
	default:
		break;
	}
	return sz;
}

QRect QThemeStyle::subControlRect(ComplexControl control, const QStyleOptionComplex* option,
								  SubControl subControl, const QWidget* widget) const
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

void QThemeStyle::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter,
								const QWidget* widget) const
{
	if (!painter || !option)
	{
		QProxyStyle::drawPrimitive(element, option, painter, widget);
		return;
	}

	if (element == PE_FrameLineEdit || element == PE_PanelLineEdit)
	{
		QString borderRole = QStringLiteral("border");
		if (!(option->state & State_Enabled))
		{
			borderRole = QStringLiteral("border.disabled");
		}
		else if (option->state & State_HasFocus)
		{
			borderRole = QStringLiteral("border.focus");
		}
		else if (option->state & State_MouseOver)
		{
			borderRole = QStringLiteral("border.hover");
		}
		const QColor bg = roleColor(QStringLiteral("edit"),
									(option->state & State_Enabled) ? QStringLiteral("bg")
																	: QStringLiteral("bg.disabled"),
									option->palette.color(QPalette::Base));
		const QColor border = roleColor(QStringLiteral("edit"), borderRole,
										option->palette.color(QPalette::Mid));
		const int radius = roleMetric(QStringLiteral("edit"), QStringLiteral("radius"), 4);
		const qreal bw = (option->state & State_HasFocus) && (option->state & State_Enabled) ? 2.0 : 1.0;
		drawRounded(painter, option->rect, radius, bg, border, bw);
		return;
	}

	if (element == PE_IndicatorCheckBox || element == PE_IndicatorRadioButton)
	{
		const bool radio = (element == PE_IndicatorRadioButton);
		const bool enabled = option->state & State_Enabled;
		const bool on = option->state & State_On;
		const bool partial = option->state & State_NoChange;
		const bool hover = option->state & State_MouseOver;
		QColor bg = roleColor(QStringLiteral("check"), QStringLiteral("bg"),
							  option->palette.color(QPalette::Base));
		QColor border = roleColor(QStringLiteral("check"), QStringLiteral("border"),
								  option->palette.color(QPalette::Mid));
		if (!enabled)
		{
			if (on || partial)
			{
				bg = roleColor(QStringLiteral("check"), QStringLiteral("bg.checked.disabled"), bg);
			}
			else
			{
				bg = roleColor(QStringLiteral("check"), QStringLiteral("bg.disabled"), bg);
			}
			border = roleColor(QStringLiteral("check"), QStringLiteral("border.disabled"), border);
		}
		else if (on || partial)
		{
			bg = roleColor(QStringLiteral("check"), QStringLiteral("bg.checked"), bg);
			border = roleColor(QStringLiteral("check"), QStringLiteral("border.checked"), border);
		}
		else if (hover)
		{
			bg = roleColor(QStringLiteral("check"), QStringLiteral("bg.hover"), bg);
		}
		const int radius =
			radio ? option->rect.width() / 2
				  : roleMetric(QStringLiteral("check"), QStringLiteral("radius"), 3);
		drawRounded(painter, option->rect.adjusted(1, 1, -1, -1), radius, bg, border, 1.5);

		if (on || partial)
		{
			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);
			const QColor ind =
				roleColor(QStringLiteral("check"),
						  enabled ? QStringLiteral("indicator") : QStringLiteral("indicator.disabled"),
						  option->palette.color(QPalette::HighlightedText));
			painter->setPen(QPen(ind, 2));
			const QRect r = option->rect.adjusted(4, 4, -4, -4);
			if (radio)
			{
				painter->setBrush(ind);
				painter->setPen(Qt::NoPen);
				painter->drawEllipse(r.adjusted(2, 2, -2, -2));
			}
			else if (partial)
			{
				painter->drawLine(r.left(), r.center().y(), r.right(), r.center().y());
			}
			else
			{
				QPainterPath tick;
				tick.moveTo(r.left(), r.center().y());
				tick.lineTo(r.center().x() - 1, r.bottom() - 1);
				tick.lineTo(r.right(), r.top());
				painter->drawPath(tick);
			}
			painter->restore();
		}
		return;
	}

	if (element == PE_PanelMenu || element == PE_FrameMenu)
	{
		const QColor bg = roleColor(QStringLiteral("menu"), QStringLiteral("bg"),
									option->palette.color(QPalette::Window));
		const QColor border = roleColor(QStringLiteral("menu"), QStringLiteral("border"),
										option->palette.color(QPalette::Mid));
		const int radius = roleMetric(QStringLiteral("menu"), QStringLiteral("radius"), 4);
		drawRounded(painter, option->rect, radius, bg, border);
		return;
	}

	if (element == PE_IndicatorArrowDown || element == PE_IndicatorArrowUp
		|| element == PE_IndicatorArrowLeft || element == PE_IndicatorArrowRight
		|| element == PE_IndicatorSpinUp || element == PE_IndicatorSpinDown
		|| element == PE_IndicatorSpinPlus || element == PE_IndicatorSpinMinus)
	{
		Qt::ArrowType type = Qt::DownArrow;
		if (element == PE_IndicatorArrowUp || element == PE_IndicatorSpinUp
			|| element == PE_IndicatorSpinPlus)
		{
			type = Qt::UpArrow;
		}
		else if (element == PE_IndicatorArrowLeft)
		{
			type = Qt::LeftArrow;
		}
		else if (element == PE_IndicatorArrowRight)
		{
			type = Qt::RightArrow;
		}
		const bool enabled = option->state & State_Enabled;
		const bool fromSpin = element == PE_IndicatorSpinUp || element == PE_IndicatorSpinDown
							  || element == PE_IndicatorSpinPlus || element == PE_IndicatorSpinMinus;
		const QString group = fromSpin ? QStringLiteral("spin") : QStringLiteral("combo");
		const QColor color =
			roleColor(group, enabled ? QStringLiteral("arrow") : QStringLiteral("arrow.disabled"),
					  option->palette.color(QPalette::WindowText));
		drawArrow(painter, option->rect, type, color);
		return;
	}

	if (element == PE_PanelButtonTool)
	{
		const bool enabled = option->state & State_Enabled;
		QString bgRole = QStringLiteral("bg");
		if (!enabled)
		{
			bgRole = QStringLiteral("bg.disabled");
		}
		else if (option->state & (State_Sunken | State_On))
		{
			bgRole = QStringLiteral("bg.pressed");
		}
		else if (option->state & State_MouseOver)
		{
			bgRole = QStringLiteral("bg.hover");
		}
		const QColor bg =
			roleColor(QStringLiteral("button"), bgRole, option->palette.color(QPalette::Button));
		const QColor border =
			roleColor(QStringLiteral("button"), QStringLiteral("border"), option->palette.mid().color());
		const int radius = roleMetric(QStringLiteral("button"), QStringLiteral("radius"), 4);
		if (bgRole != QStringLiteral("bg") || (option->state & State_Raised))
		{
			drawRounded(painter, option->rect, radius, bg, border);
		}
		return;
	}

	if (element == PE_FrameGroupBox)
	{
		const bool enabled = option->state & State_Enabled;
		const QColor bg = roleColor(QStringLiteral("groupbox"), QStringLiteral("bg"),
									option->palette.color(QPalette::Window));
		const QColor border = roleColor(QStringLiteral("groupbox"), QStringLiteral("border"),
										option->palette.mid().color());
		const int radius = roleMetric(QStringLiteral("groupbox"), QStringLiteral("radius"), 4);
		Q_UNUSED(enabled);
		drawRounded(painter, option->rect, radius, bg, border);
		return;
	}

	if (element == PE_PanelTipLabel)
	{
		const QColor bg = roleColor(QStringLiteral("tooltip"), QStringLiteral("bg"),
									option->palette.color(QPalette::ToolTipBase));
		const QColor border = roleColor(QStringLiteral("tooltip"), QStringLiteral("border"),
										option->palette.mid().color());
		const int radius = roleMetric(QStringLiteral("tooltip"), QStringLiteral("radius"), 4);
		drawRounded(painter, option->rect, radius, bg, border);
		return;
	}

	if (element == PE_IndicatorToolBarSeparator)
	{
		const QColor sep = roleColor(QStringLiteral("toolbar"), QStringLiteral("separator"),
									 option->palette.mid().color());
		painter->save();
		painter->setPen(sep);
		const QRect r = option->rect;
		if (option->state & State_Horizontal)
		{
			const int x = r.center().x();
			painter->drawLine(x, r.top() + 4, x, r.bottom() - 4);
		}
		else
		{
			const int y = r.center().y();
			painter->drawLine(r.left() + 4, y, r.right() - 4, y);
		}
		painter->restore();
		return;
	}

	if (element == PE_IndicatorToolBarHandle)
	{
		const QColor handle = roleColor(QStringLiteral("toolbar"), QStringLiteral("handle"),
										option->palette.mid().color());
		painter->save();
		painter->setPen(handle);
		painter->setBrush(handle);
		const QRect r = option->rect.adjusted(2, 2, -2, -2);
		const bool horiz = option->state & State_Horizontal;
		for (int i = 0; i < 3; ++i)
		{
			if (horiz)
			{
				const int x = r.left() + i * 3;
				painter->drawRect(x, r.top() + 2, 1, r.height() - 4);
			}
			else
			{
				const int y = r.top() + i * 3;
				painter->drawRect(r.left() + 2, y, r.width() - 4, 1);
			}
		}
		painter->restore();
		return;
	}

	if (element == PE_Frame || element == PE_FrameWindow)
	{
		const bool textEdit = isTextEditLike(widget);
		const auto* scroll = qobject_cast<const QAbstractScrollArea*>(widget);
		if (textEdit || (scroll && scroll->frameWidth() > 0 && !widget->inherits("QAbstractItemView")))
		{
			const QString group = QStringLiteral("textedit");
			const bool enabled = option->state & State_Enabled;
			const QString borderRole = stateBorderRole(option);
			const QColor bg =
				roleColor(group, enabled ? QStringLiteral("bg") : QStringLiteral("bg.disabled"),
						  option->palette.color(QPalette::Base));
			const QColor border = roleColor(group, borderRole, option->palette.mid().color());
			const int radius = roleMetric(group, QStringLiteral("radius"), 4);
			const qreal bw =
				(borderRole == QStringLiteral("border.focus") && enabled) ? 2.0 : 1.0;
			drawRounded(painter, option->rect, radius, bg, border, bw);
			return;
		}

		if (qobject_cast<const QCalendarWidget*>(widget)
			|| (widget && widget->parentWidget()
				&& qobject_cast<const QCalendarWidget*>(widget->parentWidget())))
		{
			const QColor bg = roleColor(QStringLiteral("calendar"), QStringLiteral("bg"),
										option->palette.color(QPalette::Base));
			const QColor border = roleColor(QStringLiteral("calendar"), QStringLiteral("border"),
											option->palette.mid().color());
			painter->fillRect(option->rect, bg);
			painter->setPen(border);
			painter->drawRect(option->rect.adjusted(0, 0, -1, -1));
			return;
		}

		const auto* frameOpt = qstyleoption_cast<const QStyleOptionFrame*>(option);
		const QFrame* frameWidget = qobject_cast<const QFrame*>(widget);
		const QFrame::Shape shape =
			frameOpt ? frameOpt->frameShape
					 : (frameWidget ? frameWidget->frameShape() : QFrame::NoFrame);
		if (shape == QFrame::HLine || shape == QFrame::VLine)
		{
			const QColor line =
				roleColor(QStringLiteral("frame"), QStringLiteral("line"), option->palette.mid().color());
			painter->save();
			painter->setPen(QPen(line, roleMetric(QStringLiteral("frame"), QStringLiteral("lineWidth"), 1)));
			if (shape == QFrame::HLine)
			{
				const int y = option->rect.center().y();
				painter->drawLine(option->rect.left(), y, option->rect.right(), y);
			}
			else
			{
				const int x = option->rect.center().x();
				painter->drawLine(x, option->rect.top(), x, option->rect.bottom());
			}
			painter->restore();
			return;
		}

		if (shape != QFrame::NoFrame)
		{
			const QColor bg = roleColor(QStringLiteral("frame"), QStringLiteral("bg"),
										option->palette.color(QPalette::Window));
			const QColor border = roleColor(QStringLiteral("frame"), QStringLiteral("border"),
											option->palette.mid().color());
			const int radius = roleMetric(QStringLiteral("frame"), QStringLiteral("radius"), 4);
			drawRounded(painter, option->rect, radius, bg, border);
			return;
		}
	}

	if (element == PE_FrameDockWidget)
	{
		const QColor bg =
			roleColor(QStringLiteral("dock"), QStringLiteral("bg"), option->palette.color(QPalette::Window));
		const QColor border =
			roleColor(QStringLiteral("dock"), QStringLiteral("border"), option->palette.mid().color());
		painter->fillRect(option->rect, bg);
		painter->setPen(border);
		painter->drawRect(option->rect.adjusted(0, 0, -1, -1));
		return;
	}

	if (element == PE_FrameStatusBarItem || element == PE_PanelStatusBar)
	{
		const QColor bg =
			roleColor(QStringLiteral("status"), QStringLiteral("bg"), option->palette.color(QPalette::Window));
		painter->fillRect(option->rect, bg);
		if (element == PE_PanelStatusBar)
		{
			const QColor border =
				roleColor(QStringLiteral("status"), QStringLiteral("border"), option->palette.mid().color());
			painter->setPen(border);
			painter->drawLine(option->rect.topLeft(), option->rect.topRight());
		}
		return;
	}

	if (element == PE_IndicatorDockWidgetResizeHandle)
	{
		const QColor handle = roleColor(QStringLiteral("dock"), QStringLiteral("border"),
										option->palette.mid().color());
		painter->fillRect(option->rect, handle);
		return;
	}

	if (element == PE_PanelItemViewItem || element == PE_PanelItemViewRow)
	{
		const auto* item = qstyleoption_cast<const QStyleOptionViewItem*>(option);
		const bool enabled = option->state & State_Enabled;
		const bool selected = option->state & State_Selected;
		const bool hover = option->state & State_MouseOver;
		const bool active = option->state & State_Active;
		const bool alternate =
			item && (item->features & QStyleOptionViewItem::Alternate);

		QString bgRole = QStringLiteral("bg");
		if (selected)
		{
			bgRole = active ? QStringLiteral("bg.selected") : QStringLiteral("bg.selected.inactive");
		}
		else if (hover && enabled)
		{
			bgRole = QStringLiteral("bg.hover");
		}
		else if (alternate)
		{
			bgRole = QStringLiteral("bg.alternate");
		}

		const QColor bg = roleColor(QStringLiteral("view"), bgRole,
									selected ? option->palette.color(QPalette::Highlight)
											 : option->palette.color(QPalette::Base));
		painter->fillRect(option->rect, bg);
		return;
	}

	if (element == PE_IndicatorBranch)
	{
		const bool hasChildren = option->state & State_Children;
		const bool open = option->state & State_Open;
		if (!hasChildren)
		{
			return;
		}
		const QColor branch = roleColor(QStringLiteral("view"), QStringLiteral("branch"),
										option->palette.color(QPalette::WindowText));
		drawArrow(painter, option->rect, open ? Qt::DownArrow : Qt::RightArrow, branch);
		return;
	}

	QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void QThemeStyle::drawControl(ControlElement element, const QStyleOption* option, QPainter* painter,
							  const QWidget* widget) const
{
	if (element == CE_PushButtonBevel)
	{
		const auto* btn = qstyleoption_cast<const QStyleOptionButton*>(option);
		if (btn && painter)
		{
			const bool enabled = btn->state & State_Enabled;
			const bool isDefault =
				(btn->features & QStyleOptionButton::DefaultButton) && enabled;
			QString bgRole = QStringLiteral("bg");
			if (!enabled)
			{
				bgRole = QStringLiteral("bg.disabled");
			}
			else if (isDefault)
			{
				if (btn->state & State_Sunken)
				{
					bgRole = QStringLiteral("bg.accent.pressed");
				}
				else if (btn->state & State_MouseOver)
				{
					bgRole = QStringLiteral("bg.accent.hover");
				}
				else
				{
					bgRole = QStringLiteral("bg.accent");
				}
			}
			else if (btn->state & State_Sunken)
			{
				bgRole = QStringLiteral("bg.pressed");
			}
			else if (btn->state & State_MouseOver)
			{
				bgRole = QStringLiteral("bg.hover");
			}

			const QColor bg =
				roleColor(QStringLiteral("button"), bgRole, btn->palette.color(QPalette::Button));
			const bool focused = (btn->state & State_HasFocus) && enabled;
			const QColor border =
				roleColor(QStringLiteral("button"),
						  focused || isDefault ? QStringLiteral("border.focus") : QStringLiteral("border"),
						  btn->palette.color(QPalette::Mid));
			const int radius = roleMetric(QStringLiteral("button"), QStringLiteral("radius"), 4);
			drawRounded(painter, btn->rect, radius, bg, border, focused || isDefault ? 2.0 : 1.0);
			return;
		}
	}

	if (element == CE_PushButtonLabel)
	{
		const auto* btn = qstyleoption_cast<const QStyleOptionButton*>(option);
		if (btn && painter)
		{
			const bool enabled = btn->state & State_Enabled;
			const bool isDefault =
				(btn->features & QStyleOptionButton::DefaultButton) && enabled;
			QString fgRole = QStringLiteral("fg");
			if (!enabled)
			{
				fgRole = QStringLiteral("fg.disabled");
			}
			else if (isDefault)
			{
				fgRole = QStringLiteral("fg.accent");
			}
			const QColor fg =
				roleColor(QStringLiteral("button"), fgRole, btn->palette.color(QPalette::ButtonText));
			QStyleOptionButton copy = *btn;
			copy.palette.setColor(QPalette::ButtonText, fg);
			copy.palette.setColor(QPalette::WindowText, fg);
			QProxyStyle::drawControl(element, &copy, painter, widget);
			return;
		}
	}

	if (element == CE_TabBarTabShape)
	{
		const auto* tab = qstyleoption_cast<const QStyleOptionTab*>(option);
		if (tab && painter)
		{
			const bool enabled = tab->state & State_Enabled;
			const bool selected = tab->state & State_Selected;
			const bool hover = tab->state & State_MouseOver;
			QString bgRole = QStringLiteral("bg");
			if (!enabled)
			{
				bgRole = QStringLiteral("bg.disabled");
			}
			else if (selected)
			{
				bgRole = QStringLiteral("bg.selected");
			}
			else if (hover)
			{
				bgRole = QStringLiteral("bg.hover");
			}
			const QColor bg =
				roleColor(QStringLiteral("tab"), bgRole, tab->palette.color(QPalette::Window));
			const QColor border =
				roleColor(QStringLiteral("tab"), QStringLiteral("border"), tab->palette.mid().color());
			const int radius = roleMetric(QStringLiteral("tab"), QStringLiteral("radius"), 4);
			drawRounded(painter, tab->rect.adjusted(1, 1, -1, -1), radius, bg, border);
			if (selected && enabled)
			{
				const QColor ind = roleColor(QStringLiteral("tab"), QStringLiteral("indicator"),
											 tab->palette.color(QPalette::Highlight));
				painter->save();
				painter->setPen(Qt::NoPen);
				painter->setBrush(ind);
				painter->drawRect(tab->rect.left() + 6, tab->rect.bottom() - 2, tab->rect.width() - 12,
								  2);
				painter->restore();
			}
			return;
		}
	}

	if (element == CE_TabBarTabLabel)
	{
		const auto* tab = qstyleoption_cast<const QStyleOptionTab*>(option);
		if (tab && painter)
		{
			const bool enabled = tab->state & State_Enabled;
			const bool selected = tab->state & State_Selected;
			QString fgRole = QStringLiteral("fg");
			if (!enabled)
			{
				fgRole = QStringLiteral("fg.disabled");
			}
			else if (selected)
			{
				fgRole = QStringLiteral("fg.selected");
			}
			const QColor fg =
				roleColor(QStringLiteral("tab"), fgRole, tab->palette.color(QPalette::WindowText));
			QStyleOptionTab copy = *tab;
			copy.palette.setColor(QPalette::WindowText, fg);
			QProxyStyle::drawControl(element, &copy, painter, widget);
			return;
		}
	}

	if (element == CE_MenuItem)
	{
		const auto* mi = qstyleoption_cast<const QStyleOptionMenuItem*>(option);
		if (mi && painter)
		{
			if (mi->menuItemType == QStyleOptionMenuItem::Separator)
			{
				const QColor sep = roleColor(QStringLiteral("menu"), QStringLiteral("separator"),
											 mi->palette.mid().color());
				const int y = mi->rect.center().y();
				painter->fillRect(mi->rect.left() + 8, y, mi->rect.width() - 16, 1, sep);
				return;
			}

			const bool enabled = mi->state & State_Enabled;
			const QColor bg = roleColor(QStringLiteral("menu"), QStringLiteral("bg"),
										mi->palette.color(QPalette::Window));
			const QColor hover = roleColor(QStringLiteral("menu"), QStringLiteral("bg.hover"), bg);
			QString fgRole = QStringLiteral("fg");
			if (!enabled)
			{
				fgRole = QStringLiteral("fg.disabled");
			}
			else if (mi->state & State_Selected)
			{
				fgRole = QStringLiteral("fg.selected");
			}
			const QColor fg =
				roleColor(QStringLiteral("menu"), fgRole, mi->palette.color(QPalette::WindowText));

			QStyleOptionMenuItem copy = *mi;
			copy.palette.setColor(QPalette::Window, bg);
			copy.palette.setColor(QPalette::Base, bg);
			copy.palette.setColor(QPalette::Button, bg);
			copy.palette.setColor(QPalette::Highlight, hover);
			copy.palette.setColor(QPalette::HighlightedText, fg);
			copy.palette.setColor(QPalette::WindowText, fg);
			copy.palette.setColor(QPalette::Text, fg);
			copy.palette.setColor(QPalette::ButtonText, fg);
			QProxyStyle::drawControl(element, &copy, painter, widget);
			return;
		}
	}

	if (element == CE_MenuBarItem)
	{
		const auto* mi = qstyleoption_cast<const QStyleOptionMenuItem*>(option);
		if (mi && painter)
		{
			const bool enabled = mi->state & State_Enabled;
			QString bgRole = QStringLiteral("bar.bg");
			if ((mi->state & State_Sunken) && enabled)
			{
				bgRole = QStringLiteral("bar.bg.pressed");
			}
			else if ((mi->state & (State_Selected | State_MouseOver)) && enabled)
			{
				bgRole = QStringLiteral("bar.bg.hover");
			}
			const QColor bg =
				roleColor(QStringLiteral("menu"), bgRole, mi->palette.color(QPalette::Window));
			const QColor fg =
				roleColor(QStringLiteral("menu"),
						  enabled ? QStringLiteral("bar.fg") : QStringLiteral("fg.disabled"),
						  mi->palette.color(QPalette::WindowText));
			painter->fillRect(mi->rect, bg);
			painter->setPen(fg);
			painter->drawText(mi->rect, Qt::AlignCenter | Qt::TextShowMnemonic, mi->text);
			return;
		}
	}

	if (element == CE_MenuBarEmptyArea)
	{
		const QColor bg = roleColor(QStringLiteral("menu"), QStringLiteral("bar.bg"),
									option->palette.color(QPalette::Window));
		painter->fillRect(option->rect, bg);
		return;
	}

	if (element == CE_ToolBar)
	{
		const QColor bg = roleColor(QStringLiteral("toolbar"), QStringLiteral("bg"),
									option->palette.color(QPalette::Window));
		const QColor border = roleColor(QStringLiteral("toolbar"), QStringLiteral("border"),
										option->palette.mid().color());
		painter->fillRect(option->rect, bg);
		painter->setPen(border);
		painter->drawLine(option->rect.bottomLeft(), option->rect.bottomRight());
		return;
	}

	if (element == CE_Splitter)
	{
		const bool hover = option->state & State_MouseOver;
		const QColor bg =
			roleColor(QStringLiteral("splitter"), QStringLiteral("bg"), option->palette.window().color());
		const QColor handle =
			roleColor(QStringLiteral("splitter"),
					  hover ? QStringLiteral("handle.hover") : QStringLiteral("handle"),
					  option->palette.mid().color());
		painter->fillRect(option->rect, bg);
		painter->save();
		painter->setPen(Qt::NoPen);
		painter->setBrush(handle);
		const QRect r = option->rect;
		if (option->state & State_Horizontal)
		{
			const int x = r.center().x();
			painter->drawRect(x - 1, r.center().y() - 12, 2, 24);
		}
		else
		{
			const int y = r.center().y();
			painter->drawRect(r.center().x() - 12, y - 1, 24, 2);
		}
		painter->restore();
		return;
	}

	if (element == CE_DockWidgetTitle)
	{
		const auto* dock = qstyleoption_cast<const QStyleOptionDockWidget*>(option);
		if (dock && painter)
		{
			const bool active = dock->state & State_Active;
			const bool enabled = dock->state & State_Enabled;
			const QColor bg = roleColor(
				QStringLiteral("dock"),
				active ? QStringLiteral("title.bg.active") : QStringLiteral("title.bg"),
				dock->palette.color(QPalette::Window));
			const QColor fg =
				roleColor(QStringLiteral("dock"),
						  enabled ? QStringLiteral("title.fg") : QStringLiteral("title.fg.disabled"),
						  dock->palette.color(QPalette::WindowText));
			painter->fillRect(dock->rect, bg);
			painter->setPen(fg);
			QRect textRect = dock->rect.adjusted(8, 0, -8, 0);
			painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, dock->title);
			return;
		}
	}

	if (element == CE_PushButtonLabel)
	{
		const auto* btn = qstyleoption_cast<const QStyleOptionButton*>(option);
		if (btn && painter
			&& (btn->features & QStyleOptionButton::CommandLinkButton))
		{
			const bool enabled = btn->state & State_Enabled;
			const QColor title =
				roleColor(QStringLiteral("commandlink"),
						  enabled ? QStringLiteral("fg") : QStringLiteral("fg.disabled"),
						  btn->palette.color(QPalette::ButtonText));
			const QColor desc =
				roleColor(QStringLiteral("commandlink"),
						  enabled ? QStringLiteral("description") : QStringLiteral("fg.disabled"),
						  btn->palette.color(QPalette::WindowText));
			QStyleOptionButton copy = *btn;
			copy.palette.setColor(QPalette::ButtonText, title);
			copy.palette.setColor(QPalette::WindowText, desc);
			copy.palette.setColor(QPalette::Text, desc);
			QProxyStyle::drawControl(element, &copy, painter, widget);
			return;
		}
	}

	if (element == CE_HeaderSection)
	{
		const auto* header = qstyleoption_cast<const QStyleOptionHeader*>(option);
		if (header && painter)
		{
			const bool enabled = header->state & State_Enabled;
			QString bgRole = QStringLiteral("bg");
			if (enabled)
			{
				if (header->state & State_Sunken)
				{
					bgRole = QStringLiteral("bg.pressed");
				}
				else if (header->state & State_MouseOver)
				{
					bgRole = QStringLiteral("bg.hover");
				}
			}
			const QColor bg =
				roleColor(QStringLiteral("header"), bgRole, header->palette.color(QPalette::Button));
			const QColor border =
				roleColor(QStringLiteral("header"), QStringLiteral("border"), header->palette.mid().color());
			painter->fillRect(header->rect, bg);
			painter->setPen(border);
			painter->drawLine(header->rect.topRight(), header->rect.bottomRight());
			painter->drawLine(header->rect.bottomLeft(), header->rect.bottomRight());
			return;
		}
	}

	if (element == CE_HeaderLabel)
	{
		const auto* header = qstyleoption_cast<const QStyleOptionHeader*>(option);
		if (header && painter)
		{
			const bool enabled = header->state & State_Enabled;
			const QColor fg =
				roleColor(QStringLiteral("header"),
						  enabled ? QStringLiteral("fg") : QStringLiteral("fg.disabled"),
						  header->palette.color(QPalette::ButtonText));
			QStyleOptionHeader copy = *header;
			copy.palette.setColor(QPalette::ButtonText, fg);
			copy.palette.setColor(QPalette::WindowText, fg);
			QProxyStyle::drawControl(element, &copy, painter, widget);
			return;
		}
	}

	if (element == CE_ComboBoxLabel)
	{
		const auto* combo = qstyleoption_cast<const QStyleOptionComboBox*>(option);
		if (combo && painter)
		{
			const bool enabled = combo->state & State_Enabled;
			const QColor fg =
				roleColor(QStringLiteral("combo"),
						  enabled ? QStringLiteral("fg") : QStringLiteral("fg.disabled"),
						  combo->palette.color(QPalette::ButtonText));
			QStyleOptionComboBox copy = *combo;
			copy.palette.setColor(QPalette::ButtonText, fg);
			copy.palette.setColor(QPalette::Text, fg);
			QProxyStyle::drawControl(element, &copy, painter, widget);
			return;
		}
	}

	if (element == CE_ProgressBarGroove)
	{
		const bool enabled = option->state & State_Enabled;
		const QColor groove =
			roleColor(QStringLiteral("progress"),
					  enabled ? QStringLiteral("groove") : QStringLiteral("groove.disabled"),
					  option->palette.mid().color());
		const QColor border =
			roleColor(QStringLiteral("progress"), QStringLiteral("border"), option->palette.mid().color());
		const int radius = roleMetric(QStringLiteral("progress"), QStringLiteral("radius"), 3);
		drawRounded(painter, option->rect, radius, groove, border);
		return;
	}

	if (element == CE_ProgressBarContents)
	{
		const auto* prog = qstyleoption_cast<const QStyleOptionProgressBar*>(option);
		if (prog && painter)
		{
			if (prog->minimum == prog->maximum || prog->progress < prog->minimum)
			{
				return;
			}
			const bool enabled = prog->state & State_Enabled;
			QRect full = prog->rect;
			const int range = qMax(1, prog->maximum - prog->minimum);
			const int value = qBound(0, prog->progress - prog->minimum, range);
			const bool horizontal = prog->state & QStyle::State_Horizontal;
			QRect chunk = full;
			if (horizontal)
			{
				chunk.setWidth(qRound(full.width() * (qreal(value) / range)));
			}
			else
			{
				const int h = qRound(full.height() * (qreal(value) / range));
				chunk.setTop(full.bottom() - h + 1);
				chunk.setHeight(h);
			}
			if (prog->invertedAppearance)
			{
				if (horizontal)
				{
					chunk.moveRight(full.right());
				}
				else
				{
					chunk.moveTop(full.top());
				}
			}
			const QColor fill =
				roleColor(QStringLiteral("progress"),
						  enabled ? QStringLiteral("chunk") : QStringLiteral("chunk.disabled"),
						  prog->palette.color(QPalette::Highlight));
			const int radius = roleMetric(QStringLiteral("progress"), QStringLiteral("radius"), 3);
			if (chunk.isValid() && chunk.width() > 0 && chunk.height() > 0)
			{
				drawRounded(painter, chunk, radius, fill, fill);
			}
			return;
		}
	}

	if (element == CE_ProgressBarLabel)
	{
		const auto* prog = qstyleoption_cast<const QStyleOptionProgressBar*>(option);
		if (prog && painter)
		{
			const bool enabled = prog->state & State_Enabled;
			const QColor fg =
				roleColor(QStringLiteral("progress"),
						  enabled ? QStringLiteral("fg") : QStringLiteral("fg.disabled"),
						  prog->palette.color(QPalette::WindowText));
			QStyleOptionProgressBar copy = *prog;
			copy.palette.setColor(QPalette::WindowText, fg);
			copy.palette.setColor(QPalette::Text, fg);
			QProxyStyle::drawControl(element, &copy, painter, widget);
			return;
		}
	}

	if (element == CE_ItemViewItem)
	{
		const auto* item = qstyleoption_cast<const QStyleOptionViewItem*>(option);
		if (item && painter)
		{
			const bool enabled = item->state & State_Enabled;
			const bool selected = item->state & State_Selected;
			const bool active = item->state & State_Active;

			QString fgRole = QStringLiteral("fg");
			if (!enabled)
			{
				fgRole = QStringLiteral("fg.disabled");
			}
			else if (selected)
			{
				fgRole = QStringLiteral("fg.selected");
			}
			const QColor fg =
				roleColor(QStringLiteral("view"), fgRole, item->palette.color(QPalette::Text));
			const QColor selBg =
				roleColor(QStringLiteral("view"),
						  active ? QStringLiteral("bg.selected") : QStringLiteral("bg.selected.inactive"),
						  item->palette.color(QPalette::Highlight));
			const QColor base =
				roleColor(QStringLiteral("view"), QStringLiteral("bg"), item->palette.color(QPalette::Base));
			const QColor alt = roleColor(QStringLiteral("view"), QStringLiteral("bg.alternate"),
										 item->palette.color(QPalette::AlternateBase));

			QStyleOptionViewItem copy = *item;
			copy.palette.setColor(QPalette::Text, fg);
			copy.palette.setColor(QPalette::WindowText, fg);
			copy.palette.setColor(QPalette::HighlightedText, fg);
			copy.palette.setColor(QPalette::Highlight, selBg);
			copy.palette.setColor(QPalette::Base, base);
			copy.palette.setColor(QPalette::AlternateBase, alt);
			// Panel background comes from PE_PanelItemViewItem (our override).
			QProxyStyle::drawControl(element, &copy, painter, widget);
			return;
		}
	}

	QProxyStyle::drawControl(element, option, painter, widget);
}

void QThemeStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex* option,
									 QPainter* painter, const QWidget* widget) const
{
	if (control == CC_ScrollBar)
	{
		const auto* sb = qstyleoption_cast<const QStyleOptionSlider*>(option);
		if (sb && painter)
		{
			const bool enabled = sb->state & State_Enabled;
			const QColor groove =
				roleColor(QStringLiteral("scroll"), QStringLiteral("groove"), sb->palette.window().color());
			painter->fillRect(sb->rect, groove);

			const QRect handle = subControlRect(CC_ScrollBar, sb, SC_ScrollBarSlider, widget);
			QString handleRole = QStringLiteral("handle");
			if (!enabled)
			{
				handleRole = QStringLiteral("handle.disabled");
			}
			else if (sb->state & State_Sunken)
			{
				handleRole = QStringLiteral("handle.pressed");
			}
			else if (sb->state & State_MouseOver)
			{
				handleRole = QStringLiteral("handle.hover");
			}
			const QColor hc =
				roleColor(QStringLiteral("scroll"), handleRole, sb->palette.mid().color());
			const int radius = roleMetric(QStringLiteral("scroll"), QStringLiteral("radius"), 4);
			drawRounded(painter, handle.adjusted(1, 1, -1, -1), radius, hc, hc);

			const QColor arrow =
				roleColor(QStringLiteral("scroll"),
						  enabled ? QStringLiteral("arrow") : QStringLiteral("arrow.disabled"),
						  sb->palette.color(QPalette::WindowText));
			const QRect sub = subControlRect(CC_ScrollBar, sb, SC_ScrollBarSubLine, widget);
			const QRect add = subControlRect(CC_ScrollBar, sb, SC_ScrollBarAddLine, widget);
			const bool horiz = sb->orientation == Qt::Horizontal;
			drawArrow(painter, sub, horiz ? Qt::LeftArrow : Qt::UpArrow, arrow);
			drawArrow(painter, add, horiz ? Qt::RightArrow : Qt::DownArrow, arrow);
			return;
		}
	}

	if (control == CC_Slider)
	{
		const auto* slider = qstyleoption_cast<const QStyleOptionSlider*>(option);
		if (slider && painter)
		{
			const bool enabled = slider->state & State_Enabled;
			const QRect grooveRect = subControlRect(CC_Slider, slider, SC_SliderGroove, widget);
			const QRect handleRect = subControlRect(CC_Slider, slider, SC_SliderHandle, widget);
			const int grooveThickness = roleMetric(QStringLiteral("slider"), QStringLiteral("groove"), 4);
			QRect groove = grooveRect;
			if (slider->orientation == Qt::Horizontal)
			{
				groove.setTop(grooveRect.center().y() - grooveThickness / 2);
				groove.setHeight(grooveThickness);
			}
			else
			{
				groove.setLeft(grooveRect.center().x() - grooveThickness / 2);
				groove.setWidth(grooveThickness);
			}

			const QColor grooveColor =
				roleColor(QStringLiteral("slider"),
						  enabled ? QStringLiteral("groove") : QStringLiteral("groove.disabled"),
						  slider->palette.mid().color());
			const int radius = roleMetric(QStringLiteral("slider"), QStringLiteral("radius"), 8);
			drawRounded(painter, groove, qMin(radius, grooveThickness), grooveColor, grooveColor);

			QRect fill = groove;
			if (slider->orientation == Qt::Horizontal)
			{
				if (slider->upsideDown)
				{
					fill.setLeft(handleRect.center().x());
				}
				else
				{
					fill.setRight(handleRect.center().x());
				}
			}
			else
			{
				if (slider->upsideDown)
				{
					fill.setTop(handleRect.center().y());
				}
				else
				{
					fill.setBottom(handleRect.center().y());
				}
			}
			const QColor fillColor =
				roleColor(QStringLiteral("slider"),
						  enabled ? QStringLiteral("fill") : QStringLiteral("fill.disabled"),
						  slider->palette.color(QPalette::Highlight));
			drawRounded(painter, fill, qMin(radius, grooveThickness), fillColor, fillColor);

			QString handleRole = QStringLiteral("handle");
			if (!enabled)
			{
				handleRole = QStringLiteral("handle.disabled");
			}
			else if (slider->state & State_Sunken)
			{
				handleRole = QStringLiteral("handle.pressed");
			}
			else if (slider->state & State_MouseOver)
			{
				handleRole = QStringLiteral("handle.hover");
			}
			const QColor handleBg =
				roleColor(QStringLiteral("slider"), handleRole, slider->palette.button().color());
			const bool focused = (slider->state & State_HasFocus) && enabled;
			const QColor handleBorder =
				roleColor(QStringLiteral("slider"),
						  focused ? QStringLiteral("handle.border.focus")
								  : QStringLiteral("handle.border"),
						  slider->palette.mid().color());
			drawRounded(painter, handleRect, radius, handleBg, handleBorder, focused ? 2.0 : 1.0);
			return;
		}
	}

	if (control == CC_ComboBox)
	{
		const auto* combo = qstyleoption_cast<const QStyleOptionComboBox*>(option);
		if (combo && painter)
		{
			const bool enabled = combo->state & State_Enabled;
			QString bgRole = QStringLiteral("bg");
			QString borderRole = QStringLiteral("border");
			if (!enabled)
			{
				bgRole = QStringLiteral("bg.disabled");
			}
			else if (combo->state & State_Sunken)
			{
				bgRole = QStringLiteral("bg.pressed");
			}
			else if (combo->state & State_MouseOver)
			{
				bgRole = QStringLiteral("bg.hover");
				borderRole = QStringLiteral("border.hover");
			}
			if (enabled && (combo->state & State_HasFocus))
			{
				borderRole = QStringLiteral("border.focus");
			}
			const QColor bg =
				roleColor(QStringLiteral("combo"), bgRole, combo->palette.color(QPalette::Button));
			const QColor border =
				roleColor(QStringLiteral("combo"), borderRole, combo->palette.mid().color());
			const int radius = roleMetric(QStringLiteral("combo"), QStringLiteral("radius"), 4);
			const qreal bw = (borderRole == QStringLiteral("border.focus")) ? 2.0 : 1.0;
			drawRounded(painter, combo->rect, radius, bg, border, bw);

			const QRect arrowRect = subControlRect(CC_ComboBox, combo, SC_ComboBoxArrow, widget);
			const QColor arrow =
				roleColor(QStringLiteral("combo"),
						  enabled ? QStringLiteral("arrow") : QStringLiteral("arrow.disabled"),
						  combo->palette.color(QPalette::WindowText));
			drawArrow(painter, arrowRect, Qt::DownArrow, arrow);
			return;
		}
	}

	if (control == CC_SpinBox)
	{
		const auto* spin = qstyleoption_cast<const QStyleOptionSpinBox*>(option);
		if (spin && painter)
		{
			const bool enabled = spin->state & State_Enabled;
			QString borderRole = QStringLiteral("border");
			if (!enabled)
			{
				borderRole = QStringLiteral("border.disabled");
			}
			else if (spin->state & State_HasFocus)
			{
				borderRole = QStringLiteral("border.focus");
			}
			else if (spin->state & State_MouseOver)
			{
				borderRole = QStringLiteral("border.hover");
			}
			const QColor bg = roleColor(QStringLiteral("spin"),
										enabled ? QStringLiteral("bg") : QStringLiteral("bg.disabled"),
										spin->palette.color(QPalette::Base));
			const QColor border =
				roleColor(QStringLiteral("spin"), borderRole, spin->palette.mid().color());
			const int radius = roleMetric(QStringLiteral("spin"), QStringLiteral("radius"), 4);
			const qreal bw = (borderRole == QStringLiteral("border.focus")) ? 2.0 : 1.0;
			drawRounded(painter, spin->rect, radius, bg, border, bw);

			auto paintButton = [&](SubControl sc, bool up)
			{
				if (!(spin->subControls & sc))
				{
					return;
				}
				QRect br = subControlRect(CC_SpinBox, spin, sc, widget);
				QString btnRole = QStringLiteral("button");
				const bool active = spin->activeSubControls & sc;
				if (!enabled)
				{
					btnRole = QStringLiteral("button.disabled");
				}
				else if (active && (spin->state & State_Sunken))
				{
					btnRole = QStringLiteral("button.pressed");
				}
				else if (active || (spin->state & State_MouseOver))
				{
					btnRole = QStringLiteral("button.hover");
				}
				const QColor btnBg =
					roleColor(QStringLiteral("spin"), btnRole, spin->palette.button().color());
				painter->fillRect(br.adjusted(1, 1, -1, -1), btnBg);
				const QColor arrow =
					roleColor(QStringLiteral("spin"),
							  enabled ? QStringLiteral("arrow") : QStringLiteral("arrow.disabled"),
							  spin->palette.color(QPalette::WindowText));
				drawArrow(painter, br, up ? Qt::UpArrow : Qt::DownArrow, arrow);
			};
			paintButton(SC_SpinBoxUp, true);
			paintButton(SC_SpinBoxDown, false);
			return;
		}
	}

	if (control == CC_ToolButton)
	{
		const auto* tb = qstyleoption_cast<const QStyleOptionToolButton*>(option);
		if (tb && painter)
		{
			QStyleOptionToolButton copy = *tb;
			if (tb->subControls & SC_ToolButton)
			{
				drawPrimitive(PE_PanelButtonTool, &copy, painter, widget);
			}
			if (tb->subControls & SC_ToolButtonMenu)
			{
				const QRect menuRect = subControlRect(CC_ToolButton, tb, SC_ToolButtonMenu, widget);
				QStyleOptionToolButton menuOpt = *tb;
				menuOpt.rect = menuRect;
				const QColor arrow =
					roleColor(QStringLiteral("combo"),
							  (tb->state & State_Enabled) ? QStringLiteral("arrow")
														  : QStringLiteral("arrow.disabled"),
							  tb->palette.color(QPalette::WindowText));
				drawArrow(painter, menuRect, Qt::DownArrow, arrow);
			}
			QProxyStyle::drawControl(CE_ToolButtonLabel, &copy, painter, widget);
			return;
		}
	}

	if (control == CC_Dial)
	{
		const auto* dial = qstyleoption_cast<const QStyleOptionSlider*>(option);
		if (dial && painter)
		{
			const bool enabled = dial->state & State_Enabled;
			const bool focused = (dial->state & State_HasFocus) && enabled;
			const QRect r = dial->rect.adjusted(4, 4, -4, -4);
			const int thickness = roleMetric(QStringLiteral("dial"), QStringLiteral("thickness"), 8);
			const QColor groove =
				roleColor(QStringLiteral("dial"),
						  enabled ? QStringLiteral("groove") : QStringLiteral("groove.disabled"),
						  dial->palette.mid().color());
			const QColor fill =
				roleColor(QStringLiteral("dial"),
						  enabled ? QStringLiteral("fill") : QStringLiteral("fill.disabled"),
						  dial->palette.color(QPalette::Highlight));
			const QColor handleBg =
				roleColor(QStringLiteral("dial"), QStringLiteral("handle"), dial->palette.button().color());
			const QColor handleBorder =
				roleColor(QStringLiteral("dial"),
						  focused ? QStringLiteral("handle.border.focus")
								  : QStringLiteral("handle.border"),
						  dial->palette.mid().color());

			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);
			const QPen groovePen(groove, thickness, Qt::SolidLine, Qt::RoundCap);
			painter->setPen(groovePen);
			painter->setBrush(Qt::NoBrush);
			painter->drawEllipse(r);

			const int span = dial->maximum - dial->minimum;
			const qreal ratio =
				span > 0 ? qreal(dial->sliderPosition - dial->minimum) / qreal(span) : 0.0;
			const int startAngle = 225 * 16;
			const int arcSpan = -int(270.0 * 16.0 * ratio);
			painter->setPen(QPen(fill, thickness, Qt::SolidLine, Qt::RoundCap));
			painter->drawArc(r, startAngle, arcSpan);

			const qreal angleRad = qDegreesToRadians(225.0 - 270.0 * ratio);
			const QPointF center = r.center();
			const qreal radius = qMin(r.width(), r.height()) / 2.0 - thickness / 2.0;
			const QPointF tip(center.x() + radius * qCos(angleRad),
							 center.y() - radius * qSin(angleRad));
			const int hs = qMax(6, thickness + 2);
			painter->setPen(QPen(handleBorder, focused ? 2.0 : 1.0));
			painter->setBrush(handleBg);
			painter->drawEllipse(tip, hs / 2.0, hs / 2.0);
			painter->restore();
			return;
		}
	}

	QProxyStyle::drawComplexControl(control, option, painter, widget);
}

} // namespace qtheme
