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

#ifndef CAFETERIAENGINE_H_
#define CAFETERIAENGINE_H_

#include <KUrl>
#include <Plasma/DataEngine>
#include <Plasma/DataContainer>
#include <Plasma/Service>
#include <Plasma/ServiceJob>
#include <QString>


class LocationSource;
class KJob;
class QStringList;
namespace KIO
{
    class Job;
}

class CafeteriaEngine : public Plasma::DataEngine
{
    Q_OBJECT

    public:
        struct CafeteriaLocation {
            QString name;
            uint id;
            QString address;
        };

        CafeteriaEngine(QObject* parent, const QVariantList& args);
        virtual ~CafeteriaEngine();

        static KUrl serviceUrl();
        static void setServiceUrl(const KUrl &serviceUrl);

        void init();

    protected:
        // this virtual function is called when a new source is requested
        bool sourceRequestEvent(const QString& name);

        // this virtual function is called when an automatic update
        // is triggered for an existing source (ie: when a valid update
        // interval is set when requesting a source)
        bool updateSourceEvent(const QString& source);

    private:
        bool createJob(QString action);
        int getLocationId(const QString &locationName) const;

        static KUrl m_serviceUrl;
        QStringList m_sources;
        LocationSource *m_locationSource;

};

#endif // CAFETERIAENGINE_H_
