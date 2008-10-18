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

#ifndef CAFETERIAJOB_H_
#define CAFETERIAJOB_H_

#include <QMap>
#include <KJob>
#include "cafeteriaengine.h"

class QString;
class KUrl;
class QByteArray;
namespace KIO
{
    class Job;
}

typedef QMap<QString, QString> CafeteriaJobParameters;

class CafeteriaJob : public KJob
{
    Q_OBJECT
    Q_PROPERTY(QByteArray xmlData READ xmlData)

    public:
        CafeteriaJob(const QString &action, const CafeteriaJobParameters &parameters);
        CafeteriaJob(const QString &action);
        virtual ~CafeteriaJob();
        void start();
        QByteArray xmlData() const;

    private slots:
        void fetchData();
        void httpResult(KJob *job);
        void recv(KIO::Job*, const QByteArray& data);

    private:
        KUrl m_url;
        QString m_action;
        CafeteriaJobParameters m_parameters;
        QByteArray m_rcv_data;
};

#endif /* CAFETERIAJOB_H_ */
