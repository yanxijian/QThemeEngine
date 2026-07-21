#pragma once

#include "iskinmanager.hpp"
#include "themeloader.hpp"

#include <QObject>

#include <memory>

namespace theme
{
	class SkinManager final : public QObject, public ISkinManager
	{
		Q_OBJECT
	public:
		explicit SkinManager(QObject* parent = nullptr);
		~SkinManager() override;

		bool switchSkin(const QString& name, bool force = false) override;
		SkinInfo current() const override;
		IThemeLoader* theme() const override;
		ThemeError lastError() const override;

	signals:
		void skinChanged(const QString& previous, const QString& current);

	private:
		std::unique_ptr<ThemeLoader> loader_;
		SkinInfo current_;
		ThemeError lastError_ = ThemeError::None;
		bool inited_ = false;
	};
} // namespace theme
