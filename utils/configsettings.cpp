#include "configsettings.h"

#include <QDir>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QDebug>

const QString CONFIG_PATH = QDir::homePath() +
        "/.config/deepin/deepin-draw/config.conf";

ConfigSettings::ConfigSettings(QObject *parent)
    : QObject(parent)
{
    m_settings = new  QSettings("deepin","/deepin-draw/config", this);

    if (!QFileInfo(CONFIG_PATH).exists())
    {
        setValue("common", "strokeColor", "#f6f96d");
        setValue ("common", "fillColor",  "#6bc989");
        setValue("common", "lineWidth", 3);

        setValue("line", "style", 0);
        setValue("text", "fontsize", 12);
        setValue("blur", "index", 0);
    }

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

    if (val.type() == QVariant::Int)
    {
        emit configChanged(group, key, val.toInt());
    }

    qDebug() << "ConfigSettings:" << group << key << val;
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
