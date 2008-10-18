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

#include "cafeteriajob.h"

#include <QMap>
#include <QString>
#include <KUrl>
#include <KIO/Job>
#include <QByteArray>
#include <QTimer>
#include <KIO/TransferJob>

CafeteriaJob::CafeteriaJob(const QString &action, const CafeteriaJobParameters &parameters)
    : KJob()
{
    m_action = action;
    m_parameters = parameters;
    m_url = CafeteriaEngine::serviceUrl();
}

CafeteriaJob::CafeteriaJob(const QString &action)
    : KJob()
{
    m_action = action;
    m_url = CafeteriaEngine::serviceUrl();
}

CafeteriaJob::~CafeteriaJob()
{
}

void CafeteriaJob::start()
{
    QTimer::singleShot(0, this, SLOT(fetchData()));
}

void CafeteriaJob::fetchData()
{
    // prepare query URL
    m_url.addQueryItem("action", m_action);
    QMap<QString, QString>::const_iterator i;
    for (i = m_parameters.constBegin(); i != m_parameters.constEnd(); ++i)
        m_url.addQueryItem(i.key(), i.value());

    KJob *httpJob = KIO::get(m_url, KIO::Reload, KIO::HideProgressInfo);

    connect(httpJob, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(recv(KIO::Job*, const QByteArray&)));
    connect(httpJob, SIGNAL(result(KJob*)), this, SLOT(httpResult(KJob*)));
}


void CafeteriaJob::recv(KIO::Job*, const QByteArray& data)
{
    m_rcv_data += data;
}

void CafeteriaJob::httpResult(KJob *job)
{
    if (dynamic_cast<KIO::TransferJob*>(job)->isErrorPage()) {
        // unfortunately, a 404 or 500 HTTP status code is not reported as
        // error. But in the end, it's an error we need to handle
        setError(1);
        setErrorText(i18n("got HTTP error code"));
    } else {
        setError(job->error());
        setErrorText(job->errorText());
    }

    emitResult();
}

QByteArray CafeteriaJob::xmlData() const
{
    return m_rcv_data;
}

#include "cafeteriajob.moc"
