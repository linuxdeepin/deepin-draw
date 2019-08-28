#include "idrawtool.h"
#include "cdrawscene.h"
#include <QDebug>
#include <QKeyEvent>


IDrawTool::~IDrawTool()
{

}

IDrawTool::IDrawTool(EDrawToolMode mode)
    : m_bMousePress(false)
    , m_sPointPress(0, 0)
    , m_sPointRelease(0, 0)
    , m_bShiftKeyPress(false)
    , m_bAltKeyPress(false)
    , m_mode(mode)
{

}

void IDrawTool::keyPressEvent(QKeyEvent *event, CDrawScene *scene)
{
    if ( event->key() == Qt::Key_Shift) {
        m_bShiftKeyPress = true;
    }
    //先按下SHIFT再按下ALT 会出现 Key_Meta按键值
    else if (event->key() == Qt::Key_Alt || event->key() == Qt::Key_Meta) {
        m_bAltKeyPress = true;
    } else {
        ;
    }
    qDebug() << "press" << event->key() << endl;

    scene->keyEvent(event);
}

void IDrawTool::keyReleaseEvent(QKeyEvent *event, CDrawScene *scene)
{
    if ( event->key() == Qt::Key_Shift) {
        m_bShiftKeyPress = false;
    }
    //先按下SHIFT再按下ALT 会出现 Key_Meta按键值
    else if (event->key() == Qt::Key_Alt || event->key() == Qt::Key_Meta) {
        m_bAltKeyPress = false;
    } else {
        ;
    }
    qDebug() << "release" << event->key() << endl;
    scene->keyEvent(event);
}

EDrawToolMode IDrawTool::getDrawToolMode() const
{
    return m_mode;
}
