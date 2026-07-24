#include "theme/skinmanager.hpp"

#include "theme/themeapi.hpp"

namespace theme
{
	SkinManager::SkinManager(QObject* parent)
		: QObject(parent)
		, m_loader(std::make_unique<ThemeLoader>())
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
		// L0: implement per docs/zh/theme-engine-spec.md T2.6
		Q_UNUSED(force);
		m_lastError = ThemeError::SkinNotFound;
		Q_UNUSED(name);
		return false;
	}

	SkinInfo SkinManager::current() const
	{
		return m_current;
	}

	IThemeLoader* SkinManager::theme() const
	{
		return m_loader.get();
	}

	ThemeError SkinManager::lastError() const
	{
		return m_lastError;
	}
} // namespace theme
