/*
 * Copyright (C) 2008-2009  Philipp Wagner <mail@philipp-wagner.com>
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

#include "cafeteriamenucache.h"
#include "xmldataparser.h"
#include "cafeteriajob.h"

#include <KStandardDirs>
#include <KJob>

CafeteriaMenuCache::CafeteriaMenuCache(QObject* parent)
    : QObject(parent)
{
    // initialize menu cache
    QString cacheFilePath = KStandardDirs::locateLocal("data", "plasma_engine_cafeteria/menu.cache");
    m_cache = new QSettings(cacheFilePath, QSettings::IniFormat);
    m_cache->setIniCodec("UTF-8");

    clean();
}

bool CafeteriaMenuCache::isCached(const QString& name)
{
    return m_cache->contains(name+"/location");
}

bool CafeteriaMenuCache::set(const QString& name,
                                const Plasma::DataEngine::Data* data)
{
    m_cache->beginGroup(name);
    foreach (QString key, data->keys()) {
        m_cache->setValue(key, data->value(key));
    }
    m_cache->endGroup();
    return true;
}

bool CafeteriaMenuCache::get(const QString& name,
                                Plasma::DataEngine::Data* data)
{
    if (!m_cache->contains(name+"/location")) {
        return false;
    }

    m_cache->beginGroup(name);
    foreach (QString key, m_cache->childKeys()) {
        data->insert(key, m_cache->value(key, ""));
    }
    m_cache->endGroup();
    return true;
}

void CafeteriaMenuCache::clean()
{
    foreach (QString key, m_cache->childGroups()) {
        QStringList tokens = key.split(':');
        if (tokens.at(0) != "menu" || tokens.size() < 3)
            continue;

        QDate date = QDate::fromString(tokens.at(2), Qt::ISODate);
        if (date < QDate::currentDate().addDays(-14)) {
            m_cache->remove(key);
            kDebug() << "removed entry "+key+" from cache.";
        }
    }
}

void CafeteriaMenuCache::prefetch(const int locationId)
{
    for (int i=0; i<=7; i++) {
        fetchAndStore(QDate::currentDate().addDays(i), locationId);
    }
}

void CafeteriaMenuCache::fetchAndStore(const QDate date, const int locationId)
{
    QString name;
    name = QString("menu:%1:%2").arg(locationId).arg(date.toString("yyyy-MM-dd"));
    if (isCached(name))
        return;


    CafeteriaJobParameters parameters;
    parameters.insert("date", date.toString("yyyy-MM-dd"));
    parameters.insert("location", QString::number(locationId));
    CafeteriaJob *job = new CafeteriaJob("get_menu", parameters);
    connect(job, SIGNAL(result(KJob*)), this, SLOT(readMenu(KJob*)));
    job->start();
}

void CafeteriaMenuCache::readMenu(KJob *job)
{
    if (job->error()) {
        kDebug() << "Unable to cache entry";
        return;
    }

    // don't know if that's neccessary as the KJob is deleted anyways
    disconnect(job, SIGNAL(result(KJob*)), this, SLOT(readMenu(KJob*)));


    QDomDocument doc("menu");
    QString errorMsg;
    bool ret;
    ret = doc.setContent(dynamic_cast<CafeteriaJob*>(job)->xmlData(), false, &errorMsg);
    if (!ret) {
        kDebug() << "Unable to cache entry";
        return;
    }

    Plasma::DataEngine::Data d;
    if (!XMLDataParser::parseMenuXML(doc, &d, &errorMsg)) {
        kDebug() << "Unable to cache entry";
    }

    QString name;
    CafeteriaJobParameters parameters = dynamic_cast<CafeteriaJob*>(job)->parameters();
    name = QString("menu:%1:%2").arg(parameters.value("location")).arg(parameters.value("date"));

    set(name, &d);
    kDebug() << "cache with data for entry "+name+" populated";
}
