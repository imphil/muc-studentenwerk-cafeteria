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

#include "xmldataparser.h"

#include <QDate>

XMLDataParser::XMLDataParser()
{
    // TODO Auto-generated constructor stub

}

XMLDataParser::~XMLDataParser()
{
    // TODO Auto-generated destructor stub
}

bool XMLDataParser::parseMenuXML(QDomDocument doc,
                                     Plasma::DataEngine::Data* resultData,
                                     QString* errorString)
{
    // check status
    QDomNodeList statusList = doc.documentElement().elementsByTagName("status");
    if (statusList.isEmpty()) {
        *errorString = "Result XML contains no status tag.";
        return false;
    }
    QString statusText = statusList.at(0).firstChild().nodeValue();
    /*if (statusText != "ok") {
        *errorString = "Result status not 'OK'";
        return false;
    }*/
    resultData->insert("status", statusText);

    QString value;
    // location and date
    QDomNodeList tmpList;
    tmpList = doc.documentElement().elementsByTagName("location");
    if (!tmpList.isEmpty()) {
        value = tmpList.at(0).firstChild().nodeValue();
        resultData->insert("location", value);
    }
    tmpList = doc.documentElement().elementsByTagName("date");
    if (!tmpList.isEmpty()) {
        QDate dateValue = QDate::fromString(tmpList.at(0).firstChild().nodeValue(), Qt::ISODate);
        resultData->insert("date", dateValue);
    }

    QDomNodeList items = doc.documentElement().elementsByTagName("item");
    uint cnt = 0;
    for (uint i=0; i<items.length(); i++) {
        QDomNode currentNode = items.at(i);
        QString name = currentNode.firstChildElement("name").firstChild().nodeValue();
        QString value = currentNode.firstChildElement("value").firstChild().nodeValue();
        double price = currentNode.firstChildElement("price").firstChild().nodeValue().toDouble();
        value = QString("%1|%2|%3").arg(name).arg(value).arg(price, 0, 'f', 2);
        resultData->insert("item:"+QString::number(cnt), value);
        ++cnt;
    }

    return true;
}
