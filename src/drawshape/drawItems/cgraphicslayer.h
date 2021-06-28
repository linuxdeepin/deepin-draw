#ifndef CGRAPHICSLAYER_H
#define CGRAPHICSLAYER_H
#include "cgraphicsitemselectedmgr.h"
#include <QPicture>

class CGraphicsLayer: public CGraphicsItem
{
public:
    CGraphicsLayer();

    QRectF rect() const override;

    void setRect(const QRectF rct);

    /**
     * @brief addCItem　添加图元
     */
    void addCItem(CGraphicsItem *pItem, bool calZ = true);

    /**
     * @brief removeCItem　删除图元
     */
    void removeCItem(CGraphicsItem *pItem);

    /**
     * @brief removeCItem　删除图元
     */
    QImage &layerImage();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void paintSelf(QPainter *painter, const QStyleOptionGraphicsItem *option) override;

private:
    /**
     * @brief updateShape 刷新形状
     * @return
     */
    void updateShape() override;
    /**
     * @brief initRect 初始化矩形的属性和边框小方块
     */

    void initHandle() override {clearHandle();}

    /**
     * @brief updateHandlesGeometry 刷新节点位置
     */
    void updateHandlesGeometry() override {}

protected:
    QImage m_layerImage;

    QList<CGraphicsItem *> m_items;
};
class JActivedPaintInfo
{
public:
    enum EComposBy {ESubPictures, EPainterPath, ECount};
    JActivedPaintInfo();
    ~JActivedPaintInfo();

    void beginSubPicture();
    void addSubPicture(const QPicture &subPicture);
    void endSubPicture();

    void addPoint(const QPointF &pos);
    void setPenForPath(const QPen &pen);
    void setBrushForPath(const QBrush &brush);

    static QPicture getPathPicture(const QPainterPath &path, const QPen &pen, const QBrush &brush);


    QPicture &picture(EComposBy tp = ESubPictures);

private:
    void ensurePainterPathPicture();

    QPainter *painter();

private:
    //data should only one , so we use shared pointer to manager the memory.
    QSharedPointer<QPicture> _pictures[ECount];
    QSharedPointer<QPainter> _pPainter;

    QPainterPath _path;
    QPen         _pen;
    QBrush       _brush;
    bool         _pathPictureIsDirty = true;

};
class JDynamicLayer: public CGraphicsItem
{
public:

    class JCommand: public JDyLayerCmdBase
    {
    public:
        JCommand(JDynamicLayer *layer = nullptr): _layer(layer) {}
        void setLayer(JDynamicLayer *layer) {_layer = layer;}
        virtual ~JCommand() {}

        virtual void doCommand() = 0;

    protected:
        JDynamicLayer *_layer = nullptr;

    };

    class JGeomeCommand: public JCommand
    {
    public:
        JGeomeCommand(JDynamicLayer *layer = nullptr): JCommand(layer)
        {
            if (_layer != nullptr) {
                _pos = layer->pos();
                _rotate = layer->drawRotation();
                _z = layer->drawZValue();
                _trans = layer->transform();
                _rect = layer->boundingRect();
            }
        }
        JGeomeCommand(const QPoint &pos, qreal rotate, qreal z, const QRectF &rct, const QTransform &trans): JCommand(nullptr)
        {
            _pos = pos;
            _rotate = rotate;
            _z = z;
            _trans = trans;
            _rect = rct;
        }
        int  cmdType() override {return 1;}

        void doCommand() override
        {
            if (_layer != nullptr) {
                _layer->setPos(_pos);
                _layer->setDrawRotatin(_rotate);
                _layer->setZValue(_z);
                _layer->setTransform(_trans);
                _layer->setRect(_rect);
            }
        }
        void serialization(QDataStream &out) override
        {
            out << _pos;
            out << _rotate;
            out << _z;
            out << _rect;
            out << _trans;
        }
        void deserialization(QDataStream &in) override
        {
            in >> _pos;
            in >> _rotate;
            in >> _z;
            in >> _rect;
            in >> _trans;
        }

    private:
        QPointF _pos;
        qreal   _rotate;
        qreal   _z;

        QRectF  _rect;
        QTransform   _trans;

    };
    class JPathCommand: public JCommand
    {
    public:
        JPathCommand(const QPainterPath &path = QPainterPath(), const QPen &p = QPen(), JDynamicLayer *layer = nullptr): JCommand(layer)
        {
            _path = path;
            _pen = p;
        }
        int  cmdType() override {return 2;}
        void doCommand() override
        {
            if (_layer != nullptr)
                _layer->addPenPath(_path, _pen, 0, false);
        }
        void serialization(QDataStream &out) override
        {
            out << _path;
            out << _pen;
        }
        void deserialization(QDataStream &in) override
        {
            in >> _path;
            in >> _pen;
        }

        QPainterPath _path;

        QPen _pen;
    };

    class JPaintCommand: public JCommand
    {
    public:
        JPaintCommand(const QPicture &picture = QPicture(), bool dyImag = false, JDynamicLayer *layer = nullptr): JCommand(layer)
        {
            _picture = picture;
            _dyImag = dyImag;
        }
        int  cmdType() override {return 3;}
        void doCommand() override
        {
            if (_layer != nullptr)
                _layer->addPicture(_picture, false, _dyImag);
        }
        void serialization(QDataStream &out) override
        {
            out << _picture;
            out << _dyImag;
        }
        void deserialization(QDataStream &in) override
        {
            in >> _picture;
            in >> _dyImag;
        }
        QPicture _picture;
        bool     _dyImag = false;
    };

    class JBlurCommand: public JCommand
    {
    public:
        JBlurCommand(const QPainterPath &blurPath = QPainterPath(), int blurType = 0, JDynamicLayer *layer = nullptr): JCommand(layer)
        {
            _path = blurPath;
            _tp = blurType;
        }
        int  cmdType() override {return 4;}
        void doCommand() override;

        void serialization(QDataStream &out) override
        {
            out << _path;
            out << _tp;
        }
        void deserialization(QDataStream &in) override
        {
            in >> _path;
            in >> _tp;
        }
        QPainterPath _path;
        int _tp = 0;
    };

    JDynamicLayer(const QImage &image = QImage(), QGraphicsItem *parent = nullptr);
    virtual ~JDynamicLayer();

    int  type() const override;

    DrawAttribution::SAttrisList attributions() override;
    void setAttributionVar(int attri, const QVariant &var, int phase) override;

    void clear();

    void addPenPath(const QPainterPath &path, const QPen &pen, int type = 0, bool creatCmd = true);
    void addPicture(const QPicture &picture, bool creatCmd = true, bool dyImag = false);
    void appendComand(JCommand *cmd, bool doCmd = false, bool addToStack = true);

    QRectF boundingRect() const override;

    QPainterPath shape() const override;
    bool contains(const QPointF &point) const override;

    void updateShape() override;

    QRectF rect() const override;
    void   setRect(const QRectF &rct);

    QImage &image();

    QPointF mapScenePosToMyImage(const QPointF &pos);

    void loadGraphicsUnit(const CGraphicsUnit &data) override;
    CGraphicsUnit getGraphicsUnit(EDataReason reson) const override;

    void operatingBegin(CGraphItemEvent *event) override;
    void operating(CGraphItemEvent *event) override;
    void operatingEnd(CGraphItemEvent *event) override;

    void doMoving(CGraphItemMoveEvent *event) override;
    void doScaling(CGraphItemScalEvent *event) override;
    void doRoting(CGraphItemRotEvent *event) override;


    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;


    QList<JCommand *> commands() {return _commands;}

    bool isBlurEnable()const override {return true;}

    void blurBegin(const QPointF &pos) override;

    void blurUpdate(const QPointF &pos, bool optm = false) override;

    void blurEnd() override;

    bool isBlurActived() override;

    QTransform imgTrans();

protected:
    QImage _img;

    QImage _baseImg;

    QRectF _rect;

    QList<JCommand *> _commands;
    bool    _isBluring = false;
    QImage  _tempBluredImg;
    QPointF _pos;
    QPainterPath _totalBlurPath;
    QPainterPath _totalBlurSrokePath;
};

#endif // CGRAPHICSLAYER_H
