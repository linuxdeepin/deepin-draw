#include "shapeswidget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QDebug>
#include <QTimer>
#include <QColor>
#include <QPdfWriter>
#include <QPrinter>
#include <QPrintDialog>
#include <QKeySequence>
#include <QShortcut>
#include <QMenu>
#include <QClipboard>

#include <cmath>

#include "utils/shortcut.h"
#include "utils/imageutils.h"
#include "utils/calculaterect.h"
#include "utils/configsettings.h"
#include "utils/tempfile.h"
#include "utils/global.h"
#include "controller/importer.h"

#define LINEWIDTH(index) (index*2+3)

const int DRAG_BOUND_RADIUS = 8;
const int SPACING = 12;
const QString RESIZE_POINT_IMG = ":/theme/light/images/size/resize_handle_big.svg";
const QString ROTATE_POINT_IMG = ":/theme/light/images/size/rotate.png";

const qreal WINDOW_SPACINT = 25;
const qreal ARTBOARD_MARGIN = 0;
const int PIC_SPACING = 20;
const int POINT_SPACING = 20;
const int IMG_ROTATEPOINT_SPACING = 45;
const QPointF START_POINT = QPointF(0, 45);

ShapesWidget::ShapesWidget(QWidget *parent)
    : QFrame(parent)
{
    initAttribute();
    initShortcut();
    initMenu();

    m_bottomPixmap = QPixmap(this->size());
    m_bottomPixmap.fill(Qt::transparent);
    m_emptyBgPixmap = QPixmap(this->size());
    m_emptyBgPixmap.fill(Qt::transparent);

    connect(this, &ShapesWidget::finishedDrawCut,
                    this, &ShapesWidget::showCutImageTips);
    connect(ConfigSettings::instance(), &ConfigSettings::configChanged,
            this, &ShapesWidget::updateSelectedShape);
    connect(ConfigSettings::instance(), &ConfigSettings::configChanged,
            this, [=](const QString &group, const QString &key){
        if (group == "text" && key == "fontsize")
        {
            m_textFontsize = ConfigSettings::instance()->value("text",
                                                               "fontsize").toInt();
        }
    });
    connect(Importer::instance(), &Importer::importedFiles,
            this, &ShapesWidget::loadImage);
}

void ShapesWidget::initAttribute()
{
    setObjectName("Canvas");
    setStyleSheet("QFrame#Canvas { "
                              "background-color: white;"
                              "margin: 0px;}");
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setAcceptDrops(true);

    m_isRecording = false;
    m_isMoving = false;
    m_isSelected = false;
    m_isPressed = false;
    m_isHovered = false;
    m_isRotated = false;
    m_isResize = false;

    m_editing = false;
    m_needCompress = false;
    m_moveShape = false;

    m_isShiftPressed = false;
    m_isAltPressed = false;

    m_scaledImage = false;
    m_stickCurosr = false;
    m_rationChanged = false;

    m_blurEffectExist = false;
    m_mosaicEffectExist = false;
    m_clearAllTextBorder = false;
    m_imageCutting = false;
    m_rotateImage = false;
    m_inBtmRight = false;
    m_initCanvasSideLength = false;
    m_generateBlurImage = false;
    m_cursorInBtmRight = false;
    m_recordCutImage = false;
    m_beginGrabImage = false;
    m_isCutImageResize = false;
    m_getOriginRation = false;

    m_drawArtboardSize = false;
    m_resizeByAutoCrop = false;
    m_artboardMainPoints = initFourPoints(m_artboardMainPoints);

    m_shapesIndex = -1;
    m_selectedIndex = -1;
    m_selectedOrder = -1;
    m_ration = 1;
    m_resizeRation = 1;
    m_cutImageOrder = -1;

    m_startPos = START_POINT;
    initCanvasSize();

    m_cutImageTips = new CutImageTips(this);
    m_cutImageTips->hide();
    m_degreeLabel = new TipsLabel(this);
    m_degreeLabel->setFixedSize(50, 28);
    m_degreeLabel->hide();

    m_penColor =  QColor(Qt::blue);
    m_brushColor = QColor(Qt::blue);
    m_textFontsize = 12;
    m_linewidth = 4;

    installEventFilter(this);
}

void ShapesWidget::initCanvasSize()
{
    m_artBoardActualWidth = ConfigSettings::instance()->value("artboard", "width").toInt();
    m_artBoardActualHeight = ConfigSettings::instance()->value("artboard", "height").toInt();
    m_originArtboardSize = QSize(m_artBoardActualWidth, m_artBoardActualHeight);
    if (m_artBoardActualWidth == 0 || m_artBoardActualHeight == 0)
    {
        QSize desktopSize = qApp->desktop()->size();
        m_artBoardActualWidth = desktopSize.width();
        m_artBoardActualHeight = desktopSize.height();
    }
}

void ShapesWidget::initMenu()
{
    m_menu = new QMenu(this);
    m_menu->setFocusPolicy(Qt::StrongFocus);
    QAction* cutAc = m_menu->addAction(tr("Cut"));
    QAction* copyAc = m_menu->addAction(tr("Copy"));
    QAction* pasteAc = m_menu->addAction(tr("Paste"));
    m_menu->addSeparator();
    QAction* delAc = m_menu->addAction(tr("Delete"));
//    QAction* unDoAc = m_menu->addAction(tr("Undo"));
    m_menu->addSeparator();
    QAction* upLayerAc = m_menu->addAction(tr("Raise Layer"));
    QAction* downLayerAc = m_menu->addAction(tr("Lower Layer"));
    QAction* topLayerAc = m_menu->addAction(tr("Layer to Top"));
    QAction* btmLayerAc = m_menu->addAction(tr("Layer to Bottom"));

    connect(copyAc, &QAction::triggered, this, &ShapesWidget::copyShape);
    connect(cutAc, &QAction::triggered, this, &ShapesWidget::cutShape);
    connect(pasteAc, &QAction::triggered, this, [=]{
        QPoint startPos = QPoint(0, 0);
        startPos = mapToGlobal(startPos);
        QPoint cursorPos =  QPoint(m_menu->pos());
        cursorPos = mapToGlobal(cursorPos);
        pasteShape(QPoint(cursorPos.x() - 2*startPos.x(), cursorPos.y() - 2*startPos.y()));
    });
    connect(delAc, &QAction::triggered, this, &ShapesWidget::deleteCurrentShape);
    connect(upLayerAc, &QAction::triggered, this, [=]{
        layerSwitch(LayerDirection::UpLayer);
    });
    connect(downLayerAc, &QAction::triggered, this, [=]{
        layerSwitch(LayerDirection::DownLayer);
    });
    connect(topLayerAc, &QAction::triggered, this, [=]{
        layerSwitch(LayerDirection::TopLayer);
    });
    connect(btmLayerAc, &QAction::triggered, this, [=]{
        layerSwitch(LayerDirection::BottomLayer);
    });
}

ShapesWidget::~ShapesWidget()
{
}

void ShapesWidget::updateSelectedShape(const QString &group,
                                                                     const QString &key)
{
    qDebug() << "updateSelectedShapes" << m_selectedIndex
                    << m_shapes.length() << m_selectedOrder;

    if ("common" == group) {
        int colorAlpha = 100;
        if (key == "strokeColor" || key == "strokeColor_alpha" || key == "strokeColor_transparent") {
            bool transColBtnChecked = ConfigSettings::instance()->value(
                        "common", "strokeColor_transparent").toBool();
            if (transColBtnChecked)
            {
                m_penColor = QColor(Qt::transparent);
            } else {
                m_penColor = QColor(ConfigSettings::instance()->value(
                                        "common", "strokeColor").toString());
                colorAlpha = ConfigSettings::instance()->value("common",
                                                               "strokeColor_alpha").toInt();
                m_penColor = QColor(m_penColor.red(), m_penColor.green(),
                                    m_penColor.blue());
                m_penColor.setAlphaF(qreal(colorAlpha)/qreal(100));
            }
        } else if (key == "fillColor" || key == "fillColor_alpha" || key == "fillColor_transparent") {
            bool transColBtnChecked = ConfigSettings::instance()->value(
                        "common", "fillColor_transparent").toBool();
            if (transColBtnChecked)
            {
                m_brushColor = QColor(Qt::transparent);
            } else {
                colorAlpha = ConfigSettings::instance()->value("common",
                                                               "fillColor_alpha").toInt();
                qreal value = qreal(colorAlpha)/qreal(100);
                m_brushColor = QColor(ConfigSettings::instance()->value(
                                          "common", "fillColor").toString());
                m_brushColor = QColor(m_brushColor.red(), m_brushColor.green(),
                                      m_brushColor.blue());
                m_brushColor.setAlphaF(value);
            }
        } else if (key == "lineWidth") {
            m_linewidth = ConfigSettings::instance()->value(
                        "common", "lineWidth").toInt();
        }
    } else if (group == "line" && key == "style") {
        if (m_currentType == "straightLine" || m_currentType == "arrow"
                || m_currentType == "arbitraryCurve") {
            setCurrentShape("line");
        }
    } else if (group == "blur" && key == "index") {
        m_blurLinewidth = ConfigSettings::instance()->value(
            "blur", "index").toInt();
    } else if (group == "text") {
        if (key == "fillColor" || key == "fillColor_alpha" || key == "fillColor_transparent")
        {
            bool transColBtnChecked = ConfigSettings::instance()->value("text",
                                                                        "fillColor_transparent").toBool();
            QColor brushCol = QColor(ConfigSettings::instance()->value("text",
                                                                       "fillColor").toString());
            int brushColAlpha = ConfigSettings::instance()->value("text",
                                                                  "fillColor_alpha").toInt();
            if (transColBtnChecked)
            {
                m_brushColor = QColor(Qt::transparent);
            } else {
                m_brushColor = QColor(brushCol.red(), brushCol.green(), brushCol.blue());
                m_brushColor.setAlphaF(qreal(brushColAlpha)/qreal(100));
            }
        }
    }

    bool updateSelectedShape = false;
    if (m_selectedOrder != -1) {
        if (group == "common") {
            if (key == "strokeColor" || key == "strokeColor_alpha" || key == "strokeColor_transparent") {
                m_shapes[m_selectedOrder].strokeColor = m_penColor;
                updateSelectedShape = true;
            } else if (key == "fillColor" || key == "fillColor_alpha" || key == "fillColor_transparent") {
                m_shapes[m_selectedOrder].fillColor =  m_brushColor;
                updateSelectedShape = true;
            } else if (key == "lineWidth") {
                m_shapes[m_selectedOrder].lineWidth = ConfigSettings::instance()->value(
                    "common", "lineWidth").toInt();
                updateSelectedShape = true;
            }
        } else if (group == "text" && m_shapes[m_selectedOrder].type == group)  {
            if (m_shapes[m_selectedOrder].type == "text") {
                qDebug() << "updateSelectedShape..." << m_selectedOrder
                                << m_shapes[m_selectedOrder].index
                                << m_textFontsize;
                int tmpIndex = m_shapes[m_selectedOrder].index;
                if (m_editMap.contains(tmpIndex)) {
                    qDebug() << "tmpIndex:" << tmpIndex;
                    m_editMap.value(tmpIndex)->setColor(m_brushColor);
                    m_editMap.value(tmpIndex)->setFontSize(m_textFontsize);
                    m_editMap.value(tmpIndex)->update();
                    updateSelectedShape = true;
                }
            }
        }
    }

    if (m_selectedOrder != -1)
    {
        m_needCompress = true;
        compressToImage();
        update();
//        m_needCompress = false;
    }
}

int ShapesWidget::shapesNum() const
{
    return m_shapes.length();
}

void ShapesWidget::setShapes(QList<Toolshape> shapes)
{
    m_shapes = shapes;
    qDebug() << "setShapesWidget length:" << m_shapes.length();
}

void ShapesWidget::setCurrentShape(QString shapeType)
{
    qDebug() << "setCurrentShape" << shapeType;
    if (m_currentType == shapeType && shapeType != "line")
        return;
    else {
        if (shapeType != "selected" && shapeType != "cutImage")
        {
            m_selectedOrder = -1;
            m_isSelected = false;
        }
    }

    if (shapeType != "selected")
    {
        m_moveShape = false;
        ConfigSettings::instance()->setValue("tools", "activeMove", false);
    } else {
        m_moveShape = true;
        ConfigSettings::instance()->setValue("tools", "activeMove", true);
    }

    if (shapeType == "blur")
    {
        m_blurLinewidth = ConfigSettings::instance()->value("blur",
                                                            "index").toInt();
    }

    if (m_currentType != "blur" && shapeType == "blur")
        m_generateBlurImage = true;
    else
        m_generateBlurImage = false;

    if (shapeType == "cutImage") {
        setImageCutting(true);
    } else {
        if (shapeType == "selected" && m_currentType == "cutImage")
        {
            m_recordCutImage = true;
        } else {
            m_recordCutImage = false;
        }
        setImageCutting(false);
    }

    m_currentType = shapeType;
    if (m_currentType == "line")
    {
        int lineStyle = ConfigSettings::instance()->value("line", "style").toInt();
        m_currentType = getLineStyle(lineStyle);
    }

    updateShapeAttribute();
}

void ShapesWidget::setPenColor(QColor color)
{
    m_penColor = color;
    update();
}

void ShapesWidget::setBrushColor(QColor color)
{
    m_brushColor = color;
}

void ShapesWidget::setLineWidth(int linewidth)
{
    m_linewidth = linewidth;
}

void ShapesWidget::setTextFontsize(int fontsize)
{
    m_textFontsize = fontsize;
}

void ShapesWidget::clearSelected()
{
    for(int j = 0; j < m_selectedShape.mainPoints.length(); j++)
    {
        m_selectedShape.mainPoints[j] = QPointF(0, 0);
        m_hoveredShape.mainPoints[j] = QPointF(0, 0);
    }

    qDebug() << "clear selected!!!";
    m_isSelected = false;
    m_selectedShape.points.clear();
    m_hoveredShape.points.clear();
    m_selectedOrder = -1;
    compressToImage();
    update();
}

void ShapesWidget::setAllTextEditReadOnly()
{
    QMap<int, TextEdit*>::iterator i = m_editMap.begin();
    while (i != m_editMap.end())
    {
        i.value()->setReadOnly(true);
        i.value()->releaseKeyboard();

        QTextCursor textCursor =  i.value()->textCursor();
        textCursor.clearSelection();
        i.value()->setTextCursor(textCursor);
        ++i;
    }

    if (m_currentShape.type == "text" && m_editing)
    {
        m_currentShape.type = "";
        for(int j = 0; j < m_currentShape.mainPoints.length(); j++)
        {
            m_currentShape.mainPoints[j] = QPointF(0, 0);
            m_currentShape.mainPoints[j] = QPointF(0, 0);
        }

        qDebug() << "clear selected!!!";
        m_isSelected = false;
        m_currentShape.points.clear();
        m_currentShape.points.clear();
    }

    m_editing = false;

    for(int k = 0; k < m_shapes.length(); k++)
    {
        if (m_shapes[k].type == "text" && m_editMap.value(
                    m_shapes[k].index)->toPlainText().isEmpty())
        {
            int textIndex = m_shapes[k].index;
            m_shapes.removeAt(k);
            delete m_editMap.value(textIndex);
            m_editMap.remove(textIndex);
        }
    }

    update();
}

void ShapesWidget::setFillShapeSelectedActive(bool selected)
{
    m_moveShape = selected;
    qDebug() << "setFillShapeSelectedActive" << selected;
}

void ShapesWidget::createBlurImage()
{
    QPixmap blurImage = this->grab(this->rect()).copy(
                QRect(ARTBOARD_MARGIN, ARTBOARD_MARGIN,
                           width() - ARTBOARD_MARGIN*2, height() - ARTBOARD_MARGIN*2));
    int imgWidth = blurImage.width();
    int imgHeight = blurImage.height();
    const int radius = 10;

    if (!blurImage.isNull())
    {
        blurImage = blurImage.scaled(imgWidth/radius, imgHeight/radius,
                                Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        blurImage = blurImage.scaled(imgWidth, imgHeight,
                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        blurImage.save(TempFile::instance()->getBlurFileName(), "png");
    } else {
        qWarning() << "create blur image failed!";
    }
    m_generateBlurImage = false;
}

void ShapesWidget::saveActionTriggered()
{
    qDebug() << "ShapesWidget saveActionTriggered!";
    setAllTextEditReadOnly();
    clearSelected();
    m_clearAllTextBorder = true;
}

bool ShapesWidget::clickedOnShapes(QPointF pos)
{
    bool onShapes = false;

    qDebug() << "Judge ClickedOnShapes !!!!!!!" << m_shapes.length();

    if (m_imageCutting)
    {
        if (clickedOnImage(m_cutShape.mainPoints, pos)) {
            onShapes = true;
        }
        return onShapes;
    }

    qDebug() << "clickedOnShapes:" << m_currentType << m_moveShape;
    if (m_shapes.length() == 0 || !m_moveShape)
    {
        qDebug() << "clickedOnShapes length:" << m_shapes.length();
        return onShapes;
    }

    m_selectedOrder = -1;
    for (int i = m_shapes.length() - 1; i >= 0; i--)
   {
        bool currentOnShape = false;
        qDebug() << "this moment shape:" << m_currentType;

        if (m_shapes[i].type == "image")
        {
            if (clickedOnImage(m_shapes[i].mainPoints, pos))
            {
                currentOnShape = true;
            }
        }

        if (m_shapes[i].type == "rectangle")
        {
            if (clickedOnRect(m_shapes[i].mainPoints, pos,
                              m_shapes[i].fillColor.alpha() != 0))
            {
                currentOnShape = true;

                if (m_shapes[i].fillColor == QColor(Qt::transparent))
                {
                    updateToSelectedShapeAttribute("common", "fillColor_transparent", true);
                } else {
                    updateToSelectedShapeAttribute("common", "fillColor",
                                                   m_shapes[i].fillColor.name(QColor::HexRgb));
                    qreal colorAlpha = m_shapes[i].fillColor.alphaF();
                    updateToSelectedShapeAttribute("common", "fillColor_alpha",
                                                   int(colorAlpha*100));
                }

                if (m_shapes[i].strokeColor == QColor(Qt::transparent))
                {
                    ConfigSettings::instance()->setValue("common",
                                                         "strokeColor_transparent", true);
                    updateToSelectedShapeAttribute("common", "strokeColor", true);
                } else {
                    updateToSelectedShapeAttribute("common", "strokeColor",
                                                   m_shapes[i].strokeColor.name(QColor::HexRgb));
                    qreal colorAlpha = m_shapes[i].strokeColor.alphaF();
                    updateToSelectedShapeAttribute("common", "strokeColor_alpha",
                                                   int(colorAlpha*100));
                }
                updateToSelectedShapeAttribute("common", "lineWidth", m_shapes[i].lineWidth);
            } else
            {
                qDebug() << "no clicked on rectangle:" << m_shapes[i].mainPoints << pos;
            }
        }
        if (m_shapes[i].type == "cutImage")
        {
            if (clickedOnCutImage(m_shapes[i].mainPoints, pos))
            {
                currentOnShape = true;
            }
        }
        if (m_shapes[i].type == "oval")
        {
            if (clickedOnEllipse(m_shapes[i].mainPoints, pos,
                                 m_shapes[i].fillColor.alpha() != 0))
            {
                currentOnShape = true;

                if (m_shapes[i].fillColor == QColor(Qt::transparent))
                {
                    updateToSelectedShapeAttribute("common", "fillColor_transparent", true);
                } else {
                    updateToSelectedShapeAttribute("common", "fillColor",
                                                   m_shapes[i].fillColor.name(QColor::HexRgb));
                    qreal colorAlpha = m_shapes[i].fillColor.alphaF();
                    updateToSelectedShapeAttribute("common", "fillColor_alpha",
                                                   int(colorAlpha*100));
                }

                if (m_shapes[i].strokeColor == QColor(Qt::transparent))
                {
                    ConfigSettings::instance()->setValue("common",
                                                         "strokeColor_transparent", true);
                    updateToSelectedShapeAttribute("common", "strokeColor", true);
                } else {
                    updateToSelectedShapeAttribute("common", "strokeColor",
                                                   m_shapes[i].strokeColor.name(QColor::HexRgb));
                    qreal colorAlpha = m_shapes[i].strokeColor.alphaF();
                    updateToSelectedShapeAttribute("common", "strokeColor_alpha",
                                                   int(colorAlpha*100));
                }
                updateToSelectedShapeAttribute("common", "lineWidth", m_shapes[i].lineWidth);
            }
        }
        if (m_shapes[i].type == "arrow" || m_shapes[i].type == "straightLine")
        {
            if (clickedOnArrow(m_shapes[i].points, pos))
            {
                currentOnShape = true;

                if (m_shapes[i].strokeColor == QColor(Qt::transparent))
                {
                    ConfigSettings::instance()->setValue("common",
                                                         "strokeColor_transparent", true);
                    updateToSelectedShapeAttribute("common", "strokeColor", true);
                } else {
                    updateToSelectedShapeAttribute("common", "strokeColor",
                                                   m_shapes[i].strokeColor.name(QColor::HexRgb));
                    qreal colorAlpha = m_shapes[i].strokeColor.alphaF();
                    updateToSelectedShapeAttribute("common", "strokeColor_alpha",
                                                   int(colorAlpha*100));
                }
                updateToSelectedShapeAttribute("common", "lineWidth", m_shapes[i].lineWidth);
                if (m_shapes[i].type == "straightLine")
                {
                    updateToSelectedShapeAttribute("line", "style", 0);
                } else {
                    updateToSelectedShapeAttribute("line", "style", 2);
                }
            }
        }
        if (m_shapes[i].type == "arbitraryCurve" || m_shapes[i].type == "blur")
        {
            if (clickedOnLine(m_shapes[i].mainPoints, m_shapes[i].points, pos,
                              m_shapes[i].lineWidth))
            {
                currentOnShape = true;

                if (m_shapes[i].type == "arbitraryCurve")
                {
                    if (m_shapes[i].strokeColor == QColor(Qt::transparent))
                    {
                        ConfigSettings::instance()->setValue("common",
                                                             "strokeColor_transparent", true);
                        updateToSelectedShapeAttribute("common", "strokeColor", true);
                    } else {
                        updateToSelectedShapeAttribute("common", "strokeColor",
                                                       m_shapes[i].strokeColor.name(QColor::HexRgb));
                        qreal colorAlpha = m_shapes[i].strokeColor.alphaF();
                        updateToSelectedShapeAttribute("common", "strokeColor_alpha",
                                                       int(colorAlpha*100));
                    }
                    updateToSelectedShapeAttribute("common", "lineWidth", m_shapes[i].lineWidth);
                    updateToSelectedShapeAttribute("line", "style", 1);
                } else {
                    updateToSelectedShapeAttribute("blur", "index", m_shapes[i].lineWidth);
                }
            }
        }
        if (m_shapes[i].type == "text")
        {
            if (clickedOnText(m_shapes[i].mainPoints, pos))
            {
                currentOnShape = true;
                qDebug() << "clickedOnShapes ### text!!!" << i;
            }
        }

        if (currentOnShape)
        {
            emit updateMiddleWidgets(m_shapes[i].type);
            setAllTextEditReadOnly();

            m_selectedShape = m_shapes[i];
            m_selectedIndex = m_shapes[i].index;
            m_selectedOrder = i;
            m_stickSelectedShape = m_shapes[i];
            qDebug() << "currentOnShape" << i << m_selectedIndex
                            << m_selectedOrder << m_shapes[i].type;

            onShapes = true;
            update();
            break;
        } else
        {
            continue;
        }
    }

    if (!onShapes)
    {
        m_selectedIndex = -1;
        m_selectedOrder = -1;
        m_selectedShape.type = "";
        m_stickSelectedShape.type = "";
        m_pressedPoint = QPointF(100, 100);

        if (m_currentShape.type == "text")
        {
            m_clearAllTextBorder = true;
        }
        update();
    }
//    update();
    return onShapes;
}

//TODO: selectUnique
bool ShapesWidget::clickedOnImage(FourPoints rectPoints, QPointF pos)
{
    m_isSelected = false;
    m_isResize = false;
    m_isRotated = false;

    if (!m_moveShape)
        return false;

    QPointF point1 = rectPoints[0];
    QPointF point2 = rectPoints[1];
    QPointF point3 = rectPoints[2];
    QPointF point4 = rectPoints[3];

    FourPoints otherFPoints = getAnotherFPoints(rectPoints);
    if (pointClickIn(point1, pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = First;
        m_resizeDirection = TopLeft;
        m_pressedPoint = pos;

        return true;
    } else if (pointClickIn(point2, pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Second;
        m_resizeDirection = BottomLeft;
        m_pressedPoint = pos;

        return true;
    } else if (pointClickIn(point3, pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Third;
        m_resizeDirection = TopRight;
        m_pressedPoint = pos;

        return true;
    } else if (pointClickIn(point4, pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Fourth;
        m_resizeDirection = BottomRight;
        m_pressedPoint = pos;

        return true;
    }  else if (pointClickIn(otherFPoints[0], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Fifth;
        m_resizeDirection = Left;
        m_pressedPoint = pos;

        return true;
    } else if (pointClickIn(otherFPoints[1], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Sixth;
        m_resizeDirection = Top;
        m_pressedPoint = pos;

        return true;
    } else if (pointClickIn(otherFPoints[2], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Seventh;
        m_resizeDirection = Right;
        m_pressedPoint = pos;

        return true;
    } else if (pointClickIn(otherFPoints[3], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Eighth;
        m_resizeDirection = Bottom;
        m_pressedPoint = pos;

        return true;
    } else if (rotateOnPoint(rectPoints, pos)) {
        qDebug() << "rotateOnPoint!";
        m_isSelected = true;
        m_isRotated = true;
        m_isResize = false;
        m_resizeDirection = Rotate;
        m_pressedPoint = pos;

        return true;
    } else if (pointOnLine(rectPoints[0], rectPoints[1], pos) || pointOnLine(rectPoints[1],
        rectPoints[3], pos) || pointOnLine(rectPoints[3], rectPoints[2], pos) ||
        pointOnLine(rectPoints[2], rectPoints[0], pos)) {
        m_isSelected = true;
        m_isResize = false;
        m_resizeDirection = Moving;
        m_pressedPoint = pos;

        return true;
    } else if(pointInRect(rectPoints, pos)) {
        m_isSelected = true;
        m_isResize = false;
        m_resizeDirection = Moving;
        m_pressedPoint = pos;

        return true;
    }  else {
        qDebug() << "Not on Image Rect:" << pos << rectPoints;

        m_isSelected = false;
        m_isResize = false;
        m_isRotated = false;
    }

    return false;
}

bool ShapesWidget::clickedOnCutImage(FourPoints rectPoints, QPointF pos)
{
    m_isCutImageResize = false;

    QPointF point1 = rectPoints[0];
    QPointF point2 = rectPoints[1];
    QPointF point3 = rectPoints[2];
    QPointF point4 = rectPoints[3];

    if (pointClickIn(point1, pos, POINT_SPACING)) {
        m_isCutImageResize = true;
        m_clickedKey = First;
        m_resizeDirection = TopLeft;
        m_pressedPoint = pos;

        return true;
    } else if (pointClickIn(point2, pos, POINT_SPACING)) {
        m_isCutImageResize = true;
        m_clickedKey = Second;
        m_resizeDirection = BottomLeft;
        m_pressedPoint = pos;

        return true;
    } else if (pointClickIn(point3, pos, POINT_SPACING)) {
        m_isCutImageResize = true;
        m_clickedKey = Third;
        m_resizeDirection = TopRight;
        m_pressedPoint = pos;

        return true;
    } else if (pointClickIn(point4, pos, POINT_SPACING)) {
        m_isCutImageResize = true;
        m_clickedKey = Fourth;
        m_resizeDirection = BottomRight;
        m_pressedPoint = pos;

        return true;
    }  else if (pointOnLine(rectPoints[0], rectPoints[1], pos)) {
        m_isCutImageResize = true;
        m_clickedKey = Fifth;
        m_resizeDirection = Left;
        m_pressedPoint = pos;

        return true;
    } else if (pointOnLine(rectPoints[2], rectPoints[0], pos)) {
        m_isCutImageResize = true;
        m_clickedKey = Sixth;
        m_resizeDirection = Top;
        m_pressedPoint = pos;

        return true;
    } else if (pointOnLine(rectPoints[3], rectPoints[2], pos)) {
        m_isCutImageResize = true;
        m_clickedKey = Seventh;
        m_resizeDirection = Right;
        m_pressedPoint = pos;

        return true;
    } else if (pointOnLine(rectPoints[1], rectPoints[3], pos)) {
        m_isCutImageResize = true;
        m_clickedKey = Eighth;
        m_resizeDirection = Bottom;
        m_pressedPoint = pos;

        return true;
    }  else if(pointInRect(rectPoints, pos)) {
        m_isCutImageResize = true;
        m_resizeDirection = Moving;
        m_pressedPoint = pos;

        qDebug() << "CutImage moving ........";
        return true;
    }  else {
        m_isCutImageResize = false;
    }

    qDebug() << "clicked On cutImage:" << false;
    return false;
}

bool ShapesWidget::clickedOnRect(FourPoints rectPoints,
         QPointF pos, bool isFilled)
{
    m_isSelected = false;
    m_isResize = false;
    m_isRotated = false;

    if (!m_moveShape)
    {
        return false;
    }

    QPointF point1 = rectPoints[0];
    QPointF point2 = rectPoints[1];
    QPointF point3 = rectPoints[2];
    QPointF point4 = rectPoints[3];

    FourPoints otherFPoints = getAnotherFPoints(rectPoints);
    if (pointClickIn(point1, pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = First;
        m_resizeDirection = TopLeft;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(point2, pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Second;
        m_resizeDirection = BottomLeft;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(point3, pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Third;
        m_resizeDirection = TopRight;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(point4, pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Fourth;
        m_resizeDirection = BottomRight;
        m_pressedPoint = pos;
        return true;
    }  else if (pointClickIn(otherFPoints[0], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Fifth;
        m_resizeDirection = Left;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(otherFPoints[1], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Sixth;
        m_resizeDirection = Top;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(otherFPoints[2], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Seventh;
        m_resizeDirection = Right;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(otherFPoints[3], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Eighth;
        m_resizeDirection = Bottom;
        m_pressedPoint = pos;
        return true;
    } else if (rotateOnPoint(rectPoints, pos)) {
        m_isSelected = true;
        m_isRotated = true;
        m_isResize = false;
        m_resizeDirection = Rotate;
        m_pressedPoint = pos;
        return true;
    } else if (pointOnLine(rectPoints[0], rectPoints[1], pos) || pointOnLine(rectPoints[1],
        rectPoints[3], pos) || pointOnLine(rectPoints[3], rectPoints[2], pos) ||
        pointOnLine(rectPoints[2], rectPoints[0], pos)) {
        m_isSelected = true;
        m_isResize = false;
        m_resizeDirection = Moving;
        m_pressedPoint = pos;
        return true;
    } else if(isFilled && pointInRect(rectPoints, pos)) {
        m_isSelected = true;
        m_isResize = false;
        m_resizeDirection = Moving;
        m_pressedPoint = pos;
        return true;
    }  else {
        m_isSelected = false;
        m_isResize = false;
        m_isRotated = false;
        m_resizeDirection = Outting;
    }

    return false;
}

bool ShapesWidget::clickedOnEllipse(FourPoints mainPoints,
                                    QPointF pos, bool isFilled)
{
    m_isSelected = false;
    m_isResize = false;
    m_isRotated = false;

    if (!m_moveShape)
    {
        return false;
    }

    m_pressedPoint = pos;
    FourPoints otherFPoints = getAnotherFPoints(mainPoints);
    if (pointClickIn(mainPoints[0], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = First;
        m_resizeDirection = TopLeft;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(mainPoints[1], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Second;
        m_resizeDirection = BottomLeft;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(mainPoints[2], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Third;
        m_resizeDirection = TopRight;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(mainPoints[3], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Fourth;
        m_resizeDirection = BottomRight;
        m_pressedPoint = pos;
        return true;
    }  else if (pointClickIn(otherFPoints[0], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Fifth;
        m_resizeDirection = Left;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(otherFPoints[1], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Sixth;
        m_resizeDirection = Top;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(otherFPoints[2], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Seventh;
        m_resizeDirection = Right;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(otherFPoints[3], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Eighth;
        m_resizeDirection = Bottom;
        m_pressedPoint = pos;
        return true;
    } else if (rotateOnPoint(mainPoints, pos)) {
        m_isSelected = true;
        m_isRotated = true;
        m_isResize = false;
        m_resizeDirection = Rotate;
        m_pressedPoint = pos;
        return true;
    } else if (pointOnEllipse(mainPoints, pos)) {
        m_isSelected = true;
        m_isResize = false;

        m_resizeDirection = Moving;
        m_pressedPoint = pos;
        return true;
    } else if(isFilled && pointInRect(mainPoints, pos)) {
        m_isSelected = true;
        m_isResize = false;
        m_resizeDirection = Moving;
        m_pressedPoint = pos;
        return true;
    } else {
        m_isSelected = false;
        m_isResize = false;
        m_isRotated = false;
    }

    return false;
}

bool ShapesWidget::clickedOnArrow(QList<QPointF> points,
                                  QPointF pos)
{
    if (points.length() != 2)
        return false;

    m_isSelected = false;
    m_isResize = false;
    m_isRotated = false;

    if (pointClickIn(points[0], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isRotated = true;
        m_resizeDirection = Rotate;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(points[1], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isRotated = true;
        m_resizeDirection = Rotate;
        m_pressedPoint = pos;
        return true;
    } else if (pointOnLine(points[0], points[1], pos)) {
        m_isSelected = true;
        m_isRotated = false;
        m_resizeDirection = Moving;
        m_pressedPoint = pos;
        return true;
    } else {
        m_isSelected = false;
        m_isRotated = false;
        m_isResize = false;
        m_resizeDirection = Outting;
        m_pressedPoint = pos;
        return false;
    }
}

bool ShapesWidget::clickedOnLine(FourPoints mainPoints,
                                      QList<QPointF> points, QPointF pos, int padding)
{
    m_isSelected = false;
    m_isResize = false;
    m_isRotated = false;

    if (!m_moveShape)
        return false;

    m_pressedPoint = QPoint(0, 0);
    FourPoints otherFPoints = getAnotherFPoints(mainPoints);
    if (pointClickIn(mainPoints[0], pos,  POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = First;
        m_resizeDirection = TopLeft;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(mainPoints[1], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Second;
        m_resizeDirection = BottomLeft;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(mainPoints[2], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Third;
        m_resizeDirection = TopRight;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(mainPoints[3], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Fourth;
        m_resizeDirection = BottomRight;
        m_pressedPoint = pos;
        return true;
    }  else if (pointClickIn(otherFPoints[0], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Fifth;
        m_resizeDirection = Left;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(otherFPoints[1], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Sixth;
        m_resizeDirection = Top;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(otherFPoints[2], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Seventh;
        m_resizeDirection = Right;
        m_pressedPoint = pos;
        return true;
    } else if (pointClickIn(otherFPoints[3], pos, POINT_SPACING)) {
        m_isSelected = true;
        m_isResize = true;
        m_clickedKey = Eighth;
        m_resizeDirection = Bottom;
        m_pressedPoint = pos;
        return true;
    } else if (rotateOnPoint(mainPoints, pos)) {
        m_isSelected = true;
        m_isRotated = true;
        m_isResize = false;
        m_resizeDirection = Rotate;
        m_pressedPoint = pos;
        return true;
    }  else if (pointOnArLine(points, pos, padding)) {
        m_isSelected = true;
        m_isResize = false;
        m_resizeDirection = Moving;
        m_pressedPoint = pos;
        return true;
    } else {
        m_isSelected = false;
        m_isResize = false;
        m_isRotated = false;
    }

    return false;
}

bool ShapesWidget::clickedOnText(FourPoints mainPoints, QPointF pos)
{
    if (pointInRect(mainPoints, pos)) {
        m_isSelected = true;
        m_isResize = false;
        m_resizeDirection = Moving;

        return true;
    } else {
        m_isSelected = false;
        m_isResize = false;

        return false;
    }
}

bool ShapesWidget::hoverOnCutImage(FourPoints rectPoints, QPointF pos)
{
    m_resizeDirection = Outting;

    if (pointClickIn(rectPoints[0], pos, POINT_SPACING)) {
        m_resizeDirection = TopLeft;
        return true;
    } else if (pointClickIn(rectPoints[1], pos, POINT_SPACING)) {
        m_resizeDirection = BottomLeft;
        return true;
    } else if (pointClickIn(rectPoints[2], pos, POINT_SPACING)) {
        m_resizeDirection = TopRight;
        return true;
    } else if (pointClickIn(rectPoints[3], pos, POINT_SPACING)) {
        m_resizeDirection = BottomRight;
        return true;
    } else if (pointOnLine(rectPoints[0],  rectPoints[1], pos)) {
        m_resizeDirection = Left;
        return true;
    } else if (pointOnLine(rectPoints[2], rectPoints[0], pos)) {
        m_resizeDirection = Top;
        return true;
    }  else if (pointOnLine(rectPoints[3], rectPoints[2], pos)) {
        m_resizeDirection = Right;
        return true;
    } else if (pointOnLine(rectPoints[1], rectPoints[3], pos)) {
        m_resizeDirection = Bottom;
        return true;
    } else if(pointInRect(rectPoints, pos)) {
        m_resizeDirection = Moving;
        return true;
    } else {
        m_resizeDirection = Outting;
    }

    return false;
}

bool ShapesWidget::hoverOnRect(FourPoints rectPoints,
                               QPointF pos, bool isFilled)
{
    qDebug() << "isFilled:" << isFilled << m_moveShape;
    if (isFilled && !m_moveShape)
        return false;

    FourPoints tmpFPoints = getAnotherFPoints(rectPoints);
    if (pointClickIn(rectPoints[0], pos, POINT_SPACING)) {
        m_resizeDirection = TopLeft;
        return true;
    } else if (pointClickIn(rectPoints[1], pos, POINT_SPACING)) {
        m_resizeDirection = BottomLeft;
        return true;
    } else if (pointClickIn(rectPoints[2], pos, POINT_SPACING)) {
        m_resizeDirection = TopRight;
        return true;
    } else if (pointClickIn(rectPoints[3], pos, POINT_SPACING)) {
        m_resizeDirection = BottomRight;
        return true;
    } else if (rotateOnPoint(rectPoints, pos) && m_selectedOrder != -1
               && m_selectedOrder == m_hoveredIndex /*&& !isTextBorder*/) {
        m_resizeDirection = Rotate;
        return true;
    }  else if (pointClickIn(tmpFPoints[0], pos, POINT_SPACING)) {
        m_resizeDirection = Left;
        return true;
    } else if (pointClickIn(tmpFPoints[1], pos, POINT_SPACING)) {
        m_resizeDirection = Top;
        return true;
    }  else if (pointClickIn(tmpFPoints[2], pos, POINT_SPACING)) {
        m_resizeDirection = Right;
        return true;
    } else if (pointClickIn(tmpFPoints[3], pos, POINT_SPACING)) {
        m_resizeDirection = Bottom;
        return true;
    } else if (pointOnLine(rectPoints[0],  rectPoints[1], pos) || pointOnLine(rectPoints[1],
        rectPoints[3], pos) || pointOnLine(rectPoints[3], rectPoints[2], pos) ||
               pointOnLine(rectPoints[2], rectPoints[0], pos)) {
        m_resizeDirection = Moving;
        return true;
    } else if (isFilled && pointInRect(rectPoints, pos)) {
        m_resizeDirection = Moving;
        return true;
    } else {
        m_resizeDirection = Outting;
    }

    return false;
}

bool ShapesWidget::hoverOnEllipse(FourPoints mainPoints,
                                  QPointF pos, bool isFilled)
{
    if (isFilled && !m_moveShape)
        return false;

    FourPoints tmpFPoints = getAnotherFPoints(mainPoints);

    if (pointClickIn(mainPoints[0], pos, POINT_SPACING)) {
        m_resizeDirection = TopLeft;
        return true;
    } else if (pointClickIn(mainPoints[1], pos, POINT_SPACING)) {
        m_resizeDirection = BottomLeft;
        return true;
    } else if (pointClickIn(mainPoints[2], pos, POINT_SPACING)) {
        m_resizeDirection = TopRight;
        return true;
    } else if (pointClickIn(mainPoints[3], pos, POINT_SPACING)) {
        m_resizeDirection = BottomRight;
        return true;
    } else if (rotateOnPoint(mainPoints, pos) && m_selectedIndex != -1
               && m_selectedIndex == m_hoveredIndex) {
        m_resizeDirection = Rotate;
        return true;
    }  else if (pointClickIn(tmpFPoints[0], pos, POINT_SPACING)) {
        m_resizeDirection = Left;
        return true;
    } else if (pointClickIn(tmpFPoints[1], pos, POINT_SPACING)) {
        m_resizeDirection = Top;
        return true;
    }  else if (pointClickIn(tmpFPoints[2], pos, POINT_SPACING)) {
        m_resizeDirection = Right;
        return true;
    } else if (pointClickIn(tmpFPoints[3], pos, POINT_SPACING)) {
        m_resizeDirection = Bottom;
        return true;
    }  else if (pointOnEllipse(mainPoints, pos)) {
        m_resizeDirection = Moving;
        return true;
   } else if (isFilled && pointInRect(mainPoints, pos)) {
        m_resizeDirection = Moving;
        return true;
    } else {
        m_resizeDirection = Outting;
    }

    return false;
}

bool ShapesWidget::hoverOnArrow(QList<QPointF> points,
                                QPointF pos)
{
    if (points.length() !=2)
        return false;

    if (m_selectedOrder == m_hoveredIndex
            && pointClickIn(points[0], pos, POINT_SPACING)) {
        m_clickedKey = First;
        m_resizeDirection = Rotate;
        return true;
    } else if (m_selectedOrder == m_hoveredIndex
               && pointClickIn(points[1], pos, POINT_SPACING)) {
        m_clickedKey =   Second;
        m_resizeDirection = Rotate;
        return true;
    } else if(pointOnLine(points[0], points[1], pos)) {
        m_resizeDirection = Moving;
        return true;
    } else {
        m_resizeDirection = Outting;
        return false;
    }
}

bool ShapesWidget::hoverOnArbitraryCurve(FourPoints mainPoints,
                               QList<QPointF> points, QPointF pos, int padding)
{
    m_resizeDirection = Outting;

    m_pressedPoint = QPoint(0, 0);
    FourPoints otherFPoints = getAnotherFPoints(mainPoints);

    if (rotateOnPoint(mainPoints, pos)) {
        m_resizeDirection = Rotate;
        return true;
    } else if (pointClickIn(mainPoints[0], pos, POINT_SPACING)) {
        m_resizeDirection = TopLeft;
        return true;
    } else if (pointClickIn(mainPoints[1], pos, POINT_SPACING)) {
        m_resizeDirection = BottomLeft;
        return true;
    } else if (pointClickIn(mainPoints[2], pos, POINT_SPACING)) {
        m_resizeDirection = TopRight;
        return true;
    } else if (pointClickIn(mainPoints[3], pos, POINT_SPACING)) {
        m_resizeDirection = BottomRight;
        return true;
    }  else if (pointClickIn(otherFPoints[0], pos, POINT_SPACING)) {
        m_resizeDirection = Left;
        return true;
    } else if (pointClickIn(otherFPoints[1], pos, POINT_SPACING)) {
        m_resizeDirection = Top;
        return true;
    } else if (pointClickIn(otherFPoints[2], pos, POINT_SPACING)) {
        m_resizeDirection = Right;
        return true;
    } else if (pointClickIn(otherFPoints[3], pos, POINT_SPACING)) {
        m_resizeDirection = Bottom;
        return true;
    }  else if (pointOnArLine(points, pos, padding)) {
        m_resizeDirection = Moving;
        return true;
    } else {
        m_resizeDirection = Outting;
    }

    return false;
}

bool ShapesWidget::hoverOnText(FourPoints mainPoints, QPointF pos)
{
    qDebug() << "hoverOnText:" <<  mainPoints << pos;
    if (hoverOnRect(mainPoints, pos, false) ||  (pos.x() >= mainPoints[0].x()
         && pos.x() <= mainPoints[2].x() && pos.y() >= mainPoints[0].y()
        && pos.y() <= mainPoints[2].y())) {
        qDebug() << "hoverOnText Moving";
        m_resizeDirection = Moving;
        return true;
    } else if (pointInRect(mainPoints, pos))
    {
        m_resizeDirection = Moving;
        return true;
    } else {
        qDebug() << "hoverOnText Outting";
        m_resizeDirection = Outting;
        return false;
    }
}

void ShapesWidget::hoverOnShapes(QPointF pos)
{
    m_isHovered = false;
    m_hoveredIndex = -1;
    m_hoveredShape.type = "";

    if (m_imageCutting)
    {
        if (m_shapes[m_shapes.length() - 1].type == "cutImage" && hoverOnCutImage(
                    m_shapes[m_shapes.length() - 1].mainPoints, pos)) {
            m_isHovered = true;
        } else {
            m_resizeDirection = Outting;
        }

        updateCursorDirection(m_resizeDirection);
        return;
    }

    if (!m_moveShape)
        return;

    for(int i = m_shapes.length() - 1; i >= 0; i--)
    {
        m_hoveredIndex = i;
        if (m_shapes[i].type == "image")
        {
            if (hoverOnRect(m_shapes[i].mainPoints, pos, true))
                m_isHovered = true;
        } else if (m_shapes[i].type == "rectangle") {
            if (hoverOnRect(m_shapes[i].mainPoints, pos,
                            m_shapes[i].fillColor.alpha() != 0))
                m_isHovered = true;
        } else if (m_shapes[i].type == "oval") {
            if (hoverOnEllipse(m_shapes[i].mainPoints, pos,
                             m_shapes[i].fillColor.alpha() != 0))
                m_isHovered = true;
        } else if (m_shapes[i].type == "arrow" || m_shapes[i].type == "straightLine") {
            if (hoverOnArrow(m_shapes[i].points, pos))
                m_isHovered = true;
        } else if (m_shapes[i].type == "arbitraryCurve") {
            if (hoverOnArbitraryCurve(m_shapes[i].mainPoints, m_shapes[i].points, pos,
                                      m_shapes[i].lineWidth)) {
                m_isHovered = true;
                qDebug() << "hover on arbitrary curve...";
            }
        } else if (m_shapes[i].type == "blur")
        {
            if (hoverOnArbitraryCurve(m_shapes[i].mainPoints,
                m_shapes[i].points, pos, m_shapes[i].lineWidth)) {
                m_isHovered = true;
                qDebug() << "hover on arbitrary blur..." << m_shapes[i].lineWidth;
            }
        } else if (m_shapes[i].type == "text") {
            if (hoverOnText(m_shapes[i].mainPoints, pos))
                m_isHovered = true;
        }

        if (m_isHovered)
        {
            m_hoveredIndex = i;
            m_hoveredShape = m_shapes[i];
            break;
        } else {
            continue;
        }
    }

    if (!m_isHovered)
    {
        m_hoveredIndex = -1;
        m_hoveredShape.type = "";
        for(int j = 0; j < m_hoveredShape.mainPoints.length(); j++)
        {
            m_hoveredShape.mainPoints[j] = QPointF(0, 0);
        }
        updateCursorShape();
    } else {
        updateCursorDirection(m_resizeDirection);
    }
}

bool ShapesWidget::rotateOnImagePoint(FourPoints mainPoints, QPointF pos)
{
    QPointF rotatePoint = QPointF(
                (mainPoints[0].x() + mainPoints[3].x())/2,
                (mainPoints[0].y() + mainPoints[3].y())/2);

    bool result = false;

    if (pos.x() >= rotatePoint.x() - SPACING && pos.x() <= rotatePoint.x()
            + SPACING && pos.y() >= rotatePoint.y() - SPACING && pos.y() <=
            rotatePoint.y() + SPACING)
    {
        result = true;
    } else {
        result = false;
    }

    m_pressedPoint = rotatePoint;
    return result;
}

bool ShapesWidget::rotateOnPoint(FourPoints mainPoints, QPointF pos)
{
    return hoverOnRotatePoint(mainPoints, pos);
}

bool ShapesWidget::hoverOnRotatePoint(FourPoints mainPoints,
                                      QPointF pos)
{
    QPointF rotatePoint = getRotatePoint(mainPoints[0], mainPoints[1],
                                                                        mainPoints[2], mainPoints[3]);
    rotatePoint = QPointF(rotatePoint.x() /*- 5*/, rotatePoint.y()/* - 5*/);

    bool result = false;
    if (pos.x() >= rotatePoint.x() - SPACING && pos.x() <= rotatePoint.x()
            + SPACING && pos.y() >= rotatePoint.y() - SPACING && pos.y() <=
            rotatePoint.y() + SPACING)
    {
        result = true;
    } else
    {
        result = false;
    }

    m_pressedPoint = rotatePoint;
    return result;
}

bool ShapesWidget::textEditIsReadOnly()
{
    qDebug() << "textEditIsReadOnly:" << m_editMap.count();

    QMap<int, TextEdit*>::iterator i = m_editMap.begin();
    while (i != m_editMap.end()) {
        if (m_editing || !i.value()->isReadOnly()) {
            setAllTextEditReadOnly();
            m_editing = false;
            m_currentShape.type = "";
            update();
            return true;
        }
        ++i;
    }

    return false;
}

void ShapesWidget::handleDrag(QPointF oldPoint, QPointF newPoint)
{
    qDebug() << "handleDrag:" << m_selectedIndex << m_shapes.length();

    if (m_selectedOrder == -1)
    {
        return;
    }

    if (m_shapes[m_selectedOrder].type == "arrow")
    {
        for(int i = 0; i < m_shapes[m_selectedOrder].points.length(); i++)
        {
            m_shapes[m_selectedOrder].points[i] = QPointF(
            m_shapes[m_selectedOrder].points[i].x() + (newPoint.x() - oldPoint.x()),
            m_shapes[m_selectedOrder].points[i].y() + (newPoint.y() - oldPoint.y()));
        }
        return;
    }

    if (m_shapes[m_selectedOrder].mainPoints.length() == 4)
    {
        for(int i = 0; i < m_shapes[m_selectedOrder].mainPoints.length(); i++)
        {
            m_shapes[m_selectedOrder].mainPoints[i] = QPointF(
            m_shapes[m_selectedOrder].mainPoints[i].x() + (newPoint.x() - oldPoint.x()),
            m_shapes[m_selectedOrder].mainPoints[i].y() + (newPoint.y() - oldPoint.y()));
        }
    }
    for(int i = 0; i < m_shapes[m_selectedOrder].points.length(); i++)
    {
        m_shapes[m_selectedOrder].points[i] = QPointF(
        m_shapes[m_selectedOrder].points[i].x() + (newPoint.x() - oldPoint.x()),
        m_shapes[m_selectedOrder].points[i].y() + (newPoint.y() - oldPoint.y())
                    );
    }
}

////////////////////TODO: perfect handleRotate..
void ShapesWidget::handleRotate(QPointF pos)
{
    m_isShiftPressed = GlobalShortcut::instance()->shiftSc();
    m_isAltPressed = GlobalShortcut::instance()->altSc();
    qDebug() << "handleRotate:" << m_selectedOrder << m_shapes.length();

    if (m_selectedOrder == -1 || m_selectedShape.type == "text")
    {
        return;
    }

    if (m_shapes[m_selectedOrder].type == "image")
    {
        QPointF centerInPoint = QPointF((m_shapes[m_selectedOrder].mainPoints[0].x() +
                                                                     m_shapes[m_selectedOrder].mainPoints[3].x())/2,
                                                                     (m_shapes[m_selectedOrder].mainPoints[0].y()+
                                                                     m_shapes[m_selectedOrder].mainPoints[3].y())/2);

        qreal angle = calculateAngle(m_pressedPoint, pos, centerInPoint)/35;
        angle += m_shapes[m_selectedOrder].rotate;
        m_shapes[m_selectedOrder].rotate = angle;
        showRotateDegreeLabel(angle);
    }

    if (m_selectedShape.type == "arrow" || m_selectedShape.type == "straightLine")
    {
        if (m_isShiftPressed)
        {
            if (m_shapes[m_selectedOrder].points[0].x() ==
                    m_shapes[m_selectedOrder].points[1].x())
            {
                if (m_clickedKey == First)
                {
                    m_shapes[m_selectedOrder].points[0] =
                            QPointF(m_shapes[m_selectedOrder].points[1].x(), pos.y());
                } else if (m_clickedKey == Second)
                {
                    m_shapes[m_selectedOrder].points[1] =
                            QPointF(m_shapes[m_selectedOrder].points[0].x(), pos.y());
                }
            } else
            {
                if (m_clickedKey == First)
                {
                    m_shapes[m_selectedOrder].points[0] = QPointF(pos.x(),
                                                                  m_shapes[m_selectedOrder].points[1].y());
                } else if (m_clickedKey == Second)
                {
                    m_shapes[m_selectedOrder].points[1] = QPointF(pos.x(),
                                                                  m_shapes[m_selectedOrder].points[0].y());
                }
            }
        } else {
            if (m_clickedKey == First)
            {
                m_shapes[m_selectedOrder].points[0] = m_pressedPoint;
            } else if (m_clickedKey == Second)
            {
                m_shapes[m_selectedOrder].points[1] = m_pressedPoint;
            }
        }

        m_selectedShape.points  =  m_shapes[m_selectedOrder].points;
        m_hoveredShape.points = m_shapes[m_selectedOrder].points;
        m_pressedPoint = pos;

        return;
    }

    QPointF centerInPoint = QPointF((m_selectedShape.mainPoints[0].x() +
                                                                 m_selectedShape.mainPoints[3].x())/2,
                                                                 (m_selectedShape.mainPoints[0].y()+
                                                                 m_selectedShape.mainPoints[3].y())/2);
    qreal angle = calculateAngle(m_pressedPoint, pos, centerInPoint)/35;

    if (m_shapes[m_selectedOrder].type != "image")
    {
        m_shapes[m_selectedOrder].rotate += angle;
        showRotateDegreeLabel(m_shapes[m_selectedOrder].rotate);
    }

    for (int i = 0; i < 4; i++)
    {
        m_shapes[m_selectedOrder].mainPoints[i] = pointRotate(centerInPoint,
                                                              m_selectedShape.mainPoints[i], angle);
    }

    for(int k = 0; k < m_shapes[m_selectedOrder].points.length(); k++)
    {
        m_shapes[m_selectedOrder].points[k] = pointRotate(centerInPoint,
                                                              m_selectedShape.points[k], angle);
    }

    m_selectedShape.mainPoints = m_shapes[m_selectedOrder].mainPoints;
    m_hoveredShape.mainPoints =  m_shapes[m_selectedOrder].mainPoints;
    m_pressedPoint = pos;
}

void ShapesWidget::handleResize(QPointF pos, int key)
{
    qDebug() << "handleResize***************************************************:"
                    << m_selectedIndex << m_shapes.length();
    m_isShiftPressed = GlobalShortcut::instance()->shiftSc();
    m_isAltPressed = GlobalShortcut::instance()->altSc();

    if (m_isResize && m_selectedOrder != -1) {
        if (m_shapes[m_selectedOrder].portion.isEmpty()) {
            for(int k = 0; k < m_shapes[m_selectedOrder].points.length(); k++) {
                m_shapes[m_selectedOrder].portion.append(relativePosition(
                m_shapes[m_selectedOrder].mainPoints, m_shapes[m_selectedOrder].points[k]));
            }
        }

        FourPoints newResizeFPoints;
        if (!m_isAltPressed) {
            newResizeFPoints =  resizePointPosition(
            m_shapes[m_selectedOrder].mainPoints[0],
            m_shapes[m_selectedOrder].mainPoints[1],
            m_shapes[m_selectedOrder].mainPoints[2],
            m_shapes[m_selectedOrder].mainPoints[3], pos, key,
            "free", m_isShiftPressed);
        } else {
            newResizeFPoints =  resizePointPositionByAlt(
            m_shapes[m_selectedOrder].mainPoints[0],
            m_shapes[m_selectedOrder].mainPoints[1],
            m_shapes[m_selectedOrder].mainPoints[2],
            m_shapes[m_selectedOrder].mainPoints[3], pos, key,
            "free", m_isShiftPressed);
        }

        m_shapes[m_selectedOrder].mainPoints = newResizeFPoints;
        m_selectedShape.mainPoints = newResizeFPoints;
        m_hoveredShape.mainPoints = newResizeFPoints;

        for (int j = 0; j <  m_shapes[m_selectedOrder].portion.length(); j++)
        {
              m_shapes[m_selectedOrder].points[j] =
                getNewPosition(m_shapes[m_selectedOrder].mainPoints,
                                              m_shapes[m_selectedOrder].portion[j]);
        }

        m_selectedShape.points = m_shapes[m_selectedOrder].points;
        m_hoveredShape.points = m_shapes[m_selectedOrder].points;
    }

    if (m_selectedShape.type == "image" && m_selectedOrder != -1)
    {
        QPixmap newPix;
        newPix = QPixmap(m_shapes[m_selectedOrder].imagePath);

        qreal resizeWidth = getDistance(m_shapes[m_selectedOrder].mainPoints[0],
                m_shapes[m_selectedOrder].mainPoints[2]);
        qreal resizeHeight = getDistance(m_shapes[m_selectedOrder].mainPoints[0],
                m_shapes[m_selectedOrder].mainPoints[1]);

        m_shapes[m_selectedOrder].imageSize = QSize(resizeWidth, resizeHeight);
    }
    m_pressedPoint = pos;
}

void ShapesWidget::handleCutShapeResize(QPointF pos, int key)
{
    if (m_shapes[m_shapes.length() -1].type == "cutImage")
    {
        m_isShiftPressed = GlobalShortcut::instance()->shiftSc();
        m_isAltPressed = GlobalShortcut::instance()->altSc();
        QString defaultRation = ConfigSettings::instance()->value("cut",
                                                                  "ration").toString();
        if (m_isAltPressed)
        {
            m_shapes[m_shapes.length() - 1].mainPoints = resizePointPositionByAlt(
                        m_shapes[m_shapes.length() - 1].mainPoints[0],
                    m_shapes[m_shapes.length() - 1].mainPoints[1],
                    m_shapes[m_shapes.length() - 1].mainPoints[2],
                    m_shapes[m_shapes.length() - 1].mainPoints[3], pos, key, defaultRation, m_isShiftPressed);
        } else {
            m_shapes[m_shapes.length() -1].mainPoints =  resizePointPosition(
            m_shapes[m_shapes.length() -1].mainPoints[0],
            m_shapes[m_shapes.length() -1].mainPoints[1],
            m_shapes[m_shapes.length() -1].mainPoints[2],
            m_shapes[m_shapes.length() -1].mainPoints[3], pos, key,  defaultRation, m_isShiftPressed);
        }

        m_cutShape.mainPoints = m_shapes[m_shapes.length() - 1].mainPoints;
        m_pressedPoint = pos;
    }
}

void ShapesWidget::handleImageRotate(int degree)
{
    if (m_selectedOrder != -1 && m_selectedOrder < m_shapes.length())
    {
        if (m_shapes[m_selectedOrder].type != "image")
            return;
        qreal angle;
        if (degree == 90)
        {
            angle = M_PI/2;
        } else {
            angle = -M_PI/2;
        }
         m_shapes[m_selectedOrder].rotate = m_shapes[m_selectedOrder].rotate + angle;

         QPointF centerInPoint = QPointF(
                     (m_shapes[m_selectedOrder].mainPoints[0].x() + m_shapes[m_selectedOrder].mainPoints[3].x())/2,
                     (m_shapes[m_selectedOrder].mainPoints[0].y() + m_shapes[m_selectedOrder].mainPoints[3].y())/2
                 );
        for (int i = 0; i < 4; i++)
        {
            m_shapes[m_selectedOrder].mainPoints[i] = pointRotate(centerInPoint,
                                                                  m_shapes[m_selectedOrder].mainPoints[i], angle);
        }
    }

    m_needCompress = true;
    compressToImage();
}

void ShapesWidget::mirroredImage(bool horizontal, bool vertical)
{
    if (m_selectedOrder != -1 && m_selectedOrder < m_shapes.length())
    {
        if (horizontal)
            m_shapes[m_selectedOrder].isHorFlip = !m_shapes[m_selectedOrder].isHorFlip;
        if(vertical)
            m_shapes[m_selectedOrder].isVerFlip = !m_shapes[m_selectedOrder].isVerFlip;
    }

    m_needCompress = true;
    compressToImage();
}

bool ShapesWidget::eventFilter(QObject *obj, QEvent *e)
{
    Q_UNUSED(obj);
  if (e->type() == QEvent::KeyPress)
    {
       QKeyEvent* event = static_cast<QKeyEvent*>(e);
        keyPressEvent(event);
        return true;
    }
    return false;
}

void ShapesWidget::mousePressEvent(QMouseEvent *e)
{
    auto doMousePress = [=]() {
        qDebug() << "Mouse pressed:" << e->pos() << m_imageCutting;
        if (e->modifiers() == Qt::NoModifier)
        {
            GlobalShortcut::instance()->setShiftScStatus(false);
            GlobalShortcut::instance()->setAltScStatus(false);
        }
        m_isShiftPressed = GlobalShortcut::instance()->shiftSc();
        m_isAltPressed = GlobalShortcut::instance()->altSc();

        if (e->button() == Qt::RightButton)
        {
            m_menu->popup(this->cursor().pos());
            return;
        }
        //Initialize the scale of the drawing board.
        if (!m_getOriginRation)
        {
            int tmpWidth = ConfigSettings::instance()->value("artboard", "width").toInt();
            int tmpHeight = ConfigSettings::instance()->value("artboard", "height").toInt();
            m_originArtboardSize = QSize(tmpWidth, tmpHeight);
            m_originArtboardWindowSize = this->size();
            m_ration = qreal(tmpWidth)/qreal(this->width());
            qDebug() << "Get origin ration:" << m_ration;
            m_getOriginRation = true;
            m_lastRation = m_ration;
        }

        /*Determin whether a cropped box exist, if there is an indication
         *that it  is currently in the clipping state, enter to redrawing the
         * clipping box, and if not, remove the clipping box.*/
        if (m_imageCutting && clickedOnCutImage(m_cutShape.mainPoints,
            QPointF(e->pos().x(), e->pos().y())))
        {
            m_isPressed = true;
            m_cutImageTips->hide();
            return;
        } else {
            m_cutShape.type = "";
            m_cutImageTips->hide();
            for(int i = 0; i < m_cutShape.mainPoints.length(); i++)
            {
                m_cutShape.mainPoints[i] = QPointF(0, 0);
                if (m_shapes.length() >1&& m_shapes[m_shapes.length() - 1].type
                        == "cutImage")
                {
                    m_shapes.removeLast();
                }
            }
        }

        m_pos1 = QPointF(e->pos().x(), e->pos().y());

        /*If the alt shortcut has been pressed.*/
        if (m_isAltPressed && !m_initAltStart && !m_isResize && !m_isRotated)
        {
            m_altCenterPos = m_pos1;
            m_initAltStart = true;
        }

        /*If click on the blank space, uncheck, and make the text
         in the edit read-only.*/
        if (m_selectedOrder != -1 && !m_imageCutting)
        {
            if ((!clickedOnShapes(QPointF(e->pos().x(), e->pos().y()))
                 && m_isRotated) && m_selectedOrder == -1)
            {
                clearSelected();
                setAllTextEditReadOnly();
                m_editing = false;
                m_selectedIndex = -1;
                m_selectedOrder = -1;
                m_selectedShape.type = "";
                return;
            }
        }

        m_isPressed = true;
        m_pressedPoint = QPointF(m_pos1.x(), m_pos1.y());

        /*Click on the bottom right to drag and drop the canvas size.*/
        QRect btmRightRect = rightBottomRect();
        if (m_selectedOrder == -1 && btmRightRect.contains(QPoint(m_pos1.x(),
                                                                  m_pos1.y())) && !m_isRecording)
        {
            m_inBtmRight = true;
            m_resizeDirection = Right;
            qApp->setOverrideCursor(Qt::SizeFDiagCursor);
            m_drawArtboardSize = true;
            emit updateMiddleWidgets("adjustSize");
            emit shapePressed("");
            setCurrentShape("");
            resizeArtboardByDrag(e->pos());
            return;
        }

        if (!clickedOnShapes(m_pressedPoint) && m_currentType != "image")
        {
            m_isRecording = true;
            m_currentShape.type = m_currentType;
            m_currentShape.strokeColor = m_penColor;
            m_currentShape.fillColor = m_brushColor;
            m_currentShape.lineWidth = m_linewidth;

            m_selectedIndex -= 1;
            m_shapesIndex += 1;

            m_currentIndex = m_shapesIndex;
            m_pressedPoint = QPointF(m_pos1.x(), m_pos1.y());
            qDebug() << "mousePressEvent:" << m_currentType;

            if (m_currentType == "arbitraryCurve") {
                m_currentShape.index = m_currentIndex;
                m_currentShape.points.append(m_pressedPoint);
            } else if (m_currentType == "arrow" || m_currentType == "straightLine") {
                m_currentShape.index = m_currentIndex;
                m_currentShape.points.append(m_pressedPoint);
            } else if (m_currentType == "rectangle" || m_currentType == "oval") {
                m_currentShape.index = m_currentIndex;
            }  else if (m_currentType == "blur") {
                m_blurEffectExist = true;
                m_currentShape.isBlur = true;
                m_currentShape.index = m_currentIndex;
                m_blurLinewidth = ConfigSettings::instance()->value("blur",
                                                                    "index").toInt();
                m_currentShape.lineWidth = m_blurLinewidth;
                m_currentShape.points.append(m_pressedPoint);

                if (m_generateBlurImage)
                    createBlurImage();
            } else if (m_currentType == "text") {
                if (!m_editing && m_selectedOrder == -1) {
                    m_clearAllTextBorder = false;
                    setAllTextEditReadOnly();
                    m_currentShape.mainPoints[0] = m_pressedPoint;
                    m_currentShape.index = m_currentIndex;
                    m_currentShape.fillColor =  m_brushColor;
                    qDebug() << "new textedit:" << m_currentIndex;

                    TextEdit* edit = new TextEdit(m_currentIndex, this);
                    m_editing = true;
                    m_currentShape.fontSize =  m_textFontsize;
                    edit->setFocus();
                    edit->setColor(m_brushColor);
                    edit->setFontSize(m_textFontsize);
                    edit->move(m_pressedPoint.x(), m_pressedPoint.y());
                    edit->show();
                    m_currentShape.mainPoints[0] = m_pressedPoint;
                    m_currentShape.mainPoints[1] = QPointF(
                        m_pressedPoint.x(), m_pressedPoint.y() + edit->height());
                    m_currentShape.mainPoints[2] = QPointF(
                                m_pressedPoint.x() + edit->width(), m_pressedPoint.y());
                    m_currentShape.mainPoints[3] = QPointF(
                        m_pressedPoint.x() + edit->width(), m_pressedPoint.y() + edit->height());
                    m_editMap.insert(m_currentIndex, edit);

                    connect(edit, &TextEdit::repaintTextRect, this, &ShapesWidget::updateTextRect);
                    connect(edit, &TextEdit::backToEditing, this, [=]{
                        m_editing = true;
                    });
                    connect(edit, &TextEdit::textEditSelected, this, [=](int index){
                        for (int k = 0; k < m_shapes.length(); k++) {
                            if (m_shapes[k].type == "text" && m_shapes[k].index == index) {
                                m_selectedOrder = -1;
                                if (edit->getTextColor() == QColor(Qt::transparent))
                                {
                                    updateToSelectedShapeAttribute("text", "fillColor_transparent", true);
                                } else {
                                    qreal colorAlpha = edit->getTextColor().alphaF();
                                    updateToSelectedShapeAttribute("text", "fillColor_alpha",
                                                                   int(colorAlpha*100));
                                    updateToSelectedShapeAttribute("text", "fillColor",
                                                                   edit->getTextColor().name(QColor::HexRgb));
                                }
                                m_brushColor = edit->getTextColor();
                                updateToSelectedShapeAttribute("text", "fontsize", edit->fontSize());
                                m_selectedOrder = k;
                                m_selectedShape = m_shapes[k];
                                m_textFontsize = edit->fontSize();
                                break;
                            }
                        }

                        updateMiddleWidgets("text");
                        qDebug() << "the textEdit index:" << m_selectedOrder << edit->getIndex();
                    });
                    connect(edit, &TextEdit::showMenuInTextEdit, this, [=]{
                        m_menu->popup(this->cursor().pos());
                    });
                    appendShape(m_currentShape);
                    qDebug() << "Insert text shape:" << m_currentShape.index;
                } else {
                    m_editing = false;
                    m_selectedOrder = -1;
                    setAllTextEditReadOnly();
                }
            }
        } else {
            m_isRecording = false;
            if (m_editing && m_editMap.contains(m_shapes[m_selectedOrder].index))
            {
                m_editMap.value(m_shapes[m_selectedOrder].index)->setReadOnly(true);
                m_editMap.value(m_shapes[m_selectedOrder].index)->setCursorVisible(false);
                m_editMap.value(m_shapes[m_selectedOrder].index)->setFocusPolicy(Qt::NoFocus);
            }
        }

        qDebug() << "mousePressEvent textEdit status:" << m_editing;
    };
    doMousePress();
    update();
}

void ShapesWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (m_drawArtboardSize && m_isPressed)
    {
        m_drawArtboardSize = false;
        resizeArtboardByDrag(e->pos());
    }

    if (e->button() == Qt::RightButton)
    {
        m_menu->popup(mapToGlobal(e->pos()));
    }

    if (m_imageCutting)
    {
        QPointF tmpPos = QPointF(m_cutShape.mainPoints[3].x(),
                                                      m_cutShape.mainPoints[3].y());
        showCutImageTips(tmpPos);
    }

    if (e->modifiers() == Qt::NoModifier)
    {
        GlobalShortcut::instance()->setShiftScStatus(false);
        GlobalShortcut::instance()->setAltScStatus(false);
    }
    m_isShiftPressed = GlobalShortcut::instance()->shiftSc();
    m_isAltPressed = GlobalShortcut::instance()->altSc();

    if (m_selectedOrder != -1 && m_isPressed && m_isMoving) {
        m_stickSelectedShape = m_shapes[m_selectedOrder];
        compressToImage();
        update();
    }
    m_isMoving = false;
    m_isPressed = false;
    m_isRotated = false;
    m_inBtmRight = false;
    m_degreeLabel->hide();

    if (m_isRecording && !m_isSelected && m_pos2 != QPointF(0, 0))
    {
        if (m_currentType == "arrow" || m_currentType == "straightLine")
        {
            if (m_currentShape.points.length() == 2)
            {
                if (m_isShiftPressed)
                {
                    if (std::atan2(std::abs(m_pos2.y() - m_pos1.y()),
                                   std::abs(m_pos2.x() - m_pos1.x()))
                            *180/M_PI < 45)
                    {
                        m_pos2 = QPointF(m_pos2.x(), m_pos1.y());
                    } else
                    {
                        m_pos2 = QPointF(m_pos1.x(), m_pos2.y());
                    }
                }

                m_currentShape.points[1] = QPointF(m_pos2.x(), m_pos2.y());
                m_currentShape.mainPoints = getMainPoints(
                            m_currentShape.points[0], m_currentShape.points[1]);
                appendShape(m_currentShape);
                m_needCompress = true;
            }
        } else if (m_currentType == "arbitraryCurve" || m_currentType == "blur")
        {
            FourPoints lineFPoints = fourPointsOfLine(m_currentShape.points);
            m_currentShape.mainPoints = lineFPoints;
            appendShape(m_currentShape);
        } else if (m_currentType != "text")
        {
            FourPoints rectFPoints;
            if (m_currentShape.type == "cutImage")
            {
                QString ration = ConfigSettings::instance()->value("cut", "ration").toString();
                rectFPoints = getRationFPoints(m_pos1, m_pos2, ration);
            } else {
                if (!m_isAltPressed) {
                    rectFPoints = getMainPoints(
                            QPointF(m_pos1.x(), m_pos1.y()),
                            QPointF(m_pos2.x(), m_pos2.y()), m_isShiftPressed);
                } else {
                    if (m_altCenterPos != QPointF(0, 0))
                        rectFPoints = getMainPointsByAlt(
                                QPointF(m_altCenterPos.x(), m_altCenterPos.y()),
                                QPointF(m_movingPoint.x(), m_movingPoint.y()), m_isShiftPressed
                                );
                }
            }
            m_currentShape.mainPoints = rectFPoints;

            if (m_currentShape.type == "cutImage")
            {
                m_cutShape.type = "cutImage";
                m_cutShape.mainPoints = rectFPoints;
                appendShape(m_cutShape);
                emit finishedDrawCut(m_currentShape.mainPoints[3]);
            } else {
                appendShape(m_currentShape);
                m_needCompress = true;
            }
        }

        //CompressToImage if m_bottomPixmap isn't empty.
        if (m_needCompress && m_bgContainShapeNum != m_shapes.length())
        {
            compressToImage();
        }

        if (m_imageCutting)
            clearSelected();
    }

    m_isRecording = false;

    if (m_currentShape.type != "text")
    {
        for(int i = 0; i < m_currentShape.mainPoints.length(); i++)
        {
            m_currentShape.mainPoints[i] = QPointF(0, 0);
        }
    }

    qDebug() << "RelaseEvent:" << m_shapes.length();
    m_currentShape.points.clear();
    m_pos1 = QPointF(0, 0);
    m_pos2 = QPointF(0, 0);
    m_pressedPoint = QPointF(0, 0);
    m_altCenterPos = QPointF(0, 0);
    m_initAltStart = false;
    update();
}

void ShapesWidget::mouseMoveEvent(QMouseEvent *e)
{
    auto doMouseMove = [=]() {
        qDebug() << "PressEvent:" << "Shift pressed!";
        m_isMoving = true;

        m_movingPoint = QPointF(e->pos().x(), e->pos().y());
        if (m_imageCutting && m_isCutImageResize
                && m_resizeDirection == Moving && m_isPressed)
        {
            if (m_shapes[m_shapes.length() - 1].type == "cutImage")
            {
                for(int j = 0; j < m_shapes[m_shapes.length() - 1].mainPoints.length(); j++)
                {
                    qDebug() << "cutImage:" << QPointF(e->pos().x() - m_pressedPoint.x(),
                                                   e->pos().y() - m_pressedPoint.y());
                    m_shapes[m_shapes.length() - 1].mainPoints[j] = QPointF(
                    m_shapes[m_shapes.length() - 1].mainPoints[j].x() + e->pos().x() - m_pressedPoint.x(),
                    m_shapes[m_shapes.length() - 1].mainPoints[j].y() + e->pos().y() - m_pressedPoint.y());
                }
                m_cutShape.mainPoints = m_shapes[m_shapes.length() - 1].mainPoints;
                m_pressedPoint = e->pos();
            }
            return;
        } else if (m_imageCutting && m_isCutImageResize
                   && m_resizeDirection != Moving && m_isPressed)
        {
            handleCutShapeResize(m_movingPoint, m_clickedKey);
            return;
        }

        if (m_selectedOrder == -1)
        {
            QRect btmRightRect = rightBottomRect();
            if (pointOnRect(btmRightRect, e->pos()))
            {
                m_resizeDirection = Right;
                m_stickCurosr = true;
                m_cursorInBtmRight = true;

                qApp->setOverrideCursor(Qt::SizeFDiagCursor);
            } else {
                if (!m_isPressed) {
                    m_stickCurosr = false;
                    updateCursorShape();
                }

                m_cursorInBtmRight = false;
            }

            if (m_stickCurosr && m_isPressed && m_drawArtboardSize)
            {
                emit updateMiddleWidgets("adjustSize");
                resizeArtboardByDrag(m_movingPoint);
            }
        } else {
            m_stickCurosr = false;
            m_cursorInBtmRight = false;
            m_drawArtboardSize = false;
        }

        if (m_isRecording && m_isPressed && !m_cursorInBtmRight)
        {
            m_pos2 = e->pos();

            if (m_currentShape.type == "arrow" || m_currentShape.type == "straightLine")
            {
                if (m_currentShape.points.length() <= 1)
                {
                    if (m_isShiftPressed)
                    {
                        if (std::atan2(std::abs(m_pos2.y() - m_pos1.y()),
                                       std::abs(m_pos2.x() - m_pos1.x()))*180/M_PI < 45)
                        {
                            m_currentShape.points.append(QPointF(m_pos2.x(), m_pos1.y()));
                        } else
                        {
                            m_currentShape.points.append(QPointF(m_pos1.x(), m_pos2.y()));
                        }
                    } else
                    {
                        m_currentShape.points.append(QPointF(m_pos2.x(), m_pos2.y()));
                    }
                } else {
                    if (m_isShiftPressed)
                    {
                        if (std::atan2(std::abs(m_pos2.y() - m_pos1.y()),
                                                std::abs(m_pos2.x() - m_pos1.x()))*180/M_PI < 45)
                        {
                            m_currentShape.points[1] = QPointF(m_pos2.x(), m_pos1.y());
                        } else
                        {
                            m_currentShape.points[1] = QPointF(m_pos1.x(), m_pos2.y());
                        }
                    } else
                    {
                        m_currentShape.points[1] = QPointF(m_pos2.x(), m_pos2.y());
                    }
                }
            }

            if (m_currentShape.type == "arbitraryCurve"|| m_currentShape.type == "blur")
            {
                m_currentShape.points.append(QPointF(m_pos2.x(), m_pos2.y()));
            }
        } else if (!m_isRecording && m_isPressed && !m_cursorInBtmRight)
        {
            if (m_isRotated && m_isPressed)
            {
                handleRotate(m_movingPoint);
            }

            if (m_isResize && m_isPressed)
            {
                // resize function
                m_cutImageTips->hide();
                handleResize(QPointF(m_movingPoint), m_clickedKey);
                if (m_cutShape.type == "cutImage")
                {
                    m_cutImageTips->showTips(mapToGlobal(QPoint(
                                                 int(m_cutShape.mainPoints[3].x()),
                                                 int(m_cutShape.mainPoints[3].y()))));
                } else {
                    qDebug() << "FDBBNM";
                }
            }

            if (m_isSelected && m_isPressed && m_selectedOrder != -1)
            {
                m_cutImageTips->hide();
                handleDrag(m_pressedPoint, m_movingPoint);
                m_selectedShape = m_shapes[m_selectedOrder];
                if (m_shapes[m_shapes.length() - 1].type == "cutImage")
                {
                    m_cutImageTips->showTips(mapToGlobal(QPoint(
                                             int(m_shapes[m_shapes.length() - 1].mainPoints[3].x()),
                                             int(m_shapes[m_shapes.length() - 1].mainPoints[3].y()))));
                }

                m_pressedPoint = m_movingPoint;
            }
        } else
        {
            if (!m_isRecording && !m_cursorInBtmRight)
            {
                m_isHovered = false;
                hoverOnShapes(m_movingPoint);
                qDebug() << "moving hoverOnShapes!";
            } else
            {
                //TODO
                qDebug() << "no hover!";
            }
        }
    };

    doMouseMove();
    auto currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if (currentTime - m_lastUpdateTime.toMSecsSinceEpoch() > 35) {
        m_lastUpdateTime = QDateTime::currentDateTime();
        update();
    }
//    QFrame::mouseMoveEvent(e);
}

void ShapesWidget::updateTextRect(TextEdit* edit, QRectF newRect)
{
    int index = edit->getIndex();
    qDebug() << "updateTextRect:" << newRect << index;
    for (int j = 0; j < m_shapes.length(); j++)
    {
        if (m_shapes[j].type == "text" && m_shapes[j].index == index)
        {
            m_shapes[j].mainPoints[0] = QPointF(newRect.x(), newRect.y());
            m_shapes[j].mainPoints[1] = QPointF(newRect.x() , newRect.y() + newRect.height());
            m_shapes[j].mainPoints[2] = QPointF(newRect.x() + newRect.width(), newRect.y());
            m_shapes[j].mainPoints[3] = QPointF(newRect.x() + newRect.width(),
                                                                               newRect.y() + newRect.height());
            m_shapes[j].text = edit->toPlainText();
            m_currentShape = m_shapes[j];
            m_selectedShape = m_shapes[j];
            m_selectedIndex = m_shapes[j].index;
            m_selectedOrder = j;
        }
    }
    update();
}

void ShapesWidget::paintImgPoint(QPainter &painter, QPointF pos,
                                 QPixmap img,  bool isResize)
{
    qreal tmpRation = m_resizeRation;
    if (isResize)
    {
        painter.drawPixmap(QPoint(pos.x()*tmpRation - DRAG_BOUND_RADIUS,
                                  pos.y()*tmpRation - DRAG_BOUND_RADIUS), img);
    } else
    {
        painter.drawPixmap(QPoint(pos.x()*tmpRation - 12, pos.y()*tmpRation - 12), img);
    }
}

void ShapesWidget::paintRect(QPainter &painter, Toolshape shape, bool saveTo)
{
    QPen rectPen;
    rectPen.setColor(shape.strokeColor);
    rectPen.setWidthF(shape.lineWidth - 0.5);
    rectPen.setJoinStyle(Qt::MiterJoin);
    painter.setPen(rectPen);
    painter.setBrush(QBrush(shape.fillColor));

    FourPoints rectFPoints = shape.mainPoints;
    QPainterPath rectPath;

    qreal tmpRation = shape.scaledRation;
    if (saveTo)
        tmpRation = m_lastRation*shape.scaledRation;

    rectPath.moveTo(rectFPoints[0].x()*tmpRation, rectFPoints[0].y()*tmpRation);
    rectPath.lineTo(rectFPoints[1].x()*tmpRation, rectFPoints[1].y()*tmpRation);
    rectPath.lineTo(rectFPoints[3].x()*tmpRation, rectFPoints[3].y()*tmpRation);
    rectPath.lineTo(rectFPoints[2].x()*tmpRation, rectFPoints[2].y()*tmpRation);
    rectPath.lineTo(rectFPoints[0].x()*tmpRation, rectFPoints[0].y()*tmpRation);
    painter.drawPath(rectPath);

}

void ShapesWidget::paintEllipse(QPainter &painter, Toolshape shape, bool saveTo)
{
    QPen ellipsePen;
    ellipsePen.setColor(shape.strokeColor);
    ellipsePen.setWidthF(shape.lineWidth - 0.5);
    ellipsePen.setJoinStyle(Qt::MiterJoin);
    painter.setPen(ellipsePen);
    painter.setBrush(QBrush(shape.fillColor));

    FourPoints ellipseFPoints = shape.mainPoints;
    qreal tmpRation = shape.scaledRation;
    if (saveTo)
        tmpRation = m_lastRation*shape.scaledRation;

    for(int i = 0; i < shape.mainPoints.length(); i++)
    {
        ellipseFPoints[i] = QPointF(shape.mainPoints[i].x()*tmpRation,
                                                        shape.mainPoints[i].y()*tmpRation);
    }

    FourPoints minorPoints = getAnotherFPoints(ellipseFPoints);
    QList<QPointF> eightControlPoints = getEightControlPoint(ellipseFPoints);

    QPainterPath ellipsePath;
    ellipsePath.moveTo(minorPoints[0].x(), minorPoints[0].y());
    ellipsePath.cubicTo(eightControlPoints[0], eightControlPoints[1], minorPoints[1]);
    ellipsePath.cubicTo(eightControlPoints[4], eightControlPoints[5], minorPoints[2]);
    ellipsePath.cubicTo(eightControlPoints[6], eightControlPoints[7], minorPoints[3]);
    ellipsePath.cubicTo(eightControlPoints[3], eightControlPoints[2], minorPoints[0]);

    painter.drawPath(ellipsePath);
}

void ShapesWidget::paintArrow(QPainter &painter, Toolshape shape,
                              bool saveTo, bool isStraight)
{
    QPen pen;
    pen.setColor(shape.strokeColor);
    pen.setWidthF(shape.lineWidth - 0.5);
    pen.setJoinStyle(Qt::MiterJoin);
    painter.setPen(pen);

    QList<QPointF> lineFPoints = shape.points;
    qreal tmpRation = shape.scaledRation;
    if (saveTo)
        tmpRation = m_lastRation*shape.scaledRation;

    for(int k = 0; k < shape.points.length(); k++)
    {
        lineFPoints[k] = QPointF(shape.points[k].x()*tmpRation, shape.points[k].y()*tmpRation);
    }

    if (lineFPoints.length() == 2)
    {
        if (!isStraight)
        {
            QList<QPointF> arrowPoints = pointOfArrow(lineFPoints[0],
                                                         lineFPoints[1], 8+(painter.pen().widthF() - 1)*2);
            QPainterPath path;
            const QPen oldPen = painter.pen();

            if (arrowPoints.length() >=4) {
                painter.drawLine(lineFPoints[0], arrowPoints[0]);
                path.moveTo(arrowPoints[3].x(), arrowPoints[3].y());
                path.lineTo(arrowPoints[1].x(), arrowPoints[1].y());
                path.lineTo(arrowPoints[2].x(), arrowPoints[2].y());
                path.lineTo(arrowPoints[3].x(), arrowPoints[3].y());
            }
            painter.setPen (Qt::NoPen);
            painter.fillPath(path, QBrush(oldPen.color()));
        } else
        {
            painter.drawLine(lineFPoints[0], lineFPoints[1]);
        }
    }
}

void ShapesWidget::paintStraightLine(QPainter &painter, Toolshape shape, bool saveTo)
{
    paintArrow(painter, shape, saveTo, true);
}

void ShapesWidget::paintArbitraryCurve(QPainter &painter, Toolshape shape, bool saveTo)
{
    QPen pen;
    pen.setColor(shape.strokeColor);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setWidthF(shape.lineWidth - 0.5);
    painter.setPen(pen);
    painter.setBrush(QBrush(Qt::transparent));
    QList<QPointF> lineFPoints = shape.points;
    qreal tmpRation = shape.scaledRation;
    if (saveTo)
        tmpRation = m_lastRation*shape.scaledRation;

    for(int k = 0; k < shape.points.length(); k++)
    {
        lineFPoints[k] = QPointF(shape.points[k].x()*tmpRation, shape.points[k].y()*tmpRation);
    }

    QPainterPath linePaths;
    if (lineFPoints.length() >= 1)
        linePaths.moveTo(lineFPoints[0]);
    else
        return;

    for (int k = 1; k < lineFPoints.length() - 2; k++)
    {
        linePaths.quadTo(lineFPoints[k], lineFPoints[k+1]);
    }
    painter.drawPath(linePaths);
}

void ShapesWidget::paintText(QPainter &painter, Toolshape shape, bool saveTo)
{
    //process text's zoom...
    Q_UNUSED(saveTo);
    QPen textPen;
    textPen.setStyle(Qt::DashLine);
    textPen.setColor("#01bdff");
    painter.setPen(textPen);

    qDebug() << "paintTextd" << shape.mainPoints.length() << shape.mainPoints;

    FourPoints rectFPoints = shape.mainPoints;

    if (shape.mainPoints.length() >= 4)
    {
        painter.drawLine(rectFPoints[0], rectFPoints[1]);
        painter.drawLine(rectFPoints[1], rectFPoints[3]);
        painter.drawLine(rectFPoints[3], rectFPoints[2]);
        painter.drawLine(rectFPoints[2], rectFPoints[0]);
    }
}

QPainterPath ShapesWidget::drawPair(QPainter &p,
                      QPointF p1, QSizeF size1, QColor c1,
                      QPointF p2, QSizeF size2, QColor c2,
                      QPainterPath oldpath)
{
    QPainterPath path;
    QRectF rect1(p1.x() - size1.width() / 2, p1.y() - size1.height() / 2,
                 size1.width(), size1.width());

    QRectF rect2(p2.x() - size2.width() / 2, p2.y() - size2.height() / 2,
                 size2.width(), size2.width());

    auto x1 = p1.x();
    auto y1 = p1.y();
    auto x2 = p2.x();
    auto y2 = p2.y();
    auto dx = x2 - x1;
    auto dy = y2 - y1;

    auto r1 = qSqrt(size1.width() * size1.width() * 2 / 4);
    r1 = size1.width() / 2;
    auto r2 = qSqrt(size2.width() * size2.width() * 2 / 4);
    r2 = size2.width() / 2;
    auto R = qSqrt(dx * dx + dy * dy);

    auto xp1 = x1 + r1 * (y2 - y1) / R;
    auto yp1 = y1 - r1 * (x2 - x1) / R;
    path.moveTo(xp1, yp1);

    auto xp2 = x2 + r2 * (y2 - y1) / R;
    auto yp2 = y2 - r2 * (x2 - x1) / R;
    path.lineTo(xp2, yp2);

    auto xp3 = x2 - r2 * (y2 - y1) / R;
    auto yp3 = y2 + r2 * (x2 - x1) / R;
    path.lineTo(xp3, yp3);

    auto xp4 = x1 - r1 * (y2 - y1) / R;
    auto yp4 = y1 + r1 * (x2 - x1) / R;
    path.lineTo(xp4, yp4);

    path.closeSubpath();

    path.setFillRule(Qt::WindingFill);
    path.addRoundedRect(rect1, r1, r1);

    path.addRoundedRect(rect2, r2, r2);

    QLinearGradient lg(p1, p2);
    lg.setColorAt(0, c1);
    lg.setColorAt(1, c2);
    p.setBrush(lg);

    auto com = path.subtracted(oldpath);
    p.setClipPath(com);
    p.drawPixmap(0, 0,  width(), height(),  TempFile::instance()->getBlurFileName());
    p.setClipping(false);
    p.fillPath(com, QBrush(QColor(255, 255, 255, 10)));

    return path;
}

void ShapesWidget::paintPointList(QPainter &p, QList<QPointF> points, int lineWidth)
{
    if (points.size() < 2)
    {
        return;
    }

    QList<QSizeF> sizes;
    double maxSize = lineWidth;
     for (int i = 0; i < points.size(); ++i)
    {
        auto size = maxSize - /*dsize * i*/0;
        sizes << QSizeF(size, size);
    }

    QPainterPath op;
    for (int i = 0 ; i < points.size() - 1; ++i)
    {
        auto point1 = points.at(i);
        auto point2 = points.at(i + 1);
        auto size1 = sizes.at(i);
        auto size2 = sizes.at(i + 1);
        auto c1 = Qt::transparent;
        auto c2 = Qt::transparent;

        op = drawPair(p, point1, size1, c1,
                      point2, size2, c2,
                      op);
//        qDebug() << size1 << size2;
    }
}

void ShapesWidget::paintBlur(QPainter &painter, Toolshape shape, bool saveTo)
{
    QPen pen;
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setBrush(QBrush(Qt::transparent));

    qreal tmpRation = shape.scaledRation;
    if (saveTo)
        tmpRation = m_lastRation*shape.scaledRation;

    QList<QPointF> lineFPoints = shape.points;

    for(int i = 0; i < shape.points.length(); i++)
    {
        lineFPoints[i] = QPointF(shape.points[i].x()*tmpRation,
                                                 shape.points[i].y()*tmpRation);
    }

    paintPointList(painter, lineFPoints, shape.lineWidth);
}

void ShapesWidget::paintCutImageRect(QPainter &painter, Toolshape shape)
{
    FourPoints rectFPoints = shape.mainPoints;

    QPainterPath rectPath;
    rectPath.moveTo(rectFPoints[0].x(), rectFPoints[0].y());
    rectPath.lineTo(rectFPoints[1].x(),rectFPoints[1].y());
    rectPath.lineTo(rectFPoints[3].x(),rectFPoints[3].y());
    rectPath.lineTo(rectFPoints[2].x(),rectFPoints[2].y());
    rectPath.lineTo(rectFPoints[0].x(),rectFPoints[0].y());

    QPen pen;
    pen.setColor(Qt::black);
    pen.setStyle(Qt::SolidLine);
    pen.setWidthF(0.5);
    painter.setPen(pen);
    painter.setBrush(Qt::transparent);
    painter.setClipping(true);
    painter.setClipPath(rectPath);

    painter.drawPixmap(this->rect(), m_BeforeCutBg);
    painter.setClipping(false);

    pen.setColor(Qt::white);
    pen.setStyle(Qt::DashLine);
    pen.setWidthF(0.5);
    painter.setPen(pen);
    painter.drawPath(rectPath);
    pen.setWidthF(0.5);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);

    QList<QPointF> cutPoints = trisectionPoints(rectFPoints);
    if (cutPoints.length() == 8)
    {
        painter.drawLine(cutPoints[0], cutPoints[5]);
        painter.drawLine(cutPoints[1], cutPoints[4]);
        painter.drawLine(cutPoints[2], cutPoints[7]);
        painter.drawLine(cutPoints[3], cutPoints[6]);
    } else
    {
        qDebug() << "cutPoints length:" << cutPoints.length();
    }

    qreal spacing = 8;

    QPointF pointF00 = QPointF(rectFPoints[0].x(), rectFPoints[0].y() + spacing);
    QPointF pointF01 = QPointF(rectFPoints[0].x() + spacing, rectFPoints[0].y());

    QPointF pointF10 = QPointF(rectFPoints[1].x(), rectFPoints[1].y() - spacing);
    QPointF pointF11 = QPointF(rectFPoints[1].x() + spacing, rectFPoints[1].y());

    QPointF pointF20 = QPointF(rectFPoints[2].x() - spacing, rectFPoints[2].y());
    QPointF pointF21 = QPointF(rectFPoints[2].x(), rectFPoints[2].y() + spacing);

    QPointF pointF30 = QPointF(rectFPoints[3].x(), rectFPoints[3].y() - spacing);
    QPointF pointF31 = QPointF(rectFPoints[3].x() - spacing, rectFPoints[3].y());
    pen.setStyle(Qt::SolidLine);
    pen.setWidthF(1);
    painter.setPen(pen);

    painter.drawLine(pointF00, rectFPoints[0]);
    painter.drawLine(rectFPoints[0], pointF01);

    painter.drawLine(pointF10, rectFPoints[1]);
    painter.drawLine(rectFPoints[1], pointF11);

    painter.drawLine(pointF20, rectFPoints[2]);
    painter.drawLine(rectFPoints[2], pointF21);

    painter.drawLine(pointF30, rectFPoints[3]);
    painter.drawLine(rectFPoints[3], pointF31);
}

void ShapesWidget::paintImage(QPainter &painter, Toolshape imageShape, bool saveTo)
{
    QPixmap pixmap = QPixmap::fromImage(QImage(imageShape.imagePath).
                                        mirrored(imageShape.isHorFlip, imageShape.isVerFlip));

    qreal tmpRation = imageShape.scaledRation;
    if (saveTo)
        tmpRation = m_lastRation*imageShape.scaledRation;

    QPointF startPos = QPointF(imageShape.mainPoints[0].x()*tmpRation,
                                                  imageShape.mainPoints[0].y()*tmpRation);
    QSize imgSize = QSize(imageShape.imageSize.width()*tmpRation,
                                           imageShape.imageSize.height()*tmpRation);

    pixmap = pixmap.scaled(imgSize, Qt::IgnoreAspectRatio);
    qreal rotateAngle =  imageShape.rotate;
    QMatrix matrix;
    matrix.rotate(rotateAngle*180/M_PI);
    pixmap = pixmap.transformed(matrix);
    QSize pixSize = imgSize;

    int degree = int(imageShape.rotate*180/M_PI)%360;
    qreal angle = degree*M_PI/180;
    qDebug() << "image rotate angle:" << angle;

    if (imageShape.rotate == 0)
    {
        painter.drawPixmap(startPos, pixmap);
    } else {
        if (imageShape.rotate > 0)
        {
            if (angle <= M_PI/2)
            {
                qreal x = pixSize.height()*sin(angle);
                painter.drawPixmap(startPos.x() - x, startPos.y(), pixmap);
            } else if(angle > M_PI/2 && angle <= M_PI)
            {
                qreal x = std::abs(pixSize.width()*sin(angle - M_PI/2) )+
                        std::abs(pixSize.height()*cos(angle - M_PI/2));
                qreal y = pixSize.height()*std::sin(angle - M_PI/2);

                painter.drawPixmap(startPos.x() - x, startPos.y() - y, pixmap);
            }
            else if (angle > M_PI && angle <= M_PI*1.5)
            {
                qreal x = std::abs(pixSize.width()*cos(angle - M_PI));
                qreal y = std::abs(pixSize.width()*sin(angle - M_PI))+
                        std::abs(pixSize.height()*cos(angle - M_PI));

                painter.drawPixmap(startPos.x() - x, startPos.y() - y, pixmap);
            }
            else if (angle > M_PI*1.5 && angle < M_PI*2)
            {
                qreal y = std::abs(pixSize.width()*cos(angle - M_PI*1.5));
                painter.drawPixmap(startPos.x(), startPos.y() - y, pixmap);
            }
        } else if (imageShape.rotate < 0)
        {
            if (angle >= -M_PI/2)
            {
                qreal y = std::abs(pixSize.width()*cos(angle - M_PI*1.5));
                painter.drawPixmap(startPos.x(), startPos.y() - y, pixmap);
            } else if(angle  < -M_PI/2 && angle >= -M_PI)
            {
                qreal x = std::abs(pixSize.width()*cos(angle - M_PI));
                qreal y = std::abs(pixSize.width()*sin(angle - M_PI))+
                        std::abs(pixSize.height()*cos(angle - M_PI));

                painter.drawPixmap(startPos.x()  - x, startPos.y()  - y, pixmap);
            } else if (angle < -M_PI && angle >= -M_PI*1.5)
            {
                qreal x = std::abs(pixSize.width()*sin(angle - M_PI/2) )+
                        std::abs(pixSize.height()*cos(angle - M_PI/2));
                qreal y = pixSize.height()*std::sin(angle - M_PI/2);

                painter.drawPixmap(startPos.x() - x, startPos.y()  - y, pixmap);
            } else if (angle < -M_PI*1.5 && angle >= -M_PI*2)
            {
                qreal x = pixSize.height()*sin(angle);
                painter.drawPixmap(startPos.x() - x, startPos.y(), pixmap);
            }
            else
            {
                painter.drawPixmap(startPos.x(), startPos.y(), pixmap);
            }
        }
    }
}

void ShapesWidget::paintSelectedShape(QPainter &painter, Toolshape shape,
                                      bool noRotatePoint)
{
    if (shape.type.isEmpty())
        return;

    QPen selectedPen;
    selectedPen.setColor(QColor("#01bdff"));
    selectedPen.setWidth(1);

    QPen dragPen;
    dragPen.setStyle(Qt::DashLine);
    dragPen.setColor(QColor("#888888"));
    dragPen.setWidth(1);
    if (shape.type == "rectangle" || shape.type == "oval" || shape.type == "blur"
            || shape.type == "arbitraryCurve" || shape.type == "image")
    {
        if (noRotatePoint)
            painter.setPen(dragPen);
        else{
            if (shape.type == "rectangle")
            {
                QPen newSelectPen;
                newSelectPen.setColor(Qt::transparent);
                painter.setPen(newSelectPen);
            } else {
                QPen rectPen;
                rectPen.setColor("#888888");
                rectPen.setWidth(1);
                painter.setPen(rectPen);
            }
        }

        paintSelectedRect(painter, shape.mainPoints);
        paintSelectedRectPoints(painter, shape.mainPoints, noRotatePoint);
    } else if (shape.type == "arrow" || shape.type == "straightLine")
    {
        if (shape.points.length() == 2)
        {
            if (!noRotatePoint)
                painter.setPen(selectedPen);
            else
                painter.setPen(dragPen);
            QLineF line(shape.points[0], shape.points[1]);
            painter.drawLine(line);
            paintImgPoint(painter, QPointF(shape.points[0].x(),
                          shape.points[0].y()),RESIZE_POINT_IMG);
            paintImgPoint(painter, QPointF(shape.points[1].x(),
                          shape.points[1].y()), RESIZE_POINT_IMG);
        }
    } else if (shape.type == "text") {
        selectedPen.setStyle(Qt::DashLine);
        selectedPen.setWidth(1);
        if (!noRotatePoint)
            painter.setPen(selectedPen);
        else
            painter.setPen(dragPen);
        paintSelectedRect(painter, shape.mainPoints);
    }
}

void ShapesWidget::resizeEvent(QEvent* e)
{
    Q_UNUSED(e);

    qDebug() << "ResizeEvent by canvasSize...";
    updateCanvasSize();
}

void ShapesWidget::pressFromParent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    clearSelected();
    m_isRecording = false;
    m_isPressed = false;
}

//void ShapesWidget::releaseFromParent(QMouseEvent *ev)
//{
//    mouseReleaseEvent(ev);
//    qDebug() << "mouseReleaseEvent";
//}

void ShapesWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.drawPixmap(0, 0, m_bottomPixmap);

    if (!m_imageCutting)
    {
        if (m_selectedOrder != -1 && m_selectedOrder < m_shapes.length()
             /*&& m_isPressed*/)
        {
            painter.setOpacity(1);

            if (m_isMoving && m_isPressed)
            {
                paintSelectedShape(painter, m_shapes[m_selectedOrder], true);
            }
            paintSelectedShape(painter, m_stickSelectedShape);
        }
    }

    if (m_cutImageOrder != -1 && m_cutShape.type == "cutImage")
    {
        paintShape(painter, m_cutShape, false);
    }

    if (!m_inBtmRight && ((m_pos1 != QPointF(0, 0))
        || m_currentShape.type == "text"))
    {
        Toolshape drawShape;
        drawShape = m_currentShape;
        bool drawCutRect = false;

        if (m_currentType != "text")
        {
            if (m_currentType != "cutImage")
            {
                m_isShiftPressed = GlobalShortcut::instance()->shiftSc();
                m_isAltPressed = GlobalShortcut::instance()->altSc();

                if (!m_isAltPressed) {
                    if (!m_isResize && !m_isRotated && m_pos2 != QPointF(0, 0))
                    {
                        drawShape.mainPoints = getMainPoints(
                            QPointF(m_pos1.x(), m_pos1.y()),
                            QPointF(m_pos2.x(), m_pos2.y()), m_isShiftPressed);
                    }
                } else {
                    if (m_altCenterPos != QPointF(0, 0) && !m_isResize && !m_isRotated
                            && !(m_isSelected && m_isPressed && m_selectedOrder != -1))
                    {
                        m_isShiftPressed = GlobalShortcut::instance()->shiftSc();
                        drawShape.mainPoints = getMainPointsByAlt(
                                QPointF(m_altCenterPos.x(), m_altCenterPos.y()),
                                QPointF(m_movingPoint.x(), m_movingPoint.y()), m_isShiftPressed);
                    }
                }
            } else {
                if (m_pos2 != QPointF(0, 0))
                {
                    drawCutRect = true;
                    QString ration = ConfigSettings::instance()->value("cut", "ration").toString();
                    drawShape.mainPoints = getRationFPoints(m_pos1, m_pos2, ration);
                }
            }
        } else
        {
            drawShape.mainPoints = m_currentShape.mainPoints;
        }
        drawShape.lineWidth = m_currentShape.lineWidth;
        //Draw current shape

        qDebug() << "paint current shape:" << m_currentType << m_editing;

        if (m_currentType != "cutImage")
        {
            paintShape(painter, drawShape, false);
        } else
        {
            if (drawCutRect)
            {
                paintShape(painter, drawShape, false);
            }
        }
    } else
    {
        qDebug() << "other position:" << m_pos1 << m_pos2;
    }

     if (m_isHovered && m_selectedOrder != m_hoveredIndex)
    {
        paintHoveredShape(painter, m_hoveredShape);
    }
}

void ShapesWidget::paintShape(QPainter &painter, Toolshape shape, bool saveTo, bool selected)
{
    //If choose autoCrop the first shape's coordinate(0, 0)
    if (shape.mainPoints.length() < 4 || (shape.type != "image" && (
         shape.mainPoints[0] == QPointF(0, 0)
         && shape.mainPoints[1] == QPoint(0, 0))))
    {
        qDebug() << "UUUU" << shape.type;
        return;
    }

    QPen selectedPen;
    selectedPen.setColor(QColor("#01bdff"));
    selectedPen.setWidth(1);

    if (shape.type == "rectangle")
    {
        paintRect(painter, shape, saveTo);
    } else if (shape.type == "oval")
    {
        paintEllipse(painter, shape, saveTo);
    } else if (shape.type == "image")
    {
        if (!(selected && m_isMoving && m_isPressed))
            paintImage(painter, shape, saveTo);
    } else if (shape.type == "arrow")
    {
        paintArrow(painter, shape, saveTo);
    } else if (shape.type == "arbitraryCurve")
    {
        paintArbitraryCurve(painter, shape, saveTo);
    } else if (shape.type == "blur")
    {
        paintBlur(painter, shape, saveTo);
    } else if (shape.type == "straightLine")
    {
        paintStraightLine(painter, shape, saveTo);
    } else if (shape.type == "text" && !m_clearAllTextBorder)
    {
        qDebug() << "------------------" << m_editing;
        if (m_editMap.contains(shape.index))
        {
            if (!m_editMap[shape.index]->isReadOnly())
            {
                paintText(painter, shape, saveTo);
            }
        }
    } else if (shape.type == "cutImage")
    {
        paintCutImageRect(painter, shape);
    }
}

void ShapesWidget::paintHoveredShape(QPainter &painter, Toolshape shape)
{
    QPen hoverPen;
    hoverPen.setStyle(Qt::DashLine);
    hoverPen.setColor("#01bdff");
    painter.setBrush(QBrush(Qt::transparent));
    painter.setPen(hoverPen);
    qreal tmpRation = shape.scaledRation;

    if (shape.type == "image" || shape.type == "oval" || shape.type == "rectangle"
            || shape.type == "text" || shape.type == "arbitraryCurve" || shape.type == "blur")
    {

        QPainterPath rectPath;
        FourPoints rectFPoints = shape.mainPoints;
        rectPath.moveTo(rectFPoints[0].x()*tmpRation, rectFPoints[0].y()*tmpRation);
        rectPath.lineTo(rectFPoints[1].x()*tmpRation, rectFPoints[1].y()*tmpRation);
        rectPath.lineTo(rectFPoints[3].x()*tmpRation, rectFPoints[3].y()*tmpRation);
        rectPath.lineTo(rectFPoints[2].x()*tmpRation, rectFPoints[2].y()*tmpRation);
        rectPath.lineTo(rectFPoints[0].x()*tmpRation, rectFPoints[0].y()*tmpRation);

        painter.drawPath(rectPath);
    } else if (shape.type == "arrow" || shape.type == "straightLine")
    {
        QList<QPointF> points = shape.points;
        if (points.length() == 2)
        {
            QPainterPath linePath;
            linePath.moveTo(points[0].x()*tmpRation, points[0].y()*tmpRation);
            linePath.lineTo(points[1].x()*tmpRation, points[1].y()*tmpRation);

            painter.drawPath(linePath);
        }
    }
}

void ShapesWidget::paintSelectedRect(QPainter &painter, FourPoints mainPoints)
{
    painter.setBrush(QBrush(Qt::transparent));
    FourPoints rectFPoints =  mainPoints;
    QPainterPath rectPath;

    qreal tmpRation = m_resizeRation;
    rectPath.moveTo(rectFPoints[0].x()*tmpRation, rectFPoints[0].y()*tmpRation);
    rectPath.lineTo(rectFPoints[1].x()*tmpRation, rectFPoints[1].y()*tmpRation);
    rectPath.lineTo(rectFPoints[3].x()*tmpRation, rectFPoints[3].y()*tmpRation);
    rectPath.lineTo(rectFPoints[2].x()*tmpRation, rectFPoints[2].y()*tmpRation);
    rectPath.lineTo(rectFPoints[0].x()*tmpRation, rectFPoints[0].y()*tmpRation);

    qDebug() << "ShapesWidget:" << m_penColor;
    painter.drawPath(rectPath);
}

void ShapesWidget::paintSelectedRectPoints(QPainter &painter,
                                           FourPoints mainPoints, bool noRotatePoint)
{
    if (noRotatePoint)
        return;
    FourPoints mainRationPoints = mainPoints;
    for(int i = 0; i < mainPoints.length(); i++)
    {
        mainRationPoints[i] = QPointF(mainPoints[i].x(), mainPoints[i].y());
    }

    QPointF rotatePoint = getRotatePoint(mainRationPoints[0], mainRationPoints[1],
            mainRationPoints[2], mainRationPoints[3]);
    QPointF middlePoint = QPointF((mainRationPoints[0].x() + mainRationPoints[2].x())/2,
            (mainRationPoints[0].y() + mainRationPoints[2].y())/2);

    painter.setPen(QPen(QColor(Qt::white)));
    painter.drawLine(rotatePoint, middlePoint);
    paintImgPoint(painter, rotatePoint, ROTATE_POINT_IMG, false);
    for(int j = 0; j < mainRationPoints.length(); j++)
    {
        paintImgPoint(painter, QPointF(mainRationPoints[j].x(),
                                   mainRationPoints[j].y()), RESIZE_POINT_IMG);
    }
    FourPoints anotherFPoints = getAnotherFPoints(mainRationPoints);
    for(int k = 0; k < anotherFPoints.length(); k++)
    {
        paintImgPoint(painter, anotherFPoints[k], RESIZE_POINT_IMG);
    }
}

void ShapesWidget::enterEvent(QEvent *e)
{
    Q_UNUSED(e);
    qApp->setOverrideCursor(setCursorShape(m_currentType));
}

void ShapesWidget::leaveEvent(QEvent* e)
{
    Q_UNUSED(e);
    qApp->setOverrideCursor(Qt::ArrowCursor);
}

void ShapesWidget::initShortcut()
{
    QShortcut* deleteShortcut = new QShortcut(
                QKeySequence("delete"), this);
    connect(deleteShortcut, &QShortcut::activated, this,
            &ShapesWidget::deleteCurrentShape);

    QShortcut* saveShortcut = new QShortcut(
                QKeySequence("ctrl+s"), this);
    connect(saveShortcut, &QShortcut::activated, this,
            &ShapesWidget::saveImage);

    QShortcut* rectShortcut = new QShortcut(
                QKeySequence("r"), this);
    connect(rectShortcut, &QShortcut::activated, this,
            [=]{ emit shapePressed("rectangle");});

    QShortcut* ovalShortcut = new QShortcut(
                QKeySequence("o"), this);
    connect(ovalShortcut, &QShortcut::activated, this,
            [=]{ emit shapePressed("oval");});

    QShortcut* penShortcut = new QShortcut(
                QKeySequence("p"), this);
    connect(penShortcut, &QShortcut::activated, this,
            [=]{ emit shapePressed("straightLine");});

    QShortcut* textShortcut = new QShortcut(
                QKeySequence("t"), this);
    connect(textShortcut, &QShortcut::activated, this,
            [=]{ emit shapePressed("text");});

    QShortcut* blurShortcut = new QShortcut(
                QKeySequence("b"), this);
    connect(blurShortcut, &QShortcut::activated, this,
            [=]{ emit shapePressed("blur");});

    QShortcut* selectShortcut = new QShortcut(
                QKeySequence("v"), this);
    connect(selectShortcut, &QShortcut::activated, this,
            [=]{ emit shapePressed("selected");});

    QShortcut* upLayerSc = new QShortcut(
                QKeySequence("ctrl+]"), this);
    QShortcut* downLayerSc = new QShortcut(
                QKeySequence("ctrl+["), this);
    QShortcut* topLayerSc = new QShortcut(
                QKeySequence("ctrl+shift+]"), this);
    QShortcut* bottomLayerSc = new QShortcut(
                QKeySequence("ctrl+shift+["), this);

    connect(upLayerSc, &QShortcut::activated, this, [=]{
        layerSwitch(LayerDirection::UpLayer);
    });
    connect(downLayerSc, &QShortcut::activated, this, [=]{
        layerSwitch(LayerDirection::DownLayer);
    });
    connect(topLayerSc, &QShortcut::activated, this, [=]{
        layerSwitch(LayerDirection::TopLayer);
    });
    connect(bottomLayerSc, &QShortcut::activated, this, [=]{
        layerSwitch(LayerDirection::BottomLayer);
    });

    QShortcut* copySc = new QShortcut(QKeySequence("ctrl+c"), this);
    QShortcut* cutSc = new QShortcut(QKeySequence("ctrl+x"), this);
    QShortcut* pasteSc = new QShortcut(QKeySequence("ctrl+v"), this);

    connect(copySc, &QShortcut::activated, this, [=]{
        copyShape();
    });
    connect(cutSc, &QShortcut::activated, this, [=]{
        cutShape();
    });
    connect(pasteSc, &QShortcut::activated, this, [=]{
        pasteShape();
    });
    QShortcut* showSc = new QShortcut(QKeySequence("ctrl+shift+/"), this);
    connect(showSc, &QShortcut::activated, this, [=]{
        onViewShortcut();
    });
}

void ShapesWidget::onViewShortcut() {
    QRect rect = window()->geometry();
    QPoint pos(rect.x() + rect.width()/2 , rect.y() + rect.height()/2);
    Shortcut sc;
    QStringList shortcutString;
    QString param1 = "-j="+sc.toStr();
    QString param2 = "-p=" + QString::number(pos.x()) + "," + QString::number(pos.y());
    shortcutString << param1 << param2;

    QProcess* shortcutViewProcess = new QProcess();
    shortcutViewProcess->startDetached("deepin-shortcut-viewer", shortcutString);

    connect(shortcutViewProcess, SIGNAL(finished(int)),
            shortcutViewProcess, SLOT(deleteLater()));
}

void ShapesWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->modifiers() == (Qt::AltModifier | Qt::ShiftModifier))
    {
        qDebug() << "combine keyEvent!";
        m_isShiftPressed = true;
        m_isAltPressed = true;
        GlobalShortcut::instance()->setShiftScStatus(true);
        GlobalShortcut::instance()->setAltScStatus(true);
    }
    if (e->key() == Qt::Key_Alt)
    {
        m_isAltPressed = true;
        GlobalShortcut::instance()->setAltScStatus(true);
    }
    if (e->key() == Qt::Key_Shift)
    {
        m_isShiftPressed = true;
        GlobalShortcut::instance()->setShiftScStatus(true);
    }
}

void ShapesWidget::keyReleaseEvent(QKeyEvent *e)
{
    if (e->modifiers() == (Qt::AltModifier | Qt::ShiftModifier))
    {
        qDebug() << "combine keyEvent!";
        m_isShiftPressed = false;
        m_isAltPressed = false;
        GlobalShortcut::instance()->setShiftScStatus(false);
        GlobalShortcut::instance()->setAltScStatus(false);
    }
    if (e->key() == Qt::Key_Alt)
    {
        m_isAltPressed = false;
        GlobalShortcut::instance()->setAltScStatus(false);
    }
    if (e->key() == Qt::Key_Shift)
    {
        m_isShiftPressed = false;
        GlobalShortcut::instance()->setShiftScStatus(false);
    }

//    QFrame::keyReleaseEvent(e);
}

void ShapesWidget::dragEnterEvent(QDragEnterEvent* e)
{
    e->accept();
}

void ShapesWidget::dropEvent(QDropEvent* e)
{
    QList<QUrl> urls = e->mimeData()->urls();
    if (urls.isEmpty())
        return;

    QStringList paths;
    for (QUrl url : urls)
    {
        const QString path = url.toLocalFile();
        if (QFileInfo(path).exists())
        {
            paths.append(path);
        }
    }

    loadImage(paths);
}

void ShapesWidget::deleteCurrentShape()
{
    qDebug() << "delete:" << m_selectedOrder << m_shapes.length();
    if (m_selectedOrder == -1 || m_imageCutting)
        return;

    if (m_selectedOrder < m_shapes.length())
    {
        bool compressImage = false;
        if (m_shapes[m_selectedOrder].type == "image" ||
                m_shapes[m_selectedOrder].type == "blur")
            compressImage = true;
        int deleteIndex = m_shapes[m_selectedOrder].index;
        m_shapes.removeAt(m_selectedOrder);
        if (m_selectedShape.type == "text" && m_editMap.contains(deleteIndex))
        {
            delete m_editMap.value(deleteIndex);
            m_editMap.remove(deleteIndex);
        }

        Q_UNUSED(compressImage);
        compressToImage();
        update();
    } else
    {
        qWarning() << "Invalid index";
    }

    clearSelected();
    m_selectedShape.type = "";
    m_currentShape.type = "";
    for(int i = 0; i < m_currentShape.mainPoints.length(); i++)
    {
        m_currentShape.mainPoints[i] = QPointF(0, 0);
    }

    m_selectedIndex = -1;
    m_selectedOrder = -1;
}

void ShapesWidget::undoDrawShapes()
{
    qDebug() << "undoDrawShapes m_selectedIndex:"
                     << m_selectedIndex << m_shapes.length();
    if (m_selectedOrder < m_shapes.length() && m_selectedIndex != -1)
    {
        deleteCurrentShape();
    } else if (m_shapes.length() > 0)
    {
        int tmpIndex = m_shapes[m_shapes.length() - 1].index;
        if (m_shapes[m_shapes.length() - 1].type == "text" && m_editMap.contains(tmpIndex))
        {
            m_editMap.value(tmpIndex)->clear();
            delete m_editMap.value(tmpIndex);
            m_editMap.remove(tmpIndex);
        }

        m_shapes.removeLast();
    }

    m_isSelected = false;
    update();
}

QString ShapesWidget::getCurrentType()
{
    return m_currentShape.type;
}

QString ShapesWidget::getLineStyle(int index)
{
    QString lineType;
    switch (index)
    {
    case 0: lineType = "straightLine"; break;
    case 1: lineType = "arbitraryCurve"; break;
    case 2: lineType = "arrow"; break;
    }

    return lineType;
}

void ShapesWidget::showCutImageTips(QPointF pos)
{
    QPoint tipPos = QPoint(pos.x(), pos.y());
    m_cutImageTips->showTips(QPoint(mapToGlobal(tipPos).x(),
                                    mapToGlobal(tipPos).y() + 10));

    connect(m_cutImageTips, &CutImageTips::canceled, this, [=]{
        m_imageCutting = false;
        qDebug() << "cutImageTips hide...";
        m_cutImageTips->hide();
        m_cutShape.points.clear();
        m_cutShape.type = "";
        if (m_shapes[m_shapes.length() - 1].type == "cutImage")
            m_shapes.removeAt(m_shapes.length() - 1);

        m_selectedOrder = m_cutImageOrder;
        setCurrentShape("selected");
        m_recordCutImage = false;

        qDebug() << "canceled m_selecedOrder:" << m_selectedOrder
                        << m_needCompress;
        emit cutImageFinished();
    });

    connect(m_cutImageTips, &CutImageTips::cutAction,
            this,  [=]{
        m_imageCutting = false;
        m_recordCutImage = true;
        cutImage();
    });

    connect(m_cutImageTips, &CutImageTips::cutRationChanged, this,
            &ShapesWidget::updateCutShape);
}

void ShapesWidget::loadImage(QStringList paths)
{
    qDebug() << "loadImage: " << paths.length();
    m_artBoardWindowWidth = this->width();
    m_artBoardWindowHeight = this->height();

    if (paths.length() > 1)
    {
        m_startPos = START_POINT;
    }

    for(int i = 0; i < paths.length(); i++)
    {
        if (QFileInfo(paths[i]).exists())
        {
            setCurrentShape("image");
            if (!m_needCompress)
                m_needCompress = true;

            Toolshape imageShape;
            imageShape.type = "image";
            imageShape.imagePath = paths[i];
            imageShape.imageSize = QPixmap(paths[i]).size();

            if (paths.length() == 1 &&( imageShape.imageSize.width()
              == m_artBoardWindowWidth && imageShape.imageSize.height()
              + IMG_ROTATEPOINT_SPACING == m_artBoardWindowHeight))
            {
                m_startPos = QPointF(0, IMG_ROTATEPOINT_SPACING);
            } else {
                if (imageShape.imageSize.width() > (m_artBoardWindowWidth - m_startPos.x()) ||
                    imageShape.imageSize.height() > (m_artBoardWindowHeight - m_startPos.y()))
                {
                    imageShape.imageSize = QPixmap(paths[i]).scaled(
                                int(std::abs(m_artBoardWindowWidth - 0)),
                                int(std::abs(m_artBoardWindowHeight - IMG_ROTATEPOINT_SPACING)),
                                Qt::KeepAspectRatio, Qt::SmoothTransformation).size();
                }

            }

            if (paths.length() == 1)
            {
                m_startPos = QPointF((this->width() - imageShape.imageSize.width())/2,
                (this->height() - imageShape.imageSize.height() - IMG_ROTATEPOINT_SPACING)/2
                                     + IMG_ROTATEPOINT_SPACING);
            }

            imageShape.mainPoints[0] =  QPointF(m_startPos.x(),
                                                m_startPos.y());
            imageShape.mainPoints[1] = QPointF(m_startPos.x(),
                m_startPos.y() + imageShape.imageSize.height());
            imageShape.mainPoints[2] = QPointF(m_startPos.x() +
                imageShape.imageSize.width(), m_startPos.y());
            imageShape.mainPoints[3] = QPointF(m_startPos.x() +
                imageShape.imageSize.width(), m_startPos.y() +
                                              imageShape.imageSize.height());
            appendShape(imageShape);
            m_startPos = QPointF(m_startPos.x() + PIC_SPACING,
                                                 m_startPos.y() + PIC_SPACING);
        }
    }

    m_selectedOrder = m_shapes.length() - 1;
    m_stickSelectedShape = m_shapes[m_selectedOrder];
    m_isSelected = true;
    setCurrentShape("selected");
    emit updateMiddleWidgets("image");

    qDebug() << "load image finished, compress image begins!";
     compressToImage();
}

void ShapesWidget::compressToImage()
{
    if (!m_needCompress)
        return;

    if (m_selectedOrder != -1 && m_selectedOrder < m_shapes.length())
        qDebug() << "CompressToImage" << m_shapes.length() << m_selectedOrder
                        << m_shapes[m_selectedOrder].imagePath
                        << m_selectedShape.imagePath;

    m_bottomPixmap = QPixmap(this->size());
    m_bottomPixmap.fill(Qt::transparent);

    for(int k = 0; k < m_shapes.length(); k++)
    {
        QPainter bottomPainter(&m_bottomPixmap);
        bottomPainter.setRenderHints(QPainter::Antialiasing |
                                     QPainter::SmoothPixmapTransform);
        if (m_shapes[k].type == "cutImage")
        {
            continue;
        } else
        {
            if (m_imageCutting && k != m_selectedOrder && m_shapes[k].type == "image")
            {
                bottomPainter.setOpacity(0.5);
            } else {
                bottomPainter.setOpacity(1);
            }

            paintShape(bottomPainter, m_shapes[k], false);
        }
    }

    QPainter bgPainter(&m_bottomPixmap);
    bgPainter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
//    if (!m_imageCutting && m_selectedOrder != -1 && m_selectedOrder < m_shapes.length())
//    {
//        bgPainter.setOpacity(1);
//        paintSelectedShape(bgPainter, m_shapes[m_selectedOrder]);
//    }

    if (m_beginGrabImage)
        m_BeforeCutBg = this->grab(this->rect());

    if (m_imageCutting) {
        bgPainter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        bgPainter.setOpacity(0.5);
        bgPainter.fillRect(this->rect(), QBrush(QColor("#000000")));
        bgPainter.setBrush(Qt::transparent);
        qDebug() << "m_bottomPixmap:" << m_bottomPixmap.rect() << this->rect();
    }

//    m_bottomPixmap.save("/home/ph/compressToImageResult.png");
    m_bgContainShapeNum = m_shapes.length();
}

QRect ShapesWidget::effectiveRect()
{
    const int MARGIN = 25;
    return QRect(x() - MARGIN, y() - MARGIN, width() - 2*MARGIN, height() - 2*MARGIN);
}

QRect ShapesWidget::rightBottomRect()
{
    QPoint rightBtnPos = this->rect().bottomRight();
    const int SPACING = 15;

    QPoint orginPos = QPoint(rightBtnPos.x() - SPACING, rightBtnPos.y() - SPACING);

    return QRect(
                orginPos.x() - SPACING, orginPos.y() - SPACING,
                SPACING*2 - 3, SPACING*2 - 3);
}

void ShapesWidget::updateCanvasSize()
{
    qDebug() << "updateCanvasSize !";
    if (m_resizeByAutoCrop)
    {
        updateSizeByAutoCrop();
        m_resizeByAutoCrop = false;
    } else {
        int newArtboardActualWidth = ConfigSettings::instance()->value(
                    "artboard", "width").toInt();
        int newArtboardActualHeight = ConfigSettings::instance()->value(
                    "artboard", "height").toInt();

        if (m_shapes.length() > 0)
        {
            qreal newScaledRation = qreal(newArtboardActualWidth)/qreal(this->width());

            if (m_lastRation > 1 || newScaledRation > 1)
            {
                for(int k = 0; k < m_shapes.length(); k++)
                {
                    m_shapes[k].scaledRation = m_ration/newScaledRation;
                    m_lastRation = newScaledRation;
                }
            }
        }

        m_artBoardActualWidth = newArtboardActualWidth;
        m_artBoardActualHeight = newArtboardActualHeight;
    }
    //TODO: delay to compressImage;
    if (m_shapes.length() > 0)
    {
        m_needCompress = true;
        compressToImage();
        update();
    }
}

void ShapesWidget::updateSizeByAutoCrop()
{
    m_getOriginRation = false;
    if (!m_getOriginRation)
    {
        int tmpWidth = ConfigSettings::instance()->value("artboard", "width").toInt();
        int tmpHeight = ConfigSettings::instance()->value("artboard", "height").toInt();
        m_originArtboardSize = QSize(tmpWidth, tmpHeight);
        m_originArtboardWindowSize = this->size();
        m_ration = qreal(tmpWidth)/qreal(this->width());
        qDebug() << "GetOrigin ration:" << m_ration << tmpWidth << this->width();
        m_getOriginRation = true;
        m_lastRation = m_ration;

        if (m_ration > 1)
        {
            for(int i = 0; i < m_shapes.length(); i++)
            {
                m_shapes[i].scaledRation = 1;
                if (m_shapes[i].type == "image")
                {
                    m_shapes[i].mainPoints[0] = m_shapes[i].mainPoints[0]/m_ration;
                    m_shapes[i].imageSize = m_shapes[i].imageSize/m_ration;
                    m_shapes[i].mainPoints[1] = QPointF(m_shapes[i].mainPoints[0].x(),
                            m_shapes[i].mainPoints[0].y() + m_shapes[i].imageSize.height());
                    m_shapes[i].mainPoints[2] = QPointF(m_shapes[i].mainPoints[0].x()
                            + m_shapes[i].imageSize.width(), m_shapes[i].mainPoints[0].y());
                    m_shapes[i].mainPoints[3] = QPointF(m_shapes[i].mainPoints[2].x(),
                            m_shapes[i].mainPoints[1].y());
                } else {
                    for(int k = 0; k < m_shapes[i].mainPoints.length(); k++)
                    {
                        m_shapes[i].mainPoints[k] = m_shapes[i].mainPoints[k]/m_ration;
                    }
                }
                for(int k = 0; k < m_shapes[i].points.length(); k++)
                {
                    m_shapes[i].points[k] = m_shapes[i].points[k]/m_ration;
                }
            }
        }
        m_getOriginRation = true;
    }
}

void ShapesWidget::updateShapeAttribute()
{
    qDebug() << "updateShapeAttribute currentType:" << m_currentType;
    if (m_currentType == "text")
    {
        m_textFontsize = ConfigSettings::instance()->value("text", "fontsize").toInt();
        bool textTrans = ConfigSettings::instance()->value("text", "fillColor_transparent").toBool();
        if (textTrans)
        {
            m_brushColor = QColor(Qt::transparent);
        } else {
            QColor color = QColor(ConfigSettings::instance()->value("text",
                                                             "fillColor").toString());
            int alpha = ConfigSettings::instance()->value("text",
                                                          "fillColor_alpha").toInt();

            m_brushColor = QColor(color.red(), color.green(), color.blue());
            m_brushColor.setAlphaF(qreal(alpha)/100);
        }
    }  else if (m_currentType == "blur")
    {
        m_linewidth = ConfigSettings::instance()->value("blur", "index").toInt();
    } else {
        bool needTrans = ConfigSettings::instance()->value("common",
                                                           "fillColor_transparent").toBool();
        QColor color;
        int alpha;
        if (needTrans)
            m_brushColor = QColor(Qt::transparent);
        else {
            color = QColor(ConfigSettings::instance()->value("common",
                                                             "fillColor").toString());
            alpha = ConfigSettings::instance()->value("common",
                                                          "fillColor_alpha").toInt();
            m_brushColor = QColor(color.red(), color.green(), color.blue());
            m_brushColor.setAlphaF(qreal(alpha)/100);
        }

        color = QColor(ConfigSettings::instance()->value("common",
                                                         "strokeColor").toString());
        alpha = ConfigSettings::instance()->value("common",
                                                  "strokeColor_alpha").toInt();
        m_penColor = QColor(color.red(), color.green(), color.blue());
        m_penColor.setAlphaF(qreal(alpha)/100);
        m_linewidth = ConfigSettings::instance()->value("common", "lineWidth").toInt();
    }
}

void ShapesWidget::updateToSelectedShapeAttribute(const QString &group, const QString &key,
                                                  QVariant var)
{
    ConfigSettings::instance()->setValue(group, key, var);
}

QList<QPixmap> ShapesWidget::saveCanvasImage()
{
    m_artBoardWindowWidth = width() - ARTBOARD_MARGIN*2;
    m_artBoardWindowHeight = height() - ARTBOARD_MARGIN*2;

    QPixmap transPixmap = QPixmap(QSize(m_artBoardActualWidth,
                                                                        m_artBoardActualHeight));
    QPixmap whitePixmap = QPixmap(QSize(m_artBoardActualWidth,
                                                                         m_artBoardActualHeight));
    transPixmap.fill(Qt::transparent);
    whitePixmap.fill(Qt::white);
    QPainter transPainter(&transPixmap);
    transPainter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QPainter whitePainter(&whitePixmap);
    whitePainter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    m_saveWithRation = true;

    for (int k = 0; k < m_shapes.length(); k++)
    {
        paintShape(transPainter, m_shapes[k], true);
        paintShape(whitePainter, m_shapes[k], true);
    }

    QList<QPixmap> pixmaps;
    pixmaps.append(transPixmap);
    pixmaps.append(whitePixmap);

    return pixmaps;
}

void ShapesWidget::saveImage()
{
    QList<QPixmap> saveImages = saveCanvasImage();
    m_saveWithRation = false;
    //TODO:添加异步处理
    TempFile::instance()->setCanvasShapes(m_shapes);
    TempFile::instance()->setImageFile(saveImages);
}

void ShapesWidget::printImage()
{
    QPixmap resultPixmap = saveCanvasImage()[0];
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPrinter::Custom);
    printer.setPaperSize(QPrinter::Custom);
    printer.setPaperSize(QSize(resultPixmap.width(), resultPixmap.height()),
                         QPrinter::DevicePixel);
    printer.setPageMargins(0.0f,0.0f,0.0f,0.0f, QPrinter::DevicePixel);
    printer.setFullPage(true);

    QString desktopDir = QStandardPaths::writableLocation(
                                        QStandardPaths::DesktopLocation);
    QString filePath = QString("%1/%2.pdf").arg(desktopDir).arg(tr("Unnamed"));

    if (QFileInfo(filePath).exists())
    {
        int num = 0;
        while(QFileInfo(filePath).exists())
        {
            num++;
            filePath = QString("%1/%2_%3.pdf").arg(desktopDir).arg(tr("Unnamed")).arg(num);
        }
    }
    printer.setOutputFileName(filePath);

     m_saveWithRation = false;
    QPrintDialog* printDialog = new QPrintDialog(&printer, this);
    printDialog->resize(400, 300);
    printer.setOutputFormat(QPrinter::PdfFormat);

    if (printDialog->exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        QRectF drawRectF; QRect wRect;
        if (resultPixmap.isNull()) {
            qDebug() << "resultPixmap is null, exit !";
        }
        wRect = printer.pageRect();
        if (resultPixmap.width() > wRect.width() || resultPixmap.height() > wRect.height()) {
            resultPixmap = resultPixmap.scaled(wRect.size(), Qt::KeepAspectRatio,
                                                                          Qt::SmoothTransformation);
        }
        drawRectF = QRectF(qreal(wRect.width() - resultPixmap.width())/2,
                                            qreal(wRect.height() - resultPixmap.height())/2,
                           resultPixmap.width(), resultPixmap.height());
        painter.drawPixmap(drawRectF.x(), drawRectF.y(), resultPixmap.width(),
                           resultPixmap.height(), resultPixmap);
        painter.end();

        qDebug() << "print succeed!";
        return;
    }

    QObject::connect(printDialog, &QPrintDialog::finished,
                     printDialog, &QPrintDialog::deleteLater);
}

void ShapesWidget::microAdjust(QString direction)
{
    if (m_selectedIndex != -1 && m_selectedOrder < m_shapes.length())
    {
        if (m_shapes[m_selectedOrder].type  == "text")
        {
            return;
        }

        if (direction == "Left" || direction == "Right" || direction == "Up"
                || direction == "Down")
        {
            m_shapes[m_selectedOrder].mainPoints = pointMoveMicro(
                        m_shapes[m_selectedOrder].mainPoints, direction);
        } else if (direction == "Ctrl+Shift+Left" || direction == "Ctrl+Shift+Right"
                   || direction == "Ctrl+Shift+Up"|| direction == "Ctrl+Shift+Down")
        {
            m_shapes[m_selectedOrder].mainPoints = pointResizeMicro(
                        m_shapes[m_selectedOrder].mainPoints, direction, false);
        } else
        {
            m_shapes[m_selectedOrder].mainPoints = pointResizeMicro(
                        m_shapes[m_selectedOrder].mainPoints, direction, true);
        }

        if (m_shapes[m_selectedOrder].type == "arbitraryCurve" ||
                m_shapes[m_selectedOrder].type == "arrow")
        {
            if (m_shapes[m_selectedOrder].portion.length() == 0) {
                for(int k = 0; k < m_shapes[m_selectedOrder].points.length(); k++)
                {
                    m_shapes[m_selectedOrder].portion.append(relativePosition(
                        m_shapes[m_selectedOrder].mainPoints,
                        m_shapes[m_selectedOrder].points[k]));
                }
            }
            for(int j = 0; j < m_shapes[m_selectedOrder].points.length(); j++)
            {
                m_shapes[m_selectedOrder].points[j] = getNewPosition(
                    m_shapes[m_selectedOrder].mainPoints,
                    m_shapes[m_selectedOrder].portion[j]);
            }
        }

        m_selectedShape.mainPoints = m_shapes[m_selectedOrder].mainPoints;
        m_selectedShape.points = m_shapes[m_selectedOrder].points;
        m_hoveredShape.type = "";
        update();
    }
}

void ShapesWidget::showRotateDegreeLabel(qreal angle)
{
    m_degreeLabel->show();
    m_degreeLabel->move((this->width() - m_degreeLabel->width())/2,
                        (this->height() - m_degreeLabel->height())/2);
    qreal degree = angle*180/M_PI - qreal(int(angle*180/M_PI)/360*360);
    while (degree < 0)
    {
        degree += 360;
    }
    QString degreeStr = QString::number(degree, 'f', 1);

    m_degreeLabel->setText(QString("%1°").arg(degreeStr));
}

void ShapesWidget::updateCursorDirection(ResizeDirection direction)
{
    if (direction == Left) {
        if (m_isSelected || m_isRotated || m_imageCutting)
        {
           qApp->setOverrideCursor(Qt::SizeHorCursor);
        } else {
           qApp->setOverrideCursor(Qt::ClosedHandCursor);
        }
    } else if (direction == Top) {
        if (m_isSelected || m_isRotated || m_imageCutting)
        {
           qApp->setOverrideCursor(Qt::SizeVerCursor);
        } else {
           qApp->setOverrideCursor(Qt::ClosedHandCursor);
        }
    } else if (direction == Right) {
        if (m_isSelected || m_isRotated || m_imageCutting)
        {
           qApp->setOverrideCursor(Qt::SizeHorCursor);
        } else {
           qApp->setOverrideCursor(Qt::ClosedHandCursor);
        }
    } else if (direction == Bottom) {
        if (m_isSelected || m_isRotated || m_imageCutting)
        {
           qApp->setOverrideCursor(Qt::SizeVerCursor);
        } else {
           qApp->setOverrideCursor(Qt::ClosedHandCursor);
        }
    }  else if (direction == TopLeft) {
        if (m_isSelected || m_isRotated || m_imageCutting)
        {
           qApp->setOverrideCursor(setCursorShape("topleft"));
        } else {
           qApp->setOverrideCursor(Qt::ClosedHandCursor);
        }
    } else if (direction == BottomLeft) {
        if (m_isSelected || m_isRotated || m_imageCutting)
        {
           qApp->setOverrideCursor(setCursorShape("bottomleft"));
        } else {
           qApp->setOverrideCursor(Qt::ClosedHandCursor);
        }
    } else if (direction == TopRight) {
        if (m_isSelected || m_isRotated || m_imageCutting)
        {
           qApp->setOverrideCursor(setCursorShape("topright"));
        } else {
           qApp->setOverrideCursor(Qt::ClosedHandCursor);
        }
    } else if (direction == BottomRight) {
        if (m_isSelected || m_isRotated || m_imageCutting)
        {
           qApp->setOverrideCursor(setCursorShape("bottomright"));
        } else {
           qApp->setOverrideCursor(Qt::ClosedHandCursor);
        }
    } else if (direction == Rotate) {
        qApp->setOverrideCursor(setCursorShape("rotate"));
    } else if (direction == Moving){
        qApp->setOverrideCursor(Qt::ClosedHandCursor);
    } else {
        qApp->setOverrideCursor(Qt::ArrowCursor);
    }
}

void ShapesWidget::updateCursorShape()
{
    if (m_stickCurosr)
        return;

    if (m_currentType == "blur")
    {
        qApp->setOverrideCursor(blurToolCursor(m_blurLinewidth));
    } else {
        qApp->setOverrideCursor(setCursorShape(m_currentType));
    }
}

void ShapesWidget::setImageCutting(bool cutting)
{
    qDebug() << "setImageCutting " << cutting << m_selectedOrder;
    if (cutting && m_selectedOrder != -1 && m_selectedOrder < m_shapes.length())
    {
        if (m_recordCutImage && m_selectedOrder != -1 && m_cutImageShape.type != ""
                && m_cutImageShape.mainPoints.length() == 4 && m_cutFPoints.length() == 4)
        {
            if (m_shapes[m_shapes.length() - 1].type == "cutImage")
            {
                m_shapes.removeLast();
            }
            m_shapes[m_selectedOrder].imagePath = m_cutImageShape.imagePath;
            m_shapes[m_selectedOrder].mainPoints = m_cutImageShape.mainPoints;
            m_shapes[m_selectedOrder].imageSize = m_cutImageShape.imageSize;
            m_shapes[m_selectedOrder].rotate = m_cutImageShape.rotate;
            m_cutShape.mainPoints.clear();
            update();

            m_cutImageOrder = m_selectedOrder;
            m_cutShape.type = "cutImage";

            m_needCompress = true;
            m_imageCutting = cutting;
            qDebug() << "after select cutting images!";
            compressToImage();

            if (m_cutShape.mainPoints.length() != 4)
            {
                m_cutShape.mainPoints.clear();
                for(int k = 0; k < 4; k++)
                    m_cutShape.mainPoints.append(QPointF(0, 0));
            }

            m_cutShape.type = "cutImage";
            m_cutShape.mainPoints = m_cutFPoints;
            appendShape(m_cutShape);
            showCutImageTips(m_cutShape.mainPoints[3]);
            return;
        }

        QString defaultRation = ConfigSettings::instance()->value("cut", "ration").toString();

        m_cutImageOrder = m_selectedOrder;
        m_cutShape.type = "cutImage";

        m_needCompress = true;
        m_imageCutting = cutting;
        m_beginGrabImage = true;
        compressToImage();
        m_beginGrabImage = false;

        if (m_cutShape.mainPoints.length() != 4)
        {
            m_cutShape.mainPoints.clear();
            for(int k = 0; k < 4; k++)
                m_cutShape.mainPoints.append(QPointF(0, 0));
        }

        FourPoints cutMainPoints =  mainPointsOrder(getInitFourPointByCut(m_shapes[
            m_selectedOrder].mainPoints, defaultRation));

        for(int i = 0; i < cutMainPoints.length(); i++)
        {
            m_cutShape.mainPoints[i] = cutMainPoints[i];
        }
        appendShape(m_cutShape);
        showCutImageTips(m_cutShape.mainPoints[3]);
    }

    if (!cutting)
    {
        m_cutShape.type = "";
        for(int k = 0; k < m_cutShape.mainPoints.length(); k++)
        {
            m_cutShape.mainPoints[k] = QPointF(0, 0);
        }
        m_imageCutting = cutting;
        m_needCompress = true;
        compressToImage();
    }
    m_imageCutting = cutting;

    update();
}

void ShapesWidget::cutImage()
{
    m_cutImageTips->hide();
    if (m_shapes.length() >= 1 && m_shapes[m_shapes.length() - 1].type == "cutImage")
    {
        FourPoints rectFPoints = m_shapes[m_shapes.length() - 1].mainPoints;
        m_cutFPoints = rectFPoints;

        m_cutShape.mainPoints.clear();
        m_currentShape.mainPoints.clear();
        m_cutShape.type = "";
        m_currentShape.type = "";
        if (m_cutImageOrder == -1 || m_cutImageOrder > m_shapes.length() - 1)
            return;
        FourPoints imgFourPoints = m_shapes[m_cutImageOrder].mainPoints;
        m_shapes.removeAt(m_shapes.length() - 1);
        m_cutImageShape = m_shapes[m_cutImageOrder];

        update();

        QPixmap cutImage(rect().size());
        cutImage.fill(Qt::transparent);

        QPainter cutPainter(&cutImage);
        paintShape(cutPainter, m_shapes[m_cutImageOrder], false);

        QPolygon imgPolygon;
        imgPolygon << QPoint(imgFourPoints[0].x(), imgFourPoints[0].y())
                              << QPoint(imgFourPoints[1].x(), imgFourPoints[1].y())
                              << QPoint(imgFourPoints[3].x(), imgFourPoints[3].y())
                              << QPoint(imgFourPoints[2].x(), imgFourPoints[2].y())
                              << QPoint(imgFourPoints[0].x(), imgFourPoints[0].y());
        QPolygon cutPolygon;
        cutPolygon << QPoint(rectFPoints[0].x(), rectFPoints[0].y())
                            << QPoint(rectFPoints[1].x(), rectFPoints[1].y())
                            << QPoint(rectFPoints[3].x(), rectFPoints[3].y())
                            << QPoint(rectFPoints[2].x(), rectFPoints[2].y())
                            << QPoint(rectFPoints[0].x(), rectFPoints[0].y());

        imgPolygon = imgPolygon.intersected(cutPolygon);

       QPixmap resultImage = cutImage.copy(
                   rectFPoints[0].x(), rectFPoints[0].y(),
                   std::abs(rectFPoints[3].x() - rectFPoints[0].x()),
                   std::abs(rectFPoints[3].y() - rectFPoints[0].y()));

       for(int i = 0; i < imgPolygon.count(); i++)
       {
           imgPolygon[i] = QPoint(imgPolygon.at(i).x() - rectFPoints[0].x(),
                                                     imgPolygon.at(i).y() - rectFPoints[0].y());

       }

       QRect imgPolygonRect = imgPolygon.boundingRect();
       if (imgPolygonRect.width() == 0 || imgPolygonRect.height() == 0)
       {
           emit cutImageFinished();
           return;
       }

       QPainter painter(&resultImage);
       painter.setClipping(true);
       painter.setClipRegion(imgPolygon);
       painter.drawPixmap(0, 0, resultImage);
       painter.setClipping(false);

        if (!resultImage.isNull())
        {
            QString tmpFilename = TempFile::instance()->getRandomFile("cutImage");
            resultImage.save(tmpFilename, "png");
            int imageIndex = m_cutImageOrder;
            m_shapes[imageIndex].imagePath = tmpFilename;
            m_shapes[imageIndex].imageSize = QPixmap(tmpFilename).size();

            if (m_shapes[imageIndex].imageSize.width() > (m_artBoardWindowWidth - m_startPos.x()) ||
                    m_shapes[imageIndex].imageSize.height() > (m_artBoardWindowHeight - m_startPos.y())) {
                m_shapes[imageIndex].imageSize = QPixmap(tmpFilename).scaled(
                int(std::abs(m_artBoardWindowWidth - m_startPos.x())),
                int(std::abs(m_artBoardWindowHeight - m_startPos.y())),
                Qt::KeepAspectRatio, Qt::SmoothTransformation).size();
            }

            if (m_shapes.length() == 1)
            {
                m_startPos = QPointF(
                (this->width() - m_shapes[imageIndex].imageSize.width())/2,
                (this->height() - m_shapes[imageIndex].imageSize.height())/2);
            }
            m_shapes[imageIndex].rotate = 0;
            m_shapes[imageIndex].mainPoints[0] = QPointF(m_startPos.x(), m_startPos.y());
            m_shapes[imageIndex].mainPoints[1] = QPointF(m_startPos.x(),
                                              m_startPos.y() + m_shapes[imageIndex].imageSize.height());
            m_shapes[imageIndex].mainPoints[2] = QPointF(m_startPos.x() +
                                              m_shapes[imageIndex].imageSize.width(), m_startPos.y());
            m_shapes[imageIndex].mainPoints[3] = QPointF(m_startPos.x() +
                                              m_shapes[imageIndex].imageSize.width(), m_startPos.y() +
                                              m_shapes[imageIndex].imageSize.height());
            m_needCompress = true;
            m_selectedOrder = imageIndex;
            compressToImage();
            setCurrentShape("selected");

            emit cutImageFinished();
        } else {
            qWarning() << "create cut image failed!";
        }
    }
}

void ShapesWidget::updateCutShape(CutRation ration)
{
    Q_UNUSED(ration);
    if (m_shapes.length() >= 1 && m_shapes[
            m_shapes.length() - 1].type == "cutImage")
    {
        QString defaultRation = ConfigSettings::instance()->value("cut",
                                                                  "ration").toString();
        FourPoints cutFPoints =  getInitFourPointByCut(m_shapes[
                                                       m_selectedOrder].mainPoints, defaultRation);
        if (m_cutShape.type == "cutImage")
        {
            m_cutShape.mainPoints = cutFPoints;
        }
        if (m_currentShape.type == "cutImage")
        {
            m_currentShape.mainPoints = cutFPoints;
        }

        if (m_shapes[m_shapes.length() - 1].type == "cutImage")
            m_shapes[m_shapes.length() - 1].mainPoints = cutFPoints;
        m_cutImageTips->showTips(mapToGlobal(QPoint(int(cutFPoints[3].x()),
                                                                                                 int(cutFPoints[3].y()))));
    }
}

void ShapesWidget::layerSwitch(LayerDirection direction)
{
    switch (direction) {
    case LayerDirection::TopLayer:
        m_shapes.move(m_selectedOrder, m_shapes.length() - 1);
        m_selectedOrder = m_shapes.length() - 1;
        break;
    case LayerDirection::BottomLayer:
        m_shapes.move(m_selectedOrder, 0);
        m_selectedOrder = 0;
        break;
    case LayerDirection::UpLayer:
        if (m_selectedOrder+1 < m_shapes.length())
        {
            m_shapes.move(m_selectedOrder, m_selectedOrder+1);
            m_selectedOrder +=1;
        }
        break;
    case LayerDirection::DownLayer:
        if (m_selectedOrder >= 1)
        {
            m_shapes.move(m_selectedOrder, m_selectedOrder-1);
            m_selectedOrder -=1;
        }
        break;
    default:
        break;
    }

    m_needCompress = true;
    compressToImage();
}

void ShapesWidget::copyShape()
{
    if (m_selectedOrder != -1 && m_selectedOrder < m_shapes.length())
    {
        m_hangingShape = m_shapes[m_selectedOrder];

        QString copyInfo = getStringFromShape(m_shapes[m_selectedOrder]);
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(copyInfo, QClipboard::Clipboard);
    }
}

void ShapesWidget::cutShape()
{
    if (m_selectedOrder != -1 && m_selectedOrder < m_shapes.length())
    {
        m_hangingShape = m_shapes[m_selectedOrder];

        QString cutInfo = getStringFromShape(m_shapes[m_selectedOrder]);
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(cutInfo, QClipboard::Clipboard);
        qDebug() << "Before:" << m_shapes.length();
        m_shapes.removeAt(m_selectedOrder);
        m_selectedOrder = -1;
        qDebug() << "After:" << m_shapes.length();

        compressToImage();
        update();
    }
}

void ShapesWidget::pasteShape(QPoint pos)
{
    QClipboard *clipboard = QApplication::clipboard();
    QString clipStr = clipboard->text();
    m_hangingShape = getShapeInfoFromJsonStr(clipStr);

    if (m_hangingShape.type.isEmpty() || m_hangingShape.mainPoints.length() < 4)
        return;

    m_shapesIndex += 1;
    m_hangingShape.index = m_shapesIndex;

    QPointF movePos;
    if (pos == QPoint(0, 0)) {
        if (m_hangingShape.mainPoints[0].x() > this->width()*5/6 ||
                m_hangingShape.mainPoints[0].y() > this->height()*5/6) {
            movePos = QPointF(-m_hangingShape.mainPoints[0].x() + 5,
                    -m_hangingShape.mainPoints[0].y() + 5);
        } else {
            movePos = QPointF(30, 20);
        }
    } else  {
        qDebug() << "m_hangingShape pos:" << pos;
        movePos = QPointF(-m_hangingShape.mainPoints[0].x() + pos.x(),
                -m_hangingShape.mainPoints[0].y() + pos.y());
        qDebug() << "m_movePos:" << movePos;
    }

    for(int i = 0; i < m_hangingShape.mainPoints.length(); i++)
    {
        m_hangingShape.mainPoints[i] = QPointF(
                    m_hangingShape.mainPoints[i].x() + movePos.x(),
                    m_hangingShape.mainPoints[i].y() + movePos.y());
        if (i == 0)
            qDebug() << "hanging:" << m_hangingShape.mainPoints[0];
    }

    for(int j = 0; j < m_hangingShape.points.length(); j++)
    {
        m_hangingShape.points[j] = QPointF(
                    m_hangingShape.points[j].x() + movePos.x(),
                    m_hangingShape.points[j].y() + movePos.y());
    }

    if (m_hangingShape.type == "text")
    {
        TextEdit* edit = new TextEdit(m_shapesIndex, this);
        edit->setFontSize(m_hangingShape.fontSize);
        qDebug() << "hanging shape..." << m_hangingShape.fillColor.alphaF();
        edit->setColor(m_hangingShape.fillColor);
        edit->insertPlainText(m_hangingShape.text);
        edit->move(QPoint(m_hangingShape.mainPoints[0].x(),
                   m_hangingShape.mainPoints[0].y()));
        edit->resize(QSize(m_hangingShape.mainPoints[3].x() -
            m_hangingShape.mainPoints[0].x(), m_hangingShape.mainPoints[3].y() -
            m_hangingShape.mainPoints[0].y()));
        edit->show();
        m_editMap.insert(m_shapesIndex, edit);
        connect(edit, &TextEdit::repaintTextRect, this, &ShapesWidget::updateTextRect);
        connect(edit, &TextEdit::backToEditing, this, [=]{
            m_editing = true;
        });
        connect(edit, &TextEdit::textEditSelected, this, [=](int index){
            for (int k = 0; k < m_shapes.length(); k++) {
                 if (m_shapes[k].type == "text" && m_shapes[k].index == index) {
                     m_selectedOrder = index;
                     m_selectedShape = m_shapes[k];
                     break;
                 }
             }
             setAllTextEditReadOnly();
        });
        connect(edit, &TextEdit::showMenuInTextEdit, this, [=]{
            m_menu->popup(this->cursor().pos());
        });
    }

    appendShape(m_hangingShape);

    m_selectedOrder = m_shapes.length() - 1;
    m_stickSelectedShape = m_shapes[m_selectedOrder];
    qDebug() << "pasteShape:" << m_selectedOrder;

    setAllTextEditReadOnly();
    compressToImage();
    update();
}

void ShapesWidget::resizeArtboardByDrag(QPointF pos)
{
    QPointF endPos =  pos;//mapToGlobal(e->pos());
    m_artboardMainPoints[0] = QPointF(0, 0);
    m_artboardMainPoints[1] = QPointF(0, endPos.y());
    m_artboardMainPoints[2] = QPointF(endPos.x(), 0);
    m_artboardMainPoints[3] = QPointF(endPos.x(), endPos.y());
    QSize newSize = QSize(int(m_artboardMainPoints[3].x() -
                          m_artboardMainPoints[0].x()),
          int(m_artboardMainPoints[3].y() - m_artboardMainPoints[0].y()));

    QSize addSize = QSize(int(qreal(newSize.width())),
                                           int(qreal(newSize.height())));
    qDebug() << "addSize" << addSize;
    m_resizeByAutoCrop = true;
    emit drawArtboard(m_drawArtboardSize, m_artboardMainPoints, addSize);
}

void ShapesWidget::appendShape(Toolshape shape)
{
    m_shapes.append(shape);
}

void ShapesWidget::autoCrop()
{
    qreal x1=width(), y1=height(), x2=0, y2=0;

    for(int i = 0; i < m_shapes.length(); i++)
    {
        //TODO: image's mainPoints
        QPointF shapeStartPointF = m_shapes[i].topLeftPointF();
        QPointF shapeEndPointF = m_shapes[i].bottomRightPointF();
        qDebug() << "cropping:" << shapeStartPointF << shapeEndPointF;
        x1 = std::min(x1, shapeStartPointF.x());
        y1 = std::min(y1, shapeStartPointF.y());
        x2 = std::max(x2, shapeEndPointF.x());
        y2 = std::max(y2, shapeEndPointF.y());
    }

    for(int i = 0; i < m_shapes.length(); i++)
    {
        //TODO: image's mainPoints
        if (m_shapes[i].type == "rectangle" || m_shapes[i].type == "oval" ||
                m_shapes[i].type == "image" || m_shapes[i].type == "arbitraryCurve" ||
                m_shapes[i].type == "blur" || m_shapes[i].type == "text")
        {
            for(int j = 0; j < m_shapes[i].mainPoints.length(); j++)
            {
                m_shapes[i].mainPoints[j] = QPointF(
                            m_shapes[i].mainPoints[j].x() - x1,
                            m_shapes[i].mainPoints[j].y() - y1);
            }
        } else {
            for(int j = 0; j < m_shapes[i].points.length(); j++)
            {
                m_shapes[i].points[j] = QPointF(
                            m_shapes[i].points[j].x() - x1,
                            m_shapes[i].points[j].y() - y1);
            }
        }
    }

    QSize newSize = QSize(int(x2 - x1), int(y2 - y1));

    m_resizeByAutoCrop = true;
    qDebug() << "autoCrop...:" << newSize << m_resizeByAutoCrop;
    emit drawArtboard(m_drawArtboardSize, m_artboardMainPoints, newSize);
}
