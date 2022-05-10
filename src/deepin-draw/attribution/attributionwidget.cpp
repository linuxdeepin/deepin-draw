#include<QHBoxLayout>
#include "attributionwidget.h"
//#include "positionattriwidget.h"

AttributionWidget::AttributionWidget(QWidget *parent) : QWidget(parent)
{
    setWgtAccesibleName(this, "AttributionWidget");
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Widget | Qt::NoDropShadowWindowHint);
    QVBoxLayout *l = new QVBoxLayout(this);
    setLayout(l);
    setShowFlags(ToolActived | ItemSelected | ItemAttriChanged);
    l->setContentsMargins(0, 0, 0, 0);
    l->setMargin(0);

    setMinimumWidth(250);
}

void AttributionWidget::addAttriWidget(QWidget *w)
{
    m_atrriWidgets.append(w);
    layout()->addWidget(w);
    layout()->setAlignment(Qt::AlignTop);
}

void AttributionWidget::removeAttriWidget(QWidget *w)
{
    layout()->removeWidget(w);
    for (int i = 0; i < m_atrriWidgets.count(); i++) {
        if (m_atrriWidgets.at(i) == w) {
            m_atrriWidgets.removeAt(i);
            break;
        }
    }
}

void AttributionWidget::removeAll()
{
    for (auto w : m_atrriWidgets) {
        layout()->removeWidget(w);
        if (!w->property(ChildAttriWidget).toBool()) {
            w->hide();
        }

    }
    m_atrriWidgets.clear();
}

//void AttributionWidget::loadConstAtrributionWidget()
//{
//    for (auto w : allInstallAttriWidgets()) {
//        if (w->property("constInAtrributionWidget").toBool()) {
//            addAttriWidget(w);
//            w->show();
//            w->setEnabled(false);
//        }
//    }

//}

void AttributionWidget::showAt(int active, const QPoint &pos)
{
    Q_UNUSED(active);
    Q_UNUSED(pos);
}

void AttributionWidget::showWidgets(int active, const QList<QWidget *> &oldWidgets, const QList<QWidget *> &exposeWidgets)
{
    removeAll();
    for (auto w : oldWidgets) {
        removeAttriWidget(w);
    }

    for (auto w : exposeWidgets) {

        w->setEnabled(AttributionManager::ForceShow != active);

        //如果是子窗口属性，不显示
        if (!w->property(ChildAttriWidget).toBool()) {
            addAttriWidget(w);
            w->show();
        }
    }
}
