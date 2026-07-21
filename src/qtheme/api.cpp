#include "qtheme/api.hpp"

#include "qtheme/engine.hpp"

#include <QStyle>

namespace qtheme {
namespace api {

namespace {
Engine* g_bound = nullptr;
} // namespace

void bind(Engine* engine)
{
	g_bound = engine;
	Engine::setDefault(engine);
}

Engine* engine()
{
	return g_bound ? g_bound : Engine::defaultEngine();
}

QObject* engineObject()
{
	return engine();
}

QColor color(const QString& group, const QString& role, const QColor& def)
{
	Engine* e = engine();
	if (!e || !e->store())
	{
		return def;
	}
	const ColorValue cv = e->store()->color(group, role, def);
	return cv.ok ? cv.value : def;
}

int metric(const QString& group, const QString& role, int def)
{
	Engine* e = engine();
	if (!e || !e->store())
	{
		return def;
	}
	return e->store()->metric(group, role, def);
}

QString className(const QObject* obj)
{
	if (!obj)
	{
		return {};
	}
	return obj->property(kClassProperty).toString();
}

void setClassName(QObject* obj, const QString& name)
{
	if (obj)
	{
		obj->setProperty(kClassProperty, name);
	}
}

QString roleWithState(const QString& baseRole, const QStyleOption* option)
{
	if (!option)
	{
		return baseRole;
	}
	if (!(option->state & QStyle::State_Enabled))
	{
		return baseRole + QStringLiteral(".disabled");
	}
	if (option->state & QStyle::State_Sunken)
	{
		return baseRole + QStringLiteral(".pressed");
	}
	if (option->state & QStyle::State_MouseOver)
	{
		return baseRole + QStringLiteral(".hover");
	}
	if (option->state & QStyle::State_HasFocus)
	{
		return baseRole + QStringLiteral(".focus");
	}
	return baseRole;
}

} // namespace api
} // namespace qtheme
