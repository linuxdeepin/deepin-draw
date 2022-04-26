/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: ZhangYong <zhangyong@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "attributewidget.h"
#include "attributemanager.h"

#include "csidewidthwidget.h"
#include "cspinbox.h"
#include "seperatorline.h"

#include "toolbutton.h"
#include "ccolorpanel.h"

#ifdef USE_DTK
#include <DFontSizeManager>
#include <DGuiApplicationHelper>
#endif
#include <QFontComboBox>
#include <QLineEdit>

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>

ColorSettingButton::ColorSettingButton(const QString &text,
                                       QWidget *parent, bool autoConnect):
    AttributeWgt(-1, parent), _text(text)
{
    if (autoConnect) {
        connect(this, &ColorSettingButton::colorChanged, this, [ = ](const QColor & color, int phase) {
            emit attriChanged(color, phase);
        });
    }
}

void ColorSettingButton::setVar(const QVariant &var)
{
    QSignalBlocker bloker(this);
    QColor c = var.value<QColor>();
    setColor(c);
}

CColorPickWidget *ColorSettingButton::colorPick()
{
    static CColorPickWidget *_color = nullptr;
    if (_color == nullptr) {
        //_color = new CColorPickWidget(this);
    }
    return _color;
}

void  ColorSettingButton::setColorFill(ColorSettingButton::EColorFill fillPos)
{
    _fillPos = fillPos;
    update();
}

void  ColorSettingButton::setColor(const QColor &color, int phase)
{
    _color = color;
    emit colorChanged(_color, phase);
    update();
}

void  ColorSettingButton::setDefaultButtonSize(const QSize &sz)
{
    _defaultButtonSize = sz; setMaximumHeight(_defaultButtonSize.height());
}

void  ColorSettingButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    initPainter(&painter);
    if (_fillPos == EFillArea) {
        paintFillArea(&painter);
    } else {
        paintFillBorder(&painter);
    }
}

QSize  ColorSettingButton::sizeHint() const
{
    return minimumSizeHint();
}

QSize  ColorSettingButton::minimumSizeHint() const
{
    QFontMetrics metrics(this->font());
    QSize textSz(metrics.width(_text), metrics.height());
    const int space = 6;
    return QSize(_defaultButtonSize.width() + space + textSz.width(), qMax(_defaultButtonSize.height(), textSz.width()));
}

void ColorSettingButton::mousePressEvent(QMouseEvent *event)
{
//    QMetaObject::invokeMethod(this, [ = ]() {
//        CColorPickWidget *colorPick = this->colorPick();
//        if (!_connectedColorPicker) {
//            connect(colorPick, &CColorPickWidget::colorChanged, this, [ = ](const QColor & color, EChangedPhase phase) {
//                if (colorPick->caller() == this)
//                    setColor(color, phase);
//            });
//            _connectedColorPicker = true;
//        }

//        colorPick->setCaller(this);
//        colorPick->setColor(_color);

//        QPoint btnPos = mapToGlobal(QPoint(0, 0));
//        QPoint pos(btnPos.x() + 14,
//                   btnPos.y() + this->height());

//        colorPick->show(pos.x(), pos.y());

//    }, Qt::QueuedConnection);
//    event->accept();
}

void  ColorSettingButton::paintFillArea(QPainter *painter)
{
    QSize textSize = QSize(painter->fontMetrics().width(_text), painter->fontMetrics().height());

    QRect buttonRct = QRect(QPoint(0, (height() - _defaultButtonSize.height()) / 2), QSize(_defaultButtonSize.width(), _defaultButtonSize.height()));
    QRect textRct(QPoint(buttonRct.right() + _space, (height() - textSize.height()) / 2), textSize);

    bool isNotVaild = !_color.isValid();
    const QColor borderColor = (isNotVaild || _color.alpha() == 0) ? QColor(77, 82, 93, int(0.8 * 255)) : QColor(255, 255, 255, int(0.1 * 255));
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing);

    const QSizeF btSz = buttonRct.size();

    QPointF topLeft = buttonRct.topLeft();

    QPen pen(painter->pen());

    bool   darkTheme = 1;
#ifdef USE_DTK
    darkTheme = (DGuiApplicationHelper::instance()->themeType()  == 2);
#endif
    QColor penColor  = darkTheme ? borderColor : QColor(0, 0, 0, int(0.1 * 255));

    pen.setColor(penColor);

    pen.setWidthF(1.5);    //多加0.5宽度弥补防走样的误差

    painter->setPen(pen);

    painter->translate(topLeft);

    QPainterPath path;

    QRectF outerct(QPointF(0, 0), btSz);
    QRectF inerrct(QPointF(3, 3), btSz - QSizeF(2 * 3, 2 * 3));

    path.addRoundedRect(outerct, 8, 8);

    //填充色的设置(多选颜色冲突时(图元填充色不一致那么不设置填充色在后续绘制一条斜线))
    painter->setBrush((isNotVaild || _color.alpha() == 0) ?
                      (darkTheme ? QColor(8, 15, 21, int(0.7 * 255)) : QColor(0, 0, 0, int(0.05 * 255))) : _color);

    painter->drawPath(path);

    //如果多选颜色有冲突(isMult为true时)那么就绘制"..."
    if (isNotVaild) {
        painter->save();
        //painter->setPen(darkTheme ? QColor("#C0C6D4") : QColor("#414D68"));
        QColor cp = darkTheme ? QColor("#C5CFE0") : QColor("#000000");
        cp.setAlpha(darkTheme ? int(0.8 * 255) : int(0.2 * 255));
        painter->setPen(cp);
        QFont ft;
        ft.setPixelSize(14);
        painter->setFont(ft);
        painter->drawText(outerct.translated(0, -painter->fontMetrics().height() / 4), "...", QTextOption(Qt::AlignCenter));
        painter->restore();
    } else {
        //颜色没有冲突 如果颜色是透明的要绘制一条斜线表示没有填充色
        if (_color.alpha() == 0) {
            QPen pens(QColor(255, 67, 67, 153));
            pens.setWidthF(2.0);
            painter->setPen(pens);
            painter->drawLine(QLineF(inerrct.bottomLeft(), inerrct.topRight()));
        }
    }

    painter->restore();

    //绘制常量文字("填充")
    painter->save();
    painter->setPen(darkTheme ? QColor("#C0C6D4") : QColor("#414D68"));
    painter->drawText(textRct, _text, QTextOption(Qt::AlignLeft | Qt::AlignVCenter));
    painter->restore();
}

void  ColorSettingButton::paintFillBorder(QPainter *painter)
{
    QSize textSize = QSize(painter->fontMetrics().width(_text), painter->fontMetrics().height());

    QRect buttonRct = QRect(QPoint(0, (height() - _defaultButtonSize.height()) / 2), QSize(_defaultButtonSize.width(), _defaultButtonSize.height()));
    QRect textRct(QPoint(buttonRct.right() + _space, (height() - textSize.height()) / 2), textSize);

    bool isNotVaild = !_color.isValid();
    const QColor borderColor = (isNotVaild || _color.alpha() == 0) ?
                               QColor(77, 82, 93, int(0.8 * 255)) : QColor(255, 255, 255, int(0.1 * 255));
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing);

    const QSizeF btSz = buttonRct.size();

    QPointF topLeft = buttonRct.topLeft();

    QPen pen(painter->pen());

    bool   darkTheme = false;
#ifdef USE_DTK
    darkTheme = (DGuiApplicationHelper::instance()->themeType() == 2);
#endif
    QColor penColor  = darkTheme ? borderColor : QColor(0, 0, 0, int(0.1 * 255));

    pen.setColor(penColor);

    pen.setWidthF(1.5);    //多加0.5宽度弥补防走样的误差

    painter->setPen(pen);

    painter->translate(topLeft);

    QPainterPath path;

    QRectF outerct(QPointF(0, 0), btSz);
    QRectF inerrct(QPointF(3, 3), btSz - QSizeF(2 * 3, 2 * 3));

    path.addRoundedRect(outerct, 8, 8);
    path.addRoundedRect(inerrct, 5, 5);

    //线条的颜色用path的填充色来表示(如果是选中了多个图元那么有默认的颜色(该默认颜色与主题相关))
    painter->setBrush((isNotVaild || _color.alpha() == 0) ?
                      (darkTheme ? QColor(8, 15, 21, int(0.7 * 255)) : QColor(0, 0, 0, int(0.05 * 255))) : _color);

    painter->drawPath(path);

    //如果颜色是透明的要绘制一条斜线表示没有填充色
    if (!isNotVaild && _color.alpha() == 0) {
        QPen pens(QColor(255, 67, 67, 153));
        pens.setWidthF(2.0);
        painter->setPen(pens);
        painter->drawLine(QLineF(inerrct.bottomLeft(), inerrct.topRight()));
    }

    painter->restore();

    //绘制常量文字("描边")
    painter->save();
    painter->setPen(darkTheme ? QColor("#C0C6D4") : QColor("#414D68"));
    painter->drawText(textRct, _text, QTextOption(Qt::AlignLeft | Qt::AlignVCenter));
    painter->restore();
}

ColorSettingWgt::ColorSettingWgt(QWidget *parent): AttributeWgt(-1, parent)
{
    m_colorPanel = new ColorPanel(this);
    connect(m_colorPanel, &ColorPanel::colorChanged, this, [ = ](const QColor & color, int phase) {
        emit attriChanged(color, phase);
    });
    setLayout(new QVBoxLayout);
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->addWidget(m_colorPanel);
}

void ColorSettingWgt::setVar(const QVariant &var)
{
    QSignalBlocker bloker(this);
    QColor c = var.value<QColor>();
    setColor(c);

}

const ColorPanel *ColorSettingWgt::panel()
{
    return m_colorPanel;
}

void ColorSettingWgt::setColor(const QColor &color, int phase)
{
    m_colorPanel->setColor(color, false, static_cast<EChangedPhase>(phase));
}

ComboBoxSettingWgt::ComboBoxSettingWgt(const QString &text, QWidget *parent):
    AttributeWgt(-1, parent)
{
    _comBox = new QComboBox(this);
    _comBox->setMaximumHeight(36);
    _comBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    _lab    = new QLabel(this);
    _lab->setText(text);
    QHBoxLayout *pLay = new QHBoxLayout;
    pLay->addWidget(_lab);
    pLay->addSpacing(10);
    pLay->addWidget(_comBox);
    this->setLayout(pLay);
    pLay->setContentsMargins(0, 0, 0, 0);

    if (text.isEmpty())
        _lab->hide();
    //connect(_comBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ComboBoxSettingWgt::onCurrentChanged);
#ifdef USE_DTK
    DFontSizeManager::instance()->bind(_comBox, DFontSizeManager::T6, QFont::Normal);
    DFontSizeManager::instance()->bind(_lab, DFontSizeManager::T6, QFont::Normal);
#endif
}

void ComboBoxSettingWgt::setText(const QString &text)
{
    _lab->setText(text);
    if (text.isEmpty())
        _lab->hide();
    else {
        _lab->show();
    }
}

QComboBox *ComboBoxSettingWgt::comboBox()
{
    return _comBox;
}

void ComboBoxSettingWgt::setComboBox(QComboBox *pBox)
{
    if (_comBox != nullptr) {
        //disconnect(_comBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ComboBoxSettingWgt::onCurrentChanged);
        layout()->removeWidget(_comBox);
        _comBox->hide();
        _comBox->deleteLater();
    }
    if (pBox != nullptr) {
        layout()->addWidget(pBox);
        _comBox = pBox;
        pBox->setParent(this);
#ifdef USE_DTK
        DFontSizeManager::instance()->bind(_comBox, DFontSizeManager::T6, QFont::Normal);
#endif
        //connect(_comBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ComboBoxSettingWgt::onCurrentChanged);
    }
}

void ComboBoxSettingWgt::onCurrentChanged(int index)
{
    emit attriChanged(index);
}

AttributeWgt::AttributeWgt(int attri, QWidget *parent): QWidget(parent)
{
    setAttribution(attri);
    this->hide();
}

void AttributeWgt::setAttribution(int arrti)
{
    _attri = arrti;
}

int AttributeWgt::attribution()
{
    return _attri;
}

void AttributeWgt::setVar(const QVariant &var)
{
    this->setProperty(AttriWidgetVar, var);
}

QSize AttributeWgt::recommendedSize() const
{
    return sizeHint();
}

bool AttributeWgt::event(QEvent *e)
{
    if (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseButtonRelease) {
        e->accept();
    }
    return QWidget::event(e);
}

SpinBoxSettingWgt::SpinBoxSettingWgt(const QString &text, QWidget *parent):
    AttributeWgt(-1, parent)
{
    _spinBox = new CSpinBox(this);
    _spinBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    _lab    = new QLabel(this);
    _lab->setText(text);
    QHBoxLayout *pLay = new QHBoxLayout;
    pLay->addWidget(_lab);
    pLay->addSpacing(10);
    pLay->addWidget(_spinBox);
    this->setLayout(pLay);
    pLay->setContentsMargins(0, 0, 0, 0);
    if (text.isEmpty())
        _lab->hide();

    connect(_spinBox, &CSpinBox::valueChanged, this, [ = ](int value, EChangedPhase phase) {
        emit attriChanged(value, phase);
        emit widthChanged(value);
    });
}

void SpinBoxSettingWgt::setVar(const QVariant &var)
{
    if (var.isValid()) {
        QSignalBlocker blocker(_spinBox);
        _spinBox->setValue(var.toInt());
    } else {
        spinBox()->setSpecialText();
    }
}

CSpinBox *SpinBoxSettingWgt::spinBox()
{
    return _spinBox;
}

QSize SpinBoxSettingWgt::recommendedSize() const
{
    //return _lab->sizeHint() + _spinBox->sizeHint();
    return AttributeWgt::sizeHint();
}



SAttrisList::SAttrisList(const QList<int> &list)
{
    for (int i = 0; i < list.count(); ++i) {
        SAttri attri;
        attri.attri = list.at(i);
        this->push_back(attri);
    }
}

SAttrisList SAttrisList::insected(const SAttrisList &other)
{
    SAttrisList result;
    foreach (auto it, *this) {
        foreach (auto ot, other) {
            if (it.attri == ot.attri) {
                SAttri temp;
                temp.attri = it.attri;
                if (it.var == ot.var) {
                    temp.var = ot.var;
                }
                result.append(temp);
                break;
            }
        }
    }
    return result;
}

SAttrisList SAttrisList::unionAtrri(const SAttrisList &other)
{
    SAttrisList result;
    foreach (auto ot, other) {
        bool bFind = false;
        SAttri temp = ot;
        foreach (auto it, *this) {
            if (it.attri == ot.attri) {
                bFind = true;
                break;
            }
        }

        if (!bFind)
            result.append(temp);
    }
    result << *this;
    return result;
}

bool SAttrisList::haveAttribution(int attri)
{
    for (int i = 0; i < count(); ++i) {
        if (this->at(i).attri == attri) {
            return true;
        }
    }
    return false;
}

RectSettingWgt::RectSettingWgt(QWidget *parent): AttributeWgt(ERectProperty, parent)
{
    QVBoxLayout *l = new QVBoxLayout(this);
    QHBoxLayout *lxy = new QHBoxLayout(this);
    QHBoxLayout *lwh = new QHBoxLayout(this);
    QHBoxLayout *xLayout = new QHBoxLayout;
    QHBoxLayout *yLayout = new QHBoxLayout;
    QHBoxLayout *wLayout = new QHBoxLayout;
    QHBoxLayout *hLayout = new QHBoxLayout;
    LayoutAddSpinBox(xLayout, _xSpinBox, "X");
    LayoutAddSpinBox(yLayout, _ySpinBox, "Y");
    LayoutAddSpinBox(wLayout, _wSpinBox, "W");
    LayoutAddSpinBox(hLayout, _hSpinBox, "H");

    lxy->addLayout(xLayout);
    lxy->addLayout(yLayout);
    lwh->addLayout(wLayout);
    lwh->addLayout(hLayout);
    l->addLayout(lxy);
    l->addLayout(lwh);
    setLayout(l);
    setFixedHeight(100);

//    connect(_xSpinBox, &CSpinBox::valueChanged, this, [ = ](int value, EChangedPhase phase) {
//        emit attriChanged(QRect(_xSpinBox->value(), _ySpinBox->value(), _wSpinBox->value(), _hSpinBox->value()), phase);
//    });

//    connect(_ySpinBox, &CSpinBox::valueChanged, this, [ = ](int value, EChangedPhase phase) {
//        emit attriChanged(QRect(_xSpinBox->value(), _ySpinBox->value(), _wSpinBox->value(), _hSpinBox->value()), phase);
//    });

//    connect(_wSpinBox, &CSpinBox::valueChanged, this, [ = ](int value, EChangedPhase phase) {
//        emit attriChanged(QRect(_xSpinBox->value(), _ySpinBox->value(), _wSpinBox->value(), _hSpinBox->value()), phase);
//    });

//    connect(_hSpinBox, &CSpinBox::valueChanged, this, [ = ](int value, EChangedPhase phase) {
//        emit attriChanged(QRect(_xSpinBox->value(), _ySpinBox->value(), _wSpinBox->value(), _hSpinBox->value()), phase);
//    });
}

void RectSettingWgt::setVar(const QVariant &var)
{
    QSignalBlocker block(this);
    QRect r = var.toRect();
    _xSpinBox->setValue(r.x());
    _ySpinBox->setValue(r.y());
    _wSpinBox->setValue(r.width());
    _hSpinBox->setValue(r.height());
}

QSize RectSettingWgt::recommendedSize() const
{
    return QSize(400, 100);
}

void RectSettingWgt::LayoutAddSpinBox(QLayout *l, CSpinBox *&spinBox, QString text)
{
    DLabel *hLabel = new DLabel(text, this);
    hLabel->setEnabled(false);
    spinBox = new CSpinBox(this);
    spinBox->setEnabled(false);
    spinBox->setEnabledEmbedStyle(true);
    l->addWidget(hLabel);
    l->addWidget(spinBox);
}
