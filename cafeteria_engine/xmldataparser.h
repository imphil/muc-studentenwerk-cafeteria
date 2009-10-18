/*
 * xmldataparser.h
 *
 *  Created on: 18.10.2009
 *      Author: philipp
 */

#ifndef XMLDATAPARSER_H_
#define XMLDATAPARSER_H_

#include <QDomDocument>
#include <Plasma/DataEngine>

class XMLDataParser
{
public:
    XMLDataParser();
    virtual ~XMLDataParser();

    static bool parseMenuXML(QDomDocument doc,
                                Plasma::DataEngine::Data* resultData,
                                QString* errorString);

};

#endif /* XMLDATAPARSER_H_ */
