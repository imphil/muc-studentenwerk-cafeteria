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

#include "infopanel.h"

CafeteriaPlasmoid::CafeteriaPlasmoid(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
    m_infoPanel(new InfoPanel)
{
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    setHasConfigurationInterface(true);
    resize(350, 300);

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

    // connect with engine
    m_engine = dataEngine("cafeteria");
    if (!m_engine->isValid()) {
        setFailedToLaunch(true, i18n("Failed to load cafeteria DataEngine"));
        return;
    }

    // create layout
    m_layout = new QGraphicsGridLayout();
    m_layout->addItem(m_infoPanel, 0, 0);
    setLayout(m_layout);

    // connect sources
    m_engine->connectSource("locations", this);
}

void CafeteriaPlasmoid::dataUpdated(const QString& source, const Plasma::DataEngine::Data &data)
{
    kDebug() << "dataUpdated(" << source << ") called";

    if (source == "locations") {
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
        kDebug() << "Locations updated";
    } else if (source.startsWith("menu:")) {
        // update menu
        // we use a QMap here to sort the items we get out of data by key
        QMap<uint, MenuItem> menuItemsSortMap;
        Plasma::DataEngine::Data::const_iterator i;
        for (i = data.constBegin(); i != data.constEnd(); ++i) {
            QStringList parts = i.value().toString().split('|');
            MenuItem menuItem;
            menuItem.name = parts.at(0);
            menuItem.value = parts.at(1);
            menuItem.price = parts.at(2).toDouble();
            menuItemsSortMap.insert(i.key().toInt(), menuItem);
        }

        foreach (MenuItem menuItem, menuItemsSortMap) {
            m_infoPanel->addMenuItem(menuItem);
        }
        m_infoPanel->showMenu();
    }
}

void CafeteriaPlasmoid::updateMenu()
{
    QString query = "menu:%1";
    query = query.arg(m_locationId);
    m_engine->connectSource(query, this);
    //m_engine->connectSource("Error:" + query, this);
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
    }
}


/* link the .desktop file to the class
   the first argument must match X-KDE-PluginInfo-Name in the .desktop file

   this needs to stay inside the cpp file as it can be called only once
   (otherwise we couldn't include cafeteriaplasmoid.h anywhere else) */
K_EXPORT_PLASMA_APPLET(cafeteria, CafeteriaPlasmoid)

#include "cafeteriaplasmoid.moc"
