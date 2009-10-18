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

#include "cafeteriamenusource.h"
#include "cafeteriajob.h"
#include "xmldataparser.h"

#include <QDomNode>
#include <QDomNodeList>
#include <QMap>
#include <QTimer>

#include <Solid/Networking>


CafeteriaMenuSource::CafeteriaMenuSource(CafeteriaEngine::CafeteriaLocation location,
                                         QDate &date, CafeteriaMenuCache* menuCache,
                                         QObject* parent)
    : Plasma::DataContainer(parent)
{
    m_date = date;
    m_location = location;
    m_jobRunning = false;
    m_cache = menuCache;
}

CafeteriaMenuSource::~CafeteriaMenuSource()
{
}

void CafeteriaMenuSource::setDate(QDate &date)
{
    m_date = date;
}

QDate CafeteriaMenuSource::date() const
{
    return m_date;
}

CafeteriaEngine::CafeteriaLocation CafeteriaMenuSource::location() const
{
    return m_location;
}

void CafeteriaMenuSource::setLocation(CafeteriaEngine::CafeteriaLocation location)
{
    m_location = location;
}

void CafeteriaMenuSource::update()
{
    if (m_jobRunning) {
        kDebug() << "another network job is running";
        return;
    }

    Solid::Networking::Status status = Solid::Networking::status();
    if (status == Solid::Networking::Connected ||
        status == Solid::Networking::Unknown) {
        fetchMenuFromNetwork();
    } else {
        // no idea why this is necessary, but otherwise the checkForUpdate()
        // call inside fetchMenuFromCache() does not work
        // (possibly not in main event loop?)
        QTimer::singleShot(0, this, SLOT(fetchMenuFromCache()));
    }
}

void CafeteriaMenuSource::fetchMenuFromCache()
{
    kDebug() << "reading cached menu for " << objectName();
    if (!m_cache->isCached(objectName())) {
        emit error(objectName(), i18n("Unable to load data"), "Data not cached.");
    }

    removeAllData();
    Plasma::DataEngine::Data d;
    if (!m_cache->get(objectName(), &d)) {
        emit error(objectName(), i18n("Unable to load data"), "Loading data from cache failed.");
    }
    foreach (QString key, d.keys()) {
        setData(key, d.value(key));
    }

    checkForUpdate();
}

void CafeteriaMenuSource::fetchMenuFromNetwork()
{
    kDebug() << "fetching menu from network for " << objectName();
    m_jobRunning = true;
    CafeteriaJobParameters parameters;
    parameters.insert("date", m_date.toString("yyyy-MM-dd"));
    parameters.insert("location", QString::number(m_location.id));
    CafeteriaJob *job = new CafeteriaJob("get_menu", parameters);
    connect(job, SIGNAL(result(KJob*)), this, SLOT(readMenu(KJob*)));
    job->start();
}

void CafeteriaMenuSource::readMenu(KJob *job)
{
    m_jobRunning = false;

    if (job->error()) {
        emit error(objectName(), i18n("Unable to load data"), job->errorString());
        return;
    }

    // don't know if that's neccessary as the KJob is deleted anyways
    disconnect(job, SIGNAL(result(KJob*)), this, SLOT(readMenu(KJob*)));


    QDomDocument doc("menu");
    QString errorMsg;
    bool ret;
    ret = doc.setContent(dynamic_cast<CafeteriaJob*>(job)->xmlData(), false, &errorMsg);
    if (!ret) {
        emit error(objectName(), i18n("Unable to parse result XML from the web service"), errorMsg);
        return;
    }

    Plasma::DataEngine::Data d;
    if (!XMLDataParser::parseMenuXML(doc, &d, &errorMsg)) {
        emit error(objectName(), i18n("Unable to load data"), errorMsg);
    }
    m_cache->set(objectName(), &d);

    // update data source with new data
    removeAllData();
    foreach (QString key, d.keys()) {
        setData(key, d.value(key));
    }

    checkForUpdate();
}

#include "cafeteriamenusource.h"
