/*
* Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
*
* Author: Zhang Hao<zhanghao@uniontech.com>
*
* Maintainer: Zhang Hao <zhanghao@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "dzoommenucombobox.h"

#include <QHBoxLayout>
#include <QDebug>
#include <QPalette>
#include <QStyle>
#include <QRect>

#include <DPushButton>

DZoomMenuComboBox::DZoomMenuComboBox(DWidget *parent):
    DWidget(parent)
    , m_floatingSize(32)
    , m_currentIndex(-1)
{
    initUI();
    initConnection();
}

void DZoomMenuComboBox::addItem(QString itemText)
{
    QIcon ico;
    addItem(itemText, ico);
}

void DZoomMenuComboBox::addItem(QString itemText, QIcon icon)
{
    QAction *action = new QAction(this);
    action->setText(itemText);
    action->setIcon(icon);
    addItem(action);
}

void DZoomMenuComboBox::addItem(QAction *action)
{
    action->setCheckable(false);
    m_menu->addAction(action);
    m_actions.append(action);

    if (m_currentIndex == -1) {
        m_currentIndex = 0;
        setCurrentIndex(m_currentIndex);
    }
}

void DZoomMenuComboBox::removeItem(QString itemText)
{
    for (int i = 0; i < m_actions.count(); i++) {
        if (m_actions.at(i)->text() == itemText) {
            removeItem(i);
            break;
        }
    }
}

void DZoomMenuComboBox::removeItem(int index)
{
    if (index >= m_actions.count() && index < 0) {
        qDebug() << "remove invalid index Item";
        return;
    }
    removeItem(m_actions.at(index));
}

void DZoomMenuComboBox::removeItem(QAction *action)
{
    if (action == nullptr) {
        qDebug() << "remove invalid action Item with nullptr";
        return;
    }
    action->setParent(nullptr);
    m_menu->removeAction(action);
    m_actions.removeOne(action);

    if (m_currentIndex == m_actions.count()) {
        m_currentIndex--;
    }

    setCurrentIndex(m_currentIndex);
}

void DZoomMenuComboBox::setCurrentIndex(int index)
{
    if (index >= m_actions.count() || index < 0 || m_actions.count() < 0) {
        qDebug() << "setCurrentIndex with invalid index...";
        return;
    }
    if (m_currentIndex < 0) {
        m_currentIndex = 0;
    } else if (m_currentIndex >= m_actions.count()) {
        m_currentIndex = m_actions.count() - 1;
    }

//    qDebug()<<"current Index:"<<index;

    m_actions.at(m_currentIndex)->setChecked(false);

    m_currentIndex = index;

    m_actions.at(m_currentIndex)->setChecked(true);

    // 更改按钮显示
    setMenuButtonTextAndIcon(m_actions.at(m_currentIndex)->text(), m_actions.at(m_currentIndex)->icon());

    emit signalCurrentTextChanged(m_actions.at(index)->text());
    emit signalCurrentIndexChanged(m_currentIndex);
}

int DZoomMenuComboBox::getCurrentIndex() const
{
    return m_currentIndex;
}

void DZoomMenuComboBox::setCurrentText(QString text)
{
    for (int i = 0; i < m_actions.count(); i++) {
        if (text == m_actions.at(i)->text()) {
            setCurrentIndex(i);
            break;
        }
    }
}

QString DZoomMenuComboBox::getCurrentText() const
{
    if (m_currentIndex >= m_actions.count() || m_currentIndex < 0 || m_actions.count() < 0) {
        qDebug() << "setCurrentIndex with invalid index...";
        return QString();
    }
    return m_actions.at(m_currentIndex)->text();
}

void DZoomMenuComboBox::setMenuFlat(bool flat)
{
    m_btn->setFlat(flat);
}

void DZoomMenuComboBox::setArrowDirction(Qt::LayoutDirection dir)
{
    m_btn->setLayoutDirection(dir);
}

void DZoomMenuComboBox::setItemICon(QString text, QIcon icon)
{
    for (int i = 0; i < m_actions.count(); i++) {
        if (text == m_actions.at(i)->text()) {
            setItemICon(i, icon);
        }
    }
}

void DZoomMenuComboBox::setItemICon(int index, QIcon icon)
{
    if (index >= m_actions.count() || index < 0) {
        qDebug() << "setItemICon with invalid index...";
        return;
    }
    m_actions[index]->setIcon(icon);
}

QMenu *DZoomMenuComboBox::getMenu()
{
    return m_menu;
}

void DZoomMenuComboBox::updateButtonTextAndIcon()
{
    if (m_currentIndex >= m_actions.count() || m_currentIndex < 0) {
        qDebug() << "updateButtonTextAndIcon with invalid index:" << m_currentIndex;
        return;
    }
    setMenuButtonTextAndIcon(m_actions.at(m_currentIndex)->text(), m_actions.at(m_currentIndex)->icon());
}

void DZoomMenuComboBox::setMenuButtonTextAndIcon(QString text, QIcon ico)
{
    m_btn->setText(text);
    m_btn->setIcon(ico);
}

void DZoomMenuComboBox::slotActionToggled(QAction *action)
{
    for (int i = 0; i < m_actions.count(); i++) {
        if (action->text() == m_actions.at(i)->text()) {
            setCurrentIndex(i);
            break;
        }
    }
}

void DZoomMenuComboBox::initUI()
{
    m_increaseBtn = new DFloatingButton(QIcon::fromTheme("ddc_button_add_hover"), "", this);
    m_reduceBtn = new DFloatingButton(QIcon::fromTheme("ddc_button_reduce_hover"), "", this);
    m_increaseBtn->setFixedSize(QSize(m_floatingSize, m_floatingSize));
    m_reduceBtn->setFixedSize(QSize(m_floatingSize, m_floatingSize));
    m_reduceBtn->setBackgroundRole(QPalette::Button);
    m_increaseBtn->setBackgroundRole(QPalette::Button);
    m_reduceBtn->setIconSize(QSize(24, 24));
    m_increaseBtn->setIconSize(QSize(24, 24));

    connect(m_reduceBtn, &DFloatingButton::clicked, this, [ = ]() {
        emit signalLeftBtnClicked();
    });
    connect(m_increaseBtn, &DFloatingButton::clicked, this, [ = ]() {
        emit signalRightBtnClicked();
    });

    m_btn = new DPushButton("", this);
    m_btn->setMinimumWidth(136);
    m_btn->setMaximumWidth(136);
    connect(m_btn, &DPushButton::clicked, this, [ = ]() {
        m_menu->exec(mapToGlobal(QPoint(0, this->geometry().y() + this->geometry().height())));
    });

    m_menu = new QMenu(this);

    QHBoxLayout *m_hlayout = new QHBoxLayout(this);
    m_hlayout->addWidget(m_btn);
    this->setLayout(m_hlayout);

    QHBoxLayout *hlayout = new QHBoxLayout(this);
    hlayout->addWidget(m_reduceBtn);
    hlayout->addSpacing(m_btn->width() - 2 * m_floatingSize - 2 * 5);
    hlayout->addWidget(m_increaseBtn);
    this->setLayout(hlayout);
    hlayout->setGeometry(QRect(m_btn->x() + 14, m_btn->y() + 9, m_btn->width(), m_btn->height()));

    m_reduceBtn->raise();
    m_increaseBtn->raise();
}

void DZoomMenuComboBox::initConnection()
{
    // 连接子选项按钮菜单被点击信号
    connect(m_menu, &QMenu::triggered, this, &DZoomMenuComboBox::slotActionToggled);
}
