#include "qtheme/pack.hpp"

#include "qtheme/color_util.hpp"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

namespace qtheme {

namespace {

void applyColorObject(ThemeStore* store, const QString& group, const QJsonObject& roles)
{
	for (auto it = roles.begin(); it != roles.end(); ++it)
	{
		const QColor c = parseColorLiteral(it.value().toString());
		if (c.isValid())
		{
			store->setColor(group, it.key(), c);
		}
	}
}

void applyMetricObject(ThemeStore* store, const QString& group, const QJsonObject& roles)
{
	for (auto it = roles.begin(); it != roles.end(); ++it)
	{
		store->setMetric(group, it.key(), it.value().toInt());
	}
}

void applyGroupedObject(ThemeStore* store, const QJsonObject& root, bool colors)
{
	for (auto it = root.begin(); it != root.end(); ++it)
	{
		if (!it.value().isObject())
		{
			continue;
		}
		if (colors)
		{
			applyColorObject(store, it.key(), it.value().toObject());
		}
		else
		{
			applyMetricObject(store, it.key(), it.value().toObject());
		}
	}
}

QByteArray readAllBytes(const QString& pathOrQrc, Error* err)
{
	QFile f(pathOrQrc);
	if (!f.open(QIODevice::ReadOnly))
	{
		if (err)
		{
			*err = Error::FileNotFound;
		}
		return {};
	}
	return f.readAll();
}

} // namespace

bool PackRegistry::validateStore(const ThemeStore& store, QStringList* missing)
{
	const QStringList miss = store.missingRequiredColors();
	if (missing)
	{
		*missing = miss;
	}
	return miss.isEmpty();
}

bool PackRegistry::loadPackJson(const QString& pathOrQrc, PackData* out, Error* err) const
{
	if (!out)
	{
		return false;
	}

	const QByteArray bytes = readAllBytes(pathOrQrc, err);
	if (bytes.isEmpty())
	{
		return false;
	}

	QJsonParseError pe{};
	const QJsonDocument doc = QJsonDocument::fromJson(bytes, &pe);
	if (pe.error != QJsonParseError::NoError || !doc.isObject())
	{
		if (err)
		{
			*err = Error::JsonParseError;
		}
		return false;
	}

	const QJsonObject root = doc.object();
	out->info.id = root.value(QStringLiteral("id")).toString();
	out->info.displayName = root.value(QStringLiteral("displayName")).toString();
	out->info.baseId = root.value(QStringLiteral("base")).toString();
	out->info.sourcePath = pathOrQrc;
	if (out->info.id.isEmpty())
	{
		if (err)
		{
			*err = Error::PackInvalid;
		}
		return false;
	}

	ThemeStore overlay;
	overlay.beginUpdate();
	const QString shared = root.value(QStringLiteral("sharedMetrics")).toString();
	if (!shared.isEmpty())
	{
		Error sharedErr = Error::None;
		const QByteArray sharedBytes = readAllBytes(shared, &sharedErr);
		if (!sharedBytes.isEmpty())
		{
			const QJsonDocument sdoc = QJsonDocument::fromJson(sharedBytes);
			if (sdoc.isObject())
			{
				const QJsonObject metrics = sdoc.object().value(QStringLiteral("metrics")).toObject();
				applyGroupedObject(&overlay, metrics, false);
			}
		}
	}

	if (root.contains(QStringLiteral("colors")))
	{
		applyGroupedObject(&overlay, root.value(QStringLiteral("colors")).toObject(), true);
	}
	if (root.contains(QStringLiteral("metrics")))
	{
		applyGroupedObject(&overlay, root.value(QStringLiteral("metrics")).toObject(), false);
	}
	overlay.endUpdate();

	out->overlay = std::move(overlay);
	out->hasOverlay = true;
	if (err)
	{
		*err = Error::None;
	}
	return true;
}

bool PackRegistry::registerPackFile(const QString& pathOrQrc, Error* err)
{
	PackData data;
	if (!loadPackJson(pathOrQrc, &data, err))
	{
		return false;
	}
	packs_.insert(data.info.id, data);
	if (err)
	{
		*err = Error::None;
	}
	return true;
}

int PackRegistry::registerPacksInDirectory(const QString& dir, Error* err)
{
	QDir d(dir);
	if (!d.exists())
	{
		if (err)
		{
			*err = Error::FileNotFound;
		}
		return 0;
	}

	int count = 0;
	Error firstErr = Error::None;
	const QStringList files =
		d.entryList(QStringList{QStringLiteral("*.theme.json")}, QDir::Files, QDir::Name);
	for (const QString& name : files)
	{
		Error local = Error::None;
		if (registerPackFile(d.absoluteFilePath(name), &local))
		{
			++count;
		}
		else if (firstErr == Error::None)
		{
			firstErr = local;
		}
	}

	if (err)
	{
		*err = (count > 0) ? Error::None : firstErr;
	}
	return count;
}

bool PackRegistry::registerBuiltinFluentPacks(Error* err)
{
	Q_INIT_RESOURCE(theme);

	const QStringList paths = {
		QStringLiteral(":/theme/fluent/fluent.light.theme.json"),
		QStringLiteral(":/theme/fluent/fluent.dark.theme.json"),
		QStringLiteral(":/theme/fluent/fluent.hc.theme.json"),
		QStringLiteral(":/theme/fluent/user.sample.theme.json"),
	};

	bool anyOk = false;
	Error firstErr = Error::None;
	for (const QString& p : paths)
	{
		Error local = Error::None;
		if (registerPackFile(p, &local))
		{
			anyOk = true;
		}
		else if (firstErr == Error::None)
		{
			firstErr = local;
		}
	}

	const bool haveCore = hasPack(QString::fromUtf8(kPackFluentLight))
						  && hasPack(QString::fromUtf8(kPackFluentDark))
						  && hasPack(QString::fromUtf8(kPackFluentHc));
	if (!haveCore)
	{
		if (err)
		{
			*err = (firstErr != Error::None) ? firstErr : Error::FileNotFound;
		}
		return false;
	}

	if (err)
	{
		*err = anyOk ? Error::None : firstErr;
	}
	return true;
}

QStringList PackRegistry::packIds() const
{
	return packs_.keys();
}

bool PackRegistry::hasPack(const QString& id) const
{
	return packs_.contains(id);
}

ThemePackInfo PackRegistry::info(const QString& id) const
{
	const auto it = packs_.constFind(id);
	if (it == packs_.cend())
	{
		return {};
	}
	return it->info;
}

bool PackRegistry::materializeRecursive(const QString& id, ThemeStore* out, int depth, Error* err) const
{
	if (!out || depth > 8)
	{
		if (err)
		{
			*err = Error::PackInvalid;
		}
		return false;
	}

	const auto it = packs_.constFind(id);
	if (it == packs_.cend())
	{
		if (err)
		{
			*err = Error::SkinNotFound;
		}
		return false;
	}

	if (!it->info.baseId.isEmpty())
	{
		if (!materializeRecursive(it->info.baseId, out, depth + 1, err))
		{
			return false;
		}
	}

	if (it->hasOverlay)
	{
		out->mergeFrom(it->overlay, true);
	}
	return true;
}

bool PackRegistry::materialize(const QString& id, ThemeStore* out, Error* err) const
{
	if (!out)
	{
		return false;
	}
	out->clear();

	// Aliases
	QString resolved = id;
	if (id == QLatin1String("light"))
	{
		resolved = QString::fromUtf8(kPackFluentLight);
	}
	else if (id == QLatin1String("dark"))
	{
		resolved = QString::fromUtf8(kPackFluentDark);
	}
	else if (id == QLatin1String("hc") || id == QLatin1String("highcontrast"))
	{
		resolved = QString::fromUtf8(kPackFluentHc);
	}

	if (!materializeRecursive(resolved, out, 0, err))
	{
		return false;
	}

	QStringList missing;
	if (!validateStore(*out, &missing))
	{
		if (err)
		{
			*err = Error::PackMissingTokens;
		}
		return false;
	}
	if (err)
	{
		*err = Error::None;
	}
	return true;
}

} // namespace qtheme
