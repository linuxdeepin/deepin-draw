#ifndef CTEXTEDIT_H
#define CTEXTEDIT_H

#include <DTextEdit>

#include <DMenu>

DWIDGET_USE_NAMESPACE

class CTextEdit : public DTextEdit
{
    Q_OBJECT
public:
    explicit CTextEdit(const QString &text, QWidget *parent = nullptr);

signals:

private slots:
    void setTopAlignment();
    void setRightAlignment();
    void setLeftAlignment();
    void setCenterAlignment();

protected:
    void contextMenuEvent(QContextMenuEvent *e) Q_DECL_OVERRIDE;

private:
    DMenu *m_menu;
    QAction *m_cutAction;
    QAction *m_copyAction;
    QAction *m_pasteAction;
    QAction *m_selectAllAction;
    QAction *m_leftAlignAct;
    QAction *m_topAlignAct;
    QAction *m_rightAlignAct;
    QAction *m_centerAlignAct;
};

#endif // CTEXTEDIT_H
