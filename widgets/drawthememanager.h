#ifndef DRAWTHEMEMANAGER_H
#define DRAWTHEMEMANAGER_H


#include <QObject>

class DrawThemeManager : public QObject {
    Q_OBJECT
    DrawThemeManager(QObject* parent = 0);
public:
    static DrawThemeManager* instance();

signals:
    void themeChanged(QString themeName);

public slots:
    QString getCurrentTheme();
    void setCurrentTheme(QString themeName);
    QString getQssForWidget(QString className);
    void updateQss();

private:
    static DrawThemeManager* m_drawTheme;
    QString m_currentTheme = "light";
};

#endif // DRAWTHEMEMANAGER_H
