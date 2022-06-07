#include "styleattributionregister.h"
#include "styleattriwidget.h"

StyleAttributionRegister::StyleAttributionRegister(DrawBoard *drawboard): IAttributionRegister(drawboard)
{
    m_styleAttri = new StyleAttriWidget(EStyleProper, drawboard);
    m_styleAttri->setProperty(ParentAttriWidget, true);
}

void StyleAttributionRegister::registe()
{
    drawBoard()->attributionManager()->installComAttributeWgt(m_styleAttri->attribution(), m_styleAttri, QVariant());
}

StyleAttriWidget *StyleAttributionRegister::getStyleWidget()
{
    return m_styleAttri;
}
