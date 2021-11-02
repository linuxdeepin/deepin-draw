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
#include <QCheckBox>


//#include <DGioFileInfo>

#include <malloc.h>
#define NOTSHOWPROGRESS 1

DGUI_USE_NAMESPACE
class Page::Page_private
{
public:
    explicit Page_private(Page *page): _page(page) {}
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
    explicit DrawBoard_private(DrawBoard *borad): _borad(borad)
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

    ProgressLayout  *processDialog()
    {
        if (_dialog == nullptr) {
            _dialog = new ProgressLayout(_borad);
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
        QString legeFile = FileHander::toLegalFile(file);

        if (legeFile.isEmpty()) {
            //mean file not exist.
            MessageDlg::execMessage(tr("The file does not exist"), ENormalMsg);
        } else {
            QFileInfo info(legeFile);
            QString infoName = info.fileName();
            QFontMetrics font(_topTabs->font());
            infoName = font.elidedText(infoName, Qt::ElideMiddle, 200);//返回一个带有省略号的字符串

            if (info.isFile()) {
                const QString suffix = info.suffix().toLower();
                if (FileHander::supPictureSuffix().contains(suffix) || FileHander::supDdfStuffix().contains(suffix)) {
                    if (!info.isReadable()) {
                        MessageDlg::execMessage(tr("Unable to open the write-only file \"%1\"").arg(infoName), ENormalMsg);
                    } else {
                        return legeFile;
                    }
                } else {
                    MessageDlg::execMessage(tr("Unable to open \"%1\", unsupported file format").arg(infoName), ENormalMsg);
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
        if (!result) {
            result = (qobject_cast<QMenu *>(dApp->activePopupWidget()) != nullptr);
        }
        return result;
    }

    void checkClose()
    {
        if (_borad->isAutoClose()) {
            QMetaObject::invokeMethod(_borad, [ = ]() {
                if (_borad->count() == 0
                        && qApp->activeModalWidget() == nullptr) {
                    _borad->close();
                }
            },
            Qt::QueuedConnection);
        }
    }

    void showErrorMsg(int error, const QString &describle)
    {
        qWarning() << "load result = " << error << describle;
        if (error != FileHander::NoError && (FileHander::EUserCancelLoad_OldPen != error && FileHander::EUserCancelLoad_OldBlur != error)) {
            MessageDlg::execMessage(describle, ENormalMsg);
        }
    }

private:
    DrawBoard *_borad;

    DrawToolManager *_toolManager = nullptr;
    TabBarWgt       *_topTabs     = nullptr;
    QStackedWidget  *_stackWidget = nullptr;
    ProgressLayout *_dialog = nullptr;

    CExportImageDialog *_exportImageDialog = nullptr;

    int  _touchEnchValue = 7;
    bool _autoClose = false;

    static QList<DrawBoard *> s_boards;

    friend class DrawBoard;
};
QList<DrawBoard *> DrawBoard::DrawBoard_private::s_boards = QList<DrawBoard *>();

extern QWidget *defaultParentWindow();
bool adaptImgPosAndRect(PageScene *pScene, const QString &imgName, const QImage &img, QPointF &pos, QRectF &rect, int &choice)
{
    if (nullptr == pScene)
        return false;

    QSizeF sceneSize = QSizeF(pScene->width(), pScene->height());
    bool bAddImg = true;
    if (sceneSize.width() < img.width() || sceneSize.height() < img.height()) {
        QString tmpName = imgName.isEmpty() ? QObject::tr("Unnamed") : imgName;

        auto btns = QStringList() << QObject::tr("Keep original size") << QObject::tr("Auto fit");

        int ret = choice;
        static const int cancelImport = -2;//取消导入标识
        if (-1 == choice) {
            auto parent = (pScene->page() != nullptr ? pScene->page()->borad() : defaultParentWindow());
            MessageDlg msgDlg(parent);
            QCheckBox *pBox = new QCheckBox(QObject::tr("Apply to all"));

            msgDlg.addContent(pBox);

            msgDlg.setMessage(SMessage(QObject::tr("The dimensions of %1 exceed the canvas. How to display it?").arg(tmpName),
                                       EWarningMsg, QStringList() << QObject::tr("Keep original size") << QObject::tr("Auto fit"),
                                       QList<EButtonType>() << ENormalMsgBtn << ESuggestedMsgBtn));

            ret = msgDlg.exec();
            if (pBox->isChecked() && -1 != ret) {
                choice = ret;
            } else if (pBox->isChecked() && -1 == ret) {
                //取消后面导入比场景大的图
                choice = cancelImport;
            }
        } else if (cancelImport == choice) {
            ret = -1;
        }

        if (1 == ret) {
            double wRatio = 1.0 * sceneSize.width() / img.width();
            double hRatio = 1.0 * sceneSize.height() / img.height();
            double scaleRatio = wRatio > hRatio ? hRatio : wRatio;
            rect = QRectF(QPointF(0, 0), img.size() * scaleRatio);
            QPointF tmppos = pScene->sceneRect().center() - rect.center();
            pos.setX(tmppos.x());
            pos.setY(tmppos.y());
        } else if (-1 == ret) {
            bAddImg = false;
        }


    }
    return bAddImg;
}

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

bool Page::save(const QString &file)
{
    bool refuse = currentTool_p() != nullptr ? currentTool_p()->blockPageBeforeOutput(this) : false;
    if (refuse) {
        return false;
    }

    if (!isModified())
        return true;

    if (_context != nullptr) {
        QString f = file.isEmpty() ? _context->file() : file;
        if (f.isEmpty()) {
            f = borad()->d_pri()->execFileSelectDialog(_context->name());
        }
        if (f.isEmpty())
            return false;
        bool result = _context->save(f);


        qWarning() << "save result = " << (borad()->fileHander()->lastError()) << (borad()->fileHander()->lastErrorDescribe());
        if (!result) {
            auto error = borad()->fileHander()->lastError();
            if (error != FileHander::NoError) {
                MessageDlg::execMessage(borad()->fileHander()->lastErrorDescribe(), ENormalMsg, borad());

                if (error == FileHander::EUnWritableFile || error == FileHander::EUnWritableDir) {
                    auto anotherFile = borad()->d_pri()->execFileSelectDialog(_context->name());
                    if (!anotherFile.isEmpty())
                        result = save(anotherFile);
                }
            }
        }

        return result;
    }

    return false;
}

bool Page::saveAs()
{
    if (_context != nullptr) {
        if (_context->isEmpty())
            return true;

        bool refuse = currentTool_p() != nullptr ? currentTool_p()->blockPageBeforeOutput(this) : false;
        if (refuse) {
            return false;
        }

        QString file = borad()->d_pri()->execFileSelectDialog(_context->name());
        if (file.isEmpty()) {
            return false;
        }
        bool result = _context->save(file);
        qWarning() << "save result = " << (borad()->fileHander()->lastError()) << (borad()->fileHander()->lastErrorDescribe());
        if (!result) {
            auto error = borad()->fileHander()->lastError();
            if (error != FileHander::NoError) {
                MessageDlg::execMessage(borad()->fileHander()->lastErrorDescribe(), ENormalMsg, borad());

                if (error == FileHander::EUnWritableFile) {
                    auto anotherFile = borad()->d_pri()->execFileSelectDialog(_context->name());
                    if (!anotherFile.isEmpty())
                        result = save(anotherFile);
                }
            }
        }
        return result;
    }

    return false;
}

bool Page::saveToImage(const QString &file, const QSize &desImageSize, int qulity) const
{
    if (_context != nullptr) {
        if (file.isEmpty()) {
            return false;
        }
        return borad()->fileHander()->saveToImage(_context, file, desImageSize, qulity);
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
    bool refuse = currentTool_p() != nullptr ? currentTool_p()->blockPageBeforeOutput(this) : false;

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
                    bool success = this->save(file);
                    if (!success) {
                        auto error = borad()->fileHander()->lastError();
                        if (error == FileHander::EUnWritableFile || error == FileHander::EUnWritableDir) {
                            refuse = true;
                        }
                    }
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

void Page::adjustViewScaleRatio()
{
    if (nullptr != view()) {
        int viewportWidth = view()->viewport()->width();
        int viewportHeight = view()->viewport()->height();
        qreal sceneWidth = scene()->width();
        qreal sceneHeight = scene()->height();
        if (viewportWidth < sceneWidth || viewportHeight < sceneHeight) {
            qreal wRatio = qFloor(100.0 * viewportWidth / sceneWidth);
            qreal hRatio = qFloor(100.0 * viewportHeight / sceneHeight);
            qreal rsRatio = wRatio > hRatio ? hRatio / 100 : wRatio / 100;
            view()->scale(rsRatio);
        }
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
    _fileHander = new FileHander(this);
//    connect(_fileHander, &FilePageHander::loadBegin, this, [ = ]() {
//        d_pri()->processDialog()->setText(tr("Opening..."));
//        d_pri()->processDialog()->exec();
//    });
//    connect(_fileHander, &FilePageHander::loadUpdate, this, [ = ](int process, int total) {
//        d_pri()->processDialog()->setProcess(process, total);
//        qApp->processEvents();
//    });
//    connect(_fileHander, QOverload<PageContext *, const QString &, const int>::of(&FilePageHander::loadEnd),
//    this, [ = ](PageContext * cxt, const QString & error, const int messageType) {
//        d_pri()->processDialog()->close();
//        if (error.isEmpty() && messageType == 0) {
//            if (cxt != nullptr)
//                addPage(cxt);
//            this->activateWindow();
//        } else {
//            if (cxt != nullptr)
//                cxt->deleteLater();
//            if (messageType != 1) {
//                exeMessage(error, EWarningMsg, false);
//            }
//            d_pri()->checkClose();
//        }
//    });
//    connect(_fileHander, QOverload<QImage, const QString &>::of(&FilePageHander::loadEnd),
//    this, [ = ](QImage img, const QString & error) {
//        d_pri()->processDialog()->close();
//        if (!error.isEmpty()) {
//            exeMessage(error, EWarningMsg, false);
//            d_pri()->checkClose();
//        } else {
//            if (currentPage() != nullptr) {
//                if (!img.isNull()) {
//                    auto pos = currentPage()->context()->pageRect().center() - img.rect().center();
//                    currentPage()->context()->scene()->clearSelectGroup();
//                    currentPage()->context()->addImage(img, pos, true, true);

//                    currentPage()->setCurrentTool(selection);
//                    this->activateWindow();
//                }
//            } else {
//                if (count() == 0) {
//                    PageContext *cxt = new PageContext;
//                    cxt->addImage(img);
//                    addPage(cxt);
//                }
//            }
//        }
//    });

//    connect(_fileHander, &FilePageHander::saveBegin, this, [ = ](PageContext * cxt) {
//        fileWatcher()->removePath(cxt->file());
//        d_pri()->processDialog()->setText(tr("Saving..."));
//        d_pri()->processDialog()->exec();
//    });
//    connect(_fileHander, &FilePageHander::saveUpdate, this, [ = ](PageContext * cxt, int process, int total) {
//        d_pri()->processDialog()->setProcess(process, total);
//    });
//    connect(_fileHander, &FilePageHander::saveEnd,
//    this, [ = ](PageContext * cxt, const QString & error, const QImage & resultImg) {
//        Q_UNUSED(resultImg)
//        fileWatcher()->addWather(cxt->file());
//        d_pri()->processDialog()->close();

//        if (error.isEmpty())
//            cxt->setDirty(false);
//        else {
//            //交互提示
//            int ret = exeMessage(error, EWarningMsg, false);
//            if (ret == 0) {
//                if (resultImg.isNull()) {
//                    //save error(find can't write),then should change path to retry again.
//                    auto file = d_pri()->execFileSelectDialog(cxt->file());
//                    if (!file.isEmpty())
//                        cxt->save(file);
//                }
//            }
//        }

//        this->activateWindow();
//    });
    connect(_fileHander, &FileHander::progressBegin, this, [ = ](const QString & describe) {
        d_pri()->processDialog()->reset();
        d_pri()->processDialog()->showInCenter(this);
        d_pri()->processDialog()->setText(describe);
    });
    connect(_fileHander, &FileHander::progressChanged, this, [ = ](int progress, int total, const QString & describe) {
        if (!describe.isEmpty())
            d_pri()->processDialog()->setText(describe);

        d_pri()->processDialog()->setRange(0, total);
        d_pri()->processDialog()->setSubText("");
        d_pri()->processDialog()->setProgressValue(progress);
    });
    connect(_fileHander, &FileHander::progressEnd, this, [ = ](bool success, const QString & describe) {
        d_pri()->processDialog()->delayClose();
    });
    connect(_fileHander, &FileHander::message_waitAnswer, this, [ = ](const SMessage & message, int &retureRet) {
        retureRet = MessageDlg::execMessage(message);
    });

    _fileWatcher = new CFileWatcher();
    connect(_fileWatcher, &CFileWatcher::fileChanged, this, &DrawBoard::onFileContextChanged);

    this->setAcceptDrops(true);

    static int count = 0;
    setWgtAccesibleName(this, QString("DrawBoard%1").arg(++count));

    qApp->installEventFilter(this);


//    connect(qApp, &QApplication::focusChanged, this, [ = ](QWidget * old, QWidget * now) {
//        static bool b = false;
//        if (b)
//            return;
//        if (d_pri()->isFocusFriendWgt(old)) {
//            if (!d_pri()->isFocusFriendWgt(now)) {
//                b = true;
//                currentPage()->view()->setFocus();
//                currentPage()->view()->captureFocus();
//                b = false;
//            }
//        }
//    });
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

Page *DrawBoard::addPage(const QString &name)
{
    auto pageCxt = new PageContext(name);
    auto page = new Page(pageCxt);
    addPage(page);

    return page;
}

Page *DrawBoard::addPage(const QImage &img)
{
    if (!img.isNull()) {
        PageContext *contex = new PageContext;
        contex->addImage(img, QPointF(), QRectF(0, 0, img.width(), img.height()));
        return addPage(contex);
    }
    return nullptr;
}

Page *DrawBoard::addPage(PageContext *pageCxt)
{
    if (pageCxt == nullptr)
        return nullptr;
    auto page = new Page(pageCxt);
    addPage(page);
    return page;
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
    if (page == nullptr)
        return;

    d_pri()->_stackWidget->setCurrentWidget(const_cast<Page *>(page));
    QSignalBlocker bloker(d_pri()->_topTabs);
    d_pri()->_topTabs->setCurrentIndex(d_pri()->_topTabs->index(page->key()));
    this->activateWindow();
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
    auto temp = FileHander::toLegalFile(file);
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


template <class Fun>
void doMyRun(Fun fc, bool syn = false)
{
    if (syn) {
        fc();
    } else {
        QtConcurrent::run([ = ]() {
            fc();
        });
    }
}

void DrawBoard::loadFiles(QStringList filePaths, bool bInThread,  int loadTypeForImage, bool quitIfAllFialed)
{
    doMyRun([ = ]() {
        QMetaObject::invokeMethod(this, [ = ]() {
            if (filePaths.size() > 0) {
                d_pri()->processDialog()->reset();
                d_pri()->processDialog()->setText(QObject::tr("Opening..."));
                d_pri()->processDialog()->setRange(0, filePaths.count());
                d_pri()->processDialog()->setAutoFillSubText(false);
                d_pri()->processDialog()->showInCenter(this);
            }
        }, Qt::AutoConnection);

        Qt::ConnectionType  connectType = bInThread ? Qt::BlockingQueuedConnection : Qt::DirectConnection;
        FileHander hander;
        connect(&hander, &FileHander::message_waitAnswer, this, [ = ](const SMessage & message, int &retureRet) {
            retureRet = MessageDlg::execMessage(message);
        }, connectType);

        int lastChoice = -1;
        bool loaded = false;
        int i = 0;
        foreach (auto path, filePaths) {
            QMetaObject::invokeMethod(this, [ = ]() {
                d_pri()->processDialog()->setProgressValue(i);
            }, Qt::AutoConnection);
            i++;

            QFileInfo info(path);
            auto stuffix = info.suffix();
            if (FileHander::supDdfStuffix().contains(stuffix)) {

                Page *pg = nullptr;
                QMetaObject::invokeMethod(this, [ =, &pg]() {
                    pg = getPageByFile(path);
                }, connectType);

                if (pg != nullptr) {
                    if (i == filePaths.count()) {
                        setCurrentPage(pg);
                    }
                    continue;
                }

                PageContext *p = hander.loadDdf(path);
                if (nullptr == p) {
                    QMetaObject::invokeMethod(this, [ =, &hander]() {
                        d_pri()->showErrorMsg(hander.lastError(), hander.lastErrorDescribe());
                    }
                    , connectType);
                    continue;
                }
                loaded = true;

                QMetaObject::invokeMethod(this, [ = ]() {
                    addPage(p);
                }, Qt::AutoConnection);
            } else {
                QImage img = hander.loadImage(path);
                QSize maxSize = Application::drawApplication()->maxPicSize();
                if (img.isNull()) {
                    QMetaObject::invokeMethod(this, [ =, &hander]() {
                        d_pri()->showErrorMsg(hander.lastError(), hander.lastErrorDescribe());
                    }
                    , connectType);
                    continue;
                }

                if (img.size().width() > maxSize.width() || img.size().height() > maximumSize().height()) {

                    int ret = 0;
                    emit hander.message_waitAnswer(SMessage(QObject::tr("Import failed: no more than 10,000 pixels please"), EWarningMsg), ret);
                    continue;
                }

                loaded = true;
                QMetaObject::invokeMethod(this, [ =, &lastChoice]() {

                    if (nullptr == currentPage()) {
                        addPage("");
                        currentPage()->setPageRect(QRectF(QPointF(0, 0), img.size()));
                    }

                    if (0 == loadTypeForImage) {
                        QRectF sceneRect = currentPage()->pageRect();
                        QRectF newRect = sceneRect.adjusted(
                                             (sceneRect.width() > img.width() ? 0 : (sceneRect.width() - img.width()) / 2),
                                             (sceneRect.height() > img.height() ? 0 : (sceneRect.height() - img.height()) / 2),
                                             (sceneRect.width() > img.width() ? 0 : -(sceneRect.width() - img.width()) / 2),
                                             (sceneRect.height() > img.height() ? 0 : -(sceneRect.height() - img.height()) / 2)
                                         );
                        currentPage()->setPageRect(newRect);
                        currentPage()->context()->scene()->clearSelectGroup();
                        currentPage()->context()->addImage(img, QPointF(), QRectF(), true, true);
                        currentPage()->adjustViewScaleRatio();
                    } else if (1 == loadTypeForImage) {
                        QPointF pos;
                        QRectF rect;
                        if (adaptImgPosAndRect(currentPage()->scene(), info.fileName(), img, pos, rect, lastChoice)) {
                            currentPage()->context()->scene()->clearSelectGroup();
                            currentPage()->context()->addImage(img, pos, rect, true, true);
                        }
                    }

                }, connectType);
            }
        }

        QMetaObject::invokeMethod(this, [ = ]() {
            d_pri()->processDialog()->setProgressValue(i);
            d_pri()->processDialog()->delayClose();
            if (!loaded && quitIfAllFialed) {
                drawApp->quitApp();
            }

        }, Qt::AutoConnection);
    }, !bInThread);
}

bool DrawBoard::load(const QString &file)
{
    auto filePath = file;

    if (filePath.isEmpty())
        return false;

    bool ret = false;
    QFileInfo info(filePath);
    auto stuffix = info.suffix();
    if (FileHander::supDdfStuffix().contains(stuffix)) {
        ret = loadDDf(filePath);
    } else {
        ret = loadImage(filePath);
    }

    return ret;
}

//bool DrawBoard::loadImage(const QString &file)
//{
//    auto filePath = d_pri()->execCheckLoadingFileToSupName(file);

//    if (filePath.isEmpty())
//        return false;


//    QImage img = _fileHander->loadImage(filePath);
//    auto page = currentPage();
//    if (page == nullptr) {
//        page = addPage(img);
//        setCurrentPage(page);
//    } else {
//        page->context()->addImage(img);
//    }

//    return !img.isNull();
//}

bool DrawBoard::savePage(Page *page)
{
    if (page != nullptr) {
        return page->save();
    }

    return false;
}

FileHander *DrawBoard::fileHander() const
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
//int DrawBoard::exeMessage(const QString &message,
//                          EMessageType msgTp,
//                          bool autoFitDialogWidth,
//                          const QStringList &moreBtns,
//                          const QList<int> &btnType)
//{

//    return execMessage(SMessage(message, msgTp, moreBtns, btnType));
//}

//int DrawBoard::execMessage(const SMessage &message)
//{
//    auto widgets = DrawBoard_private::s_boards;
//    int limitTextLenth = widgets.isEmpty() ? 1920 : widgets.first()->width();
//    MessageDlg dia(widgets.isEmpty() ? nullptr : widgets.first());
//    dia.setMinimumSize(403, 163);
//    dia.setModal(true);

//    auto showText = QFontMetrics(dia.font()).elidedText(message.message, Qt::ElideRight, limitTextLenth);

//    SMessage finalMessage =  message;
//    finalMessage.message = showText;
//    dia.setMessage(finalMessage);

//    int ret = dia.exec();

//    return ret;
//}

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

bool doFocusChanged(PageView *currentView, DrawBoard::DrawBoard_private *pri, QWidget *oldWgt, QWidget *now, int reson)
{
    static bool bloked = false;
    if (bloked) {
        return false;
    }

    if (oldWgt == now) {
        //slove the view menu cause focus issue (from 'view' to 'view'),it's same but if return false,the active proxWgt will lost focus.(by qt)
        //so there return true, keep textedit focus when text menu show out.
        return true;
    }

    QWidget *proxWgt = currentView->activeProxWidget();
    if (proxWgt == nullptr)
        return false;

    bool oldIsFriend = pri->isFocusFriendWgt(oldWgt);
    bool nowIsFriend = pri->isFocusFriendWgt(now);
    bool ret = false;
    //qWarning() << "oldWgt ====== " << oldWgt << "now ======= " << now << pri->isFocusFriendWgt(now);
    bloked = true;
    if (currentView == oldWgt || proxWgt == oldWgt) {
        if (nowIsFriend) {
            //保持原先的
            if (qobject_cast<QTextEdit *>(proxWgt) != nullptr) {
                QTextEdit *pTextEditor = qobject_cast<QTextEdit *>(proxWgt);
                pTextEditor->setTextInteractionFlags(pTextEditor->textInteractionFlags() & (~Qt::TextEditable));
            }
            ret = true;
        }
    } else if (oldIsFriend) {
        if (proxWgt != nullptr) {
            if (reson == Qt::TabFocus) {
                if (!nowIsFriend) {
                    if (qApp->activePopupWidget() != nullptr) {
                        qApp->activePopupWidget()->hide();
                    }
                    currentView->setFocus();
                    currentView->captureFocus();
                }
            } else {
                if (pri->isFocusFriendWgt(now)) {
                } else if (now == currentView) {
                } else {
                    proxWgt->clearFocus();
                }
            }
        }
    }
    bloked = false;
    return ret;
}

bool DrawBoard::eventFilter(QObject *o, QEvent *e)
{
    if (d_pri() == nullptr)
        return DWidget::eventFilter(o, e);
    if (e->type() == QEvent::Shortcut) {
        if (currentTool_p() != nullptr && currentTool_p()->isWorking()) {
            return true;
        }
        QMetaObject::invokeMethod(this, [ = ]() {
            if (currentTool_p() != nullptr) {
                currentTool_p()->refresh();
            }
        }, Qt::QueuedConnection);
    } else if (e->type() == QEvent::Hide) {
        if (o != nullptr && o->isWindowType()) {
            if (qApp->activePopupWidget() == nullptr) {
                if (currentPage() != nullptr) {
                    currentPage()->view()->setFocus();
                    currentPage()->view()->captureFocus();
                }
            }
        }
    } else if (e->type() == QEvent::FocusOut) {

        if (currentPage() == nullptr)
            return DWidget::eventFilter(o, e);

        if (o != nullptr && o->isWidgetType()) {
            QFocusEvent *event = static_cast<QFocusEvent *>(e);
            bool accept = doFocusChanged(currentPage()->view(), d_pri(), qobject_cast<QWidget *>(o),
                                         qApp->focusWidget(), event->reason());
            if (accept) {
                return true;
            }
        }
    }
    return DWidget::eventFilter(o, e);
}

void DrawBoard::setDrawAttribution(int attris, const QVariant &var, int phase, bool autoCmdStack)
{
    if (currentPage() != nullptr) {
        auto tool = currentPage()->currentTool_p();
        if (tool != nullptr)
            tool->setAttributionVar(attris, var, phase, autoCmdStack);

        if (attributionWidget() != nullptr) {
            attributionWidget()->changeAttribution(SAttri(attris, var));
        }
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
                load(path);
                d_pri()->checkClose();
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

bool DrawBoard::loadDDf(const QString &file)
{
    auto filePath = file;
    bool ret = false;
    auto page = getPageByFile(filePath);
    if (page == nullptr) {
        PageContext *context = _fileHander->loadDdf(filePath);
        setCurrentPage(addPage(context));
        ret = (context != nullptr);
    } else {
        setCurrentPage(page);
    }

    qWarning() << "load result = " << fileHander()->lastError() << fileHander()->lastErrorDescribe();
    auto error = fileHander()->lastError();
    if (error != FileHander::NoError && (FileHander::EUserCancelLoad_OldPen != error && FileHander::EUserCancelLoad_OldBlur != error)) {
        MessageDlg::execMessage(fileHander()->lastErrorDescribe(), ENormalMsg);

    }
    this->activateWindow();

    return ret;
}

bool DrawBoard::loadImage(const QString &file, bool adapt, bool changContexSizeToImag)
{
    auto filePath = file;
    bool ret = false;

    QImage img = _fileHander->loadImage(filePath);
    ret = !img.isNull();
    if (!img.isNull()) {
        QSize maxSize = Application::drawApplication()->maxPicSize();
        if (img.size().width() > maxSize.width() || img.size().height() > maxSize.height()) {
            MessageDlg::execMessage(QObject::tr("Import failed: no more than 10,000 pixels please"), EWarningMsg);
            return false;
        }
    }

    if (currentPage() == nullptr) {
        setCurrentPage(addPage(""));
        currentPage()->setPageRect(QRectF(QPoint(0, 0), img.size()));
    }

    if (changContexSizeToImag) {
        QRectF sceneRect = currentPage()->pageRect();
        QRectF newRect = sceneRect.adjusted(
                             (sceneRect.width() > img.width() ? 0 : (sceneRect.width() - img.width()) / 2),
                             (sceneRect.height() > img.height() ? 0 : (sceneRect.height() - img.height()) / 2),
                             (sceneRect.width() > img.width() ? 0 : -(sceneRect.width() - img.width()) / 2),
                             (sceneRect.height() > img.height() ? 0 : -(sceneRect.height() - img.height()) / 2)
                         );
        currentPage()->setPageRect(newRect);
    }

    auto currentContext = currentPage()->context();
    auto pos = currentContext->pageRect().center() - img.rect().center();

    QRectF rect = QRectF(QPointF(0, 0), img.size());
    bool bAddImg = true;
    if (adapt) {
        QFileInfo info(filePath);
        int r = -1;
        bAddImg = adaptImgPosAndRect(currentPage()->scene(), info.fileName(), img, pos, rect, r);
    }

    if (bAddImg) {
        currentContext->scene()->clearSelectGroup();
        currentContext->addImage(img, pos, rect, true, true);
        currentPage()->setCurrentTool(selection);
    }

    qWarning() << "load result = " << fileHander()->lastError() << fileHander()->lastErrorDescribe();
    auto error = fileHander()->lastError();
    if (error != FileHander::NoError && (FileHander::EUserCancelLoad_OldPen != error && FileHander::EUserCancelLoad_OldBlur != error)) {
        MessageDlg::execMessage(fileHander()->lastErrorDescribe(), ENormalMsg);
    }
    this->activateWindow();

    return ret;
}
