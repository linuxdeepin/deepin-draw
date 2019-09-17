#ifndef CVALIDATOR_H
#define CVALIDATOR_H

#include <QIntValidator>

class CIntValidator : public QIntValidator
{
public:
    explicit CIntValidator(QObject *parent = nullptr);
    explicit CIntValidator(int bottom, int top, QObject *parent = nullptr);
    ~CIntValidator();

    virtual State validate(QString &, int &) const Q_DECL_OVERRIDE;
    virtual void fixup(QString &) const Q_DECL_OVERRIDE;
};

#endif // CVALIDATOR_H
