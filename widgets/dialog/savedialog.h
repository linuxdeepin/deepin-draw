#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include "dialog.h"

#include <QKeyEvent>

#include <QComboBox>
class SaveDialog : public Dialog
{
    Q_OBJECT
public:
    explicit SaveDialog(const QPixmap &pix, QWidget* parent = 0);

signals:
    void saveToPath(const QString &path);
    void imageNameChanged(QString imagename);

public:
    QString getSaveDir(QString dir);
    void updateImageSize();
    void saveImage(const QString &path);

protected:
    void keyPressEvent(QKeyEvent *e) override;

private:
    QString m_fileDir;
    QString m_filePath;
    QString m_imagePath;

    QPixmap m_pixmap;
    QSlider* m_qualitySlider;
    QLabel* m_valueLabel;
    QComboBox* m_contentFormatCBox;
};

#endif // SAVEDIALOG_H
