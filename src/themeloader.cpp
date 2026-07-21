#include "theme/themeloader.hpp"

namespace theme
{
	struct ThemeLoader::ThemeStyle
	{
		QString base;
		QHash<QString, ThemeColor> colorMap;
		QHash<QString, int> hintMap;
	};

	ThemeLoader::ThemeLoader() = default;
	ThemeLoader::~ThemeLoader() = default;

	bool ThemeLoader::setupXml(const QString& /*filePath*/, const QString& /*relatedSkinName*/,
							   const QStringList& /*searchPaths*/, ThemeError* err)
	{
		// L0: implement per docs/zh/theme-engine-spec.md T2.4
		if (err)
		{
			*err = ThemeError::None;
		}
		return false;
	}

	void ThemeLoader::resetXml(ThemeResetMode /*mode*/)
	{
		styleMap_.clear();
	}

	ThemeColor ThemeLoader::color(const QString& className, const QString& propName, const QColor& def) const
	{
		auto it = styleMap_.find(className);
		while (it != styleMap_.end())
		{
			const auto& style = **it;
			auto cit = style.colorMap.constFind(propName);
			if (cit != style.colorMap.cend())
			{
				ThemeColor out = cit.value();
				out.paletteRef.clear();
				return out;
			}
			if (style.base.isEmpty())
			{
				break;
			}
			it = styleMap_.find(style.base);
		}
		ThemeColor miss;
		miss.value = def;
		miss.ok = false;
		return miss;
	}

	int ThemeLoader::hint(const QString& className, const QString& propName, int def, bool* ok) const
	{
		auto it = styleMap_.find(className);
		while (it != styleMap_.end())
		{
			const auto& style = **it;
			auto hit = style.hintMap.constFind(propName);
			if (hit != style.hintMap.cend())
			{
				if (ok)
				{
					*ok = true;
				}
				return hit.value();
			}
			if (style.base.isEmpty())
			{
				break;
			}
			it = styleMap_.find(style.base);
		}
		if (ok)
		{
			*ok = false;
		}
		return def;
	}

	bool ThemeLoader::hasClass(const QString& className) const
	{
		return styleMap_.contains(className);
	}
} // namespace theme
