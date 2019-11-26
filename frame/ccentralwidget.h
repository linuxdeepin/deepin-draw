/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXing
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
#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <DWidget>
#include <DLabel>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QUndoStack>
#include <QGraphicsItem>




DWIDGET_USE_NAMESPACE

class CLeftToolBar;
class CGraphicsView;
class CDrawScene;
class CPrintManager;
class ProgressLayout;
class CExportImageDialog;

class CCentralwidget: public DWidget
{
    Q_OBJECT
public:
    CCentralwidget(DWidget *parent = nullptr);
    ~CCentralwidget();
    CLeftToolBar *getLeftToolBar();
    CGraphicsView *getGraphicsView() const;
    CDrawScene *getDrawScene() const;

    void switchTheme(int type);
    void initSceneRect();


signals:
    //传递图片的旋转和翻转信号
    void signalPassPictureOper(int);

    /**
     * @brief signalAttributeChangedFromScene 传递场景中选择图元改变信号
     */
    void signalAttributeChangedFromScene(bool, int);
    void signalSetScale(const qreal);

    void signalForwardQuitCutMode();
    void signalUpdateCutSize();
    void signalUpdateTextFont();
    void signalContinueDoOtherThing();
    void signalTransmitQuitCutModeFromTopBarMenu();


public slots:
    void importPicture();
    void slotResetOriginPoint();
    void slotAttributeChanged();
    void slotZoom(qreal scale);
    void slotSetScale(const qreal scale);
    void slotShowExportDialog();
    void slotNew();
    void slotPrint();
    void slotShowCutItem();
    void slotSaveToDDF();
    void slotSaveAs();
    void slotTextFontFamilyChanged();
    void slotTextFontSizeChanged();
    void openPicture(QString path);
    void slotOnEscButtonClick();
    void slotCutLineEditeFocusChange(bool);

private slots:
    void slotDoSaveImage();

private:
    CLeftToolBar *m_leftToolbar;
    DLabel *m_seperatorLine;

    QVBoxLayout *m_vLayout;
    QHBoxLayout *m_hLayout;

    CExportImageDialog *m_exportImageDialog;
    CPrintManager *m_printManager;


    int m_horizontalMargin;
    int m_verticalMargin;

    DMenu *m_contextMenu;
    CGraphicsView *m_pGraphicsView;
    CDrawScene *m_pDrawScene;
    ProgressLayout *m_progressLayout;
    int m_picNum;

private:
    void initUI();
    void initConnect();
    QPixmap getSceneImage(int type);
    void resetSceneBackgroundBrush();

};

#endif // MAINWIDGET_H
