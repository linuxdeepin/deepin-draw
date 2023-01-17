#include <QHBoxLayout>
#include <QEvent>
#include <QSizePolicy>
#include <QGraphicsDropShadowEffect>

#include "boxlayoutwidget.h"
#include "groupbuttonwidget.h"
#include "toolbutton.h"

GroupButtonWidget::GroupButtonWidget(QWidget *parent): AttributeWgt(EGroupWgt, parent)
{
//    QVariant var; var.setValue<QMargins>(QMargins(0, 0, 0, 0));
//    setProperty(WidgetMarginInVerWindow, var);
//    setProperty(WidgetAlignInVerWindow, 0);
    //组合按钮
    groupButton = new DIconButton(nullptr);
    groupButton->setIcon(QIcon::fromTheme("group_normal"));
    groupButton->setToolTip(tr("group"));
    setWgtAccesibleName(groupButton, "groupButton");
    groupButton->setMinimumSize(38, 38);
    groupButton->setIconSize(QSize(16, 16));
    groupButton->setFocusPolicy(Qt::NoFocus);
    groupButton->setFlat(true);
    //释放组合按钮
    unGroupButton = new DIconButton(nullptr);
    unGroupButton->setIcon(QIcon::fromTheme("ungroup_normal"));
    unGroupButton->setToolTip(tr("ungroup"));
    setWgtAccesibleName(unGroupButton, "unGroupButton");
    unGroupButton->setMinimumSize(38, 38);
    unGroupButton->setIconSize(QSize(16, 16));
    unGroupButton->setFocusPolicy(Qt::NoFocus);
    unGroupButton->setFlat(true);

    m_titleLabel = new QLabel(tr("Group"), this);
    m_titleLabel->setDisabled(true);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(0, 10, 10, 10);
    mainLayout->addWidget(m_titleLabel);

    BoxLayoutWidget *boxLayoutWidget = new BoxLayoutWidget(this);
    boxLayoutWidget->addWidget(groupButton);
    boxLayoutWidget->addWidget(unGroupButton);

    mainLayout->addWidget(boxLayoutWidget);
    setLayout(mainLayout);

    groupButton->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
    unGroupButton->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);

//    _labelGroup   = new QLabel(tr("Group"), this);
//    _labelGroup->hide();

//    _labelUngroup = new QLabel(tr("Ungroup"), this);
//    _labelUngroup->hide();

    connect(groupButton, &DIconButton::clicked, this, [ = ]() {
        emit this->buttonClicked(true, false);
    });
    connect(unGroupButton, &DIconButton::clicked, this, [ = ]() {
        emit this->buttonClicked(false, true);
    });

}

void GroupButtonWidget::setGroupFlag(bool canGroup, bool canUngroup)
{
    groupButton->setEnabled(canGroup);
    unGroupButton->setEnabled(canUngroup);

    if (canGroup) {
        groupButton->setFlat(false);
    } else {
        groupButton->setFlat(true);
    }

    if (canUngroup) {
        unGroupButton->setFlat(false);
    } else {
        unGroupButton->setFlat(true);
    }

    if (!canGroup && !canUngroup) {
        m_titleLabel->setDisabled(true);
    } else {
        m_titleLabel->setDisabled(false);
    }
}

void GroupButtonWidget::paintEvent(QPaintEvent *event)
{
    if (!isEnabled()) {
        groupButton->setFlat(true);
        unGroupButton->setFlat(true);
    }

    QWidget::paintEvent(event);
}


