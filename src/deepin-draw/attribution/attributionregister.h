#ifndef ATTRIBUTIONREGISTER_H
#define ATTRIBUTIONREGISTER_H
#include <QObject>
//#include "extendattribution.h"
//#include "attributewidget.h"

class GroupButtonWidget;
class RotateAttriWidget;
class OrderWidget;

class DrawBoard;
class ColorStyleWidget;

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
    void registeAdjustImageAttri();
    void registeCircleStyleAttri();
    void registeBaseStyleAttrri();
private:
    DrawBoard *m_drawBoard;
    GroupButtonWidget *m_groupWidget = nullptr;
    RotateAttriWidget *m_rotateAttri = nullptr;
    OrderWidget *m_orderAttri = nullptr;
    ColorStyleWidget *m_fillStyle = nullptr;
    ColorStyleWidget *m_borderStyle = nullptr;
};

#endif // ATTRIBUTIONREGISTER_H
