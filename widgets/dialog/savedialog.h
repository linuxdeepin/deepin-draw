#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include "dialog.h"

#include <QKeyEvent>

class SaveDialog : public Dialog
{
    Q_OBJECT
public:
    explicit SaveDialog(QWidget* parent = 0);

signals:
    void saveToPath(const QString &path);
    void imageNameChanged(QString imagename);

public:
    QString getSaveDir(QString dir);

protected:
    void keyPressEvent(QKeyEvent *e) override;

private:
    QString m_fileDir;
    QString m_filePath;
};

#endif // SAVEDIALOG_H
