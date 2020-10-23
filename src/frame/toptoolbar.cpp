/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "toptoolbar.h"
#include "application.h"
#include "widgets/dialog/drawdialog.h"
#include "drawshape/cdrawparamsigleton.h"
#include "widgets/csvglabel.h"
#include "widgets/cmenu.h"
#include "widgets/dzoommenucombobox.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "service/cmanagerattributeservice.h"
#include "citemattriwidget.h"

#include <DComboBox>
#include <DApplication>
#include <QHBoxLayout>
#include <QString>
#include <QStandardItemModel>
#include <DApplicationHelper>
#include <QWidgetAction>
#include <QTimer>

#include <DLineEdit>

const int Text_Size = 14;

TopToolbar::TopToolbar(DWidget *parent)
    : DFrame(parent)
{
    initUI();
}

TopToolbar::~TopToolbar()
{

}

void TopToolbar::initUI()
{
    ft.setPixelSize(Text_Size);

    // 初始化缩放菜单
    initComboBox();
    initMenu();

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    hLayout->setMargin(0);
    hLayout->setSpacing(0);

    hLayout->addWidget(m_zoomMenuComboBox);

    m_pAtrriWidget = new CComAttrWidget(this);
    m_pAtrriWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    hLayout->addWidget(m_pAtrriWidget);

    hLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(hLayout);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void TopToolbar::initComboBox()
{
    m_zoomMenuComboBox = new DZoomMenuComboBox(this);
    m_zoomMenuComboBox->setFont(ft);
    m_zoomMenuComboBox->setMenuFlat(false);
    m_zoomMenuComboBox->setFixedWidth(162);
    m_zoomMenuComboBox->addItem("200%");
    m_zoomMenuComboBox->addItem("100%");
    m_zoomMenuComboBox->addItem("75%");
    m_zoomMenuComboBox->addItem("50%");
    m_zoomMenuComboBox->addItem("25%");
    m_zoomMenuComboBox->setObjectName("zoomMenuComboBox");
    connect(m_zoomMenuComboBox, &DZoomMenuComboBox::signalCurrentTextChanged, this, [ = ](QString item) {
        slotZoom(item);
    });
    // 初始化大小为 100%
    m_zoomMenuComboBox->setCurrentText("100%");

    // 放大缩小范围10%-2000% ，点击放大缩小，如区间在200%-2000%，则每次点+/-100%；如区间在10%-199%，则每次点击+/-10%
    // 左侧按钮点击信号 (-)
    connect(m_zoomMenuComboBox, &DZoomMenuComboBox::signalLeftBtnClicked, this, [ = ]() {
        qreal current_scale = CManageViewSigleton::GetInstance()->getCurView()->getScale();
        if (current_scale >= 2.0 && current_scale <= 20.0) {
            current_scale -= 1.0;
        } else if (current_scale >= 0.1 && current_scale <= 1.999) {
            current_scale -= 0.1;
        }
        if (current_scale <= 0.1) {
            current_scale = 0.1;
        }
        slotZoom(current_scale);
    });
    // 右侧按钮点击信号 (+)
    connect(m_zoomMenuComboBox, &DZoomMenuComboBox::signalRightBtnClicked, this, [ = ]() {
        qreal current_scale = CManageViewSigleton::GetInstance()->getCurView()->getScale();
        if (current_scale >= 2.0 && current_scale <= 20.0) {
            current_scale += 1.0;
        } else if (current_scale >= 0.1 && current_scale <= 1.999) {
            current_scale += 0.1;
        }
        if (current_scale >= 20.0) {
            current_scale = 20.0;
        }
        slotZoom(current_scale);
    });
}

void TopToolbar::initMenu()
{
    m_mainMenu = new CMenu(this);
//    m_mainMenu->setFixedWidth(162);

    m_newAction = new QAction(tr("New"), this);
    m_newAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
    m_mainMenu->addAction(m_newAction);
    this->addAction(m_newAction);

    QAction *importAc = new QAction(tr("Open"), this);
    importAc->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
    m_mainMenu->addAction(importAc);
    this->addAction(importAc);
    m_mainMenu->addSeparator();

    QAction *exportAc = new QAction(tr("Export"), this);
    exportAc->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E));
    m_mainMenu->addAction(exportAc);
    this->addAction(exportAc);

    m_saveAction = new QAction(tr("Save"), this);
    m_saveAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
    m_mainMenu->addAction(m_saveAction);
    this->addAction(m_saveAction);

    QAction *saveAsAc = new QAction(tr("Save as"), this);
    saveAsAc->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));
    m_mainMenu->addAction(saveAsAc);
    this->addAction(saveAsAc);

    QAction *printAc = new QAction(tr("Print"), this);
    printAc->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_P));
    m_mainMenu->addAction(printAc);
    this->addAction(printAc);
    m_mainMenu->addSeparator();

    QIcon t_icon;

    t_icon = QIcon::fromTheme("deepin-draw");
    dApp->setProductIcon(t_icon);
    dApp->setProductName(tr("Draw"));

    //画板是一款轻量级的绘图工具，支持在画板上自由绘图和简单的图片编辑。
    dApp->setApplicationDescription(tr("Draw is a lightweight drawing tool for users to freely draw and simply edit images. "));
    dApp->setApplicationAcknowledgementPage("https://www.deepin.org/acknowledgments/deepin-draw/");

    connect(importAc, &QAction::triggered, this, &TopToolbar::slotOnImportAction);
    connect(m_saveAction, &QAction::triggered, this, &TopToolbar::slotOnSaveAction);
    connect(saveAsAc, &QAction::triggered, this, &TopToolbar::slotOnSaveAsAction);
    connect(printAc, &QAction::triggered, this, &TopToolbar::signalPrint);
    connect(exportAc, &QAction::triggered, this, &TopToolbar::signalShowExportDialog);
    connect(m_newAction, &QAction::triggered, this, &TopToolbar::slotOnNewConstructAction);

    connect(m_mainMenu, &DMenu::triggered, this, &TopToolbar::slotIsCutMode);
    connect(m_mainMenu, &DMenu::aboutToShow, this, &TopToolbar::slotMenuShow);
}

void TopToolbar::slotZoom(const QString &scale)
{
    qreal fScale = 0.0;

    QString scale_num_str = scale;
    scale_num_str = scale_num_str.replace("%", "");

    int scale_num = 1;
    bool flag = false;

    scale_num = scale_num_str.toInt(&flag);

    if (flag) {
        fScale = scale_num / 100.0;
    } else {
        fScale = 1.0;
    }
    slotZoom(fScale);
}

void TopToolbar::slotZoom(const qreal &scale)
{
    emit signalZoom(scale);

    // 更新当前缩放的比例
    slotSetScale(scale);
}

void TopToolbar::slotSetScale(const qreal scale)
{
    QString strScale = QString::number(qRound(scale * 100)) + "%";
    m_zoomMenuComboBox->setMenuButtonTextAndIcon(strScale, QIcon());
}

void TopToolbar::slotIsCutMode(QAction *action)
{
    Q_UNUSED(action)
    if (cut == CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode()) {
    }
}

void TopToolbar::slotOnImportAction()
{
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::LoadDDF);
    emit signalImport();
}

void TopToolbar::slotOnNewConstructAction()
{

    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::NewDrawingBoard);
    emit signalNew();
}

void TopToolbar::slotOnSaveAction()
{
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::SaveAction);
    emit signalSaveToDDF();
}

void TopToolbar::slotOnSaveAsAction()
{
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::SaveAction);
    emit signalSaveAs();
}

void TopToolbar::slotMenuShow()
{
}

DMenu *TopToolbar::mainMenu()
{
    return m_mainMenu;
}

CComAttrWidget *TopToolbar::attributWidget()
{
    return m_pAtrriWidget;
}

void TopToolbar::resizeEvent(QResizeEvent *event)
{
    this->updateGeometry();
    QWidget::resizeEvent(event);
}

void TopToolbar::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    dApp->setApplicationCursor(Qt::ArrowCursor);
    DFrame::enterEvent(event);
}
