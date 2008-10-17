/*
 * Mensaplan data engine for KDE 4.1+
 * Copyright 2008  Philipp Wagner <mail@philipp-wagner.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cafeteriaengine.h"
#include <QStringList>
#include <QDate>
#include <KDebug>
#include <KIO/Job>
#include <KJob>
#include "locationsource.h"
#include "cafeteriamenusource.h"

KUrl CafeteriaEngine::m_serviceUrl;

CafeteriaEngine::CafeteriaEngine(QObject* parent, const QVariantList& args)
    : Plasma::DataEngine(parent, args)
{
    Q_UNUSED(args)

    // there should be absoluteley no need to update more often than every 20s
    //setMinimumPollingInterval(1 * 1000);

    // TODO: find a way to make this configurable
    m_serviceUrl = KUrl("http://philipp.wagner.name/cafeteriamenu/cafeteriamenu-xml.php");

    addSource(LocationSource::self());
}

CafeteriaEngine::~CafeteriaEngine()
{
}

void CafeteriaEngine::init()
{
}

KUrl CafeteriaEngine::serviceUrl()
{
    return m_serviceUrl;
}

void CafeteriaEngine::setServiceUrl(const KUrl &serviceUrl)
{
    m_serviceUrl = serviceUrl;
}

bool CafeteriaEngine::sourceRequestEvent(const QString &name)
{
    if (name == "locations") {
        // these are updated by the engine itself, not consumers
        return true;
    }

    if (!name.startsWith("menu:")) {
        kDebug() << "Invalid source name:" << name;
        return false;
    }

    // We do not have any special code to execute the
    // first time a source is requested, so we just call
    // updateSourceEvent().
    return updateSourceEvent(name);
}

bool CafeteriaEngine::updateSourceEvent(const QString &name)
{
    kDebug() << "updateSourceEvent(" << name << ")";
    QStringList tokens = name.split(':');
    if (tokens.at(0) != "menu") {
        kDebug() << "action != \"menu\" (got " << tokens.at(0) << ")";
        return false;
    }
    if (!LocationSource::self()->validateLocationId(tokens.at(1).toInt())) {
        kDebug() << "Invalid location ID: " << tokens.at(1);
        kDebug() << "this error might indicate that the available locations"
            "are not yet downloaded. wait until that's done and try again";
        return false;
    }
    QDate date;
    if (tokens.size() > 2) {
        date = QDate::fromString(tokens.at(2), Qt::ISODate);
        if (!date.isValid()) {
            kDebug() << "Invalid date: " << tokens.at(2) << "(YYYY-MM-DD expected)";
            return false;
        }
    } else {
        date = QDate::currentDate();
    }

    CafeteriaMenuSource *source = dynamic_cast<CafeteriaMenuSource*>(containerForSource(name));

    if (!source) {
        CafeteriaEngine::CafeteriaLocation location = LocationSource::self()->getLocation(tokens.at(1).toInt());
        if (location.id == 0) {
            kDebug() << "unable to create location with id "<<tokens.at(1);
            return false;
        }
        source = new CafeteriaMenuSource(location, date, this);
        source->setObjectName(name);
        addSource(source);
    }

    source->update();
    return true;
}


// This does the magic that allows Plasma to load
// this plugin.  The first argument must match
// the X-Plasma-EngineName in the .desktop file.
K_EXPORT_PLASMA_DATAENGINE(cafeteria, CafeteriaEngine)

#include "cafeteriaengine.moc"
