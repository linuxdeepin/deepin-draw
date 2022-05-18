// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dzoommenucombobox.h"
#include "application.h"

#include <QHBoxLayout>
#include <QDebug>
#include <QPalette>
#include <QStyle>
#include <QRect>

#include <DPushButton>
#include <DIconButton>
#include <QKeyEvent>
#include <QComboBox>

DZoomMenuComboBox::DZoomMenuComboBox(DWidget *parent):
    DWidget(parent)
  , m_floatingSize(32)
  , m_currentIndex(-1)
{
    initUI();
    initConnection();
}

DZoomMenuComboBox::~DZoomMenuComboBox()
{
    if (_btnLay != nullptr) {
        _btnLay->deleteLater();
        _btnLay = nullptr;
    }
}

void DZoomMenuComboBox::addItem(const QString &itemText)
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
    m_combox->addItem(action->text());
}

void DZoomMenuComboBox::removeItem(const QString &itemText)
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
    if (index >= m_actions.count() || index < 0) {
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

    m_actions.at(m_currentIndex)->setChecked(false);

    m_currentIndex = index;

    m_actions.at(m_currentIndex)->setChecked(true);

    emit signalCurrentTextChanged(m_actions.at(index)->text());
    emit signalCurrentIndexChanged(m_currentIndex);
}

void DZoomMenuComboBox::setCurrentText(const QString &text)
{
    for (int i = 0; i < m_actions.count(); i++) {
        if (text == m_actions.at(i)->text()) {
            setCurrentIndex(i);
            break;
        }
    }
}

void DZoomMenuComboBox::setItemICon(const QString &text, const QIcon icon)
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
    setWgtAccesibleName(this, "Zoom Form");
    // [0] 实例化菜单按钮
    m_combox = new QComboBox(this);
    setWgtAccesibleName(m_combox, "Zoom Combox button");
    m_combox->setFixedSize(QSize(75, 35));

    // [1] 左右加减按钮
    m_increaseBtn = new DIconButton(this);
    m_reduceBtn = new DIconButton(this);
    m_increaseBtn->setFlat(true);
    m_reduceBtn->setFlat(true);

    setWgtAccesibleName(m_increaseBtn, "Zoom increase button");
    setWgtAccesibleName(m_reduceBtn,   "Zoom reduce button");

    m_increaseBtn->setIcon(QIcon::fromTheme("ddc_button_add_hover"));
    m_reduceBtn->setIcon(QIcon::fromTheme("ddc_button_reduce_hover"));

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

    _btnLay = new QHBoxLayout();
    _btnLay->addWidget(m_reduceBtn);
    _btnLay->addWidget(m_combox);
    _btnLay->addWidget(m_increaseBtn);
    setLayout(_btnLay);
}

void DZoomMenuComboBox::initConnection()
{
    connect(m_combox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [ = ](int index) {Q_EMIT signalCurrentTextChanged(m_combox->currentText());});
}
