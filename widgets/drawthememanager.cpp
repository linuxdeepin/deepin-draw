#include "drawthememanager.h"

#include <QApplication>
#include <QWidget>

#include "utils/baseutils.h"

namespace {
const QString THEME_GROUP = "APP";
const QString THEME_TEXT = "AppTheme";
}

DrawThemeManager * DrawThemeManager::m_drawTheme = NULL;
DrawThemeManager *DrawThemeManager::instance()
{
    if (m_drawTheme == NULL) {
        m_drawTheme = new DrawThemeManager;
    }

    return m_drawTheme;
}

DrawThemeManager::DrawThemeManager(QObject *parent) : QObject(parent)
{
}

QString DrawThemeManager::getCurrentTheme(){
    return m_currentTheme;
}

void DrawThemeManager::setCurrentTheme(QString themeName) {
    m_currentTheme = themeName;

    emit themeChanged(m_currentTheme);
}

QString DrawThemeManager::getQssForWidget(QString className) {
    return getFileContent(QString(":/theme/%1/%2.qss").arg(m_currentTheme).arg(className));
}

void DrawThemeManager::updateQss()
{
    QWidget *w = qobject_cast<QWidget*>(sender());
    if(w){
        w->setStyleSheet(w->styleSheet());
    }
}
