#ifndef CGRAPHICSTEXTITEM_H
#define CGRAPHICSTEXTITEM_H

#include <QGraphicsTextItem>
class CGraphicsTextItem : public QGraphicsTextItem
{
public:
    explicit CGraphicsTextItem(QGraphicsItem *parent = nullptr);
    explicit CGraphicsTextItem(const QString &text, QGraphicsItem *parent = nullptr);
    ~CGraphicsTextItem() Q_DECL_OVERRIDE;

private:
    void initText();
};

#endif // CGRAPHICSTEXTITEM_H
