#ifndef SETTING_H
#define SETTING_H

#include "globaldefine.h"
#include<QObject>

class QSettings;

class DRAWLIB_EXPORT Setting : public QObject
{
public:
    ~Setting();
    static Setting *instance();
    QStringList readableFormats();
    QStringList writableFormats();
    QStringList readableFormatNameFilters();
    QStringList writableFormatNameFilters();

    /**
         * @brief defaultFileDialogPath 文件对话框的默认路径
         */
    QString defaultFileDialogPath() const;

    /**
     * @brief setDefaultFileDialogPath 设置文件对话框的默认路径
     */
    void  setDefaultFileDialogPath(const QString &defaultPath);

    /**
     * @brief defaultFileDialogNameFilter 文件对话框的默认选中过滤信息
     */
    QString defaultFileDialogNameFilter() const;

    /**
     * @brief setDefaultFileDialogNameFilter 设置文件对话框的默认选中过滤信息
     */
    void setDefaultFileDialogNameFilter(const QString &nameFilter);
    void readSettings();
    void saveSettings();

    void setValue(const QString &key, const QVariant &value);
    QVariant value(const QString &key);
private:
    Setting();
private:

    QStringList supReadFormats;
    QStringList supReadFormatsFilter;
    QStringList supWriteFormatFilters;
    QStringList supWriteFormats;

    QString _defaultFileDialogPath;
    QString _defaultFileDialogNameFilter;

    QSettings *_setting = nullptr;
};

#endif // SETTING_H
