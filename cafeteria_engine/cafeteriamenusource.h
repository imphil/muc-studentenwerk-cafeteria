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

#ifndef CAFETERIAMENUSOURCE_H_
#define CAFETERIAMENUSOURCE_H_

#include <QDate>
#include <QList>
#include <QByteArray>
#include <KJob>
#include <Plasma/DataContainer>

#include "cafeteriaengine.h"

class CafeteriaMenuSource : public Plasma::DataContainer
{
    Q_OBJECT

    Q_PROPERTY(QDate date READ date WRITE setDate)
    Q_PROPERTY(CafeteriaEngine::CafeteriaLocation location READ location WRITE setLocation)

    public:
        CafeteriaMenuSource(CafeteriaEngine::CafeteriaLocation location, QDate &date, QObject* parent);
        virtual ~CafeteriaMenuSource();

        QDate date() const;
        void setDate(QDate &date);
        CafeteriaEngine::CafeteriaLocation location() const;
        void setLocation(CafeteriaEngine::CafeteriaLocation location);
        void update();

    signals:
        void error(const QString&, const QString&, const QString&);

    private slots:
        void readMenu(KJob *job);

    private:
        QDate m_date;
        CafeteriaEngine::CafeteriaLocation m_location;
        bool m_jobRunning;
};

#endif /* CAFETERIAMENUSOURCE_H_ */
