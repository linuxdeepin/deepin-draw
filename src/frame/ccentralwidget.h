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
#include <DFloatingMessage>
#include <DDialog>
#include <QDBusInterface>
#include <DFileDialog>

#include "drawshape/globaldefine.h"
#include "cdrawparamsigleton.h"
#include "cprogressdialog.h"

DWIDGET_USE_NAMESPACE

class DrawToolManager;
class PageView;
class PageScene;
class CPrintManager;
class ProgressLayout;
class CExportImageDialog;
class CPictureTool;
class TabBarWgt;

class DrawBoard;
class IDrawTool;
class PageContext;
class Page: public QWidget
{
    Q_OBJECT
public:
    Page(DrawBoard *parent = nullptr);

    Page(PageContext *context, DrawBoard *parent = nullptr);

    ~Page();

    bool isActivedPage()const;

    bool close(bool force = false);

    bool isModified() const;
    QString title() const;
    void    setTitle(const QString &title);

    DrawAttribution::SAttrisList currentAttris() const;
    void     setAttributionVar(int attri, const QVariant &var, int phase, bool autoCmdStack);
    QVariant defaultAttriVar(int attri) const;

    QString name()const;
    void setName(const QString &name);

    QString file()const;
    void    setFile(const QString &file)const;

    DrawBoard *borad() const;
    void setBorad(DrawBoard *borad);

    QRectF pageRect() const;
    void   setPageRect(const QRectF &rect);

    QString key() const;

    PageView *view()const;
    PageScene *scene() const;

    PageContext *context() const;
    void setContext(PageContext *contex);
    bool save(bool syn = false);
    bool saveAs(bool syn = false);
    bool saveToImage(bool syn = false);
    void updateContext();

    void setCurrentTool(int tool);
    int  currentTool()const;
    IDrawTool *currentTool_p() const;

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    int    _currentTool = 1;

    PageView  *_view  = nullptr;
    PageContext *_context = nullptr;

    DECLAREPRIVATECLASS(Page)
};

/**
 * @brief The CCentralwidget class 中间控件类
 */
namespace DrawAttribution {
class CAttributeManagerWgt;
}
class FilePageHander;
class CFileWatcher;
class CShapeMimeData;

class DrawBoard: public DWidget
{
    Q_OBJECT
public:
    DrawBoard(QWidget *parent = nullptr);
    ~DrawBoard();

    Page *currentPage() const;
    Page *firstPage()const;
    Page *nextPage(Page *page)const;
    Page *endPage()const;
    int count() const;

    bool isAnyPageModified() const;

    Q_SLOT void addPage(const QString &name = "");
    Q_SLOT void addPage(PageContext *pageCxt);
    Q_SLOT void addPage(Page *page);
    Q_SLOT bool closePage(const QString &key);
    Q_SLOT bool closePage(Page *page);
    Q_SLOT void setCurrentPage(const Page *page);
    Q_SLOT void setCurrentPage(const QString &key);

    Page *page(int index)const;
    Page *page(const QString &key)const;
    Page *getPageByFile(const QString &file) const;

    void  setPageName(Page *page, const QString &name);
    QString pageName(Page *page)const;

    QString pageTitle(Page *page) const;
    void    setPageTitle(Page *page, const QString &title);

    DrawToolManager *toolManager() const;

    void setAttributionWidget(DrawAttribution::CAttributeManagerWgt *widget);
    DrawAttribution::CAttributeManagerWgt *attributionWidget() const;
    DrawAttribution::SAttrisList currentAttris() const;
    QVariant defaultAttriVar(const Page *page, int attri) const;
    Q_SLOT void setDrawAttribution(int attris, const QVariant &var, int phase = EChanged, bool autoCmdStack = true);


    void initTools();
    int  currentTool()const;
    IDrawTool *currentTool_p()const;
    IDrawTool *tool(int tool)const;
    bool setCurrentTool(int tool);
    bool setCurrentTool(IDrawTool *tool);

    bool load(const QString &file, bool forcePageContext = false,
              bool syn = false, PageContext **out = nullptr, QImage *outImg = nullptr);

    bool savePage(Page *page, bool syn = false);
    FilePageHander *fileHander() const;
    CFileWatcher *fileWatcher() const;

    void setClipBoardShapeData(QMimeData *data);
    QMimeData *clipBoardShapeData()const;

    void setTouchFeelingEnhanceValue(int value);
    int  touchFeelingEnhanceValue()const;

    void zoomTo(qreal total);


    enum   EMessageType {ENormalMsg, EWarningMsg, EQuestionMsg};
    static int exeMessage(const QString &message,
                          EMessageType msgTp,
                          bool autoFitDialogWidth = true,
                          const QStringList &moreBtns = QStringList() << tr("OK"),
                          const QList<int> &btnType = QList<int>() << 0);

signals:
    void pageAdded(Page *page);
    void pageRemoved(Page *page);
    void currentPageChanged(Page *page);
    void currentPageChanged(const QString &pageKey);

    void modified(bool modified);
    void zoomValueChanged(qreal totalScaled);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *e)override;
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *o, QEvent *e) override;

private:

    Q_SLOT void onFileContextChanged(const QString &path, int tp);

private:
    DrawAttribution::CAttributeManagerWgt *_attriWidget = nullptr;

    FilePageHander *_fileHander = nullptr;
    CFileWatcher   *_fileWatcher = nullptr;
    CShapeMimeData *_pClipBordData = nullptr;

    DECLAREPRIVATECLASS(DrawBoard)
};

#endif // MAINWIDGET_H
