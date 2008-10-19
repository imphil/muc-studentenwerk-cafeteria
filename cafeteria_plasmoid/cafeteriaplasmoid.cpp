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

#include "cafeteriaplasmoid.h"
#include <QLabel>
#include <QGraphicsGridLayout>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <KSharedConfig>
#include <KConfigDialog>
#include <Plasma/Label>
#include <Plasma/PushButton>
#include <KPushButton>
#include <QDate>
#include <KGlobal>
#include "infopanel.h"

CafeteriaPlasmoid::CafeteriaPlasmoid(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
    m_infoPanel(new InfoPanel),
    m_cafeteriaNameLabel(new Plasma::Label)
{
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    setHasConfigurationInterface(true);
    resize(420, 250); // with, height
    setMinimumWidth(420); // make it the same as infoPanel!

    setBackgroundHints(DefaultBackground);
}

CafeteriaPlasmoid::~CafeteriaPlasmoid()
{
}

void CafeteriaPlasmoid::init()
{
    // configuration
    KConfigGroup cg = config();
    m_locationId = cg.readEntry("locationId").toInt();

    // this displays a button to configure the plasmoid if no location is chosen
    setConfigurationRequired(m_locationId == 0, i18n("Please choose a cafeteria location."));

    // connect with engine
    m_engine = dataEngine("cafeteria");
    if (!m_engine->isValid()) {
        setFailedToLaunch(true, i18n("Failed to load cafeteria DataEngine"));
        return;
    }

    // create layout,
    m_previousDayButton = new Plasma::PushButton;
    m_nextDayButton = new Plasma::PushButton;
    m_previousDayButton->nativeWidget()->setIcon(KIcon("go-previous"));
    m_previousDayButton->nativeWidget()->setToolTip(i18n("go to previous day"));
    m_previousDayButton->nativeWidget()->setMaximumWidth(40);
    m_nextDayButton->nativeWidget()->setIcon(KIcon("go-next"));
    m_nextDayButton->nativeWidget()->setToolTip(i18n("go to next day"));
    m_nextDayButton->nativeWidget()->setMaximumWidth(40);
    m_layout = new QGraphicsGridLayout();
    m_layout->addItem(m_previousDayButton, 0, 0, Qt::AlignLeft);
    m_layout->addItem(m_cafeteriaNameLabel, 0, 1, Qt::AlignHCenter | Qt::AlignVCenter);
    m_layout->addItem(m_nextDayButton, 0, 2, Qt::AlignRight);
    m_layout->addItem(m_infoPanel, 1, 0, 1, 3);
    setLayout(m_layout);

    // connect date buttons
    connect(m_previousDayButton, SIGNAL(clicked()), this, SLOT(changeDay()));
    connect(m_nextDayButton, SIGNAL(clicked()), this, SLOT(changeDay()));

    m_date = QDate::currentDate();
    m_previousDayButton->nativeWidget()->setDisabled(true);

    // connect sources
    connect(m_engine, SIGNAL(sourceAdded(const QString&)),
            this, SLOT(sourceAdded(const QString&)));
    m_engine->connectSource("locations", this);
}

void CafeteriaPlasmoid::sourceAdded(const QString &source)
{
    // we always connect to error sources
    if (source.startsWith("error:")) {
        m_engine->connectSource(source, this);
    }
}

void CafeteriaPlasmoid::dataUpdated(const QString& source, const Plasma::DataEngine::Data &data)
{
    kDebug() << "dataUpdated(" << source << ") called";

    if (data.isEmpty()) {
        kDebug() << "empty update";
        return;
    }

    if (source.startsWith("error:")) {
        // an error occurred while getting some data
        m_infoPanel->displayError(data.value("msg").toString());
        return;
    } else if (source == "locations") {
        // update list of available locations
        m_locations.clear();
        Plasma::DataEngine::Data::const_iterator i;
        for (i = data.constBegin(); i != data.constEnd(); ++i) {
            QStringList parts = i.value().toString().split('|');
            m_locations.insert(parts.at(0), i.key().toInt());
        }
        // now the engine is ready to get the menu for the previously saved locationId
        if (m_locationId != 0) {
            updateMenu();
        }
    } else if (source.startsWith("menu:")) {
        // update menu
        if (data.isEmpty()) {
            kDebug() << data;
            m_infoPanel->noDataAvailable();
        } else {
            QString status;
            // we use a QMap here to sort the items we get out of data by key
            QMap<uint, MenuItem> menuItemsSortMap;
            Plasma::DataEngine::Data::const_iterator i;
            for (i = data.constBegin(); i != data.constEnd(); ++i) {
                if (i.key().startsWith("item:")) {
                    QStringList parts = i.value().toString().split('|');
                    MenuItem menuItem;
                    menuItem.name = parts.at(0);
                    menuItem.value = parts.at(1);
                    menuItem.price = parts.at(2).toDouble();
                    menuItemsSortMap.insert(i.key().mid(5).toInt(), menuItem);
                } else if (i.key() == "status") {
                    status = i.value().toString();
                }
                // other available keys (not used): date, location
            }

            m_infoPanel->clearMenu();
            if (status == "ok") {
                foreach (MenuItem menuItem, menuItemsSortMap) {
                    m_infoPanel->addMenuItem(menuItem);
                }
                m_infoPanel->showMenu();
            } else if (status == "closed") {
                m_infoPanel->closed();
            } else {
                m_infoPanel->noDataAvailable();
            }
        }

        // resize to fit
        /*qreal left, top, right, bottom;
        getContentsMargins(&left, &top, &right, &bottom);
        resize(m_layout->sizeHint(Qt::PreferredSize) + QSizeF(left+right, top+bottom));
        updateGeometry();*/
    }
}

void CafeteriaPlasmoid::updateMenu()
{
    kDebug() << "updating menu";
    //m_engine->disconnectSource(m_previousQuery, this);

     QString text = QString("<center><big>%1</big><br><small>(%2)</small></center>")
            .arg(m_locations.key(m_locationId))
            .arg(KGlobal::locale()->formatDate(m_date, KLocale::FancyLongDate));
    m_cafeteriaNameLabel->setText(text);

    m_infoPanel->displayLoadingInformation();

    QString query = QString("menu:%1:%2")
            .arg(m_locationId)
            .arg(m_date.toString(Qt::ISODate));

    if (m_engine->sources().contains(query)) {
        Plasma::DataEngine::Data data = m_engine->query(query);
        if (!data.isEmpty()) {
            dataUpdated(query, data);
        }
    }
    m_engine->connectSource(query, this);
}

void CafeteriaPlasmoid::createConfigurationInterface(KConfigDialog *parent)
{
    parent->setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Apply);
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));

    QWidget *configWidget = new QWidget();
    configUi.setupUi(configWidget);

    QMap<QString, uint>::const_iterator i;
    uint cnt = 0;
    int selectedItem = 0;
    for (i = m_locations.constBegin(); i != m_locations.constEnd(); ++i) {
        if (i.value() == m_locationId) {
            selectedItem = cnt;
        }
        configUi.locationComboBox->insertItem(configUi.locationComboBox->count(), i.key());
        ++cnt;
    }
    configUi.locationComboBox->setCurrentIndex(selectedItem);

    parent->addPage(configWidget, parent->windowTitle(), icon());
}

void CafeteriaPlasmoid::configAccepted()
{
    QString locationName = configUi.locationComboBox->currentText();
    QMap<QString, uint>::const_iterator i = m_locations.constFind(locationName);
    if (i == m_locations.constEnd()) {
        // this could happen if m_locations was updated during the time the
        // config dialog was displayed
        // FIXME: inform the user about this
        return;
    }
    uint locationId = i.value();

    bool changed = false;
    KConfigGroup cg = config();

    if (m_locationId != locationId) {
        kDebug() << "new location ID:" << locationId;
        changed = true;
        m_locationId = locationId;
        cg.writeEntry("locationId", m_locationId);
    }

    if (changed) {
        emit configNeedsSaving();
        updateMenu();
        setConfigurationRequired(false);
    }
}

void CafeteriaPlasmoid::changeDay()
{
    if (sender() == m_previousDayButton) {
        m_date = m_date.addDays(-1);
    } else if (sender() == m_nextDayButton) {
        m_date = m_date.addDays(1);
    }

    if (m_date <= QDate::currentDate()) {
        m_previousDayButton->nativeWidget()->setDisabled(true);
    } else {
        m_previousDayButton->nativeWidget()->setDisabled(false);
    }
    updateMenu();
}


/* link the .desktop file to the class
   the first argument must match X-KDE-PluginInfo-Name in the .desktop file

   this needs to stay inside the cpp file as it can be called only once
   (otherwise we couldn't include cafeteriaplasmoid.h anywhere else) */
K_EXPORT_PLASMA_APPLET(cafeteria, CafeteriaPlasmoid)

#include "cafeteriaplasmoid.moc"
