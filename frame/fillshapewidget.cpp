#include "fillshapewidget.h"

#include <QHBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QDebug>

#include "widgets/toolbutton.h"
#include "widgets/bigcolorbutton.h"
#include "widgets/bordercolorbutton.h"
#include "widgets/seperatorline.h"

#include "utils/configsettings.h"

FillshapeWidget::FillshapeWidget(QWidget *parent)
    : QWidget(parent)
{
    BigColorButton* fillBtn = new BigColorButton("common", this);
    QLabel* fillLabel = new QLabel(this);
    fillLabel->setText(tr("Fill"));

    BorderColorButton* strokeBtn = new BorderColorButton(this);
    strokeBtn->setObjectName("FillStrokeButton");

    connect(fillBtn, &BigColorButton::btnCheckStateChanged, this, [=](bool show){
            strokeBtn->resetChecked();
            emit showColorPanel(DrawStatus::Fill, cursor().pos(), show);

    });
    connect(strokeBtn, &BorderColorButton::btnCheckStateChanged, this, [=](bool show){
            fillBtn->resetChecked();
            emit showColorPanel(DrawStatus::Stroke,  cursor().pos(), show);
    });

    connect(this, &FillshapeWidget::resetColorBtns, this, [=]{
        fillBtn->resetChecked();
        strokeBtn->resetChecked();
    });

    QLabel* strokeLabel = new QLabel(this);
    strokeLabel->setText(tr("Stroke"));
    SeperatorLine* sepLine = new SeperatorLine(this);
    QLabel* lwLabel = new QLabel(this);
    lwLabel->setObjectName("BorderLabel");
    lwLabel->setText(tr("Width"));

    QStringList lwBtnNameList;
    lwBtnNameList << "FinerLineBtn" << "FineLineBtn"
                  << "MediumLineBtn" << "BoldLineBtn";

    QList<ToolButton*> lwBtnList;
    QButtonGroup* lwBtnGroup = new QButtonGroup(this);
    lwBtnGroup->setExclusive(true);

    for (int k = 0; k < lwBtnNameList.length(); k++) {
        ToolButton* lwBtn = new ToolButton(this);
        lwBtn->setObjectName(lwBtnNameList[k]);
        lwBtnList.append(lwBtn);
        lwBtnGroup->addButton(lwBtn);

        connect(lwBtn, &ToolButton::clicked, this, [=]{
            ConfigSettings::instance()->setValue("common", "lineWidth", (k+1)*2);
        });
        connect(ConfigSettings::instance(), &ConfigSettings::configChanged, this,
                [=](const QString &group, const QString &key){
            if (group == "common" && key == "lineWidth")
            {
                int value = ConfigSettings::instance()->value("common", "lineWidth").toInt();
                if (value/2 -1 == k)
                    lwBtn->setChecked(true);
            }
        });
    }


    int defaultLineWidth = ConfigSettings::instance()->value(
                "common", "lineWidth").toInt();
    lwBtnList[std::max(defaultLineWidth/2 - 1, 0)]->setChecked(true);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(6);
    layout->addStretch();
    layout->addWidget(fillBtn);
    layout->addWidget(fillLabel);
    layout->addWidget(strokeBtn);
    layout->addWidget(strokeLabel);
    layout->addWidget(sepLine);
    layout->addWidget(lwLabel);
    for(int j = 0; j < lwBtnList.length(); j++) {
        layout->addWidget(lwBtnList[j]);
    }
    layout->addStretch();
    setLayout(layout);

}

FillshapeWidget::~FillshapeWidget()
{
}
