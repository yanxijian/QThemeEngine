#include "theme/skinmanager.hpp"

#include "theme/themeapi.hpp"

namespace theme
{
	SkinManager::SkinManager(QObject* parent)
		: QObject(parent)
		, loader_(std::make_unique<ThemeLoader>())
	{
		ThemeApi::bind(this);
	}

	SkinManager::~SkinManager()
	{
		if (ThemeApi::skin() == this)
		{
			ThemeApi::bind(nullptr);
		}
	}

	bool SkinManager::switchSkin(const QString& name, bool force)
	{
		// L0: implement per docs/zh/style.md T2.6
		Q_UNUSED(force);
		lastError_ = ThemeError::SkinNotFound;
		Q_UNUSED(name);
		return false;
	}

	SkinInfo SkinManager::current() const
	{
		return current_;
	}

	IThemeLoader* SkinManager::theme() const
	{
		return loader_.get();
	}

	ThemeError SkinManager::lastError() const
	{
		return lastError_;
	}
} // namespace theme
