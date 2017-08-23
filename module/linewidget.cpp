#include "linewidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QButtonGroup>

#include "widgets/bordercolorbutton.h"
#include "widgets/seperatorline.h"
#include "widgets/toolbutton.h"

#include "utils/configsettings.h"

LineWidget::LineWidget(QWidget *parent)
    : QWidget(parent)
{
    QLabel* strokeLabel = new QLabel(this);
    strokeLabel->setObjectName("BorderStrokeLabel");
    strokeLabel->setText(tr("Stroke"));

    BorderColorButton* strokeButton = new BorderColorButton(this);

    connect(strokeButton, &BorderColorButton::clicked, this, [=]{
        showColorPanel(DrawStatus::Stroke, cursor().pos());
    });

    SeperatorLine* sep1Line = new SeperatorLine(this);

    QLabel* borderStyleLabel = new QLabel(this);
    borderStyleLabel->setObjectName("BorderStyleLabel");
    borderStyleLabel->setText(tr("Style"));

    QList<ToolButton*> lineBtnList;
    QStringList lineBtnNameList;
    lineBtnNameList << "StraightLineBtn" << "ArbiLineBtn" << "ArrowBtn";
    QButtonGroup* btnGroup = new QButtonGroup(this);
    btnGroup->setExclusive(true);

    for(int k = 0; k < lineBtnNameList.length(); k++) {
        ToolButton* lineBtn = new ToolButton(this);
        lineBtn->setObjectName(lineBtnNameList[k]);
        lineBtnList.append(lineBtn);
        btnGroup->addButton(lineBtn);

        connect(lineBtn, &ToolButton::clicked, this, [=]{
            ConfigSettings::instance()->setValue("line", "style", k);
        });
    }

    int defaultIndex = ConfigSettings::instance()->value("line", "style").toInt();
    lineBtnList[defaultIndex]->setChecked(true);

    SeperatorLine* sep2Line = new SeperatorLine(this);

    QLabel* lwLabel = new QLabel(this);
    lwLabel->setObjectName("BorderLWLabel");
    lwLabel->setText(tr("Width"));

    QList<ToolButton*> lwBtnList;
    QStringList lwBtnNameList;
    lwBtnNameList << "FinerLineBtn" << "FineLineBtn"
                                  << "MediumLineBtn" << "BoldLineBtn";

    QButtonGroup* lwBtnGroup = new QButtonGroup(this);
    lwBtnGroup->setExclusive(true);

    for (int i = 0; i < lwBtnNameList.length(); i++) {
        ToolButton* lwBtn = new ToolButton(this);
        lwBtn->setObjectName(lwBtnNameList[i]);
        lwBtnList.append(lwBtn);
        lwBtnGroup->addButton(lwBtn);
        connect(lwBtn, &ToolButton::clicked, this, [=]{
            ConfigSettings::instance()->setValue("common", "lineWidth", (i+1)*2);
        });
    }

    int lineWidthDefaultIndex = ConfigSettings::instance()->value(
                "common", "lineWidth").toInt();
    lwBtnList[std::max(lineWidthDefaultIndex/2 - 1, 0)]->setChecked(true);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(10);
    layout->addWidget(strokeLabel);
    layout->addWidget(strokeButton);
    layout->addWidget(sep1Line, 0, Qt::AlignCenter);
    layout->addWidget(borderStyleLabel);

    for(int h = 0; h < lineBtnList.length(); h++) {
        layout->addWidget(lineBtnList[h]);
    }

    layout->addWidget(sep2Line, 0, Qt::AlignCenter);
    layout->addWidget(lwLabel);
    for(int j = 0; j < lwBtnList.length(); j++)
    {
        layout->addWidget(lwBtnList[j]);
    }

    setLayout(layout);
}

LineWidget::~LineWidget()
{
}
