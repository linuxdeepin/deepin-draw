#include "polygonalstarattributewidget.h"

#include <DLabel>
//#include <DFontSizeManager>

#include <QHBoxLayout>
#include <QButtonGroup>
#include <QIntValidator>
#include <QDebug>

#include "widgets/toolbutton.h"
#include "widgets/bigcolorbutton.h"
#include "widgets/bordercolorbutton.h"
#include "widgets/seperatorline.h"
#include "widgets/csidewidthwidget.h"
#include "drawshape/cdrawparamsigleton.h"

const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;

PolygonalStarAttributeWidget::PolygonalStarAttributeWidget(DWidget *parent)
    : DWidget(parent)
    , m_isUsrDragSlider(false)
{
    initUI();
    initConnection();
}

PolygonalStarAttributeWidget::~PolygonalStarAttributeWidget()
{

}

void PolygonalStarAttributeWidget::initUI()
{
    //    DFontSizeManager::instance()->bind(this, DFontSizeManager::T1);

    m_fillBtn = new BigColorButton( this);

    DLabel *fillLabel = new DLabel(this);
    fillLabel->setText(tr("填充"));

    m_strokeBtn = new BorderColorButton(this);

    DLabel *strokeLabel = new DLabel(this);
    strokeLabel->setText(tr("描边"));

    SeperatorLine *sepLine = new SeperatorLine(this);
    DLabel *lwLabel = new DLabel(this);
    lwLabel->setText(tr("描边粗细"));

    m_sideWidthWidget = new CSideWidthWidget(this);


    DLabel *anchorNumLabel = new DLabel(this);
    anchorNumLabel->setText(tr("锚点数"));

    m_anchorNumSlider = new DSlider(this);
    m_anchorNumSlider->setOrientation(Qt::Horizontal);
    m_anchorNumSlider->setMinimum(3);
    m_anchorNumSlider->setMaximum(50);
    m_anchorNumSlider->setMinimumWidth(200);
    m_anchorNumSlider->setMaximumHeight(24);

    m_anchorNumEdit = new DLineEdit(this);
    m_anchorNumEdit->setValidator(new QRegExpValidator(QRegExp("^(([3-9]{1})|(^[1-4]{1}[0-9]{1}$)|(50))$"), this));
    m_anchorNumEdit->setClearButtonEnabled(false);
    m_anchorNumEdit->setFixedWidth(40);
    m_anchorNumEdit->setText(QString::number(m_anchorNumSlider->value()));

    DLabel *radiusLabel = new DLabel(this);
    radiusLabel->setText(tr("半径"));

    m_radiusNumSlider = new DSlider(this);
    m_radiusNumSlider->setOrientation(Qt::Horizontal);
    m_radiusNumSlider->setMinimum(0);
    m_radiusNumSlider->setMaximum(100);
    m_radiusNumSlider->setMinimumWidth(200);
    m_radiusNumSlider->setMaximumHeight(24);

    m_radiusNumEdit = new DLineEdit(this);
    QRegExp rx("^([1-9]{1}[0-9]{0,1}|0|100){0,1}%$");
    QRegExpValidator *validator = new QRegExpValidator(rx, this);
    m_radiusNumEdit->setValidator(validator);
    m_radiusNumEdit->setClearButtonEnabled(false);
    m_radiusNumEdit->setFixedWidth(55);
    m_radiusNumEdit->setText(QString("%1%").arg(m_radiusNumSlider->value()));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(BTN_SPACING);
    layout->addStretch();
    layout->addWidget(m_fillBtn);
    layout->addWidget(fillLabel);
    layout->addWidget(m_strokeBtn);
    layout->addWidget(strokeLabel);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(sepLine);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(lwLabel);
    layout->addWidget(m_sideWidthWidget);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(anchorNumLabel);
    layout->addWidget(m_anchorNumSlider);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_anchorNumEdit);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(radiusLabel);
    layout->addWidget(m_radiusNumSlider);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_radiusNumEdit);

    layout->addStretch();
    setLayout(layout);
}

void PolygonalStarAttributeWidget::initConnection()
{

    connect(m_fillBtn, &BigColorButton::btnCheckStateChanged, this, [ = ](bool show) {
        m_strokeBtn->resetChecked();
        emit showColorPanel(DrawStatus::Fill, getBtnPosition(m_fillBtn), show);

    });
    connect(m_strokeBtn, &BorderColorButton::btnCheckStateChanged, this, [ = ](bool show) {
        m_fillBtn->resetChecked();
        emit showColorPanel(DrawStatus::Stroke,  getBtnPosition(m_strokeBtn), show);
    });

    connect(this, &PolygonalStarAttributeWidget::resetColorBtns, this, [ = ] {
        m_fillBtn->resetChecked();
        m_strokeBtn->resetChecked();
    });


    ///线宽
    connect(m_sideWidthWidget, &CSideWidthWidget::signalSideWidthChange, this, [ = ] () {
        emit signalPolygonalStarAttributeChanged();
    });

    ///锚点数
    connect(m_anchorNumSlider, &DSlider::valueChanged, this, [ = ](int value) {
        if (m_isUsrDragSlider) {
            m_anchorNumEdit->setText(QString::number(value));
            CDrawParamSigleton::GetInstance()->setAnchorNum(value);
            emit signalPolygonalStarAttributeChanged();
        }
    });

    connect(m_anchorNumEdit, &DLineEdit::textEdited, this, [ = ](const QString & str) {
        if (str.isEmpty() || str == "") {
            return ;
        }
        int value = str.trimmed().toInt();
        if (value >= 1 && value <= 2) {
            return;
        }
        m_anchorNumSlider->setValue(value);
        CDrawParamSigleton::GetInstance()->setAnchorNum(value);
        emit signalPolygonalStarAttributeChanged();
    });

    connect(m_anchorNumSlider, &DSlider::sliderPressed, this, [ = ]() {
        m_isUsrDragSlider = true;
    });

    connect(m_anchorNumSlider, &DSlider::sliderReleased, this, [ = ]() {
        m_isUsrDragSlider = false;
    });

    ///半径
    connect(m_radiusNumSlider, &DSlider::valueChanged, this, [ = ](int value) {
        if (m_isUsrDragSlider) {
            m_radiusNumEdit->setText(QString("%1%").arg(value));
            CDrawParamSigleton::GetInstance()->setRadiusNum(value);
            emit signalPolygonalStarAttributeChanged();
        }
    });

    connect(m_radiusNumEdit, &DLineEdit::textEdited, this, [ = ](const QString & str) {
        if (str.isEmpty() || str == "") {
            return ;
        }

        QString tmpStr = "";
        if (str.contains("%")) {
            tmpStr = str.split("%").first();
        }
        int value = tmpStr.trimmed().toInt();

        if (value < 0 || value > 100) {
            return ;
        }

        m_radiusNumSlider->setValue(value);
        CDrawParamSigleton::GetInstance()->setRadiusNum(value);
        emit signalPolygonalStarAttributeChanged();
    });

    connect(m_radiusNumSlider, &DSlider::sliderPressed, this, [ = ]() {
        m_isUsrDragSlider = true;
    });

    connect(m_radiusNumSlider, &DSlider::sliderReleased, this, [ = ]() {
        m_isUsrDragSlider = false;
    });
}

void PolygonalStarAttributeWidget::updatePolygonalStarWidget()
{
    m_fillBtn->updateConfigColor();
    m_strokeBtn->updateConfigColor();
    m_sideWidthWidget->updateSideWidth();

    int anchorNum = CDrawParamSigleton::GetInstance()->getAnchorNum();

    if (anchorNum != m_anchorNumSlider->value()) {
        m_anchorNumSlider->setValue(anchorNum);
        m_anchorNumEdit->setText(QString("%1").arg(anchorNum));
    }

    int radiusNum = CDrawParamSigleton::GetInstance()->getRadiusNum();

    if (radiusNum != m_anchorNumSlider->value()) {
        m_radiusNumSlider->setValue(radiusNum);
        m_radiusNumEdit->setText(QString("%1%").arg(radiusNum));
    }
}

QPoint PolygonalStarAttributeWidget::getBtnPosition(const DPushButton *btn)
{
    QPoint btnPos = mapToGlobal(btn->pos());
    QPoint pos(btnPos.x() + btn->width() / 2,
               btnPos.y() + btn->height());

    return pos;
}
