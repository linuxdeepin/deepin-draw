#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <DLabel>
#include <QtMath>
#include <QTimer>
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

    m_horFlipBtn = new DToolButton(nullptr);
    m_horFlipBtn->setObjectName("PicFlipHBtn");
    m_horFlipBtn->setMaximumHeight(38);
    m_horFlipBtn->setIcon(QIcon::fromTheme("ddc_flip horizontal_normal"));
    m_horFlipBtn->setIconSize(QSize(48, 48));
    m_horFlipBtn->setMinimumWidth(48);
    m_horFlipBtn->setToolTip(tr("Flip horizontally"));
    m_horFlipBtn->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);

    m_verFlipBtn = new DToolButton(nullptr);
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

    // 修改为每次数据更新后图元立即调整
    connect(m_angle, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [&](double v) {
        double value = checkValue(v);
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

/**
 * @brief 由于外部原因，图元属性变更时，通过属性管理类更新属性控件展示的属性值
 * @param var 更新的属性值
 */
void RotateAttriWidget::setVar(const QVariant &var)
{
    // 来自外部界面更新的数据不通过信号进行二次更新
    QSignalBlocker blocker(m_angle);
    m_angle->setValue(var.toDouble());
}

void RotateAttriWidget::setAngle(double angle)
{
    m_angle->setValue(angle);
}

/**
 * @brief 显示角度提示信息控件，当用户输入的角度值超出边界时显示，
 *      2s后隐藏界面
 */
void RotateAttriWidget::showTooltip()
{
    static QWidget *w = nullptr;
    if (nullptr == w) {
        w = new QWidget;
        // 展示在顶层
        w->setWindowFlag(Qt::ToolTip);
        w->setWindowFlag(Qt::BypassGraphicsProxyWidget);
        w->setForegroundRole(QPalette::ToolTipText);
        w->setBackgroundRole(QPalette::ToolTipBase);
        w->setMouseTracking(true);
        w->setFocusPolicy(Qt::NoFocus);
        w->setAttribute(Qt::WA_TranslucentBackground);
        w->setMinimumWidth(180);

        QLabel *lab = new QLabel(tr("Please enter a value between -360 and 360"), w);
        lab->setWordWrap(true);
        QVBoxLayout *l = new QVBoxLayout;
        l->addWidget(lab);
        w->setLayout(l);
        // 初始化控件大小
        w->resize(w->sizeHint());

        // 超时后隐藏提示信息
        m_delayHideTimer = new QTimer(this);
        connect(m_delayHideTimer, &QTimer::timeout, this, [ & ]() {
            w->hide();
        });
    }

    QPoint wPos = mapToGlobal(m_angle->pos());
    // 防止提示信息显示不全，将提示框向左移动
    if (this->width() < w->width())
        wPos.rx() -= w->width() - this->width();
    wPos.setY(wPos.y() + m_angle->height());
    w->move(wPos);
    w->raise();
    w->show();

    // 超时2s后隐藏隐藏提示信息
    static const int delayTime = 2000;
    m_delayHideTimer->start(delayTime);
}

/**
 * @brief 对输入的角度值 \a value 进行判断，角度仅允许显示在 -359.9 ~ 359.9 度，
 *      超过范围时会弹出提示信息，并调整角度在合法范围内。
 * @param value 输入的角度值
 * @return 调整后的合法角度值
 */
double RotateAttriWidget::checkValue(double value)
{
    static const double minAngle = -359.9;
    static const double maxAngle = 359.9;
    static const int angle360 = 360;
    double rt = value;
    if (qFloor(abs(value)) >= angle360) {
        showTooltip();
        QSignalBlocker block(m_angle);
        if (value < minAngle) {
            rt = minAngle;
        } else if (value > maxAngle) {
            rt = maxAngle;
        } else {}
    }

    return rt;
}

void RotateAttriWidget::onSceneSelectionChanged(const QList<PageItem *> &selectedItems)
{
    if (selectedItems.count() == 1) {
        m_verFlipBtn->setEnabled(true);
        m_horFlipBtn->setEnabled(true);
    } else {
        m_verFlipBtn->setEnabled(false);
        m_horFlipBtn->setEnabled(false);
    }
}
