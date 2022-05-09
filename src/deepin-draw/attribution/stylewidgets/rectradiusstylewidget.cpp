#include "rectradiusstylewidget.h"
#include "cspinbox.h"
#include "globaldefine.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <DIconButton>
#include <QButtonGroup>

RectRadiusStyleWidget::RectRadiusStyleWidget(QWidget *parent) : AttributeWgt(ERectRadius, parent)
{
    initUi();
    initConnect();
}

void RectRadiusStyleWidget::setVar(const QVariant &var)
{
    QVariantList l = var.toList();
    if (l.isEmpty()) {
        m_radius->setSpecialText();
        m_left->setSpecialText();
        m_right->setSpecialText();
        m_leftBottom->setSpecialText();
        m_rightBottom->setSpecialText();
        m_radiusSlider->setValue(0);
        return;
    }

    int i = 0;
    if (1 == l.size()) {
        setSpinBoxValue(m_radius, l.at(i).toInt());
        setSpinBoxValue(m_left, l.at(i).toInt());
        setSpinBoxValue(m_right, l.at(i).toInt());
        setSpinBoxValue(m_leftBottom, l.at(i).toInt());
        setSpinBoxValue(m_rightBottom, l.at(i).toInt());
    } else {
        setSpinBoxValue(m_left, l.at(i++).toInt());
        setSpinBoxValue(m_right, l.at(i++).toInt());
        setSpinBoxValue(m_leftBottom, l.at(i++).toInt());
        setSpinBoxValue(m_rightBottom, l.at(i++).toInt());
    }

    int minValue = m_left->value();
    for (auto v : l) {
        minValue = minValue <= v.toInt() ? minValue : v.toInt();
    }

    setSpinBoxValue(m_radius, minValue);
}

void RectRadiusStyleWidget::initUi()
{
    setFocusPolicy(Qt::NoFocus);
    QVBoxLayout *mainLayout = new QVBoxLayout;

    DLabel *label = new DLabel(tr("Rounded corners"), this);

    m_sameRadiusButton = new DIconButton(nullptr);
    m_sameRadiusButton->setIcon(QIcon::fromTheme("group_normal"));
    m_sameRadiusButton->setToolTip(tr("Same radius"));
    setWgtAccesibleName(m_sameRadiusButton, "Same_radius");
    m_sameRadiusButton->setMinimumSize(38, 38);
    m_sameRadiusButton->setIconSize(QSize(16, 16));
    m_sameRadiusButton->setFocusPolicy(Qt::NoFocus);
    m_sameRadiusButton->setFlat(true);
    m_sameRadiusButton->setCheckable(true);

    m_diffRadiusButton = new DIconButton(nullptr);
    m_diffRadiusButton->setIcon(QIcon::fromTheme("ungroup_normal"));
    m_diffRadiusButton->setToolTip(tr("Different radius"));
    setWgtAccesibleName(m_diffRadiusButton, "Different_radius");
    m_diffRadiusButton->setMinimumSize(38, 38);
    m_diffRadiusButton->setIconSize(QSize(16, 16));
    m_diffRadiusButton->setFocusPolicy(Qt::NoFocus);
    m_diffRadiusButton->setFlat(true);
    m_diffRadiusButton->setCheckable(true);

    m_checkgroup = new QButtonGroup;
    m_checkgroup->setExclusive(true);
    m_checkgroup->addButton(m_sameRadiusButton);
    m_checkgroup->addButton(m_diffRadiusButton);

    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->addWidget(label, 8);
    titleLayout->addWidget(m_sameRadiusButton, 1);
    titleLayout->addWidget(m_diffRadiusButton, 1);

    m_diffRadiusWidget = new BackgroundWidget;
    m_diffRadiusWidget->setBackgroudColor(QColor(211, 211, 211));
    m_diffRadiusWidget->setFocusPolicy(Qt::NoFocus);

    QGridLayout *angleLayout = new QGridLayout;
    angleLayout->setContentsMargins(10, 10, 10, 10);
    angleLayout->setMargin(10);
    angleLayout->setVerticalSpacing(10);
    angleLayout->setHorizontalSpacing(10);

    m_left = new CSpinBox(this);
    m_left->setToolTip(tr("left radius"));
    setWgtAccesibleName(m_left, "left_radius");
    m_left->setMaximumWidth(110);
    m_left->setSpinRange(0, 1000);
    m_left->setEnabledEmbedStyle(true);
    m_left->setValueChangedKeepFocus(true);

    m_right = new CSpinBox(this);
    m_right->setToolTip(tr("right radius"));
    setWgtAccesibleName(m_right, "right_radius");
    m_right->setMaximumWidth(110);
    m_right->setSpinRange(0, 1000);
    m_right->setEnabledEmbedStyle(true);
    m_right->setValueChangedKeepFocus(true);

    m_leftBottom = new CSpinBox(this);
    m_leftBottom->setToolTip(tr("left bottom radius"));
    setWgtAccesibleName(m_leftBottom, "left_bottom_radius");
    m_leftBottom->setMaximumWidth(110);
    m_leftBottom->setSpinRange(0, 1000);
    m_leftBottom->setEnabledEmbedStyle(true);
    m_leftBottom->setValueChangedKeepFocus(true);

    m_rightBottom = new CSpinBox(this);
    m_rightBottom->setToolTip(tr("right bottom radius"));
    setWgtAccesibleName(m_rightBottom, "right bottom radius");
    m_rightBottom->setMaximumWidth(110);
    m_rightBottom->setSpinRange(0, 1000);
    m_rightBottom->setEnabledEmbedStyle(true);
    m_rightBottom->setValueChangedKeepFocus(true);

    m_diffRadiusWidget->setLayout(angleLayout);
    angleLayout->addWidget(m_left, 0, 0);
    angleLayout->addWidget(m_right, 0, 1);
    angleLayout->addWidget(m_leftBottom, 1, 0);
    angleLayout->addWidget(m_rightBottom, 1, 1);
    mainLayout->addLayout(titleLayout);
    mainLayout->addWidget(m_diffRadiusWidget);

    m_sameRadiusWidget = new DWidget;
    QHBoxLayout *sameLayout = new QHBoxLayout;
    m_sameRadiusWidget->setLayout(sameLayout);
    m_radiusSlider = new DSlider;
    m_radiusSlider->setMaximum(1000);

    m_radius = new CSpinBox();
    m_radius->setToolTip(tr("radius"));
    setWgtAccesibleName(m_radius, "radius");
    m_radius->setSpinRange(0, 1000);
    m_radius->setEnabledEmbedStyle(true);

    sameLayout->addWidget(m_radiusSlider, 3);
    sameLayout->addWidget(m_radius, 1);
    mainLayout->addWidget(m_sameRadiusWidget);

    m_sameRadiusButton->setChecked(true);
    m_sameRadiusWidget->show();
    m_diffRadiusWidget->hide();
    this->setLayout(mainLayout);
}

void RectRadiusStyleWidget::initConnect()
{
    connect(m_checkgroup, qOverload<QAbstractButton *>(&QButtonGroup::buttonClicked), this, [ = ](QAbstractButton * button) {
        if (button == m_sameRadiusButton) {
            m_diffRadiusWidget->hide();
            m_sameRadiusWidget->show();
        } else {
            m_diffRadiusWidget->show();
            m_sameRadiusWidget->hide();
        }
    });

    connect(m_radiusSlider, &DSlider::valueChanged, this, [ = ](int value) {
        setSpinBoxValue(m_radius, value);
        emitValueChange(value, value, value, value);
    });

    connect(m_left, &CSpinBox::valueChanged, this, [ = ](int value, EChangedPhase phase) {
        Q_UNUSED(value)
        Q_UNUSED(phase)
        emitValueChange(m_left->value(), m_right->value(), m_leftBottom->value(), m_rightBottom->value());
    });

    connect(m_right, &CSpinBox::valueChanged, this, [ = ](int value, EChangedPhase phase) {
        Q_UNUSED(value)
        Q_UNUSED(phase)
        emitValueChange(m_left->value(), m_right->value(), m_leftBottom->value(), m_rightBottom->value());
    });

    connect(m_leftBottom, &CSpinBox::valueChanged, this, [ = ](int value, EChangedPhase phase) {
        Q_UNUSED(value)
        Q_UNUSED(phase)
        emitValueChange(m_left->value(), m_right->value(), m_leftBottom->value(), m_rightBottom->value());
    });

    connect(m_rightBottom, &CSpinBox::valueChanged, this, [ = ](int value, EChangedPhase phase) {
        Q_UNUSED(value)
        Q_UNUSED(phase)
        emitValueChange(m_left->value(), m_right->value(), m_leftBottom->value(), m_rightBottom->value());
    });

    connect(m_radius, &CSpinBox::valueChanged, this, [ = ](int value, EChangedPhase phase) {
        Q_UNUSED(phase)
        QSignalBlocker block(m_radiusSlider);
        m_radiusSlider->setValue(value);
        emitValueChange(value, value, value, value);
    });

    connect(m_sameRadiusButton, &DIconButton::clicked, this, [ = ] {
        m_sameRadiusButton->setFlat(false);
        m_diffRadiusButton->setFlat(true);
    });

    connect(m_diffRadiusButton, &DIconButton::clicked, this, [ = ] {
        m_diffRadiusButton->setFlat(false);
        m_sameRadiusButton->setFlat(true);
    });
}

void RectRadiusStyleWidget::emitValueChange(int left, int right, int leftBottom, int rightBottom)
{
    QVariantList l;
    if (m_sameRadiusButton->isChecked()) {
        l << left;
    } else {
        l << left << right << leftBottom << rightBottom;
    }

    emit valueChanged(l, EChanged);
}

void RectRadiusStyleWidget::setSpinBoxValue(CSpinBox *s, int value)
{
    QSignalBlocker block(s);
    s->setValue(value);
}


void BackgroundWidget::setBackgroudColor(QColor c)
{
    m_bkColor = c;
}

void BackgroundWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.save();
    QPen pen(Qt::transparent, 5);
    painter.setPen(pen);
    QBrush brush(m_bkColor);
    painter.setBrush(brush);
    painter.drawRoundRect(rect(), 10, 10);
    painter.restore();
    QWidget::paintEvent(event);
}
