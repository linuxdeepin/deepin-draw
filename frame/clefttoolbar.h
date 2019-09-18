/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
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
#ifndef RIGHTTOOLBAR_H
#define RIGHTTOOLBAR_H

#include <DFrame>

class CCheckButton;
class QVBoxLayout;

DWIDGET_USE_NAMESPACE

class CLeftToolBar : public DFrame
{
    Q_OBJECT
public:
    explicit CLeftToolBar(DWidget *parent = nullptr);
    ~CLeftToolBar();

    void shortCutOperation(int type);

protected:
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);                      //进入QWidget瞬间事件


signals:
    void setCurrentDrawTool(int type);
    void importPic();
    void signalBegainCut();

public slots:

    /**
     * @brief slotChangedStatusToSelect 将工具栏的状态切换为选择状态
     */
    void slotChangedStatusToSelect();
    void slotSetDisableButtons(bool);
    void slotQuitCutMode();

private:
    QList<CCheckButton *> m_actionButtons;
    CCheckButton *m_picBtn;
    CCheckButton *m_rectBtn;
    CCheckButton *m_roundBtn;
    CCheckButton *m_triangleBtn;
    CCheckButton *m_starBtn;
    CCheckButton *m_polygonBtn;
    CCheckButton *m_lineBtn;
    CCheckButton *m_penBtn;
    CCheckButton *m_textBtn;
    CCheckButton *m_blurBtn;
    CCheckButton *m_cutBtn;

    QVBoxLayout *m_layout;


private:
    void initUI();
    void initConnection();
    void clearOtherSelections(CCheckButton *clickedButton);
    void initDrawTools();
};

#endif // RIGHTTOOLBAR_H
