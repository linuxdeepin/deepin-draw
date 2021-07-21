#ifndef FILEHANDER_H
#define FILEHANDER_H

#include <QObject>
#include <QImage>
#include <sitemdata.h>
#include <QFuture>

class PageContext;
class FilePageHander: public QObject
{
    Q_OBJECT
public:
    FilePageHander(QObject *parent = nullptr);
    ~FilePageHander() override;

    /**
     * @brief supPictureSuffix 返回支持的所有图片后缀名
     */
    static QStringList &supPictureSuffix();

    /**
     * @brief supDdfStuffix 返回支持的所有ddf后缀名(暂时只有.ddf)
     */
    static QStringList &supDdfStuffix();

    static bool    isLegalFile(const QString &file);
    static QString toLegalFile(const QString &file);

    bool         load(const QString &file,
                      bool forcePageContext = false,
                      bool syn = false,
                      PageContext **out = nullptr,
                      QImage *outImg = nullptr);

    void         save(PageContext *context,
                      const QString &file = "",
                      bool syn = false,
                      int imageQuility = 100);

    bool         saveToImage(const QImage &img,
                             const QString &file = "",
                             const QString &stuff = "jpg",
                             int imageQuility = 100)const;

    bool         checkFileBeforeLoad(const QString &file);
    bool         checkFileBeforeSave(const QString &file);

    bool isVolumeSpaceAvailabel(const QString &desFile, const int needBytesSize);
    EDdfVersion  getDdfVersion(const QString &file) const;
    bool isDdfFileDirty(const QString &filePath) const;


    Q_SLOT void quit();

signals:
    void loadBegin();
    void loadUpdate(int process, int total);
    void loadEnd(PageContext *result, const QString &error);
    void loadEnd(QImage img, const QString &error);

    void saveBegin(PageContext *cxt);
    void saveUpdate(PageContext *cxt, int process, int total);
    void saveEnd(PageContext *cxt, const QString &error, const QImage &resultImg = QImage());


private:
    QMap<qint64, QFuture<void> > _futures;

    DECLAREPRIVATECLASS(FilePageHander)
};

#endif // FILEHANDER_H
