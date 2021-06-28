#include "ceraser.h"
#include "application.h"
#include "cdrawscene.h"

#include <DToolButton>
#include <QtMath>


CEraser::CEraser(): IDrawTool(EDrawToolMode(MoreTool + 3))
{

}

QAbstractButton *CEraser::initToolButton()
{
    DToolButton *btn = new DToolButton;
    drawApp->setWidgetAccesibleName(btn, "eraser tool button");
    btn->setToolTip(tr("eraser"));
    btn->setIconSize(QSize(48, 48));
    btn->setFixedSize(QSize(37, 37));
    btn->setCheckable(true);
    btn->setVisible(true);

//    connect(btn, &DToolButton::toggled, btn, [ = ](bool b) {
//        QIcon icon       = QIcon::fromTheme("ddc_brush tool_normal");
//        QIcon activeIcon = QIcon::fromTheme("ddc_brush tool_active");
//        btn->setIcon(b ? activeIcon : icon);
//    });
    //:/icons/deepin/builtin/texts/eraser tool_normal_36px.svg
    btn->setIcon(QIcon::fromTheme("eraser tool_normal"));
    return btn;
}

int CEraser::decideUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    return 1;
}

void CEraser::toolStart(CDrawToolEvent *event, ITERecordInfo *pInfo)
{

}

void CEraser::toolUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    QPainter painter(&(event->scene()->sceneExImage()));

    QPointF  prePos = pInfo->_prePos - event->scene()->sceneRect().topLeft();
    QPointF  pos = event->pos() - event->scene()->sceneRect().topLeft();
    QLineF line(prePos, pos);

    const int inc = 20;

    QPen pen;
    pen.setWidth(inc);
    pen.setCapStyle(Qt::RoundCap);

    QPainterPathStroker ps(pen);

    QPainterPath path;
    path.moveTo(line.p1());
    path.lineTo(line.p2());

    path = ps.createStroke(path);

    path = path.simplified();

    painter.setCompositionMode(QPainter::CompositionMode_Source);

    painter.setPen(QColor(0, 0, 0, 0));
    painter.setBrush(QColor(0, 0, 0, 0));

    painter.drawPath(path);
    event->scene()->update();
}

void CEraser::toolFinish(CDrawToolEvent *event, ITERecordInfo *pInfo)
{

}

bool CEraser::returnToSelectTool(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    return false;
}

IDrawTool *creatTool()
{
    qWarning() << "creat CEraser !!!!!!!!!!!!!!!!!";
    return  new CEraser;
}
