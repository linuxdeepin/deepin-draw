// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dzoommenucombobox.h"
#include "cviewmanagement.h"
#include "cgraphicsview.h"
#include "application.h"

#include <QHBoxLayout>
#include <QDebug>
#include <QPalette>
#include <QStyle>
#include <QRect>

#include <DPushButton>
#include <DIconButton>
#include <QKeyEvent>

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
    action->setCheckable(false);
    action->setShortcuts(QKeySequence::UnknownKey);
    action->setAutoRepeat(false);
    m_menu->addAction(action);
    m_actions.append(action);

    if (m_currentIndex == -1) {
        m_currentIndex = 0;
        setCurrentIndex(m_currentIndex);
    }
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

//int DZoomMenuComboBox::getCurrentIndex() const
//{
//    return m_currentIndex;
//}

void DZoomMenuComboBox::setCurrentText(const QString &text)
{
    for (int i = 0; i < m_actions.count(); i++) {
        if (text == m_actions.at(i)->text()) {
            setCurrentIndex(i);
            break;
        }
    }
}

//QString DZoomMenuComboBox::getCurrentText() const
//{
//    if (m_currentIndex >= m_actions.count() || m_currentIndex < 0 || m_actions.count() < 0) {
//        qDebug() << "setCurrentIndex with invalid index...";
//        return QString();
//    }
//    return m_actions.at(m_currentIndex)->text();
//}

void DZoomMenuComboBox::setMenuFlat(bool flat)
{
    m_btn->setFlat(flat);
}

//void DZoomMenuComboBox::setArrowDirction(Qt::LayoutDirection dir)
//{
//    m_btn->setLayoutDirection(dir);
//}

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

bool DZoomMenuComboBox::eventFilter(QObject *o, QEvent *e)
{
    if (o == m_menu) {
        if (e->type() == QEvent::KeyPress) {
            QKeyEvent *pKeyEvent = dynamic_cast<QKeyEvent *>(e);

            if (!pKeyEvent->modifiers() && pKeyEvent->text().length() == 1) {
                return true;
            }
        }
    }
    return DWidget::eventFilter(o, e);
}

void DZoomMenuComboBox::initUI()
{
    setWgtAccesibleName(this, "Zoom Form");
    // [0] 实例化菜单按钮
    m_btn = new QPushButton("", this);
    setWgtAccesibleName(m_btn, "Zoom Menu button");
    m_menu = new QMenu(this);
    setWgtAccesibleName(m_menu, "Zoom Menu");
    m_menu->installEventFilter(this);
    m_btn->setMinimumWidth(136);
    m_btn->setMaximumWidth(136);
    m_btn->setObjectName("ScanleBtn");
    connect(m_btn, &QPushButton::clicked, this, [ = ]() {
        m_menu->exec(mapToGlobal(QPoint(0, this->geometry().y() + this->geometry().height())));
    });

    // [1] 左右加减按钮
    //m_increaseBtn = new DFloatingButton(QIcon::fromTheme("ddc_button_add_hover"), "", this);
    //m_reduceBtn = new DFloatingButton(QIcon::fromTheme("ddc_button_reduce_hover"), "", this);
    m_increaseBtn = new DIconButton(this);
    m_reduceBtn = new DIconButton(this);

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
    //m_reduceBtn->setObjectName("ReduceScence");
    //m_increaseBtn->setObjectName("IncreaseScence");

    m_increaseBtn->setEnabledCircle(true);
    m_reduceBtn->setEnabledCircle(true);

    connect(m_reduceBtn, &DFloatingButton::clicked, this, [ = ]() {
        emit signalLeftBtnClicked();
    });
    connect(m_increaseBtn, &DFloatingButton::clicked, this, [ = ]() {
        emit signalRightBtnClicked();
    });

    QHBoxLayout *m_hlayout = new QHBoxLayout(this);
    m_hlayout->addWidget(m_btn);
    this->setLayout(m_hlayout);

    _btnLay = new QHBoxLayout();
    _btnLay->addWidget(m_reduceBtn);
    _btnLay->addSpacing(m_btn->width() - 2 * m_floatingSize - 2 * 5);
    _btnLay->addWidget(m_increaseBtn);
    // 设置左右按钮的位置，需要悬浮于菜单按钮的上面
    _btnLay->setGeometry(QRect(m_btn->x() + 14, m_btn->y() + 9, m_btn->width(), m_btn->height()));
}

void DZoomMenuComboBox::initConnection()
{
    // 连接子选项按钮菜单被点击信号
    connect(m_menu, &QMenu::triggered, this, &DZoomMenuComboBox::slotActionToggled);
    connect(m_menu, &QMenu::aboutToHide, this, [ = ]() {
        this->setFocus();
    });
}
