// SPDX-FileCopyrightText: 2020 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cexportimagedialog.h"
#include "widgets/csvglabel.h"
#include "application.h"
#include  "dialog.h"
#include "ccentralwidget.h"
#include "cvalidator.h"
#include "cexportimagedialog_p.h"
#include "cspinbox.h"
#include "cviewmanagement.h"
#include "cgraphicsview.h"
#include "cdrawscene.h"

#include <DFileDialog>
#include <DDialog>
#include <DMessageBox>

#include <QImageWriter>
#include <QStandardPaths>
#include <QPdfWriter>
#include <QPainter>
#include <QCloseEvent>
#include <QDebug>
#include <QDateTime>
#include <QAbstractButton>
#include <DTitlebar>

#define NAME_MAX 255
const QSize DIALOG_SIZE = QSize(380, 280);
const QSize LINE_EDIT_SIZE = QSize(250, 35);
const QFont TITLE_FONT = QFont("SourceHanSansSC", 13.5, 70);
enum {ECancel = -1, EReExec, EOK};

QMap<int, QString> exportFormatMapping = {
    {0, "png"}, {1, "jpg"}, {2, "bmp"}, {3, "tiff"},
    {4, "pdf"}, {5, "ppm"}, {6, "xbm"}, {7, "xpm"}
};

CExportImageDialog::CExportImageDialog(DWidget *parent)
    : DDialog(parent)
{
    _pPrivate = new CExportImageDialog_private(this);
    initUI();
    initConnection();
}

CExportImageDialog::~CExportImageDialog()
{
    delete _pPrivate;
}

int CExportImageDialog::getImageType() const
{
    return  m_formatCombox->currentIndex();
}

QString CExportImageDialog::getImageFormate() const
{
    return  m_saveFormat;
}

int CExportImageDialog::getQuality() const
{
    return m_quality;
}

int CExportImageDialog::exec()
{
    quitRet = 1;
    m_fileNameEdit->setText(tr("Unnamed"));
    if (m_savePathCombox->count() == Other + 1) {
        m_savePathCombox->blockSignals(true);
        m_savePathCombox->removeItem(Other);
    }
    m_savePathCombox->blockSignals(false);
    m_savePathCombox->setCurrentIndex(Pictures);
    m_formatCombox->setCurrentIndex(JPG);
    m_qualitySlider->setValue(100);

    slotOnSavePathChange(Pictures);
    slotOnFormatChange(JPG);
    slotOnQualityChanged(m_qualitySlider->value());

Exec:
    quitRet = DDialog::exec();

    if (quitRet == 1) {
        int ret = execCheckFile(resultFile());

        if (ret == EReExec) {
            goto Exec;
        }
        quitRet = ret;

        saveSetting();
    }

    return quitRet;
}

int CExportImageDialog::execFor(const Page *page)
{
    d_pri()->resetImageSettingSizeTo(page->pageRect().size().toSize());

    if (exec() == 1) {
        if (page->saveToImage(resultFile(), desImageSize(), getQuality()))
            return 1;
        else {
            return 0;
        }
    }
    return -1;
}


QString CExportImageDialog::resultFile() const
{
    if (quitRet == 1)
        return getCompleteSavePath();

    return "";
}

QSize CExportImageDialog::desImageSize() const
{
    return QSize(d_pri()->_widthEditor->text().toInt(), d_pri()->_heightEditor->text().toInt());
}

void CExportImageDialog::initUI()
{
    setWgtAccesibleName(this, "Export dialog");
    resize(DIALOG_SIZE);
    setModal(true);

    setContentsMargins(0, 0, 0, 0);

    setIcon(QIcon::fromTheme("deepin-draw"));

    setWindowTitle(tr("Export"));

    //获取标题qlabel
    DTitlebar *title_name = findChild<DTitlebar *>();
    if (title_name) {
        //修改标题样式
        QFont font(TITLE_FONT);
        title_name->setFont(font);
    }
    m_fileNameEdit = new DLineEdit(this);
    setWgtAccesibleName(m_fileNameEdit, "Export name line editor");
    m_fileNameEdit->setClearButtonEnabled(false);
    //编译器会对反斜杠进行转换，要想在正则表达式中包括一个\，需要输入两次，例如\\s。要想匹配反斜杠本身，需要输入4次，比如\\\\。
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    //在 Qt 6 中，QRegExpValidator 已被弃用
    m_fileNameEdit->lineEdit()->setValidator(new QRegularExpressionValidator(QRegularExpression("[^\\\\ /:*?\"<>|]+"), m_fileNameEdit->lineEdit()));
#else
    m_fileNameEdit->lineEdit()->setValidator(new QRegExpValidator(QRegExp("[^\\\\ /:*?\"<>|]+"), m_fileNameEdit->lineEdit()));
#endif

    m_savePathCombox = new QComboBox(this);
    setWgtAccesibleName(m_savePathCombox, "Export path comboBox");
    m_savePathCombox->insertItem(Pictures, tr("Pictures"));
    if (!Application::isTabletSystemEnvir()) {
        m_savePathCombox->insertItem(Documents, tr("Documents"));
        m_savePathCombox->insertItem(Downloads, tr("Downloads"));
        m_savePathCombox->insertItem(Desktop, tr("Desktop"));
        m_savePathCombox->insertItem(Videos, tr("Videos"));
        m_savePathCombox->insertItem(Music, tr("Music"));
        m_savePathCombox->insertItem(UsrSelect, tr("Select other directories"));
    }

    m_savePathCombox->/*setFixedSize*/setFixedHeight(LINE_EDIT_SIZE.height());
    m_savePathCombox->hide();

    //add new path widget.
    m_pathEditor = new DLineEdit(this);
    m_pathEditor->setClearButtonEnabled(false);
    m_pathEditor->lineEdit()->setReadOnly(true);
    m_pathChosenButton = new PathActiveButton(this);
    m_pathChosenButton->setFixedWidth(40);
    m_pathChosenButton->setToolTip(tr("Select other directories"));
    connect(m_pathChosenButton, &PathActiveButton::clicked, this, [ = ]() {
        DFileDialog dialog(this);
        dialog.setViewMode(DFileDialog::Detail);
#if (QT_VERSION_MAJOR == 5)
    	dialog.setFileMode(DFileDialog::DirectoryOnly);
#elif (QT_VERSION_MAJOR == 6)
    	dialog.setFileMode(DFileDialog::FileMode::Directory);
#endif
        dialog.setDirectory(m_pathEditor->text());
        if (dialog.exec()) {
            auto files = dialog.selectedFiles();
            if (!files.isEmpty()) {
                QString fileDir = files.first();
                m_pathEditor->setText(fileDir);
            }
        }
    });
    QHBoxLayout *lay = new QHBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(9);
    lay->addWidget(m_pathEditor);
    lay->addWidget(m_pathChosenButton);

    m_formatCombox = new QComboBox(this);
    setWgtAccesibleName(m_formatCombox, "Export format comboBox");
    auto writeableFormats = drawApp->writableFormatNameFilters();
    writeableFormats.removeAt(0);
    m_formatCombox->addItems(writeableFormats);

    m_qualitySlider = new DSlider(Qt::Horizontal, this);
    setWgtAccesibleName(m_qualitySlider, "Export quality slider");
    m_qualitySlider->setMinimum(1);
    m_qualitySlider->setMaximum(100);
    m_qualitySlider->setValue(100);
    m_qualitySlider->setFixedWidth(120);

    m_qualityLabel = new DLabel(this);

    QHBoxLayout *qualityHLayout = new QHBoxLayout;
    qualityHLayout->setContentsMargins(0, 0, 0, 0);
    qualityHLayout->setSpacing(0);
    qualityHLayout->addSpacing(3);
    qualityHLayout->addWidget(m_qualitySlider);
    qualityHLayout->addSpacing(20);
    qualityHLayout->addWidget(m_qualityLabel);

    DWidget *contentWidget = new DWidget(this);
    setWgtAccesibleName(contentWidget, "Export content widget");
    contentWidget->setContentsMargins(0, 0, 0, 0);
    QFormLayout *fLayout = new QFormLayout(contentWidget);
    fLayout->setFormAlignment(Qt::AlignJustify);
    fLayout->setHorizontalSpacing(10);
    fLayout->setSpacing(10);
    fLayout->setContentsMargins(0, 0, 0, 0);
    fLayout->addRow(tr("Name:"), m_fileNameEdit);
    fLayout->addRow(tr("Save to:"), lay);
    //fLayout->addRow(tr("Save to:"), m_savePathCombox);
    fLayout->addRow(tr("Format:"), m_formatCombox);
    fLayout->addRow(tr("Quality:"), qualityHLayout);

    d_pri()->initSizeSettingLayoutUi(fLayout, contentWidget);

    addContent(contentWidget);

    addButton(tr("Cancel"), false, DDialog::ButtonNormal);
    m_saveBtnId = addButton(tr("Save"), true, DDialog::ButtonRecommend);

    m_questionDialog = new DDialog(this);
    m_questionDialog->setIcon(QIcon::fromTheme("dialog-warning"));
    m_questionDialog->setModal(true);
    m_questionDialog->addButton(tr("Cancel"));
    m_questionDialog->addButton(tr("Replace"), false, DDialog::ButtonWarning);
}

void CExportImageDialog::initConnection()
{
    connect(m_savePathCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotOnSavePathChange(int)), Qt::QueuedConnection);
    connect(m_formatCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotOnFormatChange(int)));
    connect(m_qualitySlider, SIGNAL(valueChanged(int)), this, SLOT(slotOnQualityChanged(int)));

    //设置的文件名为空时应该要设置保存按钮为disable
    connect(m_fileNameEdit, &DLineEdit::textChanged, this, [ = ](const QString & text) {

        QString newText = text;
        bool isEmpty = newText.isEmpty();

        if (m_saveBtnId != -1) {
            QAbstractButton *pBtn = getButton(m_saveBtnId);

            if (pBtn != nullptr) {
                pBtn->setEnabled(!isEmpty);
            }
        }
    });

}

void CExportImageDialog::slotOnSavePathChange(int index)
{
    switch (index) {
    case Pictures:
        m_savePath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
        if (Application::isTabletSystemEnvir()) {
            m_savePath += "/Drawings";
            QDir dir(m_savePath);
            if (!dir.exists()) {
                dir.mkdir(m_savePath);
            }
        }
        break;
    case Documents:
        m_savePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        break;
    case Downloads:
        m_savePath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        break;
    case Desktop:
        m_savePath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        break;
    case Videos:
        m_savePath = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
        break;
    case Music:
        m_savePath = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
        break;
    case UsrSelect:
        showDirChoseDialog();
        break;
    case Other:
        m_savePath = m_savePathCombox->itemText(index);
        break;
    default:
        m_savePath = "";
        break;
    }
    m_lastIndex = index;
}

void CExportImageDialog::slotOnFormatChange(int index)
{
    if (exportFormatMapping.find(index) != exportFormatMapping.end()) {
        m_saveFormat = exportFormatMapping[index];
    } else {
        m_saveFormat = "png";
    }
    if (m_saveFormat == "pdf") {
        m_qualitySlider->setValue(100);
        m_qualitySlider->setEnabled(false);
    } else {
        m_qualitySlider->setEnabled(true);
    }

    //m_saveFormat = m_formatCombox->itemText(index);

    QString name = m_fileNameEdit->text().trimmed();
    m_fileNameEdit->setText(name);
}

int CExportImageDialog::execCheckFile(const QString &text)
{
    //  [BUG: 30843] 希望在导出时，名字以点开头的图片，也能给到提示：以点开始会被隐藏
    QString fileName = m_fileNameEdit->text().trimmed();
    if (fileName.startsWith(".")) {
        int status = MessageDlg::execMessage(tr("This file will be hidden if the file name starts with a dot (.). Do you want to hide it?"), EWarningMsg,
                                             QStringList() << tr("Cancel") << tr("Confirm"),
                                             QList<EButtonType>() << ENormalMsgBtn << EWarningMsgBtn, this);
        if (status != 1) {
            return EReExec;
        }
    }

    // 判断路径是否超过255字符
    QString completePath = getCompleteSavePath();
    QFileInfo info(completePath);
    // NAME_MAX 文件名字最大长度
    if (info.fileName().toLocal8Bit().length() > NAME_MAX) {
        Dtk::Widget::DDialog dialog(this);
        dialog.setTextFormat(Qt::RichText);
        dialog.addButton(tr("OK"));
        dialog.setIcon(QIcon(":/icons/deepin/builtin/Bullet_window_warning.svg"));
        dialog.setMessage(tr("The file name is too long"));
        dialog.exec();
        return ECancel;
    }

    if (completePath == "") {
        return ECancel;
    }
    if (QFileInfo::exists(completePath)) {
        int ret = execFileIsExists(completePath);
        if (ret != 1) {
            return ECancel;
        }
    }
    return EOK;
}


void CExportImageDialog::slotOnQualityChanged(int value)
{
    m_qualityLabel->setText(QString("%1%").arg(value));
    m_quality = value;
}

void CExportImageDialog::showEvent(QShowEvent *event)
{
    m_pathEditor->setText(drawApp->defaultFileDialogPath());

    auto view = CManageViewSigleton::GetInstance()->getCurView();
    if (view != nullptr) {
        auto name = view->drawScene()->pageContext()->page()->name();
        m_fileNameEdit->setText(name);
    }

    auto formatFilter = drawApp->defaultFileDialogNameFilter();
    int index = drawApp->writableFormatNameFilters().indexOf(formatFilter);
    if (index != -1) {
        --index;
    } else {
        index = 0;
    }
    m_formatCombox->setCurrentIndex(qMax(0, index));
    DDialog::showEvent(event);
}

void CExportImageDialog::saveSetting()
{
    QFileInfo info(getCompleteSavePath());
    drawApp->setDefaultFileDialogPath(info.absolutePath());
    drawApp->setDefaultFileDialogNameFilter(m_formatCombox->currentText());
}

void CExportImageDialog::showDirChoseDialog()
{
    DFileDialog dialog(this);
    dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    dialog.setViewMode(DFileDialog::Detail);
#if (QT_VERSION_MAJOR == 5)
    dialog.setFileMode(DFileDialog::DirectoryOnly);
#elif (QT_VERSION_MAJOR == 6)
    dialog.setFileMode(DFileDialog::FileMode::Directory);
#endif
    if (dialog.exec()) {
        auto dirs = dialog.selectedFiles();
        QString fileDir = dirs.isEmpty() ? "" : dirs.first();
        if (!fileDir.isEmpty()) {
            if (m_savePathCombox->count() < Other + 1) {
                m_savePathCombox->insertItem(Other, fileDir);
            } else {
                m_savePathCombox->setItemText(Other, fileDir);
            }
            m_savePathCombox->setCurrentText(fileDir);
        } else {
            m_savePathCombox->setCurrentIndex(m_lastIndex);
        }
    } else {
        m_savePathCombox->setCurrentIndex(m_lastIndex);
    }
}

int CExportImageDialog::execFileIsExists(const QString &path)
{
    QString newStrMsg = path;
    QFontMetrics fontWidth(m_questionDialog->font());   //得到每个字符的宽度
    QString newPath = fontWidth.elidedText(newStrMsg, Qt::ElideRight, 350);   //最大宽度显示超出为省略号显示

    //“XXX”已经存在，您是否要替换？
    m_questionDialog->setMessage(QString(tr("%1 \n already exists, do you want to replace it?")).arg(newPath));
    return m_questionDialog->exec();
}

QString CExportImageDialog::getCompleteSavePath() const
{
    QString fileName = m_fileNameEdit->text().trimmed();

    if (fileName.isEmpty() || fileName == "") {
        return "";
    }

    fileName = fileName + "." + m_saveFormat/*m_formatCombox->currentText()*/;

    return m_pathEditor->text() + "/" + fileName;
}

void CExportImageDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == (Qt::Key_Return) || event->key() == (Qt::Key_Enter)) {
        //if (d_pri()->isFocusInEditor())
        return;
    }
    return DDialog::keyPressEvent(event);
}

void CExportImageDialog::CExportImageDialog_private::initSizeSettingLayoutUi(QFormLayout *fLayout, QWidget *contentWidget)
{
    //add radio setting ui.
    QHBoxLayout *lay1 = new QHBoxLayout;
    lay1->setContentsMargins(0, 0, 0, 0);
    lay1->setSpacing(9);
    _radioRadioBtn = new QRadioButton(tr("Percentage"), contentWidget);
    lay1->addWidget(_radioRadioBtn);
    auto spinBox = new CSpinBox(contentWidget);
    spinBox->setSpinRange(0, 999999);
    spinBox->lineEdit()->setValidator(new CIntValidator(0, 999999, spinBox));
    spinBox->setEnabledEmbedStyle(true);
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
#if (QT_VERSION_MAJOR == 5)
    // 在Qt 5中，连接按钮切换信号，使用QAbstractButton*和bool作为参数
    connect(group, SIGNAL(buttonToggled(QAbstractButton*, bool)), _q, [ = ](QAbstractButton *button, bool checked) {
        if (checked) {
            settingModel = ESizeSettingModel(group->id(button)); // 获取按钮的ID
            updateSettingModelUi();
        }
    });
#elif (QT_VERSION_MAJOR == 6)
    // 在Qt 6中，QButtonGroup::buttonToggled信号的签名发生了变化，因此需要调整连接的方式。
    // 在Qt 6中，连接按钮切换信号，使用int和bool作为参数
    connect(group, &QButtonGroup::idToggled, _q, [ = ](int model, bool checked) {
        if (checked) {
            settingModel = ESizeSettingModel(model);
            updateSettingModelUi();
        }
    });
#endif
    auto w = new QWidget(contentWidget);
    {
        //set w ui.
        QGridLayout *lay = new QGridLayout;
        lay->setContentsMargins(0, 0, 0, 0);
        w->setLayout(lay);

        _keepRaidoCheckBox = new DCheckBox(tr("Lock aspect ratio"), w);
        lay->addWidget(_keepRaidoCheckBox, 0, 0, 1, 4);

        connect(_keepRaidoCheckBox, &DCheckBox::toggled, _q, [ = ](bool checked) {
            if (settingModel == EPixelModel)
                keepRadioIfPixelModel = checked;
        });

        {
            lay->addWidget(new QLabel(tr("W:"), w), 1, 0, 1, 1);

            auto lineEditor = new DLineEdit(w);
            lineEditor->setClearButtonEnabled(false);
            lay->addWidget(lineEditor, 1, 1, 1, 1);
            lay->addWidget(new QLabel(tr("pixels"), w), 1, 2, 1, 1);
            _widthEditor = lineEditor;
        }

        {
            lay->setSpacing(10);
            lay->addWidget(new QLabel(tr("H:"), w), 2, 0, 1, 1);

            auto lineEditor = new DLineEdit(w);
            lineEditor->setClearButtonEnabled(false);
            lay->addWidget(lineEditor, 2, 1, 1, 1);

            lay->addWidget(new QLabel(tr("pixels"), w), 2, 2, 1, 1);

            _heightEditor = lineEditor;

            lay->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding), 2, 3, 1, 1);
        }
        {
            _tipLabelForOutOfBounds = new QLabel("tip label", w);//设置内容让其适应字体，然后再设置为空，避免字体与系统字体不一样，导致高度变化
            //lay->addWidget(_tipLabelForOutOfBounds, 3, 0, 1, 4);
            _tipLabelForOutOfBounds->setAlignment(Qt::AlignCenter | Qt::AlignAbsolute);

            QPalette palette = _tipLabelForOutOfBounds->palette();
            palette.setColor(QPalette::WindowText, QColor("#8AA1B4"));
            _tipLabelForOutOfBounds->setText("");
            _tipLabelForOutOfBounds->setPalette(palette);
        }
    }
    lay2->addWidget(w, Qt::AlignLeft);
    piexlWgt = w;
    fLayout->addRow("", lay2);
    fLayout->addRow(_tipLabelForOutOfBounds);

    {
        auto validtor = new CIntValidator(0, 999999, _widthEditor);
        validtor->setEmptyStrToBottom(false);
        _widthEditor->lineEdit()->setValidator(validtor);
        connect(_widthEditor, &DLineEdit::editingFinished, _q, [ = ]() {
            if (_widthEditor->text().toInt() != curShowIntSize.width())
                autoKeepSize(EKeepBaseW);
            _widthEditor->clearFocus();
        });
    }
    {
        auto validtor = new CIntValidator(0, 999999, _heightEditor);
        validtor->setEmptyStrToBottom(false);
        _heightEditor->lineEdit()->setValidator(validtor);
        connect(_heightEditor, &DLineEdit::editingFinished, _q, [ = ]() {
            if (_heightEditor->text().toInt() != curShowIntSize.height())
                autoKeepSize(EKeepBaseH);
            _heightEditor->clearFocus();
        });
    }
    connect(_radioSpinBox, &CSpinBox::valueChanged, _q, [ = ](int value, EChangedPhase phase) {
        Q_UNUSED(value)
        Q_UNUSED(phase)
        autoKeepSize(EKeepBaseRadioValue);
    });

    _radioRadioBtn->setChecked(true);
}

void CExportImageDialog::CExportImageDialog_private::updateSettingModelUi()
{
    if (_radioRadioBtn->isChecked()) {
        setSizeSettingModel(ERadioModel);
    } else if (_radioPiexlBtn->isChecked()) {
        setSizeSettingModel(EPixelModel);
    }
}
void CExportImageDialog::CExportImageDialog_private::resetImageSettingSizeTo(const QSize &sz, qreal raido, bool keepRadio, ESizeSettingModel settingModel)
{
    this->settingModel = settingModel;
    keepRadioIfPixelModel = true;

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

    //_radioSpinBox->setRange(0, INT_MAX);
    _radioSpinBox->setValue(qMin(raido, 1.0) * 100.);
    _widthEditor->setText(QString("%1").arg(sz.width()));
    _heightEditor->setText(QString("%1").arg(sz.height()));
    _keepRaidoCheckBox->setChecked(keepRadio);
}

void CExportImageDialog::CExportImageDialog_private::setSizeSettingModel(ESizeSettingModel model)
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

        //QSignalBlocker bloker(_radioSpinBox);
        _radioSpinBox->setSpecialText();


        QSignalBlocker bloker1(_keepRaidoCheckBox);
        _keepRaidoCheckBox->setChecked(keepRadioIfPixelModel);
    }

    QSignalBlocker bloker1(_widthEditor);
    _widthEditor->setText(QString::number(qRound(curSize[model].width())));

    QSignalBlocker bloker2(_heightEditor);
    _heightEditor->setText(QString::number(qRound(curSize[model].height())));
}

bool CExportImageDialog::CExportImageDialog_private::autoKeepSize(EKeepBase base)
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
    QSizeF resultSize = curSize[settingModel];
    qreal settingPrecent = _radioSpinBox->value() / 100.;
    if ((!_widthEditor->text().isEmpty()) && (!_heightEditor->text().isEmpty())) {
        int wantedWidth  = _widthEditor->text().isEmpty() ? qRound(curSize[settingModel].width()) : _widthEditor->text().toInt();
        int wantedHeight = _heightEditor->text().isEmpty() ? qRound(curSize[settingModel].height()) : _heightEditor->text().toInt();

        resultSize = autoKeepWHRadio(base, settingPrecent, QSize(wantedWidth, wantedHeight), curSize[settingModel],
                                     originSize, alert);
    }

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
            qreal value = curPrecent * 100.;
            _radioSpinBox->setSpinPhaseValue(value, EChanged);
        } else {
            curPrecent = settingPrecent;
        }
    }

    return (alert == ENoAlert);
}

QSizeF CExportImageDialog::CExportImageDialog_private::autoKeepWHRadio(int base, qreal precentRadio,
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

void CExportImageDialog::CExportImageDialog_private::showTip(EAlertReason alertReson)
{
    if (alertReson == ENoAlert)
        return;

    QString tips;
    if (alertReson == ETooSmall) {
        tips = tr("At least one pixel please");
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

bool CExportImageDialog::CExportImageDialog_private::isFocusInEditor() const
{
    if (qApp->focusWidget() == _widthEditor->lineEdit() || qApp->focusWidget() == _heightEditor->lineEdit()) {
        return true;
    }
    return false;
}

void PathActiveButton::paintEvent(QPaintEvent *event)
{
    DSuggestButton::paintEvent(event);

    //3 points
    const int w = 16;
    const int penW = 4;
    const int space = (16 - 3 * 4) / 2;
    int xBegin = (width() - w) / 2 + penW / 2;
    int y = rect().center().y() + penW / 2;
    QPolygonF polygon;
    int inc = penW + space;
    for (int i = 0; i < 3; ++i) {
        polygon << QPointF(xBegin + inc * i, y);
    }
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen p(Qt::white);
    p.setCapStyle(Qt::RoundCap);
    p.setWidth(4);
    painter.setPen(p);
    painter.drawPoints(polygon);
}
