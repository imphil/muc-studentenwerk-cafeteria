/*
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

#include "infopanel.h"

#include <Plasma/Label>
#include <QGraphicsGridLayout>
#include <QLabel>

InfoPanel::InfoPanel(QGraphicsWidget *parent)
    : QGraphicsWidget(parent),
    m_layout(new QGraphicsGridLayout),
    m_menuLabel(new Plasma::Label)
{
    m_menuLabel->nativeWidget()->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_layout->addItem(m_menuLabel, 0, 0);
    setLayout(m_layout);
}

InfoPanel::~InfoPanel()
{
}

void InfoPanel::addMenuItem(const CafeteriaPlasmoid::MenuItem item)
{
    m_menuItems.append(item);
}

void InfoPanel::showMenu()
{
    QString text = "<table>";
    foreach (const CafeteriaPlasmoid::MenuItem item, m_menuItems) {
        text += "<tr>";
        text += "<td><b>"+item.name+"</b></td>";
        if (item.price != 0) {
            text += QString("<td>%1 <i>(%L2)</i></td>").arg(item.value).arg(item.price, 0, 'f', 2);
        } else {
            text += QString("<td>%1</td>").arg(item.value);
        }
    }
    text += "</table>";
    m_menuLabel->setText(text);
}

#include "infopanel.moc"
