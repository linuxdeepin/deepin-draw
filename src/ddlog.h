// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DDLOG_H
#define DDLOG_H

#include <QDebug>
#include <QLoggingCategory>

// 全项目统一的日志分类：deepin-draw 主程序与静态链接的基础库共用，
// 便于 DConfig (deepin-debug-config / deepin-log-viewer) 按应用统一过滤。
// 分类名统一使用 org.deepin.draw 前缀（与 desktop 文件名对应），子模块用 '.' 连接，
// 便于使用 org.deepin.draw* 通配规则整体开关。

// 主程序入口与应用级日志
inline Q_LOGGING_CATEGORY(drawMainLog, "org.deepin.draw")

// 图形绘制模块
inline Q_LOGGING_CATEGORY(drawShapeLog, "org.deepin.draw.shape")

// 主窗口框架
inline Q_LOGGING_CATEGORY(drawFrameLog, "org.deepin.draw.frame")

// D-Bus 服务接口
inline Q_LOGGING_CATEGORY(drawServiceLog, "org.deepin.draw.service")

// 自定义控件
inline Q_LOGGING_CATEGORY(drawWidgetsLog, "org.deepin.draw.widgets")

// 工具类
inline Q_LOGGING_CATEGORY(drawUtilsLog, "org.deepin.draw.utils")

#endif   // DDLOG_H
