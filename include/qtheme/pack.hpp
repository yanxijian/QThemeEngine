#ifndef __QTHEME_ENGINE_PACK_H__
#define __QTHEME_ENGINE_PACK_H__

#include "store.hpp"
#include "types.hpp"

#include <QHash>
#include <QString>
#include <QStringList>

namespace qtheme {

struct ThemePackInfo
{
	QString id;
	QString displayName;
	QString baseId;
	QString sourcePath;
};

/// Load / register / materialize Theme Packs (JSON) into ThemeStore.
class PackRegistry
{
public:
	[[nodiscard]] bool registerPackFile(const QString& pathOrQrc, Error* err = nullptr);
	/// Register all `*.theme.json` files under dir (non-recursive). Returns count registered.
	[[nodiscard]] int registerPacksInDirectory(const QString& dir, Error* err = nullptr);
	[[nodiscard]] bool registerBuiltinFluentPacks(Error* err = nullptr);

	[[nodiscard]] QStringList packIds() const;
	[[nodiscard]] bool hasPack(const QString& id) const;
	[[nodiscard]] ThemePackInfo info(const QString& id) const;

	/// Build store for id (resolves base chain). Applies optional accent patch afterward via Engine.
	[[nodiscard]] bool materialize(const QString& id, ThemeStore* out, Error* err = nullptr) const;

	/// Validate required Fluent color tokens.
	[[nodiscard]] static bool validateStore(const ThemeStore& store, QStringList* missing = nullptr);

private:
	struct PackData
	{
		ThemePackInfo info;
		ThemeStore overlay; // pack-local colors/metrics (before base merge)
		bool hasOverlay = false;
	};

	[[nodiscard]] bool loadPackJson(const QString& pathOrQrc, PackData* out, Error* err) const;
	[[nodiscard]] bool materializeRecursive(const QString& id, ThemeStore* out, int depth,
											Error* err) const;

	QHash<QString, PackData> m_packs;
};

} // namespace qtheme

#endif  // __QTHEME_ENGINE_PACK_H__
