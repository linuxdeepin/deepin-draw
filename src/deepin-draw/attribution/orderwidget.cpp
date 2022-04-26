#include <QHBoxLayout>
#include <QVBoxLayout>
#include <DLabel>

#include "orderwidget.h"
#include "boxlayoutwidget.h"
#include "drawboard.h"
#include "pageview.h"
#include "pagescene.h"

OrderWidget::OrderWidget(DrawBoard *drawBoard, QWidget *parent): AttributeWgt(EOrderProperty, parent), m_drawBoard(drawBoard)
{
    initUi();
    initConnect();
}

void OrderWidget::initUi()
{
    DLabel *l = new DLabel(tr("Order"), this);
    m_oneLayerUp = createIconButton("up_on_layer", tr("Raise layer"), "Raise_layer");
    m_oneLayerDown = createIconButton("down_one_layer", tr("Lower layer"), "Lower_layer");
    m_bringToFront = createIconButton("top_layer", tr("Layer to Top"), "Layer_to_Top");
    m_sendToback = createIconButton("botton_layer", tr("Layer to Bottom"), "Layer_to_Bottom");

    m_LeftAlign = createIconButton("left_alignment", tr("Align left"), "Align_left");
    m_HCenterAlign = createIconButton("horizontal_center_alignment", tr("Align middle"), "Align_middle");
    m_RightAlign = createIconButton("right_alignment", tr("Align right"), "Align_right");
    m_TopAlign = createIconButton("top_alignment", tr("Align top"), "Align_top");
    m_VCenterAlign = createIconButton("center_alignmetn", tr("Align center"), "Align_center");
    m_BottomAlign = createIconButton("bottom_alignment", tr("Align bottom"), "Align_bottom");

    m_HEqulSpaceAlign = createIconButton("vertical_ equally_ spaced_alignment", tr("Distribute horizontally"), "Distribute_horizontally");
    m_VEqulSpaceAlign = createIconButton("horizontal_equally_spaced_alignment", tr("Distribute vertically"), "Distribute_vertically");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 10, 0);
    mainLayout->addWidget(l);

    QHBoxLayout *hLayer = new QHBoxLayout;
    BoxLayoutWidget *layerWidget = new BoxLayoutWidget(this);
    layerWidget->addWidget(m_oneLayerUp);
    layerWidget->addWidget(m_oneLayerDown);
    layerWidget->addWidget(m_bringToFront);
    layerWidget->addWidget(m_sendToback);
    BoxLayoutWidget *orderWidget = new BoxLayoutWidget(this);
    orderWidget->addWidget(m_VEqulSpaceAlign);
    orderWidget->addWidget(m_HEqulSpaceAlign);
    hLayer->addWidget(layerWidget, 2);
    hLayer->addWidget(orderWidget, 1);
    BoxLayoutWidget *aligWidget = new BoxLayoutWidget(this);
    aligWidget->addWidget(m_TopAlign);
    aligWidget->addWidget(m_VCenterAlign);
    aligWidget->addWidget(m_BottomAlign);
    aligWidget->addWidget(m_LeftAlign);
    aligWidget->addWidget(m_HCenterAlign);
    aligWidget->addWidget(m_RightAlign);

    mainLayout->addLayout(hLayer);
    mainLayout->addWidget(aligWidget);
    setLayout(mainLayout);
}

void OrderWidget::initConnect()
{
    connect(m_drawBoard, qOverload<Page *>(&DrawBoard::currentPageChanged), this, [ = ](Page * p) {
        if (nullptr != p) {
            if (nullptr != m_currentScene) {
                disconnect(m_currentScene);
            }

            m_currentScene = p->scene();
            connect(m_currentScene, &PageScene::selectionChanged, this, [ = ] {
                updateButtonsStatus();
            });
            updateButtonsStatus();
        }
    });

    connect(m_oneLayerUp, &DIconButton::clicked, this, [ = ] {
        auto scene = m_drawBoard->currentPage()->scene();
        if (scene == nullptr)
        {
            return;
        }
        scene->movePageItemsZValue(scene->selectedPageItems(), UpItemZ, 1, true);
        updateLayerButtonStatus();
    });

    connect(m_oneLayerDown, &DIconButton::clicked, this, [ = ] {
        auto scene = m_drawBoard->currentPage()->scene();
        if (scene == nullptr)
        {
            return;
        }
        scene->movePageItemsZValue(scene->selectedPageItems(), DownItemZ, 1, true);
        updateLayerButtonStatus();
    });

    connect(m_bringToFront, &DIconButton::clicked, this, [ = ] {
        auto scene = m_drawBoard->currentPage()->scene();
        if (scene == nullptr)
        {
            return;
        }
        scene->movePageItemsZValue(scene->selectedPageItems(), UpItemZ, -1, true);
        updateLayerButtonStatus();
    });

    connect(m_sendToback, &DIconButton::clicked, this, [ = ] {
        auto scene = m_drawBoard->currentPage()->scene();
        if (scene == nullptr)
        {
            return;
        }
        scene->movePageItemsZValue(scene->selectedPageItems(), DownItemZ, -1, true);
    });

    connect(m_LeftAlign, &DIconButton::clicked, this, [ = ] {
        doAlignment(Qt::AlignLeft);
    });

    connect(m_HCenterAlign, &DIconButton::clicked, this, [ = ] {
        doAlignment(Qt::AlignHCenter);
    });

    connect(m_RightAlign, &DIconButton::clicked, this, [ = ] {
        doAlignment(Qt::AlignRight);
    });

    connect(m_TopAlign, &DIconButton::clicked, this, [ = ] {
        doAlignment(Qt::AlignTop);
    });
    connect(m_VCenterAlign, &DIconButton::clicked, this, [ = ] {
        doAlignment(Qt::AlignVCenter);
    });

    connect(m_BottomAlign, &DIconButton::clicked, this, [ = ] {
        doAlignment(Qt::AlignBottom);
    });

    connect(m_HEqulSpaceAlign, &DIconButton::clicked, this, [ = ] {
        auto view = m_drawBoard->currentPage()->view();
        if (view == nullptr)
        {
            return;
        }
        view->itemsEqulSpaceAlignment(true);
        updateAlignButtonStatus();
    });

    connect(m_VEqulSpaceAlign, &DIconButton::clicked, this, [ = ] {
        auto view = m_drawBoard->currentPage()->view();
        if (view == nullptr)
        {
            return;
        }
        view->itemsEqulSpaceAlignment(true);
        updateAlignButtonStatus();
    });
}

DIconButton *OrderWidget::createIconButton(const QString &icon, const QString &toolTip, const QString &accesibleName)
{
    DIconButton *btn = new DIconButton(nullptr);
    btn->setIcon(QIcon::fromTheme(icon));
    btn->setToolTip(toolTip);
    setWgtAccesibleName(btn, accesibleName);
    btn->setMinimumSize(38, 38);
    btn->setIconSize(QSize(16, 16));
    btn->setFocusPolicy(Qt::NoFocus);
    btn->setFlat(true);
    return btn;
}

void OrderWidget::doAlignment(Qt::AlignmentFlag align)
{
    auto view = m_drawBoard->currentPage()->view();
    if (view == nullptr) {
        return;
    }
    view->alignmentSelectItmes(align);
    updateAlignButtonStatus();
}

void OrderWidget::updateAlignButtonStatus()
{
    int selectedCount = m_currentScene->selectedItemCount();
    bool acticonvistual = (selectedCount > 1);

    m_LeftAlign->setEnabled(acticonvistual);
    m_HCenterAlign->setEnabled(acticonvistual);
    m_RightAlign->setEnabled(acticonvistual);
    m_TopAlign->setEnabled(acticonvistual);
    m_VCenterAlign->setEnabled(acticonvistual);
    m_BottomAlign->setEnabled(acticonvistual);

    m_LeftAlign->setFlat(acticonvistual);
    m_HCenterAlign->setFlat(acticonvistual);
    m_RightAlign->setFlat(acticonvistual);
    m_TopAlign->setFlat(acticonvistual);
    m_VCenterAlign->setFlat(acticonvistual);
    m_BottomAlign->setFlat(acticonvistual);


    m_HEqulSpaceAlign->setEnabled(selectedCount >= 3);
    m_VEqulSpaceAlign->setEnabled(selectedCount >= 3);
    m_HEqulSpaceAlign->setFlat(selectedCount >= 3);
    m_VEqulSpaceAlign->setFlat(selectedCount >= 3);
}

void OrderWidget::updateLayerButtonStatus()
{
    auto view = m_drawBoard->currentPage()->view();
    if (view == nullptr) {
        return;
    }
    auto scene = view->pageScene();

    bool layerUp = PageScene::isItemsZMovable(scene->selectedPageItems(), UpItemZ);
    m_oneLayerUp->setEnabled(layerUp);
    m_bringToFront->setEnabled(layerUp);

    bool layerDown = PageScene::isItemsZMovable(scene->selectedPageItems(), DownItemZ);
    m_oneLayerDown->setEnabled(layerDown);
    m_sendToback->setEnabled(layerDown);
}

void OrderWidget::updateButtonsStatus()
{
    updateAlignButtonStatus();
    updateLayerButtonStatus();
}
