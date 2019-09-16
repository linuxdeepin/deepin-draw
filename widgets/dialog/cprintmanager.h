#ifndef CPRINTMANAGER_H
#define CPRINTMANAGER_H

#include <DWidget>

#include <QPixmap>

class QPrinter;

class CPrintManager : public QObject
{
    Q_OBJECT

public:
    explicit CPrintManager(QObject *parent = nullptr);
    ~CPrintManager();

    void showPrintDialog(const QPixmap &pixmap, QWidget *widget);

private slots:
    void slotPrintPreview(QPrinter *printerPixmap);
private:
    QPixmap m_pixMap;
};

#endif // CPRINTMANAGER_H
