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
#include <QStackedLayout>

#include "drawshape/globaldefine.h"

DWIDGET_USE_NAMESPACE

class CLeftToolBar;
class CGraphicsView;
class CDrawScene;
class CPrintManager;
class ProgressLayout;
class CExportImageDialog;
class CPictureTool;
class CMultipTabBarWidget;

/**
 * @brief The CCentralwidget class 中间控件类
 */
class CCentralwidget: public DWidget
{
    Q_OBJECT
public:
    CCentralwidget(DWidget *parent = nullptr);
    CCentralwidget(QStringList filepaths, DWidget *parent = nullptr);
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
    /**
     * @brief createNewScense　创建一个新的场景
     * @param scenceName 场景名字
     */
    CGraphicsView *createNewScenseByDragFile(QString ddfFile);
    void           createNewScenseByscencePath(QString scencePath);
    /**
     * @brief setCurrentView　设置活动场景
     * @param scenceName 场景名字
     */
    void setCurrentView(QString viewname);  //将被弃用

    /**
     * @brief setCurrentViewByUUID　设置活动场景
     * @param uuid 场景标识
     */
    void setCurrentViewByUUID(QString uuid);

    /**
     * @description: getAllTabBarName 获取当前所有的标签名字
    */
    QStringList getAllTabBarName();

    /**
     * @description: getAllTabBarUUID 获取当前所有的标签的uuid
    */
    QStringList getAllTabBarUUID();

    /**
     * @description: skipOpenedTab 跳转到已打开标签页
    */
    void skipOpenedTab(QString filepath);

signals:

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
     * @brief signalAddNewScence　创建新场景信号
     */
    void signalAddNewScence(CDrawScene *sence);
    /**
     * @brief signalCloseModifyScence　关闭当前修改窗口信号
     */
    void signalCloseModifyScence();
    /**
     * @brief signalDDFFileOpened　文件已经被打开信号
     * @param filename
     */
    void signalDDFFileOpened(QString filename);
    /**
     * @brief signalLastTabBarRequestClose　最后一个标签被关闭信号
     */
    void signalLastTabBarRequestClose();
    /**
     * @description: 关闭指定名字标签
     * @param:  viewNames 需要关闭的标签名字序列
     * @param:  uuids     需要关闭的标签uuid序列
    */
    void signalTabItemsCloseRequested(QStringList viewNames, const QStringList &uuids);
    /**
     * @description: 保存当前文件状态
    */
    void signalSaveFileStatus(bool);
    /**
     * @description: signalScenceViewChanged 当场景被改变后信号
    */
    void signalScenceViewChanged(QString viewName);
    /**
     * @description: signalChangeTittlebarWidget 更新顶部菜单控件
    */
    void signalChangeTittlebarWidget(int type);
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
     * @param isCloseNow 是否关闭当前保存的这个文件
     */
    void slotSaveToDDF(bool isCloseNow = false);
    /**
     * @brief slotDoNotSaveToDDF　不保存为ＤＤＦ文件
     */
    void slotDoNotSaveToDDF();
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
     * @param asFirstPictureSize 以第一张图片大小初始化场景
     */
    void slotPastePicture(QStringList picturePathList, bool asFirstPictureSize = false);
    /**
     * @brief slotPastePixmap　粘贴图片
     * @param pixmap　图片
     */
    void slotPastePixmap(QPixmap pixmap, const QByteArray &srcBytes);
    /**
     * @description: slotLoadDragOrPasteFile 当从拖拽或者粘贴板中加载数据
     * @param:  path 需要加载的路径
    */
    void slotLoadDragOrPasteFile(QString path);
    void slotLoadDragOrPasteFile(QStringList files);
    /**
     * @description: 新增加一个画板视图函数
     * @param:  viewName
     * @param:  uuid唯一标识
     * @return: 无
    */
    void addView(QString viewName, const QString &uuid);

    /**
     * @brief slotQuitApp 退出程序处理事件
     */
    void slotQuitApp();
    /**
     * @brief closeCurrentScenseView　关闭当前选中场景
     */
    void closeCurrentScenseView(bool ifTabOnlyOneCloseAqq = true, bool deleteView = true);


    void closeSceneView(CGraphicsView *pView, bool ifTabOnlyOneCloseAqq = true, bool deleteView = true);


    void closeViewScense(CGraphicsView *view);

    /*
    * @bref: slotJudgeCutStatusAndPopSaveDialog 判断当前是否需要进行提示保存裁剪状态
    * @bool: 返回是否需要执行后面的代码
    */
    bool slotJudgeCutStatusAndPopSaveDialog();

    /**
     * @brief loadFilesByCreateTag　创建一个图片大小的标签页
     * @param imagePaths　[QStringList] 图片路径
     * @param imageSize　[bool] 是否以打开图片的大小进行设置scence
     */
    void loadFilesByCreateTag(QStringList imagePaths, bool isImageSize = true);

private slots:
    /**
     * @brief slotDoSaveImage　执行保存图片
     * @param completePath　待保存路径
     */
    void slotDoSaveImage(QString completePath);

    /**
     * @description: 标签选择改变后，画板切换对应视图信号函数
     * @param:  viewName 改变对应的标签名字
     * @return: 无
    */
    void viewChanged(QString viewName, const QString &uuid);

    /**
     * @description: 当标签点击删除后的操作
     * @param:  viewName 需要关闭的标签名字
     * @param:  viewName 需要关闭的标签的uuid
     * @return: 无
    */
    void tabItemCloseRequested(QString viewName, const QString &uuid);

    /**
     * @brief currentScenseViewIsModify　当前场景状态被改变
     */
    void currentScenseViewIsModify(bool isModify);

    /**
     * @description: slotOnFileSaveFinished 对文件保存结束的响应槽
     * @param:  savedFile 文件名
     * @param:  success 是否成功
     * @param:  errorString 错误字符串
     * @param:  error 错误标记值
     * @param:  needClose 是否需要关闭这个文件
    */
    void slotOnFileSaveFinished(const QString &savedFile,
                                bool success,
                                QString errorString,
                                QFileDevice::FileError error,
                                bool needClose);

    /**
     * @brief updateTabName　当前场景状态被改变
     * @param:  uuid         根据这个标签页的uuid刷新名字
     * @param:  newTabName   要修改成的标签名字
     */
    void updateTabName(const QString &uuid, const QString &newTabName);

    /**
    * @bref: slotTransmitEndLoadDDF ddf图元被加载完成后触发信号
    */
    void slotTransmitEndLoadDDF();

public:
    /**
     * @brief updateTitle　刷新相关的标题
     */
    Q_SLOT void updateTitle();

private:
    CLeftToolBar *m_leftToolbar;

    CExportImageDialog *m_exportImageDialog;
    CPrintManager *m_printManager;

    CPictureTool *m_pictureTool;

    // 顶部多标签菜单栏
    CMultipTabBarWidget *m_topMutipTabBarWidget; // 顶部多标签栏组件
    QHBoxLayout *m_hLayout; // 水平布局
    QStackedLayout *m_stackedLayout; // 视图布局器
    int systemTheme = 0;// 保存系统主题
    bool m_isCloseNow; // 判断是否是ctrl+s保存

    QString m_tabDefaultName;
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
    QImage getSceneImage(int type);
    /**
     * @brief resetSceneBackgroundBrush　重置场景画刷
     */
    void resetSceneBackgroundBrush();
    /**
     * @brief createNewScense　创建一个新的场景
     * @param scenceName 场景名字
     */
    CGraphicsView *createNewScense(QString scenceName, const QString &uuid = "", bool isModified = false);
};

#endif // MAINWIDGET_H
