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

#include <QDomDocument>
#include <QDomNode>
#include <QDomNodeList>
#include <QMap>


CafeteriaMenuSource::CafeteriaMenuSource(CafeteriaEngine::CafeteriaLocation location, QDate &date, QObject* parent)
    : Plasma::DataContainer(parent)
{
    m_date = date;
    m_location = location;
    m_jobRunning = false;
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
        kDebug() << "another job is running";
        return;
    }

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
    if (job->error()) {
        emit error(objectName(), i18n("Unable to load data"), job->errorString());
        m_jobRunning = false;
        return;
    }

    // don't know if that's neccessary as the KJob is deleted anyways
    disconnect(job, SIGNAL(result(KJob*)), this, SLOT(readMenu(KJob*)));

    removeAllData();

    QDomDocument doc("menu");
    QString errorMsg;
    bool ret;
    ret = doc.setContent(dynamic_cast<CafeteriaJob*>(job)->xmlData(), false, &errorMsg);
    if (!ret) {
        emit error(objectName(), i18n("Unable to parse result XML from the web service"), errorMsg);
        m_jobRunning = false;
        return;
    }

    // check status
    QDomNodeList statusList = doc.documentElement().elementsByTagName("status");
    if (statusList.isEmpty()) {
        emit error(objectName(), i18n("Result XML contains no status tag."), "");
        m_jobRunning = false;
        return;
    }
    QString statusText = statusList.at(0).firstChild().nodeValue();
    setData("status", statusText);
    if (statusText != "ok") {
        checkForUpdate();
        return;
    }

    // location and date
    QDomNodeList tmpList;
    tmpList = doc.documentElement().elementsByTagName("location");
    if (!tmpList.isEmpty()) {
        setData("location", tmpList.at(0).firstChild().nodeValue());
    }
    tmpList = doc.documentElement().elementsByTagName("date");
    if (!tmpList.isEmpty()) {
        setData("date", QDate::fromString(tmpList.at(0).firstChild().nodeValue(), Qt::ISODate));
    }

    QDomNodeList items = doc.documentElement().elementsByTagName("item");
    uint cnt = 0;
    for (uint i=0; i<items.length(); i++) {
        QDomNode currentNode = items.at(i);
        QString name = currentNode.firstChildElement("name").firstChild().nodeValue();
        QString value = currentNode.firstChildElement("value").firstChild().nodeValue();
        double price = currentNode.firstChildElement("price").firstChild().nodeValue().toDouble();
        setData("item:"+QString::number(cnt), QString("%1|%2|%3").arg(name).arg(value).arg(price, 0, 'f', 2));
        ++cnt;
    }

    m_jobRunning = false;
    checkForUpdate();
    kDebug() << "menu read";
}

#include "cafeteriamenusource.h"
