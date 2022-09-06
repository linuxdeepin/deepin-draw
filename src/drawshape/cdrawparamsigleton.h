// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef CDRAWPARAMSIGLETON_H
#define CDRAWPARAMSIGLETON_H
#include "globaldefine.h"
#include "sitemdata.h"
#include "cattributeitemwidget.h"

#include <QtGlobal>
#include <QColor>
#include <QPen>
#include <QFont>
class Page;
class PageScene;

class PageContext: public QObject
{
    Q_OBJECT
public:
    PageContext(const QString &file = "", QObject *parent = nullptr);

    QString key() const;

    Page *page()const;

    QString name()const;
    void    setName(const QString &name);

    QString file()const;
    void setFile(const QString &file);
    bool save(const QString &file = "");
    bool isDirty()const;
    void setDirty(bool dirty);

    DrawAttribution::SAttrisList currentAttris() const;
    void setDefaultAttri(int type, const QVariant &var);
    QVariant defaultAttri(int type) const;

    bool isEmpty()const;
    PageScene *scene() const;
    void addSceneItem(const CGraphicsUnit &var, bool record = false, bool releaseUnit = true, bool select = false);
    void addImage(const QImage &img, const QPointF &pos = QPointF(), const QRectF &rect = QRectF(), bool record = false, bool select = false);
    //void addText(const QString &text, bool record = false, bool select = false);

    QRectF pageRect() const;
    void   setPageRect(const QRectF &rect);

    QImage renderToImage(const QColor &bgColor = Qt::transparent, const QSize &desImageSize = QSize()) const;

    void update();
signals:
    void dirtyChanged(bool isDirty);

private:
    QMap<int, QVariant> _attriValues;

    QString _key;
    QString _file;
    QString _name;

    PageScene *_scene = nullptr;
    Page *_page = nullptr;

    int _dirty: 1;
};


#endif // CDRAWPARAMSIGLETON_H
