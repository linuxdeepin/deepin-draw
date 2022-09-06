// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef PROGRESSLAYOUT_H
#define PROGRESSLAYOUT_H

#include <QHBoxLayout>
#include <DProgressBar>
#include <DBlurEffectWidget>
#include <DLabel>
#include <DWidget>

DWIDGET_USE_NAMESPACE

class ProgressLayout: public DBlurEffectWidget
{

    Q_OBJECT
public:
    explicit ProgressLayout(/*DBlurEffectWidget*/QWidget *parent = nullptr);
    ~ProgressLayout();
    void setRange(int start, int end);
    void setProgressValue(int value);
    void showInCenter(DWidget *w);
    void setText(const QString &str);
    void delayClose();

    void reset();

    void setAutoFillSubText(bool b);
    //bool isAutoFillSubText() const;
    void setSubText(const QString &str);


    QVBoxLayout *m_progressVBoxLayout;
    DLabel *m_label;
    DLabel *m_progressLabel;
    DProgressBar *m_progressbar;
    int m_start;
    int m_end;
    QString     m_subText;
    bool        m_autoFillSubText = true;

};

#endif // PROGRESSLAYOUT_H
