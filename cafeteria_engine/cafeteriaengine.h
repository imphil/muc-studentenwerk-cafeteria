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

    public slots:
        void sourceError(const QString&, const QString&, const QString&);

    protected:
        bool sourceRequestEvent(const QString& name);
        bool updateSourceEvent(const QString& source);

    private:
        bool createJob(QString action);
        int getLocationId(const QString &locationName) const;

        static KUrl m_serviceUrl;
        QStringList m_sources;
        LocationSource *m_locationSource;

};

#endif // CAFETERIAENGINE_H_
