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

#ifndef LOCATIONSOURCE_H_
#define LOCATIONSOURCE_H_

#include <QList>
#include <QByteArray>
#include <KJob>
#include <Plasma/DataContainer>

#include "cafeteriajob.h"
#include "cafeteriaengine.h"

class LocationSource : public Plasma::DataContainer
{
    Q_OBJECT

    public:
        static LocationSource* self();
        LocationSource(QObject* parent);
        virtual ~LocationSource();

        bool validateLocationName(const QString &locationName) const;
        bool validateLocationId(const uint id) const;
        QList<CafeteriaEngine::CafeteriaLocation> getAllLocations();
        CafeteriaEngine::CafeteriaLocation getLocation(const QString &locationName) const;
        CafeteriaEngine::CafeteriaLocation getLocation(const uint locationId) const;
        QList<CafeteriaEngine::CafeteriaLocation> locations() const;

    private slots:
        void readLocations(KJob*);

    private:
        void fetchLocations();
        QList<CafeteriaEngine::CafeteriaLocation> m_locations;
};

#endif /* LOCATIONSOURCE_H_ */
