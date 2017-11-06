#include "editlabel.h"

#include <QHBoxLayout>

EditLabel::EditLabel(QWidget *parent)
    : QWidget(parent)
{
    m_titleLabel = new QLabel(this);
    m_edit = new QLineEdit(this);
    m_edit->setObjectName("TitleEdit");
    m_edit->setStyleSheet("QLineEdit#TitleEdit { "
                          "border: 1px solid rgba(0, 0, 0, 100); "
                          "border-radius: 4px;}");

    QHBoxLayout* mLayout = new QHBoxLayout(this);
    mLayout->setMargin(0);
    mLayout->setSpacing(0);
    mLayout->addStretch();
    mLayout->addWidget(m_titleLabel);
    mLayout->addSpacing(4);
    mLayout->addWidget(m_edit);

    connect(m_edit, &QLineEdit::editingFinished, this, [=]{
        emit editTextChanged(m_edit->text());
    });

    setLayout(mLayout);
}

void EditLabel::setTitle(QString title)
{
    m_titleLabel->setText(title);
}

void EditLabel::setEditText(QString text)
{
    m_edit->setText(text);
}

void EditLabel::setEditWidth(int width)
{
    m_edit->setFixedWidth(width);
    this->updateGeometry();
}

QString EditLabel::editText()
{
    return m_edit->text();
}

EditLabel::~EditLabel() {}
