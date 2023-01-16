#include "setting.h"
#include "global.h"
#include <QSettings>
#include <QDir>
#include <QStandardPaths>

Setting::Setting()
{
    QString fileName = Global::configPath() + "/config.conf";
    _setting = new QSettings(fileName, QSettings::IniFormat);

    supReadFormats  = QStringList() << "ddf" << "png" << "jpeg" << "jpg" << "bmp" << "tif" << "tiff" << "ppm" << "xbm" << "xpm" << "pgm" << "pbm";
    supReadFormatsFilter = QStringList() << QObject::tr("DDF Drawings") + "(*.ddf)"
                           << "PNG(*.png)"
                           << "JPEG(*.jpeg *.jpg)"
                           << "BMP(*.bmp)"
                           << "TIFF(*.tif *.tiff)"
                           << "PPM(*.ppm)"
                           << "XBM(*.xbm)"
                           << "XPM(*.xpm)"
                            << "PBM(*.pbm)";
    supWriteFormatFilters = QStringList() << QObject::tr("DDF Drawings") + "(*.ddf)"
                            << "PNG(*.png)"
                            << "JPEG(*.jpeg *.jpg)"
                            << "BMP(*.bmp)"
                            << "TIFF(*.tif *.tiff)"
                            << "PDF(*.pdf)"
                            << "PPM(*.ppm)"
                            << "XBM(*.xbm)"
                            << "XPM(*.xpm)";

    supWriteFormats = QStringList() << "ddf" << "png" << "jpeg" << "jpg" << "bmp" << "tif" << "tiff" << "pdf" << "ppm" << "xbm" << "xpm";
}

Setting::~Setting()
{
    if (nullptr != _setting) {
        delete _setting;
        _setting = nullptr;
    }
}

Setting *Setting::instance()
{
    static Setting *setting = nullptr;
    if (nullptr == setting) {
        setting = new Setting;
        setting->readSettings();
    }

    return setting;
}

QStringList Setting::readableFormats()
{
    return supReadFormats;
}

QStringList Setting::writableFormats()
{
    return supWriteFormats;
}

QStringList Setting::readableFormatNameFilters()
{
    return supReadFormatsFilter;
}

QStringList Setting::writableFormatNameFilters()
{
    return supWriteFormatFilters;
}

QString Setting::defaultFileDialogPath() const
{
    QDir dir(_defaultFileDialogPath);
    if (dir.exists())
        return _defaultFileDialogPath;
    else {
        auto s = const_cast<QString *>(&_defaultFileDialogPath);
        auto standerPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/Draw";
        *s = standerPath;
    }

    return _defaultFileDialogPath;
}

QString Setting::defaultFileDialogNameFilter() const
{
    return _defaultFileDialogNameFilter;
}
void Setting::setDefaultFileDialogPath(const QString &defaultPath)
{
    _defaultFileDialogPath = defaultPath;
}

QPair<int,QString> Setting::defaultExportDialogPath() const
{
    return _defaultExportDialogPath;
}

void Setting::setDefaultExportDialogPath(const QPair<int,QString> &defaultPath)
{
    _defaultExportDialogPath = defaultPath;
}

int Setting::defaultExportDialogFilteFormat() const
{
    return _defaultExportDialogFilterFormats;
}

void Setting::setDefaultExportDialogFilterFormat(const int &nameFilter)
{
    _defaultExportDialogFilterFormats = nameFilter;
}

void Setting::setDefaultFileDialogNameFilter(const QString &nameFilter)
{
    _defaultFileDialogNameFilter = nameFilter;
}

void Setting::readSettings()
{
    _defaultFileDialogPath = _setting->value("defaultFileDialogPath").toString();

    if (_defaultFileDialogPath.isEmpty()) {
        _defaultFileDialogPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
        QFileInfo dir(_defaultFileDialogPath);
        if (dir.isDir() && dir.isWritable()) {
            _defaultFileDialogPath += "/Draw";
        }
    }

    _defaultFileDialogNameFilter = _setting->value("defaultFileDialogNameFilter").toString();

    if (_defaultFileDialogNameFilter.isEmpty()) {
        if (!supWriteFormatFilters.isEmpty())
            _defaultFileDialogNameFilter = supWriteFormatFilters.first();
    }
}

void Setting::saveSettings()
{
    _setting->setValue("defaultFileDialogPath", _defaultFileDialogPath);
    _setting->setValue("defaultFileDialogNameFilter", _defaultFileDialogNameFilter);
}

void Setting::setValue(const QString &key, const QVariant &value)
{
    _setting->setValue(key, value);
}

QVariant Setting::value(const QString &key)
{
    return _setting->value(key);
}

