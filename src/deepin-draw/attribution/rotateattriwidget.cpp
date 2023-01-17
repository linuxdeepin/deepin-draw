#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <DLabel>
#include <QtMath>
#include <QGraphicsDropShadowEffect>

#include "rotateattriwidget.h"
#include "drawboard.h"
#include "selecttool.h"
#include "pagescene.h"
#include "boxlayoutwidget.h"

RotateAttriWidget::RotateAttriWidget(DrawBoard *drawBoard, QWidget *parent): AttributeWgt(ERotProperty, parent)
    , m_drawBoard(drawBoard)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QLabel *titleLabel = new QLabel(tr("Rotate"), this);
    mainLayout->addWidget(titleLabel);
    QHBoxLayout *rotateWidets = new QHBoxLayout;

    m_label = new QLabel(this);
    QPixmap pix;
    //需要增加路径
    pix.load(QString(":"));
    m_label->setPixmap(pix);

    m_angle = new DDoubleSpinBox();
    m_angle->setSuffix("°");
    m_angle->setRange(-999, 999);
    m_angle->setMinimumWidth(100);
    m_angle->setSingleStep(1);
    m_angle->setDecimals(1);
    //m_angle->setToolTip("Please enter a value between -360 and 360");
    m_angle->setEnabledEmbedStyle(true);

    m_horFlipBtn = new DIconButton(nullptr);
    m_horFlipBtn->setObjectName("PicFlipHBtn");
    m_horFlipBtn->setMaximumHeight(38);
    m_horFlipBtn->setIcon(QIcon::fromTheme("ddc_flip horizontal_normal"));
    m_horFlipBtn->setIconSize(QSize(48, 48));
    m_horFlipBtn->setMinimumWidth(48);
    m_horFlipBtn->setToolTip(tr("Flip horizontally"));
    m_horFlipBtn->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);

    m_verFlipBtn = new DIconButton(nullptr);
    m_verFlipBtn->setObjectName("PicFlipVBtn");
    m_verFlipBtn->setMaximumHeight(38);
    m_verFlipBtn->setMinimumWidth(48);
    m_verFlipBtn->setIcon(QIcon::fromTheme("ddc_flip vertical_normal"));
    m_verFlipBtn->setIconSize(QSize(48, 48));
    m_verFlipBtn->setToolTip(tr("Flip vertically"));
    m_verFlipBtn->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);

    m_clockwiseBtn = new DIconButton(nullptr);
    m_clockwiseBtn->setIcon(QIcon::fromTheme("menu_group_normal"));
    m_anticlockwiseBtn = new DIconButton(nullptr);
    m_anticlockwiseBtn->setIcon(QIcon::fromTheme("menu_group_normal"));

    BoxLayoutWidget *layoutWidget = new BoxLayoutWidget(Qt::white, this);
    layoutWidget->addWidget(m_horFlipBtn);
    layoutWidget->addWidget(m_verFlipBtn);

    rotateWidets->addWidget(m_angle, 1);
    rotateWidets->addWidget(layoutWidget, 1);
    mainLayout->addLayout(rotateWidets);
    mainLayout->setContentsMargins(0, 10, 10, 0);
    setLayout(mainLayout);

    connect(m_angle, &QDoubleSpinBox::editingFinished, this, [&] {
        double value = checkValue(m_angle->value());
        QSignalBlocker block(m_angle);
        m_angle->setValue(value);
        m_drawBoard->currentPage()->scene()->rotateSelectItems(value);
    });

    connect(m_horFlipBtn, &DIconButton::clicked, this, [ = ] {
        for (auto i : m_drawBoard->currentPage()->scene()->selectedPageItems())
        {
            i->doFilp(PageItem::EFilpHor);
        }
    });

    connect(m_verFlipBtn, &DIconButton::clicked, this, [ = ] {
        for (auto i : m_drawBoard->currentPage()->scene()->selectedPageItems())
        {
            i->doFilp(PageItem::EFilpVer);
        }
    });

    connect(m_drawBoard, qOverload<Page *>(&DrawBoard::currentPageChanged), this, [ = ](Page * newPage) {
        auto newScene = (newPage == nullptr ? nullptr : newPage->scene());

        if (newScene != m_currentScene) {
            if (m_currentScene != nullptr) {
                disconnect(m_currentScene, &PageScene::selectionChanged, this, &RotateAttriWidget::onSceneSelectionChanged);
            }

            m_currentScene = newScene;

            if (m_currentScene != nullptr) {
                connect(m_currentScene, &PageScene::selectionChanged, this, &RotateAttriWidget::onSceneSelectionChanged);
                onSceneSelectionChanged(m_currentScene->selectedPageItems());
            }
        }
    });
}

void RotateAttriWidget::setVar(const QVariant &var)
{
    m_angle->setValue(var.toDouble());
}

void RotateAttriWidget::setAngle(double angle)
{
    m_angle->setValue(angle);
}

void RotateAttriWidget::showTooltip()
{
    static QWidget *w = nullptr;
    if (nullptr == w) {
        w = new QWidget;
        w->setWindowFlag(Qt::Popup);
        w->setWindowFlag(Qt::FramelessWindowHint);
        w->setMouseTracking(true);
        w->setFocusPolicy(Qt::NoFocus);
        w->setAttribute(Qt::WA_TranslucentBackground);
        w->setMinimumWidth(180);
        QLabel *lab = new QLabel(tr("Please enter a value between -360 and 360"), w);
        lab->setWordWrap(true);
        QVBoxLayout *l = new QVBoxLayout;
        l->addWidget(lab);
        w->setLayout(l);
    }

    QPoint wPos = mapToGlobal(m_angle->pos());
    wPos.setY(wPos.y() + m_angle->height());
    w->move(wPos);
    w->raise();
    w->show();

}

double RotateAttriWidget::checkValue(double value)
{
    static const double minAngle = -359.9;
    static const double maxAngle = 359.9;
    static const int angle360 = 360;
    double rt = value;
    if (qFloor(abs(value)) >= angle360) {
        showTooltip();
        QSignalBlocker block(m_angle);
        if (qFloor(abs(value)) == angle360) {
            rt = 0;
        } else if (value < minAngle) {
            rt = minAngle;
        } else if (value > maxAngle) {
            rt = maxAngle;
        }

    }

    return rt;
}

void RotateAttriWidget::onSceneSelectionChanged(const QList<PageItem *> &selectedItems)
{
    if (selectedItems.count() == 1) {
        m_verFlipBtn->setEnabled(true);
        m_horFlipBtn->setEnabled(true);
        m_horFlipBtn->setFlat(false);
        m_verFlipBtn->setFlat(false);
    } else {
        m_verFlipBtn->setEnabled(false);
        m_horFlipBtn->setEnabled(false);
        m_horFlipBtn->setFlat(true);
        m_verFlipBtn->setFlat(true);
    }
}
