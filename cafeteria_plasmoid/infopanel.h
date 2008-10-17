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


#ifndef INFOPANEL_H_
#define INFOPANEL_H_

#include <QGraphicsWidget>
#include <QMap>
#include <QString>
#include "cafeteriaplasmoid.h"

namespace Plasma {
    class Label;
}
class QGraphicsLayoutItem;
class QGraphicsGridLayout;

class InfoPanel : public QGraphicsWidget
{
    Q_OBJECT

public:
    InfoPanel(QGraphicsWidget *parent = 0);
    virtual ~InfoPanel();
    void clearMenu();
    void addMenuItem(const CafeteriaPlasmoid::MenuItem item);
    void showMenu();

private:
    QGraphicsGridLayout *m_layout;
    Plasma::Label* m_menuLabel;
    QList<CafeteriaPlasmoid::MenuItem> m_menuItems;

};

#endif /* INFOPANEL_H_ */
