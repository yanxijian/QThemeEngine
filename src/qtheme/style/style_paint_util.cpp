#include "style/style_paint_util.hpp"

#include <QAbstractSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPainterPath>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QtMath>

namespace qtheme::style_detail
{
	void drawRounded(QPainter* painter, const QRect& rect, int radius, const QColor& fill, const QColor& border, qreal borderWidth)
	{
		painter->save();
		painter->setRenderHint(QPainter::Antialiasing, true);
		const QRectF r = QRectF(rect).adjusted(0.5, 0.5, -0.5, -0.5);
		QPainterPath path;
		path.addRoundedRect(r, radius, radius);
		painter->fillPath(path, fill);
		if (borderWidth > 0.0)
		{
			painter->setPen(QPen(border, borderWidth));
			painter->drawPath(path);
		}
		painter->restore();
	}

	void drawFluentFocusRing(QPainter* painter, const QRect& rect, int radius, const QColor& outer, const QColor& inner, qreal outerWidth,
							 qreal innerWidth)
	{
		if (!painter || rect.isEmpty())
		{
			return;
		}
		painter->save();
		painter->setRenderHint(QPainter::Antialiasing, true);
		painter->setBrush(Qt::NoBrush);

		const qreal halfOuter = outerWidth * 0.5;
		const QRectF outerR = QRectF(rect).adjusted(halfOuter, halfOuter, -halfOuter, -halfOuter);
		painter->setPen(QPen(outer, outerWidth, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
		if (radius > 0)
		{
			painter->drawRoundedRect(outerR, radius, radius);
		}
		else
		{
			painter->drawRect(outerR);
		}

		if (innerWidth > 0.0)
		{
			const qreal inset = outerWidth + innerWidth * 0.5;
			const QRectF innerR = QRectF(rect).adjusted(inset, inset, -inset, -inset);
			if (innerR.width() >= 2.0 && innerR.height() >= 2.0)
			{
				const qreal innerRadius = radius > 0 ? qMax(0.0, qreal(radius) - outerWidth) : 0.0;
				painter->setPen(QPen(inner, innerWidth, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
				if (innerRadius > 0)
				{
					painter->drawRoundedRect(innerR, innerRadius, innerRadius);
				}
				else
				{
					painter->drawRect(innerR);
				}
			}
		}
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
			poly << QPoint(c.x(), c.y() - s) << QPoint(c.x() - s, c.y() + s / 2) << QPoint(c.x() + s, c.y() + s / 2);
			break;
		case Qt::DownArrow:
			poly << QPoint(c.x(), c.y() + s) << QPoint(c.x() - s, c.y() - s / 2) << QPoint(c.x() + s, c.y() - s / 2);
			break;
		case Qt::LeftArrow:
			poly << QPoint(c.x() - s, c.y()) << QPoint(c.x() + s / 2, c.y() - s) << QPoint(c.x() + s / 2, c.y() + s);
			break;
		case Qt::RightArrow:
		default:
			poly << QPoint(c.x() + s, c.y()) << QPoint(c.x() - s / 2, c.y() - s) << QPoint(c.x() - s / 2, c.y() + s);
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

	bool chromeOwnsFocusStroke(const QWidget* widget)
	{
		// Walk parents: TextEdit focus often targets the viewport; editable Combo uses an inner LineEdit.
		for (const QWidget* w = widget; w; w = w->parentWidget())
		{
			if (qobject_cast<const QLineEdit*>(w) || qobject_cast<const QComboBox*>(w) || qobject_cast<const QAbstractSpinBox*>(w)
				|| isTextEditLike(w))
			{
				return true;
			}
		}
		return false;
	}

	void clearPopupToTransparent(QPainter* painter, const QRect& rect)
	{
		if (!painter || rect.isEmpty())
		{
			return;
		}
		painter->save();
		painter->setCompositionMode(QPainter::CompositionMode_Source);
		painter->fillRect(rect, Qt::transparent);
		painter->restore();
	}
} // namespace qtheme::style_detail
