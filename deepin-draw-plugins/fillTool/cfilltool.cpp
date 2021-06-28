#include "cfilltool.h"
#include "application.h"
#include "cdrawscene.h"
#include "cgraphicsview.h"
#include "cdrawparamsigleton.h"

#include <DToolButton>
#include <QtMath>


CFillTool::CFillTool(): IDrawTool(EDrawToolMode(MoreTool + 2))
{

}

QAbstractButton *CFillTool::initToolButton()
{
    DToolButton *btn = new DToolButton;
    drawApp->setWidgetAccesibleName(btn, "Paint bucket tool button");
    btn->setToolTip(tr("Paint bucket"));
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
    QColor fillColor = event->view()->getDrawParam()->value(DrawAttribution::EBrushColor).value<QColor>();

    if (!fillColor.isValid())
        return;

    QImage &img = event->scene()->sceneExImage();
    QPoint  posInImage = (event->pos() - event->scene()->sceneRect().topLeft()).toPoint();
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

IDrawTool *creatTool()
{
    qWarning() << "creat CFillTool !!!!!!!!!!!!!!!!!";
    return  new CFillTool;
}
