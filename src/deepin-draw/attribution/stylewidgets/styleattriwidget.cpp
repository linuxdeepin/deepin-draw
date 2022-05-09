#include "styleattriwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <DArrowLineExpand>
#include <DWidget>
#include <DSplitter>

StyleAttriWidget::StyleAttriWidget(int attri, QWidget *parent): AttributeWgt(attri, parent)
{
    initUi();
}

QVBoxLayout *StyleAttriWidget::getLayout()
{
    return m_layout;//dynamic_cast<QVBoxLayout *>(layout());
}

void StyleAttriWidget::removeChildAtrri(AttributeWgt *attri)
{
    m_childAtrri.removeOne(attri);
}

void StyleAttriWidget::addChildAtrri(AttributeWgt *attri, bool bAddLayout)
{
    m_childAtrri.append(attri);

    if (bAddLayout) {
        m_layout->addWidget(attri);
    }

}

void StyleAttriWidget::setVar(const QVariant &var)
{
    QVariantList l = var.toList();
    for (auto w : m_childAtrri) {

        bool bFind = false;
        for (auto v : l) {
            if (v.toInt() == w->attribution()) {
                bFind = true;
                w->show();
                break;
            }
        }

        if (!bFind) {
            w->hide();
        }
    }
}

void StyleAttriWidget::initUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setMargin(0);
    this->setLayout(mainLayout);

    m_layout = new QVBoxLayout;
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setMargin(0);

    auto l =  new QLabel(tr("style"));
    mainLayout->addWidget(l);
    mainLayout->addLayout(m_layout);
}
