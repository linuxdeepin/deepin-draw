/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
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
class CPictureTool;

/**
 * @brief The CCentralwidget class 中间控件类
 */
class CCentralwidget: public DWidget
{
    Q_OBJECT
public:
    CCentralwidget(DWidget *parent = nullptr);
    ~CCentralwidget();
    /**
     * @brief getLeftToolBar　获取工具栏句柄
     * @return
     */
    CLeftToolBar *getLeftToolBar();
    /**
     * @brief getGraphicsView　获取GraphicsView句柄
     * @return
     */
    CGraphicsView *getGraphicsView() const;
    /**
     * @brief getDrawScene　获取场景句柄
     * @return
     */
    CDrawScene *getDrawScene() const;
    /**
     * @brief switchTheme　切换主题
     * @param type
     */
    void switchTheme(int type);
    /**
     * @brief initSceneRect　初始化场景矩形
     */
    void initSceneRect();

signals:
    /**
     * @brief signalPassPictureOper　传递图片的旋转和翻转信号
     */
    void signalPassPictureOper(int);

    /**
     * @brief signalAttributeChangedFromScene 传递场景中选择图元改变信号
     */
    void signalAttributeChangedFromScene(bool, int);
    /**
     * @brief signalSetScale　设置缩放因子信号
     */
    void signalSetScale(const qreal);
    /**
     * @brief signalForwardQuitCutMode　传递退出裁剪模式信号
     */
    void signalForwardQuitCutMode();
    /**
     * @brief signalUpdateCutSize　更新裁剪尺寸信号
     */
    void signalUpdateCutSize();
    /**
     * @brief signalUpdateTextFont　更新文字图元字体信号
     */
    void signalUpdateTextFont();
    /**
     * @brief signalContinueDoOtherThing　保存完成后继续做某事信号
     */
    void signalContinueDoOtherThing();
    /**
     * @brief signalTransmitQuitCutModeFromTopBarMenu　传递属性栏退出裁剪模式信号
     */
    void signalTransmitQuitCutModeFromTopBarMenu();
    /**
     * @brief signalTransmitLoadDragOrPasteFile　传递拖拽或粘贴文件到画板信号
     */
    void signalTransmitLoadDragOrPasteFile(QString);


public slots:
    /**
     * @brief importPicture　导入图片
     */
    void importPicture();
    /**
     * @brief slotResetOriginPoint　重置原始点
     */
    void slotResetOriginPoint();
    /**
     * @brief slotAttributeChanged　属性改变
     */
    void slotAttributeChanged();
    /**
     * @brief slotZoom　执行缩放ｖｉｅｗ
     * @param scale　缩放因子
     */
    void slotZoom(qreal scale);
    /**
     * @brief slotSetScale　设置ｖｉｅｗ缩放大小
     * @param scale　缩放因子
     */
    void slotSetScale(const qreal scale);
    /**
     * @brief slotShowExportDialog　显示导出对话框
     */
    void slotShowExportDialog();
    /**
     * @brief slotNew　新建画板
     */
    void slotNew();
    /**
     * @brief slotPrint　打印
     */
    void slotPrint();
    /**
     * @brief slotShowCutItem　显示裁剪
     */
    void slotShowCutItem();
    /**
     * @brief slotSaveToDDF　保存为ＤＤＦ文件
     */
    void slotSaveToDDF();
    /**
     * @brief slotSaveAs　另存为ＤＤＦ文件
     */
    void slotSaveAs();
    /**
     * @brief slotTextFontFamilyChanged　文字图元字体改变
     */
    void slotTextFontFamilyChanged();
    /**
     * @brief slotTextFontSizeChanged　文字图元字体大小改变
     */
    void slotTextFontSizeChanged();
    /**
     * @brief openPicture　点击图片进行导入
     * @param path　图片路径
     */
    void openPicture(QString path);
    /**
     * @brief onEscButtonClick　ＥＳＣ按钮触发槽函数
     */
    void onEscButtonClick();
    /**
     * @brief slotCutLineEditeFocusChange　裁剪尺寸输入框焦点改变
     */
    void slotCutLineEditeFocusChange(bool);
    /**
     * @brief slotPastePicture　粘贴图片
     * @param picturePathList　图片路径
     */
    void slotPastePicture(QStringList picturePathList);
    /**
     * @brief slotPastePixmap　粘贴图片
     * @param pixmap　图片
     */
    void slotPastePixmap(QPixmap pixmap);


private slots:
    /**
     * @brief slotDoSaveImage　执行保存图片
     * @param completePath　待保存路径
     */
    void slotDoSaveImage(QString completePath);

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
    CPictureTool *m_pictureTool;
    int m_picNum;

private:
    /**
     * @brief initUI 初始化ＵＩ
     */
    void initUI();
    /**
     * @brief initConnection 初始化连接
     */
    void initConnect();
    /**
     * @brief getSceneImage 获取场景图片
     * @param type
     * @return
     */
    QPixmap getSceneImage(int type);
    /**
     * @brief resetSceneBackgroundBrush　重置场景画刷
     */
    void resetSceneBackgroundBrush();

};

#endif // MAINWIDGET_H
