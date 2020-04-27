/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "cviewmanagement.h"
#include <QGuiApplication>
#include "frame/cgraphicsview.h"

CManageViewSigleton *CManageViewSigleton::m_pInstance = nullptr;

CManageViewSigleton::CManageViewSigleton()
    : m_thremeType(0)
{

}

CManageViewSigleton *CManageViewSigleton::GetInstance()
{
    if (m_pInstance == nullptr) {
        m_pInstance  = new CManageViewSigleton();
    }
    return m_pInstance;
}

int CManageViewSigleton::getThemeType() const
{
    return m_thremeType;
}

void CManageViewSigleton::setThemeType(const int type)
{
    m_thremeType = type;
}

CGraphicsView *CManageViewSigleton::getCurView()
{
    CGraphicsView *curView = nullptr;

    //获取当前窗口
    if (!m_allViews.isEmpty() && m_curIndex >= 0 && m_curIndex < m_allViews.size()) {
        curView = m_allViews[m_curIndex];
    }

    // 为了解决关闭程序的时候，view有可能被提前释放了，获取view的时候异常退出程序，临时补丁
    if (curView == nullptr) {
        if (m_allViews.size() > 0) {
            curView = m_allViews[0];
        } else {
            curView = new CGraphicsView(this);
            m_allViews.append(curView);
        }
    }

    return curView;
}

void CManageViewSigleton::addView(CGraphicsView *view)
{
    if (nullptr == view) {
        return;
    }
    if (!m_allViews.contains(view)) {
        m_allViews.append(view);
    }
    if (m_curIndex == -1 && !m_allViews.isEmpty()) {
        m_curIndex = 0;
    }
}

void CManageViewSigleton::setCurView(CGraphicsView *view)
{
    if (nullptr == view) {
        return;
    }
    if (m_allViews.contains(view)) {
        m_curIndex = m_allViews.indexOf(view);
    }
    if (m_curIndex == -1 && !m_allViews.isEmpty()) {
        m_curIndex = 0;
    }
}

void CManageViewSigleton::removeView(CGraphicsView *view)
{
    if (nullptr == view) {
        return;
    }
    if (m_allViews.contains(view)) {
        auto curIndex = m_allViews.indexOf(view);
        m_allViews.removeAt(curIndex);
        if (curIndex == m_curIndex) {
            if (m_allViews.isEmpty()) {
                m_curIndex = -1;
            } else {
                m_curIndex = 0;
            }
        }
    }
}

