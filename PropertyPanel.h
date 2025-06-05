#ifndef PROPERTYPANEL_H
#define PROPERTYPANEL_H

#include "DrawArea.h"
#include <QWidget>
#include <QStackedWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QColor>
#include <QCheckBox>
#include <QButtonGroup>
#include <QGroupBox>
#include <QComboBox>
#include <QColorDialog>
#include <QGridLayout>
#include <QIcon>

class PropertyPanel : public QWidget {
    Q_OBJECT

public:
    explicit PropertyPanel(QWidget *parent = nullptr);

    ~PropertyPanel() = default;

    void initWithDrawArea(DrawArea *drawArea);         // 根据绘图区域属性初始化属性面板中的页面相关属性

    void updateFromShape(const ShapeBase *shape);      // 从图形属性中恢复属性面板中的图形相关属性

signals:
    void backgroundColorChanged(const QColor &color);  // 页面背景颜色改变信号

    void borderColorChanged(const QColor &color);      // 图形边框颜色改变信号

    void fillColorChanged(const QColor &color);        // 图形填充颜色改变信号

    void borderWidthChanged(int width);                // 图形边框线条大小改变信号

    void borderStyleChanged(Qt::PenStyle style);       // 图形边框线条样式改变信号

    void fontColorChanged(const QColor &color);        // 图形文本字体颜色改变信号

    void fontSizeChanged(int size);                    // 图形文本字体大小改变信号

    void fontFamilyChanged(const QString &family);     // 图形文本字体改变信号

    void textBoldChanged(bool bold);                   // 图形文本加粗改变信号

    void textItalicChanged(bool italic);               // 图形文本斜体改变信号

    void textUnderlineChanged(bool underline);         // 图形文本下划线改变信号

    void textAlignmentChanged(Qt::Alignment alignment); // 图形文本对齐方式改变信号

    void pageOrientationChanged(bool isLandscape);     // 页面方向改变信号

    void pageSizeChanged(const QSize &size);           // 页面大小改变信号

    void pageWidthChanged(int width);                  // 页面宽度改变信号

    void pageHeightChanged(int height);                // 页面高度改变信号

public slots:
    void updatePanel(bool hasSelection);               // 更新属性面板状态

public:
    QSpinBox *widthSpinBox;                            // 页面宽度数值输入框
    QSpinBox *heightSpinBox;                           // 页面高度数值输入框
    QComboBox *pageSizeCombo;                          // 页面大小下拉框
    QCheckBox *gridCheckBox;                           // 网格可见性选择框
    QButtonGroup *orientationGroup;                    // 页面方向切换按钮组
    QComboBox *borderStyleComboBox;                    // 图形边框线条样式下拉框
    QSpinBox *borderWidthSpinBox;                      // 图形边框线条大小数值输入框
    QSpinBox *fontSizeSpinBox;                         // 图形文本字体大小数值输入框
    QComboBox *fontFamilyComboBox;                     // 图形文本字体下拉框
    QPushButton *boldButton;                           // 图形文本字体加粗按钮
    QPushButton *italicButton;                         // 图形文本字体斜体按钮
    QPushButton *underlineButton;                      // 图形文本字体下划线按钮
    QPushButton *leftAlignButton;                      // 图形文本文本左对齐按钮
    QPushButton *centerAlignButton;                    // 图形文本文本居中对齐按钮
    QPushButton *rightAlignButton;                     // 图形文本文本右对齐按钮
    QPushButton *moveTopButton;                        // 移动图形到顶部按钮
    QPushButton *moveBottomButton;                     // 移动图形到底部按钮
    QPushButton *moveUpButton;                         // 移动图形上移一层按钮
    QPushButton *moveDownButton;                       // 移动图形下移一层按钮

private slots:
    void onSelectBackgroundColor();                    // 页面背景颜色改变对应的槽函数

    void onSelectBorderColor();                        // 图形边框颜色改变对应的槽函数

    void onSelectFillColor();                          // 图形填充颜色改变对应的槽函数

    void onSelectFontColor();                          // 图形文本字体颜色改变对应的槽函数

    void onClearDefaultStyle();                        // 清除为默认样式对应的槽函数

    void onPageSizeChanged(int index);                 // 页面大小改变对应的槽函数

    void onOrientationChanged(bool isLandscape);       // 页面方向改变对应的槽函数

    void onBorderWidthChanged(int width);              // 图形边框线条大小改变对应的槽函数

    void onBorderStyleChanged(int index);              // 图形边框线条样式改变对应的槽函数

    void onFontSizeChanged(int size);                  // 图形文本字体大小改变对应的槽函数

    void onFontFamilyChanged(const QString &family);   // 图形文本字体改变对应的槽函数

    void onTextStyleButtonClicked();                   // 图形文本样式按钮点击对应的槽函数

private:
    void addColorButtonToLayout(QGridLayout *layout, QList<QColor> &colors, bool isFillColor);     // 添加颜色按钮到布局中

    void updateButtonColor(QPushButton *button, const QColor &color);           // 更新按钮颜色

    QPushButton *createColorButton(const QColor &color, bool isFillColor);      // 创建颜色按钮

    void updateRecentColors(const QColor &color);                               // 更新最近颜色

    void refreshRecentColorButtons();                                           // 更新最近颜色按钮

    QIcon createLineStyleIcon(Qt::PenStyle style, const QColor &color = Qt::gray, int size = 24);  // 创建线样式图标

    void blockAllSignals(bool block);                                           // 阻塞信号

private:
    DrawArea *m_drawArea = nullptr;                       // 绘图区域

    QPushButton *backgroundColorButton;                   // 页面背景颜色按钮
    QPushButton *borderColorButton;                       // 图形边框颜色按钮
    QPushButton *fillColorButton;                         // 图形填充颜色按钮
    QPushButton *fontColorButton;                         // 图形文本字体颜色按钮

    QStackedWidget *stackedWidget;                        // 多页面切换的容器控件
    QTabWidget *pageSettingWidget;                        // 页面属性设置
    QTabWidget *shapeTabWidget;                           // 图形属性设置

    QColor currentBackgroundColor;                        // 当前背景颜色
    // 背景系统颜色列表
    QList<QColor> bgSystemColors = {
            QColor("#EFECEB"), QColor("#F2F2F2"), QColor("#E7EBED"), QColor("#FADCDB"),
            QColor("#FBEADA"), QColor("#FCF9EA"), QColor("#E5F6DA"), QColor("#DBF5F5"),
            QColor("#DED9D7"), QColor("#D9D9D9"), QColor("#E0E0E0"), QColor("#F5B9B7"),
            QColor("#F8D5B5"), QColor("#F6EDCE"), QColor("#CAEDB4"), QColor("#B7EAEB"),
            QColor("#BEB3AF"), QColor("#BFBFBF"), QColor("#9E9E9E"), QColor("#F19594"),
            QColor("#F4CE8F"), QColor("#F1E4A2"), QColor("#B0E38F"), QColor("#94E0E1"),
            QColor("#9D8C88"), QColor("#A6A6A6"), QColor("#616161"), QColor("#EC7270"),
            QColor("#FEAC6A"), QColor("#E9D66F"), QColor("#95DA69"), QColor("#70D5D7"),
            QColor("#5C4038"), QColor("#7F7F7F"), QColor("#262626"), QColor("#A23735"),
            QColor("#A66A30"), QColor("#A7932C"), QColor("#569230"), QColor("#358E90"),
            QColor("#FFFFFF"), QColor("#7F8B98"), QColor("#000000"), QColor("#E74F4C"),
            QColor("#ED9745"), QColor("#E0C431"), QColor("#7BD144"), QColor("#4CCBCD")
    };
    QGroupBox *bgSystemColorGroup;                           // 页面背景系统颜色组
    QGroupBox *recentColorGroup;                             // 最近颜色组
    QList<QColor> recentColors;                              // 最近颜色列表

    QGroupBox *pageSizeGroup;                                // 页面大小组
    // QGroupBox样式
    const QString groupBoxStyle =
            "QGroupBox {"
            "font: bold 10pt 'Microsoft YaHei';"
            "margin-top: 20px;"
            "padding-top: 5px;"
            "border: 1px solid rgba(210, 210, 210, 128);"
            "border-radius: 3px;"
            "}"
            "QGroupBox::title {"
            "subcontrol-origin: margin;"
            "position: absolute;"
            "top: -2px;"
            "left: 3px;"
            "padding: 0 3px;"
            "}";

    QColor currentBorderColor;                              // 当前图形边框颜色
    QColor currentFillColor;                                // 当前图形填充颜色
    QColor currentFontColor;                                // 当前图形文本字体颜色
    QGroupBox *shapeFillColorGroup;                         // 图形填充系统颜色组
    // 图形填充系统颜色列表
    QList<QColor> shapeFillColors = {
            QColor("#EFECEB"), QColor("#F2F2F2"), QColor("#E7EBED"), QColor("#FADCDB"),
            QColor("#FBEADA"), QColor("#FCF9EA"), QColor("#E5F6DA"), QColor("#DBF5F5"),
            QColor("#DED9D7"), QColor("#D9D9D9"), QColor("#E0E0E0"), QColor("#F5B9B7"),
            QColor("#F8D5B5"), QColor("#F6EDCE"), QColor("#CAEDB4"), QColor("#B7EAEB"),
            QColor("#BEB3AF"), QColor("#BFBFBF"), QColor("#9E9E9E"), QColor("#F19594"),
            QColor("#F4CE8F"), QColor("#F1E4A2"), QColor("#B0E38F"), QColor("#94E0E1"),
            QColor("#9D8C88"), QColor("#A6A6A6"), QColor("#616161"), QColor("#EC7270"),
            QColor("#FEAC6A"), QColor("#E9D66F"), QColor("#95DA69"), QColor("#70D5D7"),
            QColor("#5C4038"), QColor("#7F7F7F"), QColor("#262626"), QColor("#A23735"),
            QColor("#A66A30"), QColor("#A7932C"), QColor("#569230"), QColor("#358E90"),
            QColor("#FFFFFF"), QColor("#7F8B98"), QColor("#000000"), QColor("#E74F4C"),
            QColor("#ED9745"), QColor("#E0C431"), QColor("#7BD144"), QColor("#4CCBCD")
    };

    // tab样式
    const QString tabStyle =
            "QTabBar::tab {"
            "    font-family: Microsoft YaHei;"
            "    font-size: 10pt;"
            "}"
            "QTabBar::tab:selected {"
            "    color: #0c58bb;"
            "    font-weight: bold;"
            "}"
            "QTabBar::tab:hover {"
            "    font-weight: bold;"
            "}";

};

#endif  // PROPERTYPANEL_H