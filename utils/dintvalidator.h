#ifndef DINTVALIDATOR_H
#define DINTVALIDATOR_H

#include <QIntValidator>

class DIntValidator : public QIntValidator
{
    Q_OBJECT
public:
    explicit DIntValidator(QObject* parent = 0) : QIntValidator(parent){}
    DIntValidator(int bottom, int top, QObject* parent) : QIntValidator(
                                                              bottom, top, parent){}
    ~DIntValidator() {}
    virtual void setRange(int bottom, int top)
    {
        QIntValidator::setRange(bottom, top);
    }
    virtual State validate(QString &s, int &n) const
    {
        return QIntValidator::validate(s, n);
    }
    virtual void fixup(QString &s) const
    {
        s = QString("%1").arg(bottom());
    }

};
#endif // DINTVALIDATOR_H
