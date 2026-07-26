#ifndef __QTHEME_ENGINE_ISKINMANAGER_H__
#define __QTHEME_ENGINE_ISKINMANAGER_H__

#include "ithemeloader.hpp"

#include <QString>
#include <qtheme/qtheme_export.hpp>

namespace theme
{
	struct SkinInfo
	{
		QString name;		   // e.g. "light" / "dark"
		QString themeFilePath; // main .theme.xml
	};

	class QTE_EXPORT ISkinManager
	{
	public:
		virtual ~ISkinManager() = default;
		virtual bool switchSkin(const QString& name, bool force = false) = 0;
		virtual SkinInfo current() const = 0;
		virtual IThemeLoader* theme() const = 0;
		virtual ThemeError lastError() const = 0;
		// Concrete SkinManager (QObject) also provides:
		//   void skinChanged(const QString& previous, const QString& current);
	};
} // namespace theme

#endif // __QTHEME_ENGINE_ISKINMANAGER_H__
