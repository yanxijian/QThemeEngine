#include "qtheme/color_util.hpp"

#include <QtMath>

namespace qtheme
{
	QColor parseColorLiteral(QStringView s)
	{
		if (s.isEmpty() || !s.startsWith(QLatin1Char('#')))
		{
			return {};
		}
		if (s.size() == 7)
		{
			QColor c(s);
			if (c.isValid())
			{
				c.setAlpha(255);
			}
			return c;
		}
		if (s.size() == 9)
		{
			const QColor rgb(s.left(7));
			bool ok = false;
			const int aa = s.mid(7, 2).toInt(&ok, 16);
			if (!rgb.isValid() || !ok)
			{
				return {};
			}
			QColor c(rgb);
			c.setAlpha(aa);
			return c;
		}
		return {};
	}

	QColor colorFromRgbaHex(QStringView hex)
	{
		return parseColorLiteral(hex);
	}

	bool hasUtf8Bom(const QByteArray& bytes)
	{
		return bytes.size() >= 3 && static_cast<unsigned char>(bytes[0]) == 0xEF && static_cast<unsigned char>(bytes[1]) == 0xBB
			   && static_cast<unsigned char>(bytes[2]) == 0xBF;
	}

	qreal relativeLuminance(const QColor& c)
	{
		if (!c.isValid())
		{
			return 0.0;
		}
		auto linearize = [](int channel) -> qreal
		{
			const qreal s = qreal(channel) / 255.0;
			return s <= 0.04045 ? s / 12.92 : qPow((s + 0.055) / 1.055, 2.4);
		};
		return 0.2126 * linearize(c.red()) + 0.7152 * linearize(c.green()) + 0.0722 * linearize(c.blue());
	}

	int focusContrastDirection(const QColor& ambient, qreal lightThreshold)
	{
		// Opposite ambient luminance: light surfaces darken the ring; dark surfaces lighten it.
		return relativeLuminance(ambient) >= lightThreshold ? -1 : 1;
	}

	QColor focusOuterFromAmbient(const QColor& ambient, int lightnessOffset, qreal lightThreshold)
	{
		const QColor base = ambient.isValid() ? ambient : QColor(Qt::gray);
		int h = 0, s = 0, l = 0, a = 0;
		base.getHsl(&h, &s, &l, &a);
		const int direction = focusContrastDirection(base, lightThreshold);
		const int offset = qMax(0, lightnessOffset);
		const int focusL = qBound(0, l + direction * offset, 255);
		QColor out;
		// Achromatic high-contrast edge (hue irrelevant for the focus stroke).
		out.setHsl(0, 0, focusL, 255);
		return out;
	}

	QColor focusInnerFromOuter(const QColor& outer)
	{
		if (!outer.isValid())
		{
			return QColor(Qt::white);
		}
		return outer.lightness() >= 128 ? QColor(Qt::black) : QColor(Qt::white);
	}
} // namespace qtheme
