# AT-SPI 扫描报告 - deepin-draw

## 扫描范围
- 仓库: https://github.com/linuxdeepin/deepin-draw (master)
- 扫描时间: 2026-08-05
- 扫描方法: 本地源码静态分析（libclang AST 口径模拟）
- 扫描文件: 全部 `src/` 目录下的 `.cpp/.h` 文件（排除 tests/）

## 扫描结果

### 已有 AT-SPI 覆盖（AN = setAccessibleName 或 setWgtAccesibleName）

#### MainWindow 及主框架
| 文件 | accessibleName |
|------|---------------|
| mainwindow.cpp | `MainWindow` |
| toptoolbar.cpp | `TopToolbar`, `ComAttrWidget` |
| clefttoolbar.cpp | `LeftTool bar` |
| cmultiptabbarwidget.cpp | `MultipTabBarWidget` |
| ccentralwidget.cpp | `Page{N}`, `DrawBoard{N}` |

#### 绘图工具按钮 (drawTools/)
| 文件 | accessibleName |
|------|---------------|
| cselecttool.cpp | `Select tool button` |
| crecttool.cpp | `Rectangle tool button`, `stroken color button`, `Pen width combox`, `Line width combox`, `fill color button`, `Rect Radio spinbox` |
| cellipsetool.cpp | `Ellipse tool button` |
| ctringletool.cpp | `Triangle tool button` |
| clinetool.cpp | `Line tool button` |
| ctexttool.cpp | `Text tool button`, `Text color button`, `Text font family comboBox`, `Text font style comboBox`, `Text font size comboBox` |
| cpentool.cpp | `Pencil tool button`, `Line start style combox`, `Line end style combox` |
| cerasertool.cpp | `Eraser tool button`, `Eraser inner spinbox` |
| ccutTool.cpp | `Crop tool button`, `scene cut attribution widget` |
| cpicturetool.cpp | `Import tool button` |
| cpolygontool.cpp | `Polygon tool button`, `Polgon edges spinbox` |
| cpolygonalstartool.cpp | `Star tool button`, `Star Anchor spinbox`, `Star inner radius spinbox` |
| cmasicotool.cpp | `Blur tool button` |

#### 属性控件 (AttributesWidgets/)
| 文件 | accessibleName |
|------|---------------|
| ccolorpickwidget.cpp | `ColorPickWidget` |
| calphacontrolwidget.cpp | `Color Alpha slider` |
| blurwidget.cpp | `Blur type button`, `Masic type button` |
| ccutwidget.cpp | `Cut ratio(1:1) pushbutton`, `Cut ratio(2:3) pushbutton`, `Cut ratio(8:5) pushbutton`, `Cut ratio(16:9) pushbutton`, `Cut ratio(free) pushbutton`, `Cut ratio(Original) pushbutton`, `Cut done pushbutton`, `Cut cancel pushbutton` |
| colorpanel.cpp | `Panel {color} pushbutton` (for color buttons) |
| cattributeitemwidget.cpp | `groupButton`, `unGroupButton`, `_pExpWidget` |

#### 插件工具按钮 (deepin-draw-plugins/)
| 文件 | accessibleName |
|------|---------------|
| ccalligraphypen.cpp | `Calligraphy Pen tool button` |
| ceraser.cpp | `eraser tool button` |
| cfilltool.cpp | `Paint bucket tool button` |

#### 通用控件 (widgets/)
| 文件 | accessibleName |
|------|---------------|
| csidewidthwidget.cpp | `CSideWidthWidget` |
| ccutdialog.cpp | `Notice cut info dialog` |
| cexportimagedialog.cpp | `Export dialog`, `Export name line editor`, `Export path comboBox`, `Export format comboBox`, `Export quality slider`, `Export content widget` |
| drawdialog.cpp | `Notice save dialog` |
| dzoommenucombobox.cpp | `Zoom Form`, `Zoom Menu button`, `Zoom Menu`, `Zoom increase button`, `Zoom reduce button` |

### 缺失 AT-SPI 覆盖

以下 widget 类未调用 setWgtAccesibleName/setAccessibleName：

#### 1. src/frame/AttributesWidgets/private/pickcolorwidget.cpp
- PickColorWidget 自身
- m_picker (CIconButton) - 取色器按钮
- m_colorLabel (ColorLabel) - 色彩选取区域
- m_colorSlider (ColorSlider) - 色相滑块
- m_redEditLabel / m_greenEditLabel / m_blueEditLabel (EditLabel) - RGB 编辑框

#### 2. src/frame/AttributesWidgets/private/colorpanel.cpp
- `m_colorfulBtn` 仅 `setObjectName("CIconButton")` 缺 setAccessibleName
- `m_pickColWidget` 仅 `setObjectName("PickColorWidget")` 缺 setAccessibleName
- `m_colLineEdit` 仅 `setObjectName("ColorLineEdit")` 缺 setAccessibleName
- `m_alphaControlWidget` 仅 `setObjectName("CAlphaControlWidget")` 缺 setAccessibleName

#### 3. src/frame/cgraphicsview.cpp
- `m_itemsHEqulSpaceAlign` 仅 `setObjectName` 缺 setAccessibleName
- `m_itemsVEqulSpaceAlign` 仅 `setObjectName` 缺 setAccessibleName

#### 4. 独立控件类（自身构造函数中未设置）
| 类名 | 文件 | 说明 |
|------|------|------|
| CIconButton | src/widgets/ciconbutton.cpp | 自绘图标按钮 |
| CSpinBox | src/widgets/cspinbox.cpp | 数字输入框 |
| ColorLabel | src/widgets/colorlabel.cpp | 色彩选取 Label |
| ColorSlider | src/widgets/colorslider.cpp | 色相滑块 |
| CTextEdit | src/widgets/ctextedit.cpp | 文本编辑框 |
| ToolButton | src/widgets/toolbutton.cpp | 工具按钮 |
| EditLabel | src/widgets/editlabel.cpp | 编辑标签 |
| CMenu | src/widgets/cmenu.cpp | 菜单 |
| MessageDlg | src/widgets/dialog/dialog.cpp | 消息对话框 |
| ProgressDialog | src/widgets/dialog/cprogressdialog.cpp | 进度对话框 |

## 覆盖率统计
- 有 setAccessibleName 的文件: 30
- 缺 setAccessibleName 的 widget 类: 10
- 预计补全后覆盖率达到: 95%+

## 补全策略
1. 对已有 `setObjectName` 但缺 `setAccessibleName` 的控件：改用 `setWgtAccesibleName`
2. 对完全缺失的控件类构造函数：添加 `setWgtAccesibleName`
3. 对属性面板子控件：在创建处添加 `setWgtAccesibleName`