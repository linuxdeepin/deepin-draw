// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "cfilltool.h"
#include "application.h"
#include "cdrawscene.h"
#include "cgraphicsview.h"
#include "cdrawparamsigleton.h"

#include <DToolButton>
#include <QtMath>

static void fillColorInPos(QImage &img, const QPoint &posInImage, const QColor &fillColor)
{
    if (img.isNull())
        return;

    if (!img.rect().contains(posInImage)) {
        return;
    }
    QColor borderIfColor = img.pixelColor(posInImage);

    QStack<QPoint> waitToFillPos;
    waitToFillPos << posInImage;
    while (!waitToFillPos.isEmpty()) {
        auto pos   = waitToFillPos.pop();
        auto color = img.pixelColor(pos);
        if (color != fillColor) {
            img.setPixelColor(pos, fillColor);

            for (int di = -1; di < 2; ++di) {
                int x = pos.x() + di;
                for (int dj = -1; dj < 2; ++dj) {
                    int y = pos.y() + dj;
                    QPoint neberPos(x, y);
                    if (img.rect().contains(neberPos)) {
                        auto colorN = img.pixelColor(neberPos);
                        if (colorN == borderIfColor) {
                            waitToFillPos.push(neberPos);
                        }
                    }
                }
            }
        }
    }
}

class JFillCommand: public JDynamicLayer::JCommand
{
public:
    JFillCommand(const QColor &color, const QPoint pos, JDynamicLayer *layer = nullptr): JDynamicLayer::JCommand(layer)
    {
        _color = color;
        _posInImage = pos;
    }
    int  cmdType() override {return 4;}
    void doCommand() override
    {
        if (_layer != nullptr) {
            fillColorInPos(_layer->image(), _posInImage, _color);
        }
    }
    void serialization(QDataStream &out) override
    {
        out << _color;
        out << _posInImage;
    }
    void deserialization(QDataStream &in) override
    {
        in >> _color;
        in >> _posInImage;
    }
    QColor     _color;
    QPoint     _posInImage;
};

CFillTool::CFillTool(): IDrawTool(EDrawToolMode(MoreTool + 2))
{

}

QAbstractButton *CFillTool::initToolButton()
{
    DToolButton *btn = new DToolButton;
    setWgtAccesibleName(btn, "Paint bucket tool button");
    btn->setToolTip("Paint bucket");
    btn->setIconSize(QSize(48, 48));
    btn->setFixedSize(QSize(37, 37));
    btn->setCheckable(true);
    btn->setVisible(true);

    connect(btn, &DToolButton::toggled, btn, [ = ](bool b) {
        QIcon icon       = QIcon::fromTheme("ddc_brush tool_normal");
        QIcon activeIcon = QIcon::fromTheme("ddc_brush tool_active");
        btn->setIcon(b ? activeIcon : icon);
    });
    btn->setIcon(QIcon::fromTheme("ddc_brush tool_normal"));
    return btn;
}

DrawAttribution::SAttrisList CFillTool::attributions()
{
    DrawAttribution::SAttrisList result;
    result << defaultAttriVar(DrawAttribution::EBrushColor);
    return result;
}

int CFillTool::minMoveUpdateDistance()
{
    return 0;
}

int CFillTool::decideUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    return 0;
}

void CFillTool::toolStart(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    auto layer = currentLayer(event->scene());
    if (layer == nullptr) {
        return;
    }

    QColor fillColor = event->view()->getDrawParam()->value(DrawAttribution::EBrushColor).value<QColor>();

    if (!fillColor.isValid())
        return;

    QImage &img = layer->image();
    QPoint  posInImage = layer->mapScenePosToMyImage(event->pos()).toPoint();
    fillColorInPos(img, posInImage, fillColor);
    auto cmd = new JFillCommand(fillColor, posInImage, layer);
    layer->appendComand(cmd, false);
    event->scene()->update();
}

void CFillTool::toolUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
}

void CFillTool::toolFinish(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
}

bool CFillTool::returnToSelectTool(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    return false;
}

void CFillTool::onStatusChanged(EStatus oldStatus, EStatus nowStatus)
{
    Q_UNUSED(oldStatus)
    Q_UNUSED(nowStatus)
}

JDynamicLayer *CFillTool::currentLayer(CDrawScene *scene)
{
    auto items = scene->selectGroup()->items();

    if (items.count() == 1) {
        return dynamic_cast<JDynamicLayer *>(items.first());
    }
    return nullptr;
}

IDrawTool *creatTool()
{
    qWarning() << "creat CFillTool !!!!!!!!!!!!!!!!!";
    return  new CFillTool;
}
