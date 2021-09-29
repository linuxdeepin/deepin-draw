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
    void initSizeSettingLayoutUi(QFormLayout *fLayout, QWidget *contentWidget)
    {
        //add radio setting ui.
        QHBoxLayout *lay1 = new QHBoxLayout;
        lay1->setContentsMargins(0, 0, 0, 0);
        lay1->setSpacing(9);
        _radioRadioBtn = new QRadioButton(tr("Percentage"), contentWidget);
        lay1->addWidget(_radioRadioBtn);
        auto spinBox = new DSpinBox(contentWidget);
        spinBox->setEnabledEmbedStyle(true);
        spinBox->setSuffix("%");
        spinBox->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
        spinBox->setMaximumSize(QSize(100, 36));
        _radioSpinBox = spinBox;
        lay1->addWidget(spinBox);
        _precentStuff = new QLabel("%");
        lay1->addWidget(_precentStuff);
        lay1->addSpacing(25);
        _radioPiexlBtn = new QRadioButton(tr("Pixels"), contentWidget);
        lay1->addWidget(_radioPiexlBtn);
        fLayout->addRow(tr("Dimensions:"), lay1);

        QHBoxLayout *lay2 = new QHBoxLayout;
        lay2->setContentsMargins(0, 0, 0, 0);
        lay2->addSpacing(30);

        QButtonGroup *group = new QButtonGroup(_q);
        group->addButton(_radioRadioBtn, ERadioModel);
        group->addButton(_radioPiexlBtn, EPixelModel);
        connect(group, QOverload<int, bool>::of(&QButtonGroup::buttonToggled), _q, [ = ](int model, bool checked) {
            if (checked) {
                settingModel = ESizeSettingModel(model);
                updateSettingModelUi();
            }
        });
        auto w = new QWidget(contentWidget);
        {
            //set w ui.
            QGridLayout *lay = new QGridLayout;
            lay->setContentsMargins(0, 0, 0, 0);
            w->setLayout(lay);

            _keepRaidoCheckBox = new DCheckBox(tr("Lock aspect ratio"), w);
            lay->addWidget(_keepRaidoCheckBox, 0, 0, 1, 4);

            {
                lay->addWidget(new QLabel(tr("W:"), w), 1, 0, 1, 1);

                auto lineEditor = new DLineEdit(w);
                lineEditor->setClearButtonEnabled(false);
                lay->addWidget(lineEditor, 1, 1, 1, 1);
                lay->addWidget(new QLabel(tr("pixels"), w), 1, 2, 1, 1);
                _widthEditor = lineEditor;
            }

            {
                lay->addWidget(new QLabel(tr("H:"), w), 2, 0, 1, 1);

                auto lineEditor = new DLineEdit(w);
                lineEditor->setClearButtonEnabled(false);
                lay->addWidget(lineEditor, 2, 1, 1, 1);

                lay->addWidget(new QLabel(tr("pixels"), w), 2, 2, 1, 1);

                _heightEditor = lineEditor;

                lay->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding), 2, 3, 1, 1);
            }
            {
                _tipLabelForOutOfBounds = new QLabel("", w);
                lay->addWidget(_tipLabelForOutOfBounds, 3, 0, 1, 4);
            }
        }
        lay2->addWidget(w, Qt::AlignLeft);
        piexlWgt = w;
        fLayout->addRow("", lay2);

        {
            auto validtor = new CIntValidator(0, INT_MAX, _widthEditor);
            _widthEditor->lineEdit()->setValidator(validtor);
            connect(_widthEditor, &DLineEdit::editingFinished, _q, [ = ]() {
                if (_widthEditor->text().toInt() != curShowIntSize.width())
                    autoKeepSize(EKeepBaseW);
                _widthEditor->clearFocus();
            });
        }
        {
            auto validtor = new CIntValidator(0, INT_MAX, _heightEditor);
            _heightEditor->lineEdit()->setValidator(validtor);
            connect(_heightEditor, &DLineEdit::editingFinished, _q, [ = ]() {
                if (_heightEditor->text().toInt() != curShowIntSize.height())
                    autoKeepSize(EKeepBaseH);
                _heightEditor->clearFocus();
            });
        }
        connect(_radioSpinBox, QOverload<int>::of(&DSpinBox::valueChanged), _q, [ = ](int value) {
            Q_UNUSED(value)
            if (value < 1) {
                QSignalBlocker blocker(_radioSpinBox);
                _radioSpinBox->setValue(1);
            }
            autoKeepSize(EKeepBaseRadioValue);
        });

        _radioRadioBtn->setChecked(true);
    }

    void updateSettingModelUi()
    {
        if (_radioRadioBtn->isChecked()) {
            setSizeSettingModel(ERadioModel);
        } else if (_radioPiexlBtn->isChecked()) {
            setSizeSettingModel(EPixelModel);
        }
    }
    void resetImageSettingSizeTo(const QSize &sz, qreal raido = 1.0, bool keepRadio = true, ESizeSettingModel settingModel = ERadioModel)
    {
        this->settingModel = settingModel;

        originSize = sz;
        for (int i = 0; i < ESettingModelCount; ++i) {
            curSize[i] = sz;
        }
        curPrecent = raido;


        if (settingModel == ERadioModel) {
            _radioRadioBtn->setChecked(true);
        } else if (settingModel == EPixelModel) {
            _radioPiexlBtn->setChecked(true);
        }

        _radioSpinBox->setRange(0, INT_MAX);
        _radioSpinBox->setValue(qMin(raido, 1.0) * 100.);
        _widthEditor->setText(QString("%1").arg(sz.width()));
        _heightEditor->setText(QString("%1").arg(sz.height()));
        _keepRaidoCheckBox->setChecked(keepRadio);
    }

    void setSizeSettingModel(ESizeSettingModel model)
    {
        if (model == ERadioModel) {
            _radioSpinBox->setEnabled(true);
            _precentStuff->setEnabled(true);
            piexlWgt->setEnabled(false);

            _keepRaidoCheckBox->setChecked(true);

            QSignalBlocker bloker(_radioSpinBox);
            _radioSpinBox->setValue(qRound((qreal(curSize[ERadioModel].width()) / originSize.width()) * 100.));

        } else if (model == EPixelModel) {
            _radioSpinBox->setEnabled(false);
            _precentStuff->setEnabled(false);
            piexlWgt->setEnabled(true);

            QSignalBlocker bloker(_radioSpinBox);
            _radioSpinBox->setSpecialValueText("— —");
            _radioSpinBox->setValue(-1);
        }

        QSignalBlocker bloker1(_widthEditor);
        _widthEditor->setText(QString::number(qRound(curSize[model].width())));

        QSignalBlocker bloker2(_heightEditor);
        _heightEditor->setText(QString::number(qRound(curSize[model].height())));
    }


    bool autoKeepSize(EKeepBase base)
    {
        EAlertReason alert = ENoAlert;

        QSignalBlocker bloker0(_widthEditor);
        QSignalBlocker bloker1(_heightEditor);
        QSignalBlocker bloker2(_radioSpinBox);

        if (base == EKeepBaseH || base == EKeepBaseW) {
            bool keepHW = _keepRaidoCheckBox->isChecked();
            if (!keepHW) {
                base = EFreeSetting;
            }
        }

        int wantedWidth  = _widthEditor->text().toInt();
        int wantedHeight = _heightEditor->text().toInt();
        qreal settingPrecent = _radioSpinBox->value() / 100.;
        QSizeF resultSize = autoKeepWHRadio(base, settingPrecent, QSize(wantedWidth, wantedHeight), curSize[settingModel],
                                            originSize, alert);
        showTip(alert);

        int showWidth  = qRound(resultSize.width());
        int showHeight = qRound(resultSize.height());
        curShowIntSize = QSize(showWidth, showHeight);
        _widthEditor->setText(QString::number(showWidth));
        _heightEditor->setText(QString::number(showHeight));

        curSize[settingModel] = resultSize;
        if (base == EKeepBaseRadioValue) {
            if (alert != ENoAlert) {
                QSignalBlocker bloker(_radioSpinBox);
                _radioSpinBox->setValue(curPrecent * 100.);
            } else {
                curPrecent = settingPrecent;
            }
        }

        return (alert == ENoAlert);
    }

    static QSizeF autoKeepWHRadio(int base, qreal precentRadio,
                                  const QSize &wantedSize,
                                  const QSizeF &preSize,
                                  const QSize &orgSz,
                                  EAlertReason &aler)
    {
        const int c_MaxPiexl = 10000;
        const int c_MinPiexl = 1;
        QSizeF size = preSize;

        aler = ENoAlert;

        switch (base) {
        case EKeepBaseRadioValue: {
            size = QSizeF(orgSz.width() * precentRadio, orgSz.height() * precentRadio);
            if ((size.width() > c_MaxPiexl || size.width() < c_MinPiexl) || (size.height() > c_MaxPiexl || size.height() < c_MinPiexl)) {
                if (size.width() < c_MinPiexl || size.height() < c_MinPiexl) {
                    aler = ETooSmall;
                } else if (size.width() > c_MaxPiexl || size.height() > c_MaxPiexl) {
                    aler = ETooBig;
                }
                size = preSize;
            }
            break;
        }
        case EKeepBaseW:
        case EKeepBaseH: {
            if (wantedSize.width() < c_MinPiexl || wantedSize.height() < c_MinPiexl) {
                aler = ETooSmall;
            } else if (wantedSize.width() > c_MaxPiexl || wantedSize.height() > c_MaxPiexl) {
                aler = ETooBig;
            }

            const QSize tempSize = QSize(qMin(c_MaxPiexl, qMax(c_MinPiexl, wantedSize.width())), qMin(c_MaxPiexl, qMax(c_MinPiexl, wantedSize.height())));
            size = tempSize;

            qreal radioWH = qreal(preSize.width()) / preSize.height();
            if (base == EKeepBaseW) {
                qreal resultH = size.width() / radioWH;
                if (resultH >= c_MinPiexl && resultH <= c_MaxPiexl) {
                    size.setHeight(resultH);
                } else {
                    if (resultH > c_MaxPiexl)
                        aler = ETooBig;
                    else if (resultH < c_MinPiexl)
                        aler = ETooSmall;
                    size = preSize;
                }
            } else if (base == EKeepBaseH) {
                qreal resultW = size.height() * radioWH;
                if (resultW >= c_MinPiexl && resultW <= c_MaxPiexl) {
                    size.setWidth(resultW);
                } else {
                    if (resultW > c_MaxPiexl)
                        aler = ETooBig;
                    else if (resultW < c_MinPiexl)
                        aler = ETooSmall;
                    size = preSize;
                }
            }
            break;
        }
        case EFreeSetting: {
            size = wantedSize;
            if (size.width() > c_MaxPiexl) {
                size.rwidth() = c_MaxPiexl;
                aler = ETooBig;
            } else if (size.width() < c_MinPiexl) {
                aler = ETooSmall;
                size.rwidth() = c_MinPiexl;
            }

            if (size.height() > c_MaxPiexl) {
                aler = ETooBig;
                size.rheight() = c_MaxPiexl;
            } else if (size.height() < c_MinPiexl) {
                aler = ETooSmall;
                size.rheight() = c_MinPiexl;
            }
            break;
        }

        }
        return size;
    }

    void showTip(EAlertReason alertReson = ETooBig)
    {
        if (alertReson == ENoAlert)
            return;

        QString tips;
        if (alertReson == ETooSmall) {
            tips = tr("At least one pixel please.");
        } else if (alertReson == ETooBig) {
            tips = tr("It supports up to 10,000 pixels");
        }
        _tipLabelForOutOfBounds->setText(tips);
        if (timer == nullptr) {
            timer = new QTimer(_q);
            timer->setSingleShot(true);
            connect(timer, &QTimer::timeout, _q, [ = ]() {
                _tipLabelForOutOfBounds->setText("");
            });
        }
        timer->start(2000);
    }

    bool isFocusInEditor()const
    {
        if (qApp->focusWidget() == _widthEditor->lineEdit() || qApp->focusWidget() == _heightEditor->lineEdit()) {
            return true;
        }
        return false;
    }

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
