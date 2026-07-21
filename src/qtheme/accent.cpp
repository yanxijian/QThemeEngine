#include "qtheme/accent.hpp"

#include "qtheme/store.hpp"

#include <QGuiApplication>
#include <QPalette>

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

namespace qtheme {
namespace AccentResolver {

QColor systemAccent(const QGuiApplication* app)
{
	const QGuiApplication* a = app ? app : qobject_cast<QGuiApplication*>(QGuiApplication::instance());
	if (!a)
	{
		return QColor(QStringLiteral("#0078D4"));
	}
	const QColor hi = a->palette().color(QPalette::Active, QPalette::Highlight);
	return hi.isValid() ? hi : QColor(QStringLiteral("#0078D4"));
}

bool systemHighContrast()
{
#ifdef Q_OS_WIN
	HIGHCONTRASTW hc{};
	hc.cbSize = sizeof(hc);
	if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0))
	{
		return (hc.dwFlags & HCF_HIGHCONTRASTON) != 0;
	}
#endif
	return false;
}

QColor lighten(const QColor& c, int delta)
{
	int h = 0, s = 0, v = 0, a = 0;
	c.getHsv(&h, &s, &v, &a);
	v = qMin(255, v + delta);
	QColor out;
	out.setHsv(h, s, v, a);
	return out.isValid() ? out : c.lighter(110);
}

QColor darken(const QColor& c, int delta)
{
	int h = 0, s = 0, v = 0, a = 0;
	c.getHsv(&h, &s, &v, &a);
	v = qMax(0, v - delta);
	QColor out;
	out.setHsv(h, s, v, a);
	return out.isValid() ? out : c.darker(110);
}

void applyAccentPatch(ThemeStore* store, const QColor& accent)
{
	if (!store || !accent.isValid())
	{
		return;
	}

	store->beginUpdate();

	const QColor onAccent = (accent.lightness() > 140) ? QColor(Qt::black) : QColor(Qt::white);
	const QColor hover = lighten(accent, 20);
	const QColor pressed = darken(accent, 24);

	store->setColor(QStringLiteral("palette"), QStringLiteral("accent"), accent);
	store->setColor(QStringLiteral("palette"), QStringLiteral("accent.text"), onAccent);
	store->setColor(QStringLiteral("palette"), QStringLiteral("highlight"), accent);
	store->setColor(QStringLiteral("palette"), QStringLiteral("highlightedText"), onAccent);

	store->setColor(QStringLiteral("button"), QStringLiteral("border.focus"), accent);
	store->setColor(QStringLiteral("edit"), QStringLiteral("border.focus"), accent);
	store->setColor(QStringLiteral("check"), QStringLiteral("bg.checked"), accent);
	store->setColor(QStringLiteral("check"), QStringLiteral("border.checked"), accent);
	store->setColor(QStringLiteral("check"), QStringLiteral("indicator"), onAccent);
	store->setColor(QStringLiteral("tab"), QStringLiteral("indicator"), accent);

	store->setColor(QStringLiteral("button"), QStringLiteral("bg.accent"), accent);
	store->setColor(QStringLiteral("button"), QStringLiteral("bg.accent.hover"), hover);
	store->setColor(QStringLiteral("button"), QStringLiteral("bg.accent.pressed"), pressed);
	store->setColor(QStringLiteral("button"), QStringLiteral("fg.accent"), onAccent);

	store->endUpdate();
}

} // namespace AccentResolver
} // namespace qtheme
