#include "commonshapewidget.h"

#include <DLabel>

#include <QHBoxLayout>
#include <QButtonGroup>
#include <QDebug>

#include "widgets/toolbutton.h"
#include "widgets/bigcolorbutton.h"
#include "widgets/bordercolorbutton.h"
#include "widgets/seperatorline.h"
#include "utils/configsettings.h"
#include "utils/global.h"


const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;

CommonshapeWidget::CommonshapeWidget(QWidget *parent)
    : DWidget(parent)
{
    DRAW_THEME_INIT_WIDGET("FillshapeWidget");
    this->setObjectName("FillshapeWidget");
    BigColorButton *fillBtn = new BigColorButton("common", this);
    DLabel *fillLabel = new DLabel(this);
    fillLabel->setObjectName("FillLabel");
    fillLabel->setText(tr("填充"));

    BorderColorButton *strokeBtn = new BorderColorButton(this);
    strokeBtn->setObjectName("FillStrokeButton");

    connect(fillBtn, &BigColorButton::btnCheckStateChanged, this, [ = ](bool show) {
        strokeBtn->resetChecked();
        emit showColorPanel(DrawStatus::Fill, cursor().pos(), show);

    });
    connect(strokeBtn, &BorderColorButton::btnCheckStateChanged, this, [ = ](bool show) {
        fillBtn->resetChecked();
        emit showColorPanel(DrawStatus::Stroke,  cursor().pos(), show);
    });

    connect(this, &CommonshapeWidget::resetColorBtns, this, [ = ] {
        fillBtn->resetChecked();
        strokeBtn->resetChecked();
    });

    DLabel *strokeLabel = new DLabel(this);
    strokeLabel->setObjectName("StrokeLabel");
    strokeLabel->setText(tr("描边"));
    SeperatorLine *sepLine = new SeperatorLine(this);
    DLabel *lwLabel = new DLabel(this);
    lwLabel->setObjectName("BorderLabel");
    lwLabel->setText(tr("描边粗细"));

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


    int defaultLineWidth = ConfigSettings::instance()->value(
                               "common", "lineWidth").toInt();
    lwBtnList[std::max(defaultLineWidth / 2 - 1, 0)]->setChecked(true);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(BTN_SPACING);
    layout->addStretch();
    layout->addWidget(fillBtn);
    layout->addWidget(fillLabel);
    layout->addWidget(strokeBtn);
    layout->addWidget(strokeLabel);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(sepLine);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(lwLabel);
    for (int j = 0; j < lwBtnList.length(); j++) {
        layout->addWidget(lwBtnList[j]);
    }
    layout->addStretch();
    setLayout(layout);

}

CommonshapeWidget::~CommonshapeWidget()
{
}
