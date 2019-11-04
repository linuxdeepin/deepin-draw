/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXing
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
#include "application.h"
#include "shortcut.h"

#include "shortcut.h"

Shortcut::Shortcut(QObject *parent) : QObject(parent)
{
    ShortcutGroup group1;
    ShortcutGroup group2;
    ShortcutGroup group3;

    group1.groupName = tr("Files");
    group2.groupName = tr("Drawing");
    group3.groupName = tr("Graphics/Images");



    group1.groupItems << ShortcutItem(tr("New"), "Ctrl+N")
                      << ShortcutItem(tr("Open"), "Ctrl+O")
                      << ShortcutItem(tr("Export"), "Ctrl+E")
                      << ShortcutItem(tr("Save"), "Ctrl+S")
                      << ShortcutItem(tr("Save as"), "Ctrl+Shift+S")
                      << ShortcutItem(tr("Print"), "Ctrl+P");

    group2.groupItems <<
                      ShortcutItem(tr("Selected"),  "v") <<
                      ShortcutItem(tr("Export Picture"),  "i") <<
                      ShortcutItem(tr("Rectangle"),        "r") <<
                      ShortcutItem(tr("Ellipse"),        "o") <<
                      ShortcutItem(tr("Triangle"),        "s") <<
                      ShortcutItem(tr("Star"),        "f") <<
                      ShortcutItem(tr("Polygon"),        "h") <<
                      ShortcutItem(tr("Line"),        "l") <<
                      ShortcutItem(tr("Pencil"),        "p") <<
                      ShortcutItem(tr("Text"),           "t") <<
                      ShortcutItem(tr("Blur"),           "b") <<
                      ShortcutItem(tr("Cut"),         "c");


    group3.groupItems <<
                      ShortcutItem(tr("Cut"), "Ctrl + x") <<
                      ShortcutItem(tr("Copy"), "Ctrl + c") <<
                      ShortcutItem(tr("Paste"), "Ctrl + v") <<
                      ShortcutItem(tr("Delete"), "Delete") <<
                      ShortcutItem(tr("Raise Layer"), "Ctrl + ]") <<
                      ShortcutItem(tr("Lower Layer"), "Ctrl + [") <<
                      ShortcutItem(tr("Layer to Top"), "Ctrl + Shift + ]") <<
                      ShortcutItem(tr("Layer to Bottom"), "Ctrl + Shift + [");

    m_shortcutGroups << group1 << group2 << group3;

    //convert to json object
    QJsonArray jsonGroups;
    for (auto scg : m_shortcutGroups) {
        QJsonObject jsonGroup;
        jsonGroup.insert("groupName", scg.groupName);
        QJsonArray jsonGroupItems;
        for (auto sci : scg.groupItems) {
            QJsonObject jsonItem;
            jsonItem.insert("name", sci.name);
            jsonItem.insert("value", sci.value);
            jsonGroupItems.append(jsonItem);
        }
        jsonGroup.insert("groupItems", jsonGroupItems);
        jsonGroups.append(jsonGroup);
    }
    m_shortcutObj.insert("shortcut", jsonGroups);
}
QString Shortcut::toStr()
{
    QJsonDocument doc(m_shortcutObj);
    return doc.toJson().data();
}
