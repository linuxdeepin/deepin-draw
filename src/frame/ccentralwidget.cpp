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
#include "ccentralwidget.h"

#include "clefttoolbar.h"
#include "cgraphicsview.h"
#include "mainwindow.h"
#include "application.h"
#include "cshapemimedata.h"

#include "widgets/dialog/cexportimagedialog.h"
#include "widgets/dialog/cprintmanager.h"
#include "widgets/progresslayout.h"
#include "widgets/dialog/ccutdialog.h"

#include "drawshape/cdrawscene.h"
#include "bzItems/cgraphicsitem.h"
#include "bzItems/cpictureitem.h"
#include "drawTools/cpicturetool.h"
#include "bzItems/cgraphicstextitem.h"
#include "bzItems/cgraphicsellipseitem.h"
#include "bzItems/cgraphicstriangleitem.h"
#include "drawshape/cdrawparamsigleton.h"
#include "cgraphicsitemselectedmgr.h"
#include "drawTools/cpicturetool.h"
#include "drawTools/ccuttool.h"
#include "drawTools/cdrawtoolmanagersigleton.h"
#include "ccutwidget.h"
#include "toptoolbar.h"
#include "cprogressdialog.h"

#include "frame/cviewmanagement.h"
#include "frame/cmultiptabbarwidget.h"
#include "filehander.h"
#include "drawdialog.h"

#include <DMenu>
#include <DGuiApplicationHelper>

#include <QDebug>
#include <QGraphicsItem>
#include <QtConcurrent>
#include <QDesktopWidget>
#include <QApplication>
#include <DMessageManager>
#include <DFloatingMessage>
#include <QPdfWriter>
#include <QScreen>
#include <QWindow>
#include <QTextEdit>
#include <QClipboard>

#include <malloc.h>
#define NOTSHOWPROGRESS 1

DGUI_USE_NAMESPACE
class Page::Page_private
{
public:
    Page_private(Page *page): _page(page) {}
    inline bool &rForceClose()  {return _forceClose;}
private:
    Page *_page;
    bool _forceClose = false;
    bool _blockSettingDrawCursor = false;
    friend class Page;
};
class DrawBoard::DrawBoard_private
{
public:
    DrawBoard_private(DrawBoard *borad): _borad(borad)
    {
        initUi();

        //init signal and slots
        QObject::connect(_stackWidget, &QStackedWidget::currentChanged, _borad, [ = ](int index) {
            auto page = qobject_cast<Page *>(_stackWidget->widget(index));
            if (page != nullptr) {
                //1.set ui show page current tool.
                QSignalBlocker bloker(_toolManager);
                _toolManager->setCurrentTool(page->currentTool());

                //2.set ui show page current total scaled value.
                emit _borad->zoomValueChanged(page->view()->getScale());

                //3.set ui show page context.
                page->updateContext();

                //4. emit current page changed.
                CManageViewSigleton::GetInstance()->setCurView(page->view());
                emit _borad->currentPageChanged(page);
                emit _borad->currentPageChanged(page->key());
            }
        });

        connect(_toolManager, &DrawToolManager::currentToolChanged, _borad, [ = ](int oldTool, int nowTool) {
            if (_borad->currentPage() != nullptr) {
                _borad->currentPage()->setCurrentTool(nowTool);
            }
            if (_borad->currentPage() != nullptr) {
                _borad->currentPage()->updateContext();
            }
        });
        s_boards.append(_borad);
    }
    ~DrawBoard_private() {s_boards.removeOne(_borad);}
    void initUi()
    {
        //init layout
        _toolManager = new DrawToolManager(_borad);
        _topTabs     = new TabBarWgt(_borad);
        _stackWidget = new QStackedWidget(_borad);

        QHBoxLayout *hLay = new QHBoxLayout;
        hLay->setContentsMargins(0, 0, 0, 0);
        hLay->setSpacing(0);
        hLay->addWidget(_toolManager);

        QVBoxLayout *subVLay = new QVBoxLayout;
        subVLay->setContentsMargins(0, 0, 0, 0);
        subVLay->setSpacing(0);
        subVLay->addWidget(_topTabs);
        subVLay->addWidget(_stackWidget);

        hLay->addItem(subVLay);

        _borad->setLayout(hLay);
    }

    ProgressDialog *processDialog()
    {
        if (_dialog == nullptr) {
            _dialog = new ProgressDialog("", _borad);
//            QObject::connect(_dialog, &ProgressDialog::closed, _borad, [ = ]() {
//                _borad->fileHander()->quit();
//            });
        }
        return _dialog;
    }
    QString execFileSelectDialog(const QString &defualFileName, bool toddf = true)
    {
        if (toddf) {
            FileSelectDialog dialog(_borad);
            dialog.selectFile(defualFileName);
            dialog.setNameFilter("*.ddf");
            dialog.exec();
            return dialog.resultFile();
        }
        CExportImageDialog dialog(_borad);
        dialog.exec();
        return dialog.resultFile();
    }
    QString execCheckLoadingFileToSupName(const QString &file)
    {
        QString legeFile = FilePageHander::toLegalFile(file);

        if (legeFile.isEmpty()) {
            //mean file not exist.
            DrawBoard::exeMessage(tr("The file does not exist"), ENormalMsg, false);
        } else {
            QFileInfo info(legeFile);
            QString infoName = info.fileName();
            QFontMetrics font(_topTabs->font());
            infoName = font.elidedText(infoName, Qt::ElideMiddle, 200);//返回一个带有省略号的字符串

            if (info.isFile()) {
                const QString suffix = info.suffix().toLower();
                if (FilePageHander::supPictureSuffix().contains(suffix) || FilePageHander::supDdfStuffix().contains(suffix)) {
                    if (!info.isReadable()) {
                        DrawBoard::exeMessage(tr("Unable to open the write-only file \"%1\"").arg(infoName), ENormalMsg, false);
                    } else {
                        return legeFile;
                    }
                } else {
                    DrawBoard::exeMessage(tr("Unable to open \"%1\", unsupported file format").arg(infoName), ENormalMsg, false);
                }
            }
        }
        return "";
    }
    void addPageHelper(Page *page)
    {
        _stackWidget->addWidget(page);
        _topTabs->addItem(page->name(), page->key());

        emit _borad->pageAdded(page);
    }
    void closePageHelper(Page *page)
    {
        _stackWidget->removeWidget(page);
        _topTabs->removeItem(page->key());
        emit _borad->pageRemoved(page);
        checkClose();
    }
    bool isFocusFriendWgt(QWidget *w)
    {
        bool result = false;
        if (_borad->attributionWidget() != nullptr) {
            result = _borad->attributionWidget()->isLogicAncestorOf(w);
        }
//        if (!result && colorPickWidget() != nullptr) {
//            result = (colorPickWidget()->isAncestorOf(w) || colorPickWidget() == w);
//        }
        if (!result) {
            result = (qobject_cast<QMenu *>(dApp->activePopupWidget()) != nullptr);
        }
        return result;
    }

    void checkClose()
    {
        if (_borad->isAutoClose()) {
            QMetaObject::invokeMethod(_borad, [ = ]() {
                if (_borad->count() == 0 && _borad->_fileHander->activedCount() == 0
                        && qApp->activeModalWidget() == nullptr) {
                    _borad->close();
                }
            },
            Qt::QueuedConnection);
        }
    }


private:
    DrawBoard *_borad;

    DrawToolManager *_toolManager = nullptr;
    TabBarWgt       *_topTabs     = nullptr;
    QStackedWidget  *_stackWidget = nullptr;

    ProgressDialog *_dialog = nullptr;

    CExportImageDialog *_exportImageDialog = nullptr;

    int  _touchEnchValue = 7;
    bool _autoClose = false;

    static QList<DrawBoard *> s_boards;

    friend class DrawBoard;
};
QList<DrawBoard *> DrawBoard::DrawBoard_private::s_boards = QList<DrawBoard *>();

static QString genericOneKey()
{
    static int s_pageCount = 0;
    return QString("%1").arg(++s_pageCount);
}

Page::Page(DrawBoard *parent)
{
    _pPrivate = new Page_private(this);
    _view     = new PageView(this);
    _view->setFrameShape(QFrame::NoFrame);

    QHBoxLayout *hLay = new QHBoxLayout(this);
    hLay->setContentsMargins(0, 0, 0, 0);
    this->setLayout(hLay);
    hLay->addWidget(_view);

    if (parent != nullptr)
        setBorad(parent);

    connect(_view, &PageView::signalSetScale, this, [ = ](const qreal scale) {
        if (this == borad()->currentPage())
            emit borad()->zoomValueChanged(scale);
    });

    setWgtAccesibleName(this, QString("Page%1").arg(genericOneKey()));
}

Page::Page(PageContext *context, DrawBoard *parent): Page(parent)
{
    setContext(context);
}

Page::~Page()
{
    delete _pPrivate;
    _pPrivate = nullptr;
}

bool Page::isActivedPage() const
{
    return (this->borad()->currentPage() == this);
}

bool Page::close(bool force)
{
    d_pri()->rForceClose() = force;
    bool ret = QWidget::close();
    d_pri()->rForceClose() = false;
    return ret;
}

bool Page::isModified() const
{
    if (_context != nullptr)
        return _context->isDirty();
    return false;
}

QString Page::title() const
{
    auto name = this->name();
    if (name.isEmpty())
        return "";
    return isModified() ? ("* " + name) : name;
}

void Page::setTitle(const QString &title)
{
    borad()->setPageTitle(this, title);
}

SAttrisList Page::currentAttris() const
{
    if (_context != nullptr) {
        return _context->currentAttris();
    }

    SAttrisList result;
    //result << DrawAttribution::SAttri(DrawAttribution::ETitle, title());
    return result;
}

void Page::setAttributionVar(int attri, const QVariant &var, int phase, bool autoCmdStack)
{
    if (_context != nullptr) {
        _context->scene()->setAttributionVar(attri, var, phase, autoCmdStack);
    }
    setDefaultAttriVar(attri, var);
}

void Page::setDefaultAttriVar(int attri, const QVariant &var)
{
    if (_context != nullptr) {
        _context->setDefaultAttri(attri, var);
    }
}

QVariant Page::defaultAttriVar(int attri) const
{
    QVariant var;
    if (_context != nullptr) {
        var = _context->defaultAttri(attri);
    }
    if (var.isNull()) {
        if (borad() != nullptr && borad()->attributionWidget() != nullptr)
            var = borad()->attributionWidget()->defaultAttriVar(attri);
    }
    return var;
}

QString Page::name() const
{
    if (_context != nullptr)
        return _context->name();

    return "";
}

void Page::setName(const QString &name)
{
    borad()->setPageName(this, name);
}

QString Page::file() const
{
    if (_context != nullptr)
        return _context->file();
    return "";
}

void Page::setFile(const QString &file) const
{
    if (_context != nullptr)
        return _context->setFile(file);
}

DrawBoard *Page::borad() const
{
    if (parentWidget() != nullptr && parentWidget()->parentWidget() != nullptr) {
        return qobject_cast<DrawBoard *>(parentWidget()->parentWidget());
    }
    return nullptr;
}

void Page::setBorad(DrawBoard *borad)
{
    if (borad != nullptr)
        borad->addPage(this);
}

QRectF Page::pageRect() const
{
    if (_context != nullptr)
        return _context->pageRect();

    return QRectF(0, 0, 0, 0);
}

void Page::setPageRect(const QRectF &rect)
{
    if (_context != nullptr)
        return _context->setPageRect(rect);
}

QString Page::key() const
{
    if (_context != nullptr)
        return _context->key();
    return "";
}

PageView *Page::view() const
{
    return _view;
}

PageScene *Page::scene() const
{
    if (_context != nullptr) {
        return _context->scene();
    }
    return nullptr;
}

PageContext *Page::context() const
{
    return _context;
}

void Page::setContext(PageContext *contex)
{
    if (_context != contex) {
        _context = contex;
        _view->setScene(_context->scene());
    }
}

bool Page::save(bool syn, const QString &file)
{
    if (!isModified())
        return true;

    if (_context != nullptr) {
//        if (_context->isEmpty())
//            return true;

        QString f = file.isEmpty() ? _context->file() : file;
        if (f.isEmpty()) {
            f = borad()->d_pri()->execFileSelectDialog(_context->name());
        }
        if (f.isEmpty())
            return false;
        return _context->save(file, syn);
    }

    return false;
}

bool Page::saveAs(bool syn)
{
    if (_context != nullptr) {
        if (_context->isEmpty())
            return true;

        QString file = borad()->d_pri()->execFileSelectDialog(_context->name());
        if (file.isEmpty()) {
            return false;
        }
        return _context->save(file, syn);
    }

    return false;
}

bool Page::saveToImage(const QString &file, int qulity, bool syn)
{
    if (_context != nullptr) {
        //QString file = borad()->d_pri()->execFileSelectDialog(_context->name(), false);
        if (file.isEmpty()) {
            return false;
        }
        return _context->save(file, syn, qulity);
    }
    return false;
}

void Page::updateContext()
{
    if (_context != nullptr)
        _context->update();
}

void Page::setCurrentTool(int tool)
{
    if (_currentTool != tool) {
        _currentTool = tool;
        if (borad() != nullptr) {

            if (borad()->currentPage() == this)
                borad()->toolManager()->setCurrentTool(tool);

            auto pTool = borad()->toolManager()->tool(tool);
            if (pTool != nullptr) {
                setDrawCursor(pTool->cursor());
                this->view()->setFocus();
            }
        }
    }
}

int Page::currentTool() const
{
    return _currentTool;
}

IDrawTool *Page::currentTool_p() const
{
    if (borad() != nullptr)
        return borad()->toolManager()->tool(currentTool());

    return nullptr;
}

void Page::setDrawCursor(const QCursor &cursor)
{
    if (d_pri()->_blockSettingDrawCursor)
        return;
    view()->viewport()->setCursor(cursor);
}

QCursor Page::drawCursor() const
{
    return view()->viewport()->cursor();
}

void Page::blockSettingDrawCursor(bool b)
{
    d_pri()->_blockSettingDrawCursor = b;
}

void Page::closeEvent(QCloseEvent *event)
{
    if (d_pri()->rForceClose()) {
        event->accept();
        borad()->d_pri()->closePageHelper(this);
        return;
    }
    bool refuse = currentTool_p() != nullptr ? currentTool_p()->blockPageClose(this) : false;

    if (!refuse) {
        if (this->isModified()) {
            borad()->setCurrentPage(this);
            DrawDialog quitQuestionDialog(this);
            int ret = quitQuestionDialog.exec();
            if (ret == 2) {
                QString file = this->file();
                if (file.isEmpty()) {
                    file = borad()->d_pri()->execFileSelectDialog(_context->name());
                }
                if (file.isEmpty()) {
                    refuse = true;
                } else {
                    this->save(true, file);
                }

            } else if (ret <= 0) {
                refuse = true;
            }
        }
    }

    if (refuse) {
        event->ignore();
    } else {
        event->accept();
        borad()->d_pri()->closePageHelper(this);
    }
}


static int noticeFileContextChanged(Page *page, DrawBoard *borad)
{
    static QMap<QString, DDialog *> noticeDialogMap;
    static int reload = 0;
    static int cancel = 1;
    auto itf = noticeDialogMap.find(page->file());
    if (itf == noticeDialogMap.end()) {
        auto dia = new DDialog(borad);
        dia->setFixedSize(404, 163);
        dia->setAttribute(Qt::WA_DeleteOnClose);
        dia->setModal(true);
        dia->setIcon(QPixmap(":/icons/deepin/builtin/Bullet_window_warning.svg"));
        dia->setProperty("file", page->file());
        //设置显示文字与交互按钮
        QFileInfo fInfo(page->file());
        QString shortenFileName = QFontMetrics(dia->font()).elidedText(fInfo.fileName(), Qt::ElideMiddle, dia->width() / 2);
        dia->setMessage(QObject::tr("%1 has been modified in other programs. Do you want to reload it?").arg(shortenFileName));
        reload  = dia->addButton(QObject::tr("Reload"), false, DDialog::ButtonNormal);
        cancel  = dia->addButton(QObject::tr("Cancel"), false, DDialog::ButtonWarning);
        //设置message的两边间隙为10
        QWidget *pWidget = dia->findChild<QWidget *>("MessageLabel"); //如果dtk修改了object名字，记得修改
        if (pWidget != nullptr) {
            QMargins margins = pWidget->contentsMargins();
            margins.setLeft(10);
            margins.setRight(10);
            pWidget->setContentsMargins(margins);
        }
        noticeDialogMap[page->file()] = dia;
    } else {
        borad->activateWindow();
        return 2;
    }

    auto dialog = noticeDialogMap[page->file()];
    int ret = dialog->exec();
    noticeDialogMap.remove(page->file());
    if (ret == reload) {
        return 0;
    } else if (ret == cancel)
        return 1;

    return 2;
}

DrawBoard::DrawBoard(QWidget *parent): DWidget(parent)
{
    _pPrivate = new DrawBoard_private(this);

    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, [ = ]() {
        setClipBoardShapeData(const_cast<QMimeData *>(QApplication::clipboard()->mimeData()));
    });

    //install tools
    auto tools = CDrawToolFactory::allTools();
    foreach (auto tool, tools) {
        toolManager()->installTool(tool);
    }
    setCurrentTool(selection);

    //file hander init
    _fileHander = new FilePageHander(this);
    connect(_fileHander, &FilePageHander::loadBegin, this, [ = ]() {
        d_pri()->processDialog()->setText(tr("Opening..."));
        d_pri()->processDialog()->exec();
    });
    connect(_fileHander, &FilePageHander::loadUpdate, this, [ = ](int process, int total) {
        d_pri()->processDialog()->setProcess(process, total);
    });
    connect(_fileHander, QOverload<PageContext *, const QString &, const int>::of(&FilePageHander::loadEnd),
    this, [ = ](PageContext * cxt, const QString & error, const int messageType) {
        d_pri()->processDialog()->close();
        if (error.isEmpty() && messageType == 0) {
            if (cxt != nullptr)
                addPage(cxt);
            this->activateWindow();
        } else {
            if (cxt != nullptr)
                cxt->deleteLater();
            if (messageType != 1) {
                exeMessage(error, EWarningMsg, false);
            }
            d_pri()->checkClose();
        }
    });
    connect(_fileHander, QOverload<QImage, const QString &>::of(&FilePageHander::loadEnd),
    this, [ = ](QImage img, const QString & error) {
        d_pri()->processDialog()->close();
        if (!error.isEmpty()) {
            exeMessage(error, EWarningMsg, false);
            d_pri()->checkClose();
        } else {
            if (currentPage() != nullptr) {
                if (!img.isNull()) {
                    auto pos = currentPage()->context()->pageRect().center() - img.rect().center();
                    currentPage()->context()->scene()->clearSelectGroup();
                    currentPage()->context()->addImage(img, pos, true, true);
                    this->activateWindow();
                }
            } else {
                if (count() == 0) {
                    PageContext *cxt = new PageContext;
                    cxt->addImage(img);
                    addPage(cxt);
                }
            }
        }
    });

    connect(_fileHander, &FilePageHander::saveBegin, this, [ = ](PageContext * cxt) {
        fileWatcher()->removePath(cxt->file());
        d_pri()->processDialog()->setText(tr("Saving..."));
        d_pri()->processDialog()->exec();
    });
    connect(_fileHander, &FilePageHander::saveUpdate, this, [ = ](PageContext * cxt, int process, int total) {
        d_pri()->processDialog()->setProcess(process, total);
    });
    connect(_fileHander, &FilePageHander::saveEnd,
    this, [ = ](PageContext * cxt, const QString & error, const QImage & resultImg) {
        Q_UNUSED(resultImg)
        fileWatcher()->addWather(cxt->file());
        d_pri()->processDialog()->close();

        if (error.isEmpty())
            cxt->setDirty(false);
        else {
            //交互提示
            int ret = exeMessage(error, EWarningMsg, false);
            if (ret == 0) {
                if (resultImg.isNull()) {
                    //save error(find can't write),then should change path to retry again.
                    auto file = d_pri()->execFileSelectDialog(cxt->file());
                    if (!file.isEmpty())
                        cxt->save(file);
                }
            }
        }

        this->activateWindow();
    });

    _fileWatcher = new CFileWatcher();
    connect(_fileWatcher, &CFileWatcher::fileChanged, this, &DrawBoard::onFileContextChanged);

    this->setAcceptDrops(true);

    static int count = 0;
    setWgtAccesibleName(this, QString("DrawBoard%1").arg(++count));

    qApp->installEventFilter(this);
}

DrawBoard::~DrawBoard()
{
    delete _pPrivate;
    _pPrivate = nullptr;

    _fileWatcher->deleteLater();
    _fileWatcher = nullptr;

    if (_pClipBordData != nullptr) {
        delete _pClipBordData;
        _pClipBordData = nullptr;
    }
}

void DrawBoard::addPage(const QString &name)
{
    auto pageCxt = new PageContext(name);
    auto page = new Page(pageCxt);
    addPage(page);
}

void DrawBoard::addPage(PageContext *pageCxt)
{
    auto page = new Page(pageCxt);
    addPage(page);
}

void DrawBoard::addPage(Page *page)
{
    CManageViewSigleton::GetInstance()->addView(page->view());

    d_pri()->addPageHelper(page);

    fileWatcher()->addWather(page->file());

    setCurrentPage(page);
}

bool DrawBoard::closePage(const QString &key)
{
    auto page = this->page(key);
    return closePage(page);
}

bool DrawBoard::closePage(Page *page)
{
    return page->close();
}

Page *DrawBoard::currentPage() const
{
    return qobject_cast<Page *>(d_pri()->_stackWidget->currentWidget());
}

Page *DrawBoard::firstPage() const
{
    return page(0);
}

Page *DrawBoard::nextPage(Page *page) const
{
    int index = d_pri()->_topTabs->index(page->key());

    if (index == -1)
        return nullptr;

    ++index;

    auto key = d_pri()->_topTabs->key(index);

    return this->page(key);
}

Page *DrawBoard::endPage() const
{
    return page(count() - 1);
}

int DrawBoard::count() const
{
    return d_pri()->_stackWidget->count();
}

bool DrawBoard::isAnyPageModified() const
{
    for (int i = 0; i < count(); ++i) {
        auto page = this->page(i);
        if (page->isModified()) {
            return true;
        }
    }
    return false;
}

void DrawBoard::setCurrentPage(const Page *page)
{
    d_pri()->_stackWidget->setCurrentWidget(const_cast<Page *>(page));
    QSignalBlocker bloker(d_pri()->_topTabs);
    d_pri()->_topTabs->setCurrentIndex(d_pri()->_topTabs->index(page->key()));
}

void DrawBoard::setCurrentPage(const QString &key)
{
    auto page = this->page(key);

    if (page != nullptr)
        setCurrentPage(page);
}

Page *DrawBoard::page(int index) const
{
    auto key = d_pri()->_topTabs->key(index);
    return page(key);
}

Page *DrawBoard::page(const QString &key) const
{
    for (int i = 0; i < d_pri()->_stackWidget->count(); ++i) {
        auto page = qobject_cast<Page *>(d_pri()->_stackWidget->widget(i));
        if (page->key() == key) {
            return page;
        }
    }
    return nullptr;
}

Page *DrawBoard::getPageByFile(const QString &file) const
{
    auto temp = FilePageHander::toLegalFile(file);
    if (temp.isEmpty())
        return nullptr;

    for (int i = 0; i < d_pri()->_stackWidget->count(); ++i) {
        auto page = qobject_cast<Page *>(d_pri()->_stackWidget->widget(i));
        if (page->file() == temp) {
            return page;
        }
    }
    return nullptr;
}

void DrawBoard::setPageName(Page *page, const QString &name)
{
    if (page->context() != nullptr)
        page->context()->setName(name);

    setPageTitle(page, page->title());
}

QString DrawBoard::pageName(Page *page) const
{
    return page->name();
}

QString DrawBoard::pageTitle(Page *page) const
{
    return page->title();
}

void DrawBoard::setPageTitle(Page *page, const QString &title)
{
    auto tabIndex = d_pri()->_topTabs->index(page->key());
    d_pri()->_topTabs->setTabText(tabIndex, title);
    d_pri()->_topTabs->setTabToolTip(tabIndex, title);
}

DrawToolManager *DrawBoard::toolManager() const
{
    return d_pri()->_toolManager;
}

void DrawBoard::setAttributionWidget(DrawAttribution::CAttributeManagerWgt *widget)
{
    if (_attriWidget != nullptr) {
        disconnect(_attriWidget, &DrawAttribution::CAttributeManagerWgt::attributionChanged,
                   this, &DrawBoard::setDrawAttribution);
    }

    _attriWidget = widget;

    if (_attriWidget != nullptr)
        connect(_attriWidget, &DrawAttribution::CAttributeManagerWgt::attributionChanged,
                this, &DrawBoard::setDrawAttribution);
}

CAttributeManagerWgt *DrawBoard::attributionWidget() const
{
    return _attriWidget;
}

SAttrisList DrawBoard::currentAttris() const
{
    SAttrisList attris;

    //1.first check page attris(contxt attris).
    if (currentPage() != nullptr && currentTool() == selection) {
        attris = currentPage()->currentAttris();
    }

    //2.second check tools attris(contxt attris).
    if (attris.isEmpty()) {
        if (currentTool_p() != nullptr) {
            attris = currentTool_p()->attributions();
        }
    }

    //3.third check current page title attri.
    if (attris.isEmpty()) {
        if (currentPage() != nullptr)
            attris << DrawAttribution::SAttri(DrawAttribution::ETitle, currentPage()->title());
    }

    //4.finish show application title.
    if (attris.isEmpty()) {
        attris << DrawAttribution::SAttri(DrawAttribution::ETitle, qApp->applicationName());
    }
    return attris;
}

QVariant DrawBoard::defaultAttriVar(const Page *page, int attri) const
{
    if (page == nullptr) {
        return _attriWidget->defaultAttriVar(attri);
    }
    return page->defaultAttriVar(attri);
}

void DrawBoard::initTools()
{
    toolManager()->registerAllTools();
}

int DrawBoard::currentTool() const
{
    return toolManager()->currentTool();
}

IDrawTool *DrawBoard::currentTool_p() const
{
    return toolManager()->tool(toolManager()->currentTool());
}

IDrawTool *DrawBoard::tool(int tool) const
{
    return toolManager()->tool(tool);
}

bool DrawBoard::setCurrentTool(int tool)
{
    return toolManager()->setCurrentTool(tool);
}

bool DrawBoard::setCurrentTool(IDrawTool *tool)
{
    return toolManager()->setCurrentTool(tool);
}

bool DrawBoard::load(const QString &file, bool forcePageContext,
                     bool syn, PageContext **out, QImage *outImg)
{
    auto filePath = d_pri()->execCheckLoadingFileToSupName(file);

    if (filePath.isEmpty())
        return false;

    auto page = getPageByFile(filePath);
    if (page == nullptr)
        return _fileHander->load(filePath, forcePageContext, syn, out, outImg);

    setCurrentPage(page);

    return false;
}

bool DrawBoard::savePage(Page *page, bool syn)
{
    if (page != nullptr)
        return page->save(syn);

    return false;
}

FilePageHander *DrawBoard::fileHander() const
{
    return _fileHander;
}

CFileWatcher *DrawBoard::fileWatcher() const
{
    return _fileWatcher;
}

void DrawBoard::setClipBoardShapeData(QMimeData *data)
{
    if (_pClipBordData != nullptr) {
        delete _pClipBordData;
        _pClipBordData = nullptr;
    }

    if (data != nullptr) {
        auto shapeData = qobject_cast<CShapeMimeData *>(data);
        if (shapeData != nullptr) {
            _pClipBordData = shapeData;
        }
    }
}

QMimeData *DrawBoard::clipBoardShapeData() const
{
    if (_pClipBordData != nullptr)
        return _pClipBordData;

    return const_cast<QMimeData *>(QApplication::clipboard()->mimeData());
}

void DrawBoard::setTouchFeelingEnhanceValue(int value)
{
    d_pri()->_touchEnchValue = value;
}

int DrawBoard::touchFeelingEnhanceValue() const
{
    return d_pri()->_touchEnchValue;
}

void DrawBoard::zoomTo(qreal total)
{
    if (currentPage() != nullptr) {
        currentPage()->view()->scale(total);
    }
}

void DrawBoard::setAutoClose(bool b)
{
    d_pri()->_autoClose = b;
}

bool DrawBoard::isAutoClose() const
{
    return d_pri()->_autoClose;
}
int DrawBoard::exeMessage(const QString &message,
                          EMessageType msgTp,
                          bool autoFitDialogWidth,
                          const QStringList &moreBtns,
                          const QList<int> &btnType)
{
    auto widgets = DrawBoard_private::s_boards;
    DDialog dia(widgets.isEmpty() ? nullptr : widgets.first());
    dia.setFixedSize(404, 163);
    dia.setModal(true);
    dia.setMessage(message);

    QString iconSvg;
    switch (msgTp) {
    case ENormalMsg:
        iconSvg = ":/theme/common/images/deepin-draw-64.svg";
        break;
    case EWarningMsg:
        iconSvg = ":/icons/deepin/builtin/texts/Bullet_window_warning.svg";
        break;
    case EQuestionMsg:
        iconSvg = ":/icons/deepin/builtin/texts/Bullet_window_warning.svg";
        break;
    }
    dia.setIcon(QPixmap(iconSvg));

    if (moreBtns.size() == btnType.size())
        for (int i = 0; i < moreBtns.size(); ++i)
            dia.addButton(moreBtns.at(i), false, DDialog::ButtonType(btnType.at(i)));

    //保持弹窗在主窗口中心
    QMetaObject::invokeMethod(&dia, [ =, &dia]() {
        QMetaObject::invokeMethod(&dia, [ =, &dia]() {

            QPoint centerPos = dia.parentWidget()->window()->geometry().center() - dia.geometry().center();
            QRect parentWindowGem = dia.parentWidget()->window()->geometry();

            centerPos = parentWindowGem.topLeft() + QPoint((parentWindowGem.width() - dia.width()) / 2,
                                                           (parentWindowGem.height() - dia.height()) / 2);

            dia.move(centerPos);
        }, Qt::QueuedConnection);
    }, Qt::QueuedConnection);
    return dia.exec();
}

void DrawBoard::dragEnterEvent(QDragEnterEvent *e)
{
    DWidget::dragMoveEvent(e);
}

void DrawBoard::dragMoveEvent(QDragMoveEvent *event)
{
    DWidget::dragMoveEvent(event);
}

void DrawBoard::dropEvent(QDropEvent *e)
{
    DWidget::dropEvent(e);
}

void DrawBoard::closeEvent(QCloseEvent *event)
{
    bool refuse = false;
    Page *loopPage = this->firstPage();
    while (loopPage != nullptr) {
        if (!closePage(loopPage)) {
            refuse = true;
            break;
        }
        loopPage = this->firstPage();
    }
    if (refuse)
        event->ignore();
    else {
        event->accept();
        emit toClose();
    }
}

bool DrawBoard::eventFilter(QObject *o, QEvent *e)
{
    if (d_pri() == nullptr)
        return DWidget::eventFilter(o, e);

    if (e->type() == QEvent::FocusOut) {
        //1.如果丢失焦点的是view，那么如果当前view有激活的代理widget且新的当前的焦点控件是焦点友好的(如属性设置界面或者颜色板设置控件)那么应该不丢失焦点
        auto currentFocus = qApp->focusWidget();

        //当前焦点和丢失焦点是一样的那么什么都不用处理
        if (currentFocus == o)
            return true;

        // 解决打开不支持的文件时，页面崩溃的问题
        if (currentPage() == nullptr)
            return false;

        //qDebug() << "currentFocus ========= " << currentFocus << "foucus out o = " << o;
        auto currenView = currentPage()->view();
        if (currenView == o) {
            if (currenView->activeProxWidget() != nullptr && d_pri()->isFocusFriendWgt(currentFocus)) {
                auto activeWgtFocusWgt = currenView->activeProxWidget();
                if (activeWgtFocusWgt != nullptr && qobject_cast<QTextEdit *>(activeWgtFocusWgt) != nullptr) {
                    auto textEditor = qobject_cast<QTextEdit *>(activeWgtFocusWgt);
                    textEditor->setTextInteractionFlags(textEditor->textInteractionFlags() & (~Qt::TextEditable));
                }
                return true;
            }
        }
        //2.如果丢失焦点的是属性界面的控件，如果当前的焦点控件不是焦点友好的(如属性设置界面或者颜色板设置控件)那么需要
        else if (d_pri()->isFocusFriendWgt(qobject_cast<QWidget *>(o))) {
            if (currenView->activeProxWidget() != nullptr) {
                bool focusToView = false;
                if (!d_pri()->isFocusFriendWgt(currentFocus) && currentFocus != currenView) {
                    QFocusEvent *event = static_cast<QFocusEvent *>(e);
                    if (event->reason() == Qt::TabFocusReason) {
                        focusToView = true;
                        currentFocus->clearFocus();
                        currenView->setFocus();
                    } else
                        currenView->activeProxWidget()->clearFocus();
                } else {
                    if (currentFocus == currenView) {
                        focusToView = true;
                    }
                }
                if (focusToView) {
                    auto activeWgtFocusWgt = currenView->activeProxWidget();
                    if (activeWgtFocusWgt != nullptr && qobject_cast<QTextEdit *>(activeWgtFocusWgt) != nullptr) {
                        auto textEditor = qobject_cast<QTextEdit *>(activeWgtFocusWgt);
                        textEditor->setTextInteractionFlags(textEditor->textInteractionFlags() | (Qt::TextEditable));
                    }
                }
            }
        }
    } else if (e->type() == QEvent::FocusIn) {
        static bool sss = false;
        if (currentPage() != nullptr && o == currentPage()->view() && !sss) {
            if (currentPage()->view()->activeProxWidget() != nullptr) {
                sss = true;
                currentPage()->view()->setFocus();
                currentPage()->view()->activeProxWidget()->setFocus();
                sss = false;
                return true;
            }
        }
    } else if (e->type() == QEvent::Shortcut) {
        if (currentTool_p() != nullptr && currentTool_p()->isWorking()) {
            return true;
        }
        QMetaObject::invokeMethod(this, [ = ]() {
            if (currentTool_p() != nullptr) {
                currentTool_p()->refresh();
            }
        }, Qt::QueuedConnection);
        //QMetaObject::invokeMethod(currentTool_p(), &IDrawTool::refresh, Qt::QueuedConnection);
    }

    return DWidget::eventFilter(o, e);
}

void DrawBoard::setDrawAttribution(int attris, const QVariant &var, int phase, bool autoCmdStack)
{
    if (currentPage() != nullptr) {
        auto tool = currentPage()->currentTool_p();
        if (tool != nullptr)
            tool->setAttributionVar(attris, var, phase, autoCmdStack);

        currentPage()->setDefaultAttriVar(attris, var);
    }
}

void DrawBoard::onFileContextChanged(const QString &path, int tp)
{
    auto page = getPageByFile(path);
    if (page != nullptr) {
        QFileInfo info(path);
        if (info.exists()) {
            enum {EReload, ECLOSE, EJustActiveWindow};
            int ret = noticeFileContextChanged(page, this);
            switch (ret) {
            case EReload: {
                bool autoClose = this->isAutoClose();
                this->setAutoClose(false);
                page->close(true);
                this->setAutoClose(autoClose);
                load(path, true);
                break;
            }
            case ECLOSE: {
                page->close(true);
                break;
            }
            default: {
                this->activateWindow();
            }
            }
        } else {
            fileWatcher()->removePath(path);
            page->setFile("");
            page->context()->setDirty(true);
        }
    }
}
