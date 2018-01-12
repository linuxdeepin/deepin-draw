#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include "dialog.h"

#include <QKeyEvent>
#include <QComboBox>

class SaveDialog : public Dialog
{
    Q_OBJECT
public:
    explicit SaveDialog(QList<QPixmap> pixs, QWidget* parent = 0);

signals:
    void saveToPath(const QString &path);
    void imageNameChanged(QString imagename);

public:
    QString getSaveDir(QString dir);
    void updateImageSize();
    void saveImage(const QString &path);
    QString createSaveBaseName();
    void setVisible(bool visible);

protected:
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *e) Q_DECL_OVERRIDE;

private:
    bool m_closeDialog;
    QString m_fileDir;
    QString m_filePath;
    QString m_imagePath;
    QString m_lastDir;

    QList<QPixmap> m_pixmaps;
    QSlider* m_qualitySlider;
    QLabel* m_valueLabel;
    QComboBox* m_contentFormatCBox;
    QComboBox* m_contentSaveDirCBox;
};

#endif // SAVEDIALOG_H
