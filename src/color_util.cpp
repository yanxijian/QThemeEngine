#include "theme/color_util.hpp"

namespace theme
{
	QColor parseColorLiteral(const QString& s)
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

	QColor themeColorFromRgbaHex(const QString& hex)
	{
		return parseColorLiteral(hex);
	}

	bool hasUtf8Bom(const QByteArray& bytes)
	{
		return bytes.size() >= 3 && static_cast<unsigned char>(bytes[0]) == 0xEF
			   && static_cast<unsigned char>(bytes[1]) == 0xBB && static_cast<unsigned char>(bytes[2]) == 0xBF;
	}
} // namespace theme
