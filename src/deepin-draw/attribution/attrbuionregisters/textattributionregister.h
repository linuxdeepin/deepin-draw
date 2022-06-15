/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Tan Lang <tanlang@uniontech.com>
 *
 * Maintainer: Tan Lang <tanlang@uniontech.com>
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
#ifndef TEXTATTRIBUTIONREGISTER_H
#define TEXTATTRIBUTIONREGISTER_H
#include <QAbstractItemDelegate>
#include <QPainter>
#include <QComboBox>

#include "iattributionregister.h"

class IAttributionRegister;
class ComboBoxSettingWgt;
class SliderSpinBoxWidget;
class TextItem;
class ColorStyleWidget;

class TextAttributionRegister : public IAttributionRegister
{
    Q_OBJECT
public:
    using IAttributionRegister::IAttributionRegister;
    virtual void registe() override;
    QList<QWidget *> getAttriWidgets();
private:
    void registeTextAtrri();
    void initFontFamilyWidget(QComboBox *fontHeavy);
    void initFontWeightWidget();
    void reInitFontWeightComboxItems(const QString &family, QComboBox *fontHeavy);
    void initFontFontSizeWidget();
    bool isTextEnableUndoThisTime(bool considerRecorderEmpty = true);
    QStyleOptionMenuItem getStyleOption(const QStyleOptionViewItem &option,                                        const QModelIndex &index) const;
    virtual bool eventFilter(QObject *o, QEvent *event)override;

    void resetItemsFontFamily();
    void cachedItemsFontFamily();
    void restoreItemsFontFamily();

private:
    QComboBox *m_fontComBox = nullptr;
    QComboBox *m_fontHeavy = nullptr;
    QComboBox *m_fontSize = nullptr;

    bool      _activePackup = false;
    bool      _fontViewShowOut = false;
    QString   _cachedFontWeightStyle;

    QStringList supWeightStyleList;

    struct Font {
        QString fontFamily;
        QString FontWeight;
    };

    QMap<TextItem *, Font> _cachedFontFamily;


    int        _currenFontSize = 14;
    QList<int> _defaultFontSizeSet;

    ComboBoxSettingWgt *m_fontAttri = nullptr;
    SliderSpinBoxWidget *m_fontSizeAttri = nullptr;
    ComboBoxSettingWgt *m_fontWeightAttri = nullptr;
    ColorStyleWidget *m_fontColorAttri = nullptr;
};


class QComboxMenuDelegate: public QAbstractItemDelegate
{
    Q_OBJECT
public:
    QComboxMenuDelegate(QObject *parent, QComboBox *cmb)
        : QAbstractItemDelegate(parent), mCombo(cmb), pressedIndex(-1)
    {}

    void dontShowCheckState(bool b) {showCheckStates = !b;}

protected:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        QStyleOptionMenuItem opt = getStyleOption(option, index);
        if (!showCheckStates)
            opt.checked = false;
        painter->fillRect(option.rect, opt.palette.window());
        mCombo->style()->drawControl(QStyle::CE_MenuItem, &opt, painter, mCombo);
    }
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override
    {
        QStyleOptionMenuItem opt = getStyleOption(option, index);
        return mCombo->style()->sizeFromContents(
                   QStyle::CT_MenuItem, &opt, option.rect.size(), mCombo);
    }
//    bool editorEvent(QEvent *event, QAbstractItemModel *model,
//                     const QStyleOptionViewItem &option, const QModelIndex &index) override;

private:
    QStyleOptionMenuItem getStyleOption(const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const;
    QComboBox *mCombo;
    int pressedIndex;
    bool showCheckStates = false;
};
#endif // TEXTATTRIBUTIONREGISTER_H
