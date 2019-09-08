#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include "dialog.h"

#include <QKeyEvent>
#include <DComboBox>
#include <DSlider>

//该类作用是导出图片的功能，不是保存和另存为功能

class SaveDialog : public Dialog
{
    Q_OBJECT
public:
    explicit SaveDialog(QList<QPixmap> pixs, DWidget *parent = 0);

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
    DSlider *m_qualitySlider;
    DLabel *m_valueLabel;
    DComboBox *m_contentFormatCBox;
    DComboBox *m_contentSaveDirCBox;
};

#endif // SAVEDIALOG_H
