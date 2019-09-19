#include "cgraphicsproxywidget.h"
#include <QTextEdit>
CGraphicsProxyWidget::CGraphicsProxyWidget(QGraphicsItem *parent, Qt::WindowFlags wFlags)
    : QGraphicsProxyWidget(parent, wFlags)
{

}

//void CGraphicsProxyWidget::focusOutEvent(QFocusEvent *event)
//{
//    //清空选中状态
//    //static_cast<QTextEdit *>(this->widget())->setTextCursor(QTextCursor());
//    this->hide();
//}
