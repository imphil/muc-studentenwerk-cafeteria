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

#include "locationsource.h"
#include "cafeteriajob.h"

#include <QDomDocument>
#include <QDomNode>
#include <QDomNodeList>

class LocationSourceSingleton
{
    public:
        LocationSourceSingleton()
            : self(0)
        {
        }

        LocationSource self;
};

K_GLOBAL_STATIC(LocationSourceSingleton, privateLocationSourceSelf)


LocationSource* LocationSource::self()
{
    return &privateLocationSourceSelf->self;
}

LocationSource::LocationSource(QObject *parent)
    : Plasma::DataContainer(parent)
{
    setObjectName("locations");
    fetchLocations();
}

LocationSource::~LocationSource()
{
}

bool LocationSource::validateLocationName(const QString &locationName) const
{
    // TODO: add implementation
}

bool LocationSource::validateLocationId(const uint id) const
{
    foreach (CafeteriaEngine::CafeteriaLocation locationInfo, m_locations) {
        if (locationInfo.id == id) {
            return true;
        }
    }
    return false;
}

QList<CafeteriaEngine::CafeteriaLocation> LocationSource::getAllLocations()
{
    return m_locations;
}

void LocationSource::fetchLocations()
{
    CafeteriaJob *job = new CafeteriaJob("get_locations");
    connect(job, SIGNAL(result(KJob*)), this, SLOT(readLocations(KJob*)));
    job->start();
}

void LocationSource::readLocations(KJob *job)
{
    if (job->error()) {
        emit error(objectName(), i18n("Unable to load data"), job->errorString());
        return;
    }

    QDomDocument doc;
    QString errorMsg;
    bool ret;
    ret = doc.setContent(dynamic_cast<CafeteriaJob*>(job)->xmlData(), false, &errorMsg);
    if (!ret) {
        emit error(objectName(), i18n("Unable to parse result XML from the web service"), errorMsg);
        return;
    }

    // temporary variable for this->m_locations to provide atomic updates
    QList<CafeteriaEngine::CafeteriaLocation> locationsTmp;

    QDomNodeList locations = doc.documentElement().elementsByTagName("location");
    for (uint i=0; i<locations.length(); i++) {
        QDomNode currentNode = locations.at(i);
        QString location = currentNode.firstChildElement("name").firstChild().nodeValue();
        uint locationId = currentNode.firstChildElement("id").firstChild().nodeValue().toInt();
        QString address = currentNode.firstChildElement("address").firstChild().nodeValue();

        CafeteriaEngine::CafeteriaLocation locationInfo;
        locationInfo.id = locationId;
        locationInfo.name = location;
        locationInfo.address = address;

        locationsTmp << locationInfo;

        setData(QString::number(locationId), location+"|"+address);
    }

    m_locations = locationsTmp;
    checkForUpdate();
}

CafeteriaEngine::CafeteriaLocation LocationSource::getLocation(const QString &locationName) const
{
    foreach (CafeteriaEngine::CafeteriaLocation locationInfo, m_locations) {
        if (locationInfo.name.compare(locationName, Qt::CaseInsensitive) == 0) {
            return locationInfo;
        }
    }
    CafeteriaEngine::CafeteriaLocation invalidLocation;
    invalidLocation.id = 0;
    kDebug() << "Attention: Returning invalid CafeteriaLocation for name " << locationName;
    return invalidLocation;
}

CafeteriaEngine::CafeteriaLocation LocationSource::getLocation(const uint locationId) const
{
    foreach (CafeteriaEngine::CafeteriaLocation locationInfo, m_locations) {
        if (locationInfo.id == locationId) {
            return locationInfo;
        }
    }
    CafeteriaEngine::CafeteriaLocation invalidLocation;
    invalidLocation.id = 0;
    kDebug() << "Attention: Returning invalid CafeteriaLocation for ID " << locationId;
    return invalidLocation;
}

QList<CafeteriaEngine::CafeteriaLocation> LocationSource::locations() const
{
    return m_locations;
}

#include "locationsource.moc"
