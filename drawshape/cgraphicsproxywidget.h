#ifndef CGRAPHICSPROXYWIDGET_H
#define CGRAPHICSPROXYWIDGET_H

#include <QGraphicsProxyWidget>
class CGraphicsProxyWidget : public QGraphicsProxyWidget
{
public:
    CGraphicsProxyWidget(QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = Qt::WindowFlags());

//protected:
//    void focusOutEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
};

#endif // CGRAPHICSPROXYWIDGET_H
