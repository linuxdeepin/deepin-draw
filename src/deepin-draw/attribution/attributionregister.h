#ifndef ATTRIBUTIONREGISTER_H
#define ATTRIBUTIONREGISTER_H
#include <QObject>
//#include "extendattribution.h"
//#include "attributewidget.h"

class GroupButtonWidget;
class RotateAttriWidget;
class OrderWidget;

class DrawBoard;

enum  USER_DEFINE_ATTRI {
    EUserAttriStart = 2000,
    EUserAttriEnd
};

class AttributionRegister : public QObject
{
    Q_OBJECT
public:
    explicit AttributionRegister(DrawBoard *d);
    void registe();
    void initConnect();
    void registeGroupAttri();
    void resgisteRotateAttri();
    void registeOrderAttri();
private:
    DrawBoard *m_drawBoard;
    GroupButtonWidget *m_groupWidget = nullptr;
    RotateAttriWidget *m_rotateAttri = nullptr;
    OrderWidget *m_orderAttri = nullptr;
};

#endif // ATTRIBUTIONREGISTER_H
