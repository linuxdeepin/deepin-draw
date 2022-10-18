#include "itemstyleregisters.h"

#include <QButtonGroup>
#include <QAbstractItemView>
#include <QPoint>
#include <QObject>
#include <QListView>
#include <QFontComboBox>

#include "pageitem.h"
#include "drawboard.h"
#include "groupbuttonwidget.h"
#include "rotateattriwidget.h"
#include "orderwidget.h"
#include "attributemanager.h"
#include "pageview.h"
#include "adjustmentatrriwidget.h"
#include "cundoredocommand.h"
#include "layeritem.h"
#include "styleattriwidget.h"
#include "colorstylewidget.h"
#include "csidewidthwidget.h"
#include "rectradiusstylewidget.h"
#include "cspinbox.h"
#include "sliderspinboxwidget.h"
#include "attributewidget.h"
#include "application.h"
#include "hboxlayoutwidget.h"
#include "cutattributionwidget.h"
#include "rectradiusstylewidget.h"

/**
 * @brief 用于默认设置的属性工具配置属性值，
 *      包含设置范围及默认值
 */
enum AttriDefaultValue {
    EStarAnchorMin = 3,         // 星形图元锚点默认值
    EStarAnchorMax = 50,
    EStarAnchorDefault = 5,

    EStarRadioMin = 0,          // 星形图元半径默认值
    EStarRadioMax = 100,
    EStarRadioDefault = 50,

    EPolygonSidesMin = 4,       // 多边形图元侧边数默认值
    EPolygonSidesMax = 10,
    EPolygonSidesDefault = 5,

    EEraserWidthMin = 1,        //橡皮檫
    EEraserWidthMax = 500,
    EEraserWidthDefault = 20,

    EBlurWidthMin = 5,          //模糊
    EBlurWidthMax = 500,
    EBlurWidthDefault = 20,
};



void AdjustmentAttriRegister::registe()
{
    AdjustmentAtrriWidget *widget = new AdjustmentAtrriWidget;
    connect(widget->button(), &QPushButton::clicked, widget, [ = ]() {
        auto page = drawBoard()->currentPage();
        if (page == nullptr)
            return;

        auto curScene = page->scene();
        if (curScene != nullptr) {
            auto rect = curScene->selectedPageItems().first()->sceneBoundingRect();
            if (rect != curScene->sceneRect()) {
                UndoRecorder block(page->scene()->currentTopLayer(), LayerUndoCommand::RectChanged);
                curScene->currentTopLayer()->setRect(rect);
                page->view()->setSceneRect(rect);
                curScene->setSceneRect(rect);
            }
        }
    });

    connect(drawBoard()->attributionManager()->helper(), &AttributionManagerHelper::updateWgt, this,
    [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == widget) {
            widget->setEnabled(var.toBool());
        }
    });

    drawBoard()->attributionManager()->installComAttributeWgt(EImageAdaptScene, widget, false);

}

void StarAnchorAttriRegister::registe()
{
    if (nullptr != m_starAnchorAttri) {
        return;
    }

    m_starAnchorAttri = new SliderSpinBoxWidget(EStarAnchor);
    m_starAnchorAttri->setRange(EStarAnchorMin, EStarAnchorMax);
    m_starAnchorAttri->setVar(EStarAnchorDefault);
    m_starAnchorAttri->setTitle(tr("Vertices"));
    m_starAnchorAttri->addHSeparateLine();

    setWgtAccesibleName(m_starAnchorAttri, "starAnchorAttri");
    drawBoard()->attributionManager()->installComAttributeWgt(EStarAnchor, m_starAnchorAttri, 5);

    connect(m_starAnchorAttri, &SliderSpinBoxWidget::sigValueChanged, this, [ = ](int value, EChangedPhase phase) {
        drawBoard()->setDrawAttribution(EStarAnchor, value, phase);
    });
}

void StartAttriRegister::registe()
{
    if (nullptr != m_starRadioAttri) {
        return;
    }

    m_starRadioAttri = new SliderSpinBoxWidget(EStarInnerOuterRadio, SliderSpinBoxWidget::EPercentStyle);
    m_starRadioAttri->setRange(EStarRadioMin, EStarRadioMax);
    m_starRadioAttri->setVar(EStarRadioDefault);
    m_starRadioAttri->setTitle(tr("Radius"));
    m_starRadioAttri->addHSeparateLine();

    setWgtAccesibleName(m_starRadioAttri, "starInnerOuterRadio");
    drawBoard()->attributionManager()->installComAttributeWgt(EStarInnerOuterRadio, m_starRadioAttri, 5);

    connect(m_starRadioAttri, &SliderSpinBoxWidget::sigValueChanged, this, [ = ](int value, EChangedPhase phase) {
        drawBoard()->setDrawAttribution(EStarInnerOuterRadio, value, phase);
    });
}

void PolygonSidesAttriRegister::registe()
{
    if (nullptr != m_polygonSidesAttri) {
        return;
    }

    m_polygonSidesAttri = new SliderSpinBoxWidget(EPolygonSides);
    m_polygonSidesAttri->setRange(EPolygonSidesMin, EPolygonSidesMax);
    m_polygonSidesAttri->setVar(EPolygonSidesDefault);
    m_polygonSidesAttri->setTitle(tr("Sides"));
    m_polygonSidesAttri->addHSeparateLine();

    setWgtAccesibleName(m_polygonSidesAttri, "polygonSidesAttri");
    drawBoard()->attributionManager()->installComAttributeWgt(EPolygonSides, m_polygonSidesAttri, 5);

    connect(m_polygonSidesAttri, &SliderSpinBoxWidget::sigValueChanged, this, [ = ](int value, EChangedPhase phase) {
        drawBoard()->setDrawAttribution(EPolygonSides, value, phase);
    });
}

void LineAttriRegister::registe()
{
    m_streakStyle = new HBoxLayoutWidget(drawBoard());
    m_comboxstart = new QComboBox(m_streakStyle);
    m_comboxend = new QComboBox(m_streakStyle);

    m_streakStyle->addWidget(m_comboxstart);
    m_streakStyle->addWidget(m_comboxend);

    m_comboxstart->setMinimumSize(QSize(110, 36));
    m_comboxstart->setIconSize(QSize(60, 20));
    m_comboxstart->setFocusPolicy(Qt::NoFocus);

    m_comboxend->setMinimumSize(QSize(110, 36));
    m_comboxend->setIconSize(QSize(60, 20));
    m_comboxend->setFocusPolicy(Qt::NoFocus);

    QStringList icon_leftarry = {"ddc_none_arrow", "ddc_leftcircle_hollow", "ddc_leftcircle_solid"
                                 , "ddc_leftarrow_fine", "ddc_leftarrow_crude"
                                };
    QStringList icon_rightarry = {"ddc_none_arrow", "ddc_rightcircle_hollow", "ddc_rightcircle_solid"
                                  , "ddc_rightarrow_fine", "ddc_rightarrow_crude"
                                 };

    for (int i = 0; i < icon_leftarry.size(); ++i) {
        m_comboxstart->addItem(QIcon::fromTheme(icon_leftarry[i]), "");
        m_comboxend->addItem(QIcon::fromTheme(icon_rightarry[i]), "");
    }

    m_streakStyle->addWidget(m_comboxstart);
    m_streakStyle->addWidget(m_comboxend);
    m_comboxstart->setProperty(ChildAttriWidget, true);
    m_comboxend->setProperty(ChildAttriWidget, true);

    drawBoard()->attributionManager()->installComAttributeWgt(EStreakStyle, m_streakStyle);
    drawBoard()->attributionManager()->installComAttributeWgt(EStreakBeginStyle, m_comboxstart);
    drawBoard()->attributionManager()->installComAttributeWgt(EStreakEndStyle, m_comboxend);

    connect(m_comboxstart, QOverload<int>::of(&QComboBox::currentIndexChanged),
    [ = ](int index) {drawBoard()->setDrawAttribution(EStreakBeginStyle, index);});
    connect(m_comboxend, QOverload<int>::of(&QComboBox::currentIndexChanged),
    [ = ](int index) {drawBoard()->setDrawAttribution(EStreakEndStyle, index);});

    auto attriMangerWgt = drawBoard()->attributionManager();
    connect(attriMangerWgt->helper(), &AttributionManagerHelper::updateWgt, m_comboxstart, [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == m_comboxstart) {
            QSignalBlocker blocker(m_comboxstart);
            if (var.isValid()) {
                int string = var.toInt();
                m_comboxstart->setCurrentIndex(string);
            } else {
                m_comboxstart->setCurrentIndex(0);
            }
        }
    });

    connect(attriMangerWgt->helper(), &AttributionManagerHelper::updateWgt, m_comboxend, [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == m_comboxend) {
            QSignalBlocker blocker(m_comboxstart);
            if (var.isValid()) {
                int string = var.toInt();
                m_comboxend->setCurrentIndex(string);
            } else {
                m_comboxend->setCurrentIndex(0);
            }
        }
    });
}

void EraserAttriRegister::registe()
{
    m_eraserAttri = new SliderSpinBoxWidget(EEraserWidth);
    m_eraserAttri->setRange(EEraserWidthMin, EEraserWidthMax);
    m_eraserAttri->setVar(EEraserWidthDefault);
    m_eraserAttri->setTitle(tr("Width"));

    setWgtAccesibleName(m_eraserAttri, "EraserWidHBoxLayoutWidgetthAttri");
    drawBoard()->attributionManager()->installComAttributeWgt(EEraserWidth, m_eraserAttri, 5);

    connect(m_eraserAttri, &SliderSpinBoxWidget::sigValueChanged, this, [ = ](int value, EChangedPhase phase) {
        drawBoard()->setDrawAttribution(EEraserWidth, value, phase);
    });
}

void BlurAttriRegister::registe()
{
    m_blurStyle = new HBoxLayoutWidget(drawBoard());
    m_blurEffect = new QToolButton(m_blurStyle);
    m_masicoEffect = new QToolButton(m_blurStyle);

    QButtonGroup *button_group = new QButtonGroup(this);
    m_blurStyle->setTitle("");
    m_blurEffect->setText(tr("Blur"));
    m_masicoEffect->setText(tr("Mosaic"));

    button_group->addButton(m_blurEffect);
    button_group->addButton(m_masicoEffect);

    m_blurStyle->getLayout()->setContentsMargins(0, 1, 1, 0);
    m_blurStyle->getLabel()->setFixedSize(QSize(100, 10));

    m_blurEffect->setFixedSize(QSize(100, 30));
    m_masicoEffect->setFixedSize(QSize(100, 30));
    m_blurEffect->setCheckable(true);
    m_masicoEffect->setCheckable(true);
    //设置初始值为高斯模糊
    m_masicoEffect->setChecked(true);

    m_blurStyle->addWidget(m_blurEffect);
    m_blurStyle->addWidget(m_masicoEffect);
    m_blurEffect->setProperty(ChildAttriWidget, true);
    m_masicoEffect->setProperty(ChildAttriWidget, true);

    m_blurAttri = new SliderSpinBoxWidget(EBlurAttri);
    m_blurAttri->setRange(EBlurWidthMin, EBlurWidthMax);
    m_blurAttri->setVar(EBlurWidthDefault);
    m_blurAttri->setTitle(tr("Width"));

    setWgtAccesibleName(m_blurAttri, "EBlurAttri");
    drawBoard()->attributionManager()->installComAttributeWgt(EBlurAttri, m_blurAttri, EBlurWidthDefault);
    setWgtAccesibleName(m_blurStyle, "EEBlurStyle");
    drawBoard()->attributionManager()->installComAttributeWgt(EEBlurStyle, m_blurStyle);

    connect(m_blurAttri, &SliderSpinBoxWidget::sigValueChanged, this, [ = ](int value, EChangedPhase phase) {
        drawBoard()->setDrawAttribution(EBlurAttri, value, phase);
    });

    connect(button_group, QOverload<QAbstractButton *, bool>::of(&QButtonGroup::buttonToggled), this, [ = ](QAbstractButton * button, bool ischeckd) {
        if (button == m_blurEffect && ischeckd)
            drawBoard()->setDrawAttribution(EEBlurStyle, BlurEffect);
        else if (button == m_masicoEffect && ischeckd)
            drawBoard()->setDrawAttribution(EEBlurStyle, MasicoEffect);
    });
}

void CutAttriRegister::registe()
{
    m_cutAttri = new CutAttributionWidget(drawBoard());
    setWgtAccesibleName(m_cutAttri, "ECutAttri");
    drawBoard()->attributionManager()->installComAttributeWgt(ECutToolAttri, m_cutAttri);
    connect(drawBoard()->attributionManager()->helper(), &AttributionManagerHelper::updateWgt, m_cutAttri,
    [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == m_cutAttri && var.isValid()) {
            QSignalBlocker bloker(m_cutAttri);
            QVariantList valuelist = var.toList();
            m_cutAttri->setCutType(valuelist[0].toInt());
            m_cutAttri->setCutSize(valuelist[1].toSize(), true);
        }
    });

}

void PenAttriRegister::registe()
{
    m_PenBrushStyle = new ColorStyleWidget(drawBoard());
    m_PenBrushStyle->setAttribution(EPenBrushColor);
    m_PenBrushStyle->setColorFill(0);
    drawBoard()->attributionManager()->installComAttributeWgt(m_PenBrushStyle->attribution(), m_PenBrushStyle, QColor(0, 0, 0));

    m_enablePenStyle = new CheckBoxSettingWgt("", m_PenBrushStyle);
    m_enablePenStyle->setAttribution(EEnablePenBrushStyle);
    drawBoard()->attributionManager()->installComAttributeWgt(m_enablePenStyle->attribution(), m_enablePenStyle, true);
    QObject::connect(m_enablePenStyle, &CheckBoxSettingWgt::checkChanged, drawBoard(), [ = ](bool value) {
        drawBoard()->setDrawAttribution(m_enablePenStyle->attribution(), value);
        m_PenBrushStyle->setContentEnable(value);
    });

    QObject::connect(m_enablePenStyle, &CheckBoxSettingWgt::checkEnable, drawBoard(), [ = ](bool value) {
        m_PenBrushStyle->setContentEnable(value);
    });

    QObject::connect(m_enablePenStyle, &CheckBoxSettingWgt::checkStatusChanged, drawBoard(), [ = ](int status) {
        m_PenBrushStyle->setContentEnable(status != Qt::CheckState::Unchecked);
    });

    QObject::connect(m_PenBrushStyle, &ColorStyleWidget::colorChanged, drawBoard(), [ = ] {
        if (m_enablePenStyle->checkBox()->isTristate())
        {
            emit m_enablePenStyle->checkChanged(true);
            m_enablePenStyle->checkBox()->setTristate(false);
            m_enablePenStyle->checkBox()->setChecked(true);
            m_enablePenStyle->checkBox()->update();
        }
    });

    m_PenBrushStyle->addTitleWidget(m_enablePenStyle, Qt::AlignRight);

    m_penStyle = new ComboBoxSettingWgt(tr("Brush"));
    m_penStyle->setAttribution(EPenStyle);
    QComboBox *m_pPenStyleComboBox = new QComboBox;

    m_pPenStyleComboBox->view()->setAlternatingRowColors(false);
    drawApp->setWidgetAccesibleName(m_pPenStyleComboBox, "Pen style combobox");

    m_pPenStyleComboBox->setIconSize(QSize(90, 30));
    m_pPenStyleComboBox->setFocusPolicy(Qt::NoFocus);

    m_pPenStyleComboBox->addItem(QIcon::fromTheme("water_color_pen"), tr("Watercolor"));
    m_pPenStyleComboBox->addItem(QIcon::fromTheme("calligraphy"), tr("Calligraphy"));
    m_pPenStyleComboBox->addItem(QIcon::fromTheme("crayon"), tr("Crayon"));

    m_penStyle->setComboBox(m_pPenStyleComboBox);

    connect(m_pPenStyleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), m_penStyle, [ = ](int index) {
        emit drawBoard()->attributionManager()->helper()->attributionChanged(EPenStyle, index + 1);
    });

    connect(drawBoard()->attributionManager()->helper(), &AttributionManagerHelper::updateWgt, m_penStyle,
    [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == m_penStyle) {
            QSignalBlocker bloker(m_pPenStyleComboBox);
            m_pPenStyleComboBox->setCurrentIndex(var.toInt() - 1);
        }
    });

    drawBoard()->attributionManager()->installComAttributeWgt(m_penStyle->attribution(), m_penStyle, 1);
    m_penStyle->installEventFilter(this);

    m_sliderPenWidth = new SliderSpinBoxWidget(EPenWidthProperty);
    m_sliderPenWidth->setRange(1, 10);
    m_sliderPenWidth->setTitle(tr("Brush size"));

    setWgtAccesibleName(m_sliderPenWidth, "penWidth");
    drawBoard()->attributionManager()->installComAttributeWgt(EPenWidthProperty, m_sliderPenWidth, 2);

    connect(m_sliderPenWidth, &SliderSpinBoxWidget::sigValueChanged, this, [ = ](int value, int phase) {
        drawBoard()->setDrawAttribution(EPenWidthProperty, value, phase);
    });

    connect(m_PenBrushStyle, &ColorStyleWidget::colorChanged, this, [ = ](const QColor & color, int phase) {
        drawBoard()->setDrawAttribution(m_PenBrushStyle->attribution(), color, phase);
    });


}


void RectAttriRegister::registe()
{
    m_rectRadius = new RectRadiusStyleWidget(drawBoard(), drawBoard());
    drawBoard()->attributionManager()->installComAttributeWgt(m_rectRadius->attribution(), m_rectRadius, 5);
    QObject::connect(m_rectRadius, &RectRadiusStyleWidget::valueChanged, this, [ = ](QVariant value, EChangedPhase phase) {
        drawBoard()->setDrawAttribution(m_rectRadius->attribution(), value, phase);
    });

}
