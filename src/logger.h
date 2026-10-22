// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DRAW_LOGGER_H
#define DRAW_LOGGER_H

#include <QObject>
#include <QString>
#include <dtkcore_global.h>

#ifdef DTKCORE_CLASS_DConfigFile
DCORE_BEGIN_NAMESPACE
class DConfig;
DCORE_END_NAMESPACE
#endif

/**
 * @brief 动态日志规则管理器（参考 deepin-reader 的 MLogger 实现）。
 *
 * 在 main() 中尽早实例化（Application 之前），完成三件事：
 * 1. 读取并清除 QT_LOGGING_RULES 环境变量（qunsetenv 之前不能有任何日志打印）；
 * 2. 读取 DConfig "org.deepin.draw" 的 log_rules 配置项作为默认过滤规则；
 * 3. 监听 DConfig valueChanged，实现调试模式下日志级别的运行时动态切换。
 */
class MLogger : public QObject
{
    Q_OBJECT
public:
    explicit MLogger(QObject *parent = nullptr);
    ~MLogger();

    inline QString rules() const { return m_rules; }
    void setRules(const QString &rules);

private:
    void appendRules(const QString &rules);

private:
    QString m_rules;
#ifdef DTKCORE_CLASS_DConfigFile
    Dtk::Core::DConfig *m_config;
#endif
};

#endif   // DRAW_LOGGER_H
