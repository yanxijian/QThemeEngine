#pragma once

#include "ithemeloader.hpp"

#include <QHash>
#include <QString>

#include <memory>

namespace theme
{
	class ThemeLoader final : public IThemeLoader
	{
	public:
		ThemeLoader();
		~ThemeLoader() override;

		bool setupXml(const QString& filePath, const QString& relatedSkinName, const QStringList& searchPaths,
					  ThemeError* err = nullptr) override;
		void resetXml(ThemeResetMode mode) override;

		ThemeColor color(const QString& className, const QString& propName,
						 const QColor& def = QColor()) const override;
		int hint(const QString& className, const QString& propName, int def = 0, bool* ok = nullptr) const override;
		bool hasClass(const QString& className) const override;

	private:
		struct ThemeStyle;
		QHash<QString, std::shared_ptr<ThemeStyle>> styleMap_;
	};
} // namespace theme
