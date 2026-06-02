// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// 本头文件提供 Qt5 / Qt6 通用的事件构造辅助宏。
//
// 背景：QWheelEvent / QMouseEvent 在 Qt6 中删除了 Qt5 时期的兼容构造函数，
// 测试代码需要同时兼容两种 Qt 版本，因此在此集中处理差异。
//
// 用法：
//   QT_COMPAT_WHEEL_EVENT(name, pos, delta, button, modifiers)
//   QT_COMPAT_MOUSE_EVENT(name, type, pos, button, buttons, modifiers)
// 这两个宏会以正确的参数列表「原地构造」对应类型的局部变量 name。

#ifndef QTEVENTCOMPAT_H
#define QTEVENTCOMPAT_H

#include <QtGlobal>
#include <QEvent>
#include <QPointF>
#include <QPoint>
#include <Qt>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#  include <QWheelEvent>
#  include <QMouseEvent>
#endif

// 构造一个名为 name 的 QWheelEvent 局部变量，参数语义沿用 Qt5 风格：
//   pos        : 鼠标位置 (QPointF)
//   delta      : 滚轮增量 (int，正负代表方向)
//   button     : 触发按键 (Qt::MouseButton)
//   modifiers  : 修饰键 (Qt::KeyboardModifiers)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#  define QT_COMPAT_WHEEL_EVENT(name, pos, delta, button, modifiers)         \
      QWheelEvent name((pos), (pos), QPoint(), QPoint(0, (delta)),           \
                       Qt::MouseButtons(button), (modifiers),                \
                       Qt::NoScrollPhase, false)
#else
#  define QT_COMPAT_WHEEL_EVENT(name, pos, delta, button, modifiers)         \
      QWheelEvent name((pos), (delta), (button), (modifiers))
#endif

// 构造一个名为 name 的 QMouseEvent 局部变量，参数语义沿用 Qt5 风格：
//   type       : 事件类型 (QEvent::Type)
//   pos        : 鼠标位置 (QPointF)
//   button     : 触发按键 (Qt::MouseButton)
//   buttons    : 当前按键状态 (Qt::MouseButtons)
//   modifiers  : 修饰键 (Qt::KeyboardModifiers)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#  define QT_COMPAT_MOUSE_EVENT(name, type, pos, button, buttons, modifiers) \
      QMouseEvent name((type), (pos), (pos), (button), (buttons), (modifiers))
#else
#  define QT_COMPAT_MOUSE_EVENT(name, type, pos, button, buttons, modifiers) \
      QMouseEvent name((type), (pos), (button), (buttons), (modifiers))
#endif

#endif // QTEVENTCOMPAT_H
