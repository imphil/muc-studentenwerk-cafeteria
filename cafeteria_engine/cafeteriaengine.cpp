/*
 * Copyright (C) 2008  Philipp Wagner <mail@philipp-wagner.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "cafeteriaengine.h"
#include <QStringList>
#include <QDate>
#include <KDebug>
#include <KIO/Job>
#include <KJob>
#include "locationsource.h"
#include "cafeteriamenusource.h"
#include "config.h"

KUrl CafeteriaEngine::m_serviceUrl;

CafeteriaEngine::CafeteriaEngine(QObject* parent, const QVariantList& args)
    : Plasma::DataEngine(parent, args)
{
    Q_UNUSED(args)

    // there should be no need to update more often than every 20 minutes
    setMinimumPollingInterval(20 * 60 * 1000);

    // the URL is defined inside config.h
    m_serviceUrl = KUrl(WEBSERVICE_URL);

    addSource(LocationSource::self());
    connect(LocationSource::self(), SIGNAL(error(const QString&, const QString&, const QString&)),
            this, SLOT(sourceError(const QString&, const QString&, const QString&)));
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
        connect(source, SIGNAL(error(const QString&, const QString&, const QString&)),
                this, SLOT(sourceError(const QString&, const QString&, const QString&)));
    }

    source->update();
    return true;
}

void CafeteriaEngine::sourceError(const QString &source, const QString &errorMsg, const QString &additionalInfo)
{
    Plasma::DataContainer *errorContainer = new Plasma::DataContainer(this);
    errorContainer->setObjectName("error:"+source);
    errorContainer->setData("msg", errorMsg);
    errorContainer->setData("additionalInfo", additionalInfo);
    addSource(errorContainer);
}


// This does the magic that allows Plasma to load
// this plugin.  The first argument must match
// the X-Plasma-EngineName in the .desktop file.
K_EXPORT_PLASMA_DATAENGINE(cafeteria, CafeteriaEngine)

#include "cafeteriaengine.moc"
