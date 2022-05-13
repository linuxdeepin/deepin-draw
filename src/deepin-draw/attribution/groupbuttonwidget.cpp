#include <QHBoxLayout>
#include <QEvent>
#include <QSizePolicy>
#include <QGraphicsDropShadowEffect>

#include "boxlayoutwidget.h"
#include "groupbuttonwidget.h"
#include "toolbutton.h"

GroupButtonWidget::GroupButtonWidget(QWidget *parent): AttributeWgt(EGroupWgt, parent)
{
    //组合按钮
    groupButton = new DToolButton(nullptr);
    groupButton->setIcon(QIcon::fromTheme("group_normal"));
    groupButton->setToolTip(tr("group"));
    setWgtAccesibleName(groupButton, "groupButton");
    groupButton->setMinimumSize(38, 38);
    groupButton->setIconSize(QSize(16, 16));
    groupButton->setFocusPolicy(Qt::NoFocus);

    //释放组合按钮
    unGroupButton = new DToolButton(nullptr);
    unGroupButton->setIcon(QIcon::fromTheme("ungroup_normal"));
    unGroupButton->setToolTip(tr("ungroup"));
    setWgtAccesibleName(unGroupButton, "unGroupButton");
    unGroupButton->setMinimumSize(38, 38);
    unGroupButton->setIconSize(QSize(16, 16));
    unGroupButton->setFocusPolicy(Qt::NoFocus);

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

    if (!canGroup && !canUngroup) {
        m_titleLabel->setDisabled(true);
    } else {
        m_titleLabel->setDisabled(false);
    }
}


