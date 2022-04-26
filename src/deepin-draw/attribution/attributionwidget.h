#ifndef ATTRIBUTIONWIDGET_H
#define ATTRIBUTIONWIDGET_H

#include <QWidget>
#include "attributemanager.h"

class AttributionWidget : public QWidget, public AttributionManager
{
    Q_OBJECT
public:
    explicit AttributionWidget(QWidget *parent = nullptr);
    void addAttriWidget(QWidget *w);
    void removeAttriWidget(QWidget *w);
    void removeAll();
    //void loadConstAtrributionWidget();
protected:
    void showAt(int active, const QPoint &pos) override;
    void showWidgets(int active,
                     const QList<QWidget *> &oldWidgets, const QList<QWidget *> &exposeWidgets) override;
signals:

private:
    QList<QWidget *> m_atrriWidgets;
};

#endif // ATTRIBUTIONWIDGET_H
