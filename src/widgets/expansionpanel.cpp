#include "expansionpanel.h"
#include <QVBoxLayout>

ExpansionPanel::ExpansionPanel(QWidget *parent) : DBlurEffectWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;

    //背景 透明度
    auto effect = QColor("#f9f9f9");
    effect.setAlpha(240);
    setMaskColor(effect);

    //设置圆角
    setBlurRectXRadius(18);
    setBlurRectYRadius(18);
//    setRadius(30);
    setBlurEnabled(true);
    setMode(DBlurEffectWidget::GaussianBlur);

    groupButton = new DToolButton(this);
    groupButton->setFixedSize(165, 40);
    groupButton->setObjectName("groupButton");
    groupButton->setText(tr("Group"));
    groupButton->setIcon(QIcon::fromTheme("icon_group_normal"));
    groupButton->setIconSize(QSize(38, 38));
    groupButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    unGroupButton = new DToolButton(this);
    unGroupButton->setFixedSize(165, 40);
    unGroupButton->setObjectName("unGroupButton");
    unGroupButton->setText(tr("Ungroup"));
    unGroupButton->setIcon(QIcon::fromTheme("icon_ungroup_normal"));
    unGroupButton->setIconSize(QSize(38, 38));
    unGroupButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    layout->addWidget(groupButton);
    layout->addWidget(unGroupButton);

    setLayout(layout);

    connect(groupButton, &DToolButton::clicked, this, [ = ] {

        Q_EMIT signalItemGroup();
    });

    connect(unGroupButton, &DToolButton::clicked, this, [ = ] {

        Q_EMIT signalItemgUngroup();
    });
}

DToolButton *ExpansionPanel::getGroupButton()
{
    return  groupButton;
}

DToolButton *ExpansionPanel::getUngroupButton()
{
    return  unGroupButton;
}


