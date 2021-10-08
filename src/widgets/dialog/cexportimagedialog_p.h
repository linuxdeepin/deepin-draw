/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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
#ifndef CEXPORTIMAGEDIALOG_P_H
#define CEXPORTIMAGEDIALOG_P_H

#include "cexportimagedialog.h"
#include "cvalidator.h"
#include "application.h"

#include <DSpinBox>
#include <DCheckBox>

#include <QFormLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QTimer>

//这里模仿QtBase，把d指针的内容挪到_p.h文件中，增强软件可测试性

class CExportImageDialog::CExportImageDialog_private
{
public:
    enum ESizeSettingModel {ERadioModel, EPixelModel, ESettingModelCount};
    explicit CExportImageDialog_private(CExportImageDialog *q): _q(q) {}

    enum EKeepBase {EKeepBaseW, EKeepBaseH, EKeepBaseRadioValue, EFreeSetting};
    enum EAlertReason {ETooSmall = 0, ETooBig, ENoAlert};

    void initSizeSettingLayoutUi(QFormLayout *fLayout, QWidget *contentWidget);
    void updateSettingModelUi();
    void resetImageSettingSizeTo(const QSize &sz, qreal raido = 1.0, bool keepRadio = true, ESizeSettingModel settingModel = ERadioModel);
    void setSizeSettingModel(ESizeSettingModel model);
    bool autoKeepSize(EKeepBase base);
    static QSizeF autoKeepWHRadio(int base, qreal precentRadio,
                                  const QSize &wantedSize,
                                  const QSizeF &preSize,
                                  const QSize &orgSz,
                                  EAlertReason &aler);
    void showTip(EAlertReason alertReson = ETooBig);
    bool isFocusInEditor() const;

    CExportImageDialog *_q;
    DRadioButton *_radioRadioBtn = nullptr;
    DRadioButton *_radioPiexlBtn = nullptr;

    DSpinBox  *_radioSpinBox = nullptr;
    QLabel *_precentStuff = nullptr;

    DCheckBox *_keepRaidoCheckBox = nullptr;
    DLineEdit *_widthEditor = nullptr;
    DLineEdit *_heightEditor = nullptr;
    QLabel *_tipLabelForOutOfBounds = nullptr;
    QTimer *timer = nullptr;
    QWidget *piexlWgt = nullptr;

    ESizeSettingModel settingModel = ERadioModel;

    QSize originSize;
    qreal  curPrecent = 1.0;
    QSizeF curSize[ESettingModelCount] = {QSizeF(), QSizeF()};
    QSize  curShowIntSize;
};

#endif // CEXPORTIMAGEDIALOG_P_H
