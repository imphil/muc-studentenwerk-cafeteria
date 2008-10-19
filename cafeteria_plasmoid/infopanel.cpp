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

#include "infopanel.h"

#include <Plasma/TextEdit>
#include <Plasma/Theme>
#include <QGraphicsGridLayout>
#include <KTextEdit>
#include <QSize>
#include <KColorScheme>
#include <QColor>

InfoPanel::InfoPanel(QGraphicsWidget *parent)
    : QGraphicsWidget(parent),
    m_layout(new QGraphicsGridLayout),
    m_menuTextEdit(new Plasma::TextEdit),
    m_colorScheme(0)
{
    m_colorScheme = new KColorScheme(QPalette::Active,
                                     KColorScheme::View,
                                     Plasma::Theme::defaultTheme()->colorScheme());

    KTextEdit *c = m_menuTextEdit->nativeWidget();
    c->setFrameShape(QFrame::NoFrame);
    c->setAttribute(Qt::WA_NoSystemBackground);
    c->viewport()->setAutoFillBackground(false);
    c->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // Attention: this causes a crash currently when right-clicking on the edit
    // http://bugs.kde.org/show_bug.cgi?id=173071
    //c->setTextInteractionFlags(Qt::NoTextInteraction);
    c->setCursor(Qt::ArrowCursor);
    c->setReadOnly(true);
    c->setMinimumWidth(350);
    m_layout->addItem(m_menuTextEdit, 0, 0);

    setLayout(m_layout);
}

InfoPanel::~InfoPanel()
{
    delete m_colorScheme;
}

void InfoPanel::addMenuItem(const CafeteriaPlasmoid::MenuItem item)
{
    m_menuItems.append(item);
}

void InfoPanel::showMenu()
{
    QColor foregroundColor = m_colorScheme->foreground().color();

    QString text = "<table cellpadding=2>";
    foreach (const CafeteriaPlasmoid::MenuItem item, m_menuItems) {
        text += "<tr>";
        text += QString("<td align='right'><font color='%1'><b>%2</b></font></td>")
                .arg(foregroundColor.name())
                .arg(item.name);
        text += QString("<td><font color='%1'>").arg(foregroundColor.name());
        if (item.price != 0) {
            text += QString("%1 <i>(%L2)</i>").arg(item.value).arg(item.price, 0, 'f', 2);
        } else {
            text += QString("%1").arg(item.value);
        }
        text += "</font></td>";
    }
    text += "</table>";
    m_menuTextEdit->setText(text);
    m_menuTextEdit->nativeWidget()->adjustSize();
    m_layout->updateGeometry();
}

void InfoPanel::clearMenu()
{
    m_menuTextEdit->setText("");
    m_menuItems.clear();
}

void InfoPanel::noDataAvailable()
{
    QColor foregroundColor = m_colorScheme->foreground().color();
    m_menuTextEdit->setText(QString("<center><font color='%1'><big>%2</big></font></center>")
            .arg(foregroundColor.name())
            .arg(i18n("No menu available for today.")));
}

void InfoPanel::closed()
{
    QColor foregroundColor = m_colorScheme->foreground().color();
    m_menuTextEdit->setText(QString("<center><font color='%1'><big>%2</big></font></center>")
            .arg(foregroundColor.name())
            .arg(i18n("Cafeteria is closed today.")));
}

void InfoPanel::displayLoadingInformation()
{
    QColor foregroundColor = m_colorScheme->foreground().color();
    m_menuTextEdit->setText(QString("<center><font color='%1'><big>%2</big></font></center>")
            .arg(foregroundColor.name())
            .arg(i18n("Loading data ...")));
}

void InfoPanel::displayError(const QString &msg)
{
    m_menuTextEdit->setText(QString("<center><font color='red'><big>%1</big></font></center>")
            .arg(msg));
}
#include "infopanel.moc"
