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

#ifndef CAFETERIAMENUCACHE_H_
#define CAFETERIAMENUCACHE_H_

#include <QSettings>
#include <QDate>

#include <Plasma/DataEngine>
class KJob;

class CafeteriaMenuCache : public QObject
{
    Q_OBJECT
public:
    CafeteriaMenuCache(QObject* parent=0);

    bool isCached(const QString& name);
    bool set(const QString& name, const Plasma::DataEngine::Data* data);
    bool get(const QString& name, Plasma::DataEngine::Data* data);
    void fetchAndStore(const QDate date, const int locationId);
    /**
     * Delete all cached entries older than two weeks
     */
    void clean();

public slots:
    /**
     * Prefetch data for the next 7 days for the given location
     */
    void prefetch(const int locationId);

private slots:
    void readMenu(KJob *job);

private:
    QSettings* m_cache;
};

#endif /* CAFETERIAMENUCACHE_H_ */
