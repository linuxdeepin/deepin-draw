#include "editlabel.h"

#include <QHBoxLayout>

EditLabel::EditLabel(QWidget *parent)
    : QWidget(parent) {
    m_titleLabel = new QLabel(this);
    m_edit = new QLineEdit(this);

    QHBoxLayout* mLayout = new QHBoxLayout(this);
    mLayout->setMargin(0);
    mLayout->setSpacing(0);
    mLayout->addWidget(m_titleLabel);
    mLayout->addWidget(m_edit);

    setLayout(mLayout);
}

void EditLabel::setTitle(QString title) {
    m_titleLabel->setText(title);
}

void EditLabel::setEditText(QString text) {
    m_edit->setText(text);
}

EditLabel::~EditLabel() {}
