#include "cgraphicslayer.h"
#include "cdrawscene.h"
#include "cundoredocommand.h"
#include "cgraphicsitemevent.h"
#include "global.h"
#include "cgraphicsview.h"
REGISTITEMCLASS(JDynamicLayer, int(DyLayer))
CGraphicsLayer::CGraphicsLayer(): CGraphicsItem(nullptr)
{
    setAutoCache(false);
}

QRectF CGraphicsLayer::rect() const
{
    return m_boundingRect;
}

void CGraphicsLayer::setRect(const QRectF rct)
{
    if (!rct.isValid())
        return;

    QRectF oldRect = m_boundingRect;

    prepareGeometryChange();

    m_boundingRect     = rct;
    m_boundingRectTrue = rct;

    if (m_layerImage.isNull()) {
        m_layerImage = QImage(boundingRect().size().toSize(), QImage::Format_ARGB32);
        m_layerImage.fill(Qt::transparent);
    } else {
        auto inscet = oldRect.intersected(rct);
        if (inscet.isValid()) {
            auto oldInNew = oldRect.translated(-rct.topLeft().x(), -rct.topLeft().y());
            auto insectInNew = inscet.translated(-rct.topLeft().x(), -rct.topLeft().y());
            QImage image = QImage(boundingRect().size().toSize(), QImage::Format_ARGB32);
            image.fill(Qt::transparent);
            QPainter painter(&image);
            painter.setClipRect(insectInNew);
            painter.drawImage(oldInNew, m_layerImage);

            m_layerImage = image;
        } else {
            m_layerImage = QImage(boundingRect().size().toSize(), QImage::Format_ARGB32);
            m_layerImage.fill(Qt::transparent);
        }
    }
}

void CGraphicsLayer::addCItem(CGraphicsItem *pItem, bool calZ)
{
    if (pItem == nullptr)
        return;
    if (pItem == this)
        return;

    if (!m_items.contains(pItem)) {
        m_items.append(pItem);
        //pItem->setParentItem(this);
        pItem->setLayer(this);
    }
}

void CGraphicsLayer::removeCItem(CGraphicsItem *pItem)
{
    if (m_items.contains(pItem)) {
        //pItem->setParentItem(nullptr);
        m_items.removeOne(pItem);
        pItem->setLayer(nullptr);
    }
}

QImage &CGraphicsLayer::layerImage()
{
    return m_layerImage;
}

void CGraphicsLayer::paintSelf(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->drawImage(boundingRect(), m_layerImage);
}

void CGraphicsLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    CGraphicsItem::paint(painter, option, widget);
}

void CGraphicsLayer::updateShape()
{
    QRectF rct;
    if (scene() != nullptr) {
        rct = scene()->sceneRect();
    }
    setRect(rct);
}


JDyLayerCmdBase *JDyLayerCmdBase::creatCmd(int tp)
{
    JDyLayerCmdBase *result = nullptr;
    switch (tp) {
    case 1: {
        result = new JDynamicLayer::JGeomeCommand();
        break;
    }
    case 2: {
        result = new JDynamicLayer::JPathCommand();
        break;
    }
    case 3: {
        result = new JDynamicLayer::JPaintCommand();
        break;
    }
    case 4: {
        result = new JDynamicLayer::JBlurCommand();
        break;
    }
    }

    return result;
}
JActivedPaintInfo::JActivedPaintInfo()
{
    for (int i = 0; i < ECount; ++i) {
        if (_pictures[i].data() == nullptr) {
            _pictures[i] = QSharedPointer<QPicture>(new QPicture);
        }
    }
}

JActivedPaintInfo::~JActivedPaintInfo()
{
}

void JActivedPaintInfo::beginSubPicture()
{
    painter()->begin(&picture(ESubPictures));
}

void JActivedPaintInfo::addSubPicture(const QPicture &subPicture)
{
    if (painter()->device() != nullptr && !subPicture.isNull()) {
        painter()->drawPicture(QPointF(0, 0), subPicture);
    }
}

QPicture JActivedPaintInfo::getPathPicture(const QPainterPath &path, const QPen &pen, const QBrush &brush)
{
    QPicture picture;
    QPainter painter(&picture);
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawPath(path);
    painter.end();
    return picture;
}

void JActivedPaintInfo::endSubPicture()
{
    painter()->end();
}

void JActivedPaintInfo::addPoint(const QPointF &pos)
{
    if (_path.elementCount() == 0) {
        _path.moveTo(pos);
    } else {
        _path.lineTo(pos);
    }
    _pathPictureIsDirty = 1;
}

void JActivedPaintInfo::setPenForPath(const QPen &pen)
{
    _pen = pen;
    _pathPictureIsDirty = 1;
}

void JActivedPaintInfo::setBrushForPath(const QBrush &brush)
{
    _brush = brush;
    _pathPictureIsDirty = 1;
}

QPicture &JActivedPaintInfo::picture(EComposBy tp)
{
    if (tp == EPainterPath) {
        ensurePainterPathPicture();
    }
    return *(_pictures[tp].data());
}

void JActivedPaintInfo::ensurePainterPathPicture()
{
    if (_pathPictureIsDirty) {
        *(_pictures[EPainterPath]) = getPathPicture(_path, _pen, _brush);
        _pathPictureIsDirty = 0;
    }
}

QPainter *JActivedPaintInfo::painter()
{
    if (_pPainter.data() == nullptr) {
        _pPainter = QSharedPointer<QPainter>(new QPainter);
    }
    return _pPainter.data();
}
JDynamicLayer::JDynamicLayer(const QImage &image, QGraphicsItem *parent): CGraphicsItem(parent)
{
    auto img = image.convertToFormat(QImage::Format_ARGB32);
    _img     = img;
    _baseImg = img;
    _rect  = QRectF(0, 0, _img.width(), _img.height());
}

JDynamicLayer::~JDynamicLayer()
{
//    foreach (auto c, _commands) {
//        delete c;
//    }
    clear();
}

int JDynamicLayer::type() const
{
    return DyLayer;
}

void JDynamicLayer::setBlocked(bool b)
{
    _isBlocked = b;
}

bool JDynamicLayer::isBlocked() const
{
    return _isBlocked;
}

//SAttrisList JDynamicLayer::attributions()
//{
//    DrawAttribution::SAttrisList result;
//    result << DrawAttribution::SAttri(DrawAttribution::EPenColor, pen().color())
//           << DrawAttribution::SAttri(DrawAttribution::EPenWidth,  pen().width());
//    return result;
//}
DrawAttribution::SAttrisList JDynamicLayer::attributions()
{
    DrawAttribution::SAttrisList result;
    if (isBlocked()) {
        bool enable = (drawScene()->selectGroup()->getBzItems(true).count() == 1);
        result << DrawAttribution::SAttri(DrawAttribution::EImageLeftRot, enable)
               << DrawAttribution::SAttri(DrawAttribution::EImageRightRot, enable)
               << DrawAttribution::SAttri(DrawAttribution::EImageHorFilp, enable)
               << DrawAttribution::SAttri(DrawAttribution::EImageVerFilp, enable)
               << DrawAttribution::SAttri(DrawAttribution::EImageAdaptScene,
                                          drawScene()->selectGroup()->sceneBoundingRect() != drawScene()->sceneRect());
    }
    return result;
}

void JDynamicLayer::setAttributionVar(int attri, const QVariant &var, int phase)
{
    Q_UNUSED(var)
    switch (attri) {
    case EImageLeftRot: {
        setRotation90(true);
        auto p = new JGeomeCommand(this);
        appendComand(p, false, false);
        break;
    }
    case EImageRightRot: {
        setRotation90(false);
        auto p = new JGeomeCommand(this);
        appendComand(p, false, false);
        break;
    }
    case EImageHorFilp: {
        doFilp(EFilpHor);
        setTransform(getFilpTransform());
        auto p = new JGeomeCommand(this);
        appendComand(p, false, false);
        break;
    }
    case EImageVerFilp: {
        doFilp(EFilpVer);
        setTransform(getFilpTransform());
        auto p = new JGeomeCommand(this);
        appendComand(p, false, false);
        break;
    }
    case EImageAdaptScene: {
        break;
    }
    default:
        break;
    }
}

void JDynamicLayer::clear()
{
    _commands.clear();
    prepareGeometryChange();
    setPos(QPointF(0, 0));
    setDrawRotatin(0);
    resetTransform();

    _img = _baseImg;

    _rect = QRectF(0, 0, _img.width(), _img.height());
}


void JDynamicLayer::addPenPath(const QPainterPath &path, const QPen &pen, int type, bool creatCmd)
{
    if (_isBlocked)
        return;

    QImage oldImg = _img;

    QPainterPathStroker ps(pen);

    QPainterPath pathStroke;

    pathStroke = ps.createStroke(path);

    pathStroke = pathStroke.simplified();

    auto rect = _img.isNull() ? pathStroke.boundingRect() : boundingRect() | pathStroke.boundingRect();

    if (type == 1) {
        rect = boundingRect();
    }

    if (oldImg.size() != rect.size().toSize()) {
        _img = QImage(rect.size().toSize(), QImage::Format_ARGB32);
        _img.fill(Qt::transparent);
    }

    QPainter painter(&_img);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(-rect.topLeft());

    if (!oldImg.isNull() && oldImg.size() != rect.size().toSize()) {
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.drawImage(boundingRect(), oldImg, QRectF(0, 0, oldImg.width(), oldImg.height()));
    }

    QPen pen1 = pen;
    if (type == 1)
        painter.setCompositionMode(QPainter::CompositionMode_Source);

    pen1.setJoinStyle(Qt::RoundJoin);
    pen1.setCapStyle(Qt::RoundCap);
    painter.setPen(pen1);
    painter.drawPath(path);

    setRect(rect);

    if (creatCmd) {
        auto p = new JPathCommand(path, pen, this);
        appendComand(p);
    }
}

void JDynamicLayer::addPicture(const QPicture &picture, bool creatCmd, bool dyImag)
{
    if (_isBlocked)
        return;

    QImage oldImg = _img;
    QRect pictureRectInlayer = picture.boundingRect();
    auto rect = (_img.isNull() ?  pictureRectInlayer : boundingRect()).toRect();
    if (dyImag) {
        rect = (boundingRect() | pictureRectInlayer).toRect();
    }
    if (oldImg.size() != rect.size()) {
        _img = QImage(rect.size(), QImage::Format_ARGB32);
        _img.fill(Qt::transparent);
        QPainter painter(&_img);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.translate(-rect.topLeft());
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.drawImage(boundingRect(), oldImg, QRectF(0, 0, oldImg.width(), oldImg.height()));
    }
    QPainter painter(&_img);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(-rect.topLeft());
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawPicture(QPoint(0, 0), picture);

    if (creatCmd) {
        auto p = new JPaintCommand(picture, dyImag, this);
        appendComand(p);
    }
    setRect(rect);
}

void JDynamicLayer::appendComand(JCommand *cmd, bool doCmd, bool addToStack)
{
    if (_isBlocked)
        return;

    CCmdBlock blocker(addToStack ? this : nullptr);
    _commands.append(cmd);
    if (doCmd) {
        cmd->doCommand();
    }
}

QRectF JDynamicLayer::boundingRect() const
{
    return _rect;
}

QPainterPath JDynamicLayer::shape() const
{
    QPainterPath path;
    path.addRect(_rect);
    return path;
}

bool JDynamicLayer::contains(const QPointF &point) const
{
    return boundingRect().contains(point);
}

void JDynamicLayer::updateShape()
{
    if (drawScene() != nullptr)
        drawScene()->selectGroup()->updateBoundingRect();
}

QRectF JDynamicLayer::rect() const
{
    return _rect;
}

void JDynamicLayer::setRect(const QRectF &rct)
{
    prepareGeometryChange();
    _rect = rct;

    if (drawScene() != nullptr && isSelected()) {
        drawScene()->updateMrItemBoundingRect();
    }
//    if (!_img.isNull()) {
//        if (rct.size().toSize() != _img.size())
//            _img = _img.scaled(rct.size().toSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//    }
}

QImage &JDynamicLayer::image()
{
    return _img;
}

QPointF JDynamicLayer::mapScenePosToMyImage(const QPointF &pos)
{
    auto result = mapFromScene(pos);
    QTransform trans = imgTrans();
    return trans.map(result);
}

void JDynamicLayer::loadGraphicsUnit(const CGraphicsUnit &data)
{
    _baseImg = data.data.pDyLayer->baseImg;
    _isBlocked = data.data.pDyLayer->blocked;
    LayerBlockerKeeper keeper(this, false);
    this->clear();
    auto cmds = data.data.pDyLayer->commands;
    foreach (auto c, cmds) {
        static_cast<JCommand *>(c)->setLayer(this);
        c->doCommand();
    }
    foreach (auto c, cmds) {
        auto dc = dynamic_cast<JCommand *>(c);
        if (dc != nullptr)
            this->_commands.append(dc);
    }

    if (!data.head.pos.isNull()) {
        this->setPos(data.head.pos);
    }
}

CGraphicsUnit JDynamicLayer::getGraphicsUnit(EDataReason reson) const
{
    CGraphicsUnit unit;
    unit.head.dataType = DyLayer;
    unit.reson = reson;
    unit.data.pDyLayer = new SDynamicLayerUnitData;
    unit.data.pDyLayer->baseImg = _baseImg;
    unit.data.pDyLayer->blocked = _isBlocked;
    foreach (auto p, this->_commands) {
        unit.data.pDyLayer->commands.append(p);
    }
    return unit;
}

void JDynamicLayer::operatingBegin(CGraphItemEvent *event)
{
    CGraphicsItem::operatingBegin(event);
}

void JDynamicLayer::operating(CGraphItemEvent *event)
{
    CGraphicsItem::operating(event);
}

void JDynamicLayer::operatingEnd(CGraphItemEvent *event)
{
    CGraphicsItem::operatingEnd(event);
    auto p = new JGeomeCommand(this);
    appendComand(p, false, false);
}

void JDynamicLayer::doMoving(CGraphItemMoveEvent *event)
{
    QPointF move = event->_scenePos - event->_oldScenePos;
    this->moveBy(move.x(), move.y());
}

void JDynamicLayer::doScaling(CGraphItemScalEvent *event)
{
    QTransform trans = event->trans();
    QRectF rct = this->rect();
    QPointF pos1 = trans.map(rct.topLeft());
    QPointF pos4 = trans.map(rct.bottomRight());
    QRectF newRect = QRectF(pos1, pos4);

    if (newRect.isValid())
        this->setRect(newRect);
}

void JDynamicLayer::doRoting(CGraphItemRotEvent *event)
{
    CGraphicsItem::doRoting(event);
}

void JDynamicLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    beginCheckIns(painter);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->drawImage(boundingRect(), _img, QRectF(0, 0, _img.width(), _img.height()));

    if (isBlurActived()) {
        //painter->translate(boundingRect().topLeft());
        //painter->setTransform(imgTrans().inverted());
        painter->setClipPath(_totalBlurSrokePath);
        painter->drawImage(boundingRect(), _tempBluredImg, QRect(0, 0, _tempBluredImg.width(), _tempBluredImg.height()));
    }
    endCheckIns(painter);
}

void JDynamicLayer::blurBegin(const QPointF &pos)
{
    _isBluring = true;
    _pos = pos;
    _tempBluredImg = NSBlur::blurImage(_img, 10, curView()->page()->defaultAttriVar(BlurPenEffect).toInt());
    _totalBlurPath.moveTo(_pos);
}

void JDynamicLayer::blurUpdate(const QPointF &pos, bool optm)
{
    _totalBlurPath.lineTo(pos);
    QPen pen; pen.setWidthF(curView()->page()->defaultAttriVar(BlurPenWidth).toDouble());
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    _totalBlurSrokePath = CGraphicsItem::getGraphicsItemShapePathByOrg(_totalBlurPath, pen, true, 0, false);

    _pos = pos;
    this->update();
}

void JDynamicLayer::blurEnd()
{
    auto cmd = new JBlurCommand(imgTrans().map(_totalBlurSrokePath), curView()->page()->defaultAttriVar(BlurPenEffect).toInt(), this);
    appendComand(cmd, true, true);

    _isBluring = false;
    _tempBluredImg = QImage();
    _totalBlurPath = QPainterPath();
    _totalBlurSrokePath = QPainterPath();
}

bool JDynamicLayer::isBlurActived()
{
    return _isBluring;
}

QPainterPath JDynamicLayer::getHighLightPath()
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

//item pos to image pos
QTransform JDynamicLayer::imgTrans()
{
    if (_img.isNull())
        return QTransform();

    auto pos = boundingRect().topLeft();
    QTransform scaledTrans;
    scaledTrans.scale(_img.width() / boundingRect().width(), _img.height() / boundingRect().height());
    scaledTrans.translate(-pos.x(), -pos.y());
    return scaledTrans;
}

void JDynamicLayer::JBlurCommand::doCommand()
{
    if (_layer != nullptr) {
        auto blurImag = NSBlur::blurImage(_layer->image(), 10, _tp);
        QPainter painter(&_layer->image());
        painter.setClipPath(_path);
        painter.drawImage(QRect(0, 0, blurImag.width(), blurImag.height()), blurImag);
        //painter.setBrush(QColor(255, 0, 0));
        //painter.drawPath(_path);
    }
}
