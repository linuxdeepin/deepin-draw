#include "attributionregistermanager.h"
#include "textattributionregister.h"
#include "commonattributionregister.h"
#include "styleattributionregister.h"
#include "styleattriwidget.h"

void AttributionRegisterManager::registe()
{
    addDefaultAttri();

    for (IAttributionRegister *r : m_registers) {
        r->registe();
    }
}

void AttributionRegisterManager::addRegister(IAttributionRegister *pRegister)
{
    pRegister->setDrawBoard(drawBoard());
    m_registers.append(pRegister);
}

AttributionRegisterManager::~AttributionRegisterManager()
{
    for (int i = m_registers.count() - 1; i >= 0; i--) {
        delete m_registers.takeAt(i);
    }
}

void AttributionRegisterManager::addDefaultAttri()
{
    TextAttributionRegister *textAttr = new TextAttributionRegister(drawBoard());
    CommonAttributionRegister *commonAttr = new CommonAttributionRegister(drawBoard());
    StyleAttributionRegister *styleAtti = new StyleAttributionRegister(drawBoard());

    textAttr->registe();
    commonAttr->registe();
    styleAtti->registe();

    auto ls = commonAttr->getStyleAttriWidgets();
    for (QWidget *l : ls) {
        styleAtti->getStyleWidget()->addChildAtrri(dynamic_cast<AttributeWgt *>(l));
    }

    ls = textAttr->getAttriWidgets();
    for (QWidget *l : ls) {
        styleAtti->getStyleWidget()->addChildAtrri(dynamic_cast<AttributeWgt *>(l));
    }

}
