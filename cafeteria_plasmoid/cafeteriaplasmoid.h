/*
 * Mensaplan plasmoid for KDE 4.1+
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

#ifndef CAFETERIAPLASMOID_H_
#define CAFETERIAPLASMOID_H_

#include <KIcon>
#include <Plasma/Applet>
#include <Plasma/DataEngine>

#include "ui_configuration.h"

class QGraphicsProxyWidget;
class QSizeF;
class QGraphicsGridLayout;
class InfoPanel;
class KConfigDialog;

class CafeteriaPlasmoid : public Plasma::Applet
{
    Q_OBJECT

    public:
        struct MenuItem {
            QString name;
            QString value;
            double price;
        };

        CafeteriaPlasmoid(QObject *parent, const QVariantList &args);
        ~CafeteriaPlasmoid();
        void init();



    public slots:
        void dataUpdated(const QString &name, const Plasma::DataEngine::Data &data);
        void createConfigurationInterface(KConfigDialog *parent);

    private slots:
        void configAccepted();

    private:
        Plasma::DataEngine *m_engine;
        InfoPanel *m_infoPanel;
        QGraphicsGridLayout *m_layout;
        uint m_locationId;
        Ui::CafeteriaConfig configUi;
        QMap<QString, uint> m_locations;
        void updateMenu();

};
#endif // CAFETERIAPLASMOID_H_
