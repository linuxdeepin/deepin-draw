#include "configsettings.h"

#include <QDir>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

const QString CONFIG_PATH = QDir::homePath() +
        "/.config/deepin/deepin-draw/config.conf";

ConfigSettings::ConfigSettings(QObject *parent)
    : QObject(parent)
{
    m_settings = new  QSettings("deepin","/deepin-draw/config", this);
    QSize canvasSize = qApp->desktop()->size();

    if (!QFileInfo(CONFIG_PATH).exists())
    {
        setValue("artboard", "width", canvasSize.width());
        setValue("artboard", "height", canvasSize.height());
        setValue("common", "strokeColor_transparent", false);
        setValue("common", "strokeColor_alpha", 100);
        setValue("common", "strokeColor", "#dddddd");

        setValue("common", "fillColor_transparent", true);
        setValue("common", "fillColor_alpha", 100);
        setValue("common", "fillColor",  "#000000");
        setValue("common", "lineWidth", 2);

        setValue("line", "style", 1);
        setValue("text", "fontsize", 12);
        setValue("text", "fillColor_transparent", false);
        setValue("text", "fillColor", "#000000");
        setValue("text", "fillColor_alpha", 100);
        setValue("blur", "index", 20);

        setValue("cut", "ration", "free");
    }

    setValue("tools", "activeMove", "false");

    qDebug() << "Setting file:" << m_settings->fileName();
}

ConfigSettings* ConfigSettings::m_configSettings = nullptr;
ConfigSettings* ConfigSettings::instance()
{
    if (!m_configSettings)
    {
        m_configSettings = new ConfigSettings();
    }

    return m_configSettings;
}

void ConfigSettings::setValue(const QString &group, const QString &key,
              QVariant val)
{
    m_settings->beginGroup(group);
    m_settings->setValue(key, val);
    m_settings->endGroup();
    m_settings->sync();

    if (key == "strokeColor")
        qDebug() << "config settings update:" << group << key << val;

    emit configChanged(group, key);
}

QVariant ConfigSettings::value(const QString &group, const QString &key,
                               const QVariant &defaultValue)
{
    QVariant value;
    m_settings->beginGroup(group);
    value = m_settings->value(key, defaultValue);
    m_settings->endGroup();

    return value;
}

QStringList ConfigSettings::keys(const QString &group)
{
    QStringList v;
    m_settings->beginGroup(group);
    v = m_settings->childKeys();
    m_settings->endGroup();

    return v;
}

ConfigSettings::~ConfigSettings()
{
}
