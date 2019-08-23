#include "testwidget.h"
#include "utils/configsettings.h"
#include <QHBoxLayout>
#include <QButtonGroup>
#include "toolbutton.h"
#include "cpushbutton.h"

TestWidget::TestWidget(QWidget *parent) : QWidget(parent)
{
    QStringList lwBtnNameList;
    lwBtnNameList << "FinerLineBtn" << "FineLineBtn"
                  << "MediumLineBtn" << "BoldLineBtn";

    QList<ToolButton *> lwBtnList;
    QButtonGroup *lwBtnGroup = new QButtonGroup(this);
    lwBtnGroup->setExclusive(true);

    for (int k = 0; k < lwBtnNameList.length(); k++) {
        ToolButton *lwBtn = new ToolButton(this);
        lwBtn->setObjectName(lwBtnNameList[k]);
        lwBtnList.append(lwBtn);
        lwBtnGroup->addButton(lwBtn);

        connect(lwBtn, &ToolButton::clicked, this, [ = ] {
            ConfigSettings::instance()->setValue("common", "lineWidth", (k + 1) * 2);
        });
        connect(ConfigSettings::instance(), &ConfigSettings::configChanged, this,
        [ = ](const QString & group, const QString & key) {
            if (group == "common" && key == "lineWidth") {
                int value = ConfigSettings::instance()->value("common", "lineWidth").toInt();
                if (value / 2 - 1 == k)
                    lwBtn->setChecked(true);
            }
        });
    }

    QMap<CPushButton::CButtonSattus, QString> pictureMap;
    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/draw/line_border_1_normal.svg");
    pictureMap[CPushButton::Hover]  = QString(":/theme/light/images/draw/line_border_1_hover.svg");
    pictureMap[CPushButton::Press]  = QString(":/theme/light/images/draw/line_border_1_active.svg");
    pictureMap[CPushButton::Active] = QString(":/theme/light/images/draw/line_border_1_active.svg");
    CPushButton *m_finerButton = new CPushButton(pictureMap, this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    for (int j = 0; j < lwBtnList.length(); j++) {
        layout->addWidget(lwBtnList[j]);
    }
    layout->addWidget(m_finerButton);
    layout->addStretch();
    setLayout(layout);
}
