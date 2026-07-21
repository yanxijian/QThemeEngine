#ifndef __QTHEME_ENGINE_ITHEMELOADER_H__
#define __QTHEME_ENGINE_ITHEMELOADER_H__

#include <QColor>
#include <QString>
#include <QStringList>

namespace theme
{
	struct ThemeColor
	{
		QColor value;
		bool ok = false;
		QString paletteRef; // non-empty => token; must reexpand after setupXml merge
	};

	enum class ThemeResetMode
	{
		Soft,  // clear main style map; keep extension metadata (L0: optional)
		Final, // Soft + clear extensions / hooks
	};

	enum class ThemeError
	{
		None = 0,
		FileNotFound,
		InvalidEncoding, // missing UTF-8 BOM or decode failure
		XmlParseError,
		ImportCycle,
		ImportMissing,
		NoMatchingTheme,
		SkinNotFound,
	};

	class IThemeLoader
	{
	public:
		virtual ~IThemeLoader() = default;

		virtual bool setupXml(const QString& filePath, const QString& relatedSkinName, const QStringList& searchPaths,
							  ThemeError* err = nullptr) = 0;

		virtual void resetXml(ThemeResetMode mode) = 0;

		virtual ThemeColor color(const QString& className, const QString& propName,
								 const QColor& def = QColor()) const = 0;

		virtual int hint(const QString& className, const QString& propName, int def = 0, bool* ok = nullptr) const = 0;

		virtual bool hasClass(const QString& className) const = 0;
	};
} // namespace theme

#endif  // __QTHEME_ENGINE_ITHEMELOADER_H__
