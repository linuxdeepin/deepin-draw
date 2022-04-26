#ifndef ROTATEATTRIWIDGET_H
#define ROTATEATTRIWIDGET_H
#include <QPushButton>
#include <QLineEdit>
#include <DDoubleSpinBox>

#include "attributewidget.h"
DWIDGET_USE_NAMESPACE
class DrawBoard;
class PageItem;
class PageScene;

class RotateAttriWidget: public AttributeWgt
{
    Q_OBJECT
public:
    RotateAttriWidget(DrawBoard *drawBoard, QWidget *parent = nullptr);
    void setVar(const QVariant &var) override;
    void setAngle(double angle);
private:
    void showTooltip();
private:
    double checkValue(double value);
signals:
    void buttonClicked(bool doHorFlip, bool doVerFlip);

protected:
    Q_SLOT void onSceneSelectionChanged(const QList<PageItem * > &selectedItems);
private:
    QLabel *m_label;
    DDoubleSpinBox *m_angle;
    DIconButton *m_horFlipBtn;
    DIconButton *m_verFlipBtn;
    DIconButton *m_clockwiseBtn;
    DIconButton *m_anticlockwiseBtn;
    DrawBoard   *m_drawBoard = nullptr;
    PageScene   *m_currentScene = nullptr;
};
#endif // ROTATEATTRIWIDGET_H
