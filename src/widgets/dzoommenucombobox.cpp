// SPDX-FileCopyrightText: 2020 - 2023 UnionTech Software Technology Co., Ltd.
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

// 紧凑模式下的标题栏高度
const int COMPACT_TITLE_HEIGHT = 40;
const int SPACING_ITEM_IDX = 1;
const int X_OFFSET = 14;
const int Y_OFFSET = 9;

// 用于普通模式下的布局
const int BOX_WITDH = 162;
const int BOX_BTN_WITDH = 136;
const int FLOAT_BTN_SIZE = 32;
const int ICON_SIZE = 24;
const int FLOAT_X_OFFSET = 14;
const int FLOAT_Y_OFFSET = 9;

// 用于紧凑模式下的布局
const int COMPACT_LEFT_OFFSET = 8;      // 紧凑模式下向左调整8px
const int COMPACT_BOX_WITDH = 146;
const int COMPACT_BOX_BTN_WIDTH = 120;
const int COMPACT_FLOAT_BTN_SIZE = 24;
const int COMPACT_ICON_SIZE = 18;
const int COMPACT_FLOAT_X_OFFSET = 17 - COMPACT_LEFT_OFFSET;
const int COMPACT_FLOAT_Y_OFFSET = 8;

DZoomMenuComboBox::DZoomMenuComboBox(DWidget *parent)
    : DWidget(parent)
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
    qDebug() << "Added menu item:" << action->text();
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
        qWarning() << "Failed to remove item: Invalid index" << index;
        return;
    }
    removeItem(m_actions.at(index));
}

void DZoomMenuComboBox::removeItem(QAction *action)
{
    if (action == nullptr) {
        qWarning() << "Failed to remove item: Action is nullptr";
        return;
    }
    action->setParent(nullptr);
    m_menu->removeAction(action);
    m_actions.removeOne(action);

    if (m_currentIndex == m_actions.count()) {
        m_currentIndex--;
    }

    setCurrentIndex(m_currentIndex);
    qDebug() << "Removed menu item:" << action->text();
}

void DZoomMenuComboBox::setCurrentIndex(int index)
{
    if (index >= m_actions.count() || index < 0 || m_actions.count() < 0) {
        qWarning() << "Failed to set current index: Invalid index" << index << "with actions count" << m_actions.count();
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

    // 更改按钮显示
    setMenuButtonTextAndIcon(m_actions.at(m_currentIndex)->text(), m_actions.at(m_currentIndex)->icon());

    qDebug() << "Current index changed to:" << m_currentIndex << "with text:" << m_actions.at(m_currentIndex)->text();
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

void DZoomMenuComboBox::setMenuFlat(bool flat)
{
    m_btn->setFlat(flat);
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
        qWarning() << "Failed to set item icon: Invalid index" << index;
        return;
    }
    m_actions[index]->setIcon(icon);
    qDebug() << "Set icon for item at index:" << index;
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
    // DTK 在 5.6.4 后提供大小模式匹配，为避免可能的编译与运行时环境差异，提供根据控件高度变更的处理
#if DTK_VERSION_CHECK(5, 6, 4, 0) > DTK_VERSION_CHECK(DTK_VERSION_MAJOR, DTK_VERSION_MINOR, DTK_VERSION_PATCH, DTK_VERSION_BUILD)
    else if (o == m_btn) {
        if (QEvent::Resize == e->type()) {
            QResizeEvent *resizeEvent = dynamic_cast<QResizeEvent *>(e);
            if (resizeEvent) {
                setSizeMode(resizeEvent->size().height() <= COMPACT_TITLE_HEIGHT);
            }
        }
    }
#endif

    return DWidget::eventFilter(o, e);
}

/**
   @brief 设置当前尺寸模式，当 `isCompact` 为 true 时，标识为紧凑模式，
    调整图标和控件大小以更匹配界面布局.
 */
void DZoomMenuComboBox::setSizeMode(bool isCompact)
{
    if (isCompact == m_isCompact) {
        return;
    }
    m_isCompact = isCompact;
    qInfo() << "Size mode changed to:" << (isCompact ? "Compact" : "Normal");

    if (m_isCompact) {
        setFixedWidth(COMPACT_BOX_WITDH);
        m_btn->setFixedWidth(COMPACT_BOX_BTN_WIDTH);
        // 向右侧插入16px，居中展示内容，达到向左侧调整8px的目的
        // 此调整是在普通模式/紧凑模式下保持图标和缩放控件的间接为32px
        m_hlayout->setContentsMargins(0, 0, COMPACT_LEFT_OFFSET * 2, 0);

        m_floatingSize = COMPACT_FLOAT_BTN_SIZE;
        m_increaseBtn->setFixedSize(QSize(m_floatingSize, m_floatingSize));
        m_reduceBtn->setFixedSize(QSize(m_floatingSize, m_floatingSize));
        m_reduceBtn->setIconSize(QSize(COMPACT_ICON_SIZE, COMPACT_ICON_SIZE));
        m_increaseBtn->setIconSize(QSize(COMPACT_ICON_SIZE, COMPACT_ICON_SIZE));

        auto spacingItem = _btnLay->itemAt(SPACING_ITEM_IDX);
        if (spacingItem) {
            auto itemGeomertry = spacingItem->geometry();
            itemGeomertry.setWidth(m_btn->width() - 2 * m_floatingSize - 2 * 5);
            spacingItem->setGeometry(itemGeomertry);
        }
        _btnLay->setGeometry(QRect(COMPACT_FLOAT_X_OFFSET, COMPACT_FLOAT_Y_OFFSET, m_btn->width(), m_floatingSize));

    } else {
        setFixedWidth(BOX_WITDH);
        m_btn->setFixedWidth(BOX_BTN_WITDH);
        m_hlayout->setContentsMargins(0, 0, 0, 0);

        m_floatingSize = FLOAT_BTN_SIZE;
        m_increaseBtn->setFixedSize(QSize(m_floatingSize, m_floatingSize));
        m_reduceBtn->setFixedSize(QSize(m_floatingSize, m_floatingSize));
        m_reduceBtn->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
        m_increaseBtn->setIconSize(QSize(ICON_SIZE, ICON_SIZE));

        auto spacingItem = _btnLay->itemAt(SPACING_ITEM_IDX);
        if (spacingItem) {
            auto itemGeomertry = spacingItem->geometry();
            itemGeomertry.setWidth(m_btn->width() - 2 * m_floatingSize - 2 * 5);
            spacingItem->setGeometry(itemGeomertry);
        }
        _btnLay->setGeometry(QRect(FLOAT_X_OFFSET, FLOAT_Y_OFFSET, m_btn->width(), m_floatingSize));
    }

    update();
}

void DZoomMenuComboBox::initUI()
{
    setFixedWidth(BOX_WITDH);

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
    connect(m_btn, &QPushButton::clicked, this, [=]() {
        m_menu->exec(mapToGlobal(QPoint(0, this->geometry().y() + this->geometry().height())));
    });

    // [1] 左右加减按钮
    m_increaseBtn = new DIconButton(this);
    m_reduceBtn = new DIconButton(this);

    setWgtAccesibleName(m_increaseBtn, "Zoom increase button");
    setWgtAccesibleName(m_reduceBtn, "Zoom reduce button");

    m_increaseBtn->setIcon(QIcon::fromTheme("ddc_button_add_hover"));
    m_reduceBtn->setIcon(QIcon::fromTheme("ddc_button_reduce_hover"));

    m_increaseBtn->setFixedSize(QSize(m_floatingSize, m_floatingSize));
    m_reduceBtn->setFixedSize(QSize(m_floatingSize, m_floatingSize));
    m_reduceBtn->setBackgroundRole(QPalette::Button);
    m_increaseBtn->setBackgroundRole(QPalette::Button);
    m_reduceBtn->setIconSize(QSize(24, 24));
    m_increaseBtn->setIconSize(QSize(24, 24));

    m_increaseBtn->setEnabledCircle(true);
    m_reduceBtn->setEnabledCircle(true);

    connect(m_reduceBtn, &DFloatingButton::clicked, this, [=]() { emit signalLeftBtnClicked(); });
    connect(m_increaseBtn, &DFloatingButton::clicked, this, [=]() { emit signalRightBtnClicked(); });

    m_hlayout = new QHBoxLayout(this);
    m_hlayout->setSpacing(0);
    m_hlayout->addWidget(m_btn, 0, Qt::AlignVCenter);
    this->setLayout(m_hlayout);

    _btnLay = new QHBoxLayout();
    _btnLay->addWidget(m_reduceBtn, 0, Qt::AlignVCenter);
    _btnLay->addSpacing(m_btn->width() - 2 * m_floatingSize - 2 * 5);
    _btnLay->addWidget(m_increaseBtn, 0, Qt::AlignVCenter);
    // 设置左右按钮的位置，需要悬浮于菜单按钮的上面
    _btnLay->setGeometry(QRect(m_btn->x() + X_OFFSET, m_btn->y() + Y_OFFSET, m_btn->width(), m_btn->height()));

    // DTK 在 5.6.4 后提供大小模式匹配
#if DTK_VERSION_CHECK(5, 6, 4, 0) <= DTK_VERSION_CHECK(DTK_VERSION_MAJOR, DTK_VERSION_MINOR, DTK_VERSION_PATCH, DTK_VERSION_BUILD)
    int checkVersion = DTK_VERSION_CHECK(5, 6, 4, 0);
    if (checkVersion <= DApplication::runtimeDtkVersion()) {
        // 初始化设置
        if (DGuiApplicationHelper::isCompactMode()) {
            setSizeMode(true);
        }

        connect(
            DGuiApplicationHelper::instance(),
            &DGuiApplicationHelper::sizeModeChanged,
            this,
            [this](DGuiApplicationHelper::SizeMode sizeMode) { setSizeMode(DGuiApplicationHelper::CompactMode == sizeMode); });
    }
#endif
}

void DZoomMenuComboBox::initConnection()
{
    // 连接子选项按钮菜单被点击信号
    connect(m_menu, &QMenu::triggered, this, &DZoomMenuComboBox::slotActionToggled);
    connect(m_menu, &QMenu::aboutToHide, this, [=]() { this->setFocus(); });
}
