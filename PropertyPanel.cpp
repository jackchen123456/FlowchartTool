#include "PropertyPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>         // 下拉选择框控件
#include <QSpinBox>             // 数值输入框控件
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>
#include <QPainter>
#include <QPixmap>

PropertyPanel::PropertyPanel(QWidget *parent)
        : QWidget(parent), recentColors() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);   // 属性面板主布局
    mainLayout->setContentsMargins(5, 5, 5, 5);
    stackedWidget = new QStackedWidget(this);          // 创建堆栈控件，索引为0时展示页面属性配置面板状态，为1时展示图形属性配置面板状态

    // ==============================页面样式设置==========================
    pageSettingWidget = new QTabWidget;                // 页面属性配置Tab容器，通过标签页的方式组织和展示多个页面
    pageSettingWidget->setStyleSheet(tabStyle);
    QWidget *pageSettingTab = new QWidget;
    pageSettingWidget->addTab(pageSettingTab, tr("PageSetting"));    // 添加页面属性标签页

    QVBoxLayout *pageLayout = new QVBoxLayout(pageSettingTab);       // 页面属性配置整体为垂直布局
    pageLayout->setSpacing(10);                                      // 设置垂直布局中控件之间的间距
    pageLayout->addSpacing(5);                                       // 添加顶部间距

    // 网格复选框
    QHBoxLayout *gridLayout = new QHBoxLayout;
    gridLayout->setContentsMargins(0, 5, 0, 0);
    gridCheckBox = new QCheckBox(tr("grid"));
    gridLayout->addWidget(gridCheckBox);
    gridLayout->addStretch();
    pageLayout->addLayout(gridLayout);                               // 将网格复选框布局添加到页面属性配置布局中

    // 背景颜色区域
    QVBoxLayout *bgColorMainLayout = new QVBoxLayout;
    bgColorMainLayout->setContentsMargins(0, 5, 0, 5);

    QHBoxLayout *bgColorHeaderLayout = new QHBoxLayout;              // 背景颜色区域标题布局
    QLabel *bgColorLabel = new QLabel(tr("background color"));
    bgColorLabel->setFont(QFont("Microsoft YaHei", 12, QFont::Bold));
    bgColorHeaderLayout->addWidget(bgColorLabel);
    bgColorHeaderLayout->addStretch();
    backgroundColorButton = new QPushButton;
    backgroundColorButton->setFixedSize(50, 30);
    backgroundColorButton->setStyleSheet(QString("background-color: %1;").arg(currentBackgroundColor.name()));

    // 设置背景颜色按钮点击事件与颜色选择功能绑定
    connect(backgroundColorButton, &QPushButton::clicked, this, &PropertyPanel::onSelectBackgroundColor);

    bgColorHeaderLayout->addWidget(backgroundColorButton);
    bgColorMainLayout->addLayout(bgColorHeaderLayout);

    // 背景填充系统颜色
    bgSystemColorGroup = new QGroupBox(tr("system color"));
    QGridLayout *bgSystemColorLayout = new QGridLayout(bgSystemColorGroup);
    bgSystemColorLayout->setContentsMargins(5, 5, 5, 10);
    bgSystemColorLayout->setSpacing(5);

    addColorButtonToLayout(bgSystemColorLayout, bgSystemColors, false);     // 添加系统颜色按钮组
    bgColorMainLayout->addWidget(bgSystemColorGroup);
    bgSystemColorGroup->setStyleSheet(groupBoxStyle);

    // 最近使用
    recentColorGroup = new QGroupBox(tr("recent use"));
    QGridLayout *recentColorLayout = new QGridLayout(recentColorGroup);

    addColorButtonToLayout(recentColorLayout, recentColors, false);        // 添加最近使用颜色按钮组
    bgColorMainLayout->addWidget(recentColorGroup);
    recentColorGroup->setStyleSheet(groupBoxStyle);

    pageLayout->addLayout(bgColorMainLayout);                              // 将背景颜色区域布局添加到页面属性配置布局中

    // 添加分割线
    QFrame *pageLine = new QFrame(this);
    pageLine->setFrameShape(QFrame::HLine);             // 设置为水平线
    pageLine->setFrameShadow(QFrame::Sunken);           // 设置凹陷样式，增强立体感

    pageLayout->addWidget(pageLine);

    // 页面尺寸设置
    pageSizeGroup = new QGroupBox(tr("page size"));
    QGridLayout *sizeLayout = new QGridLayout(pageSizeGroup);          // 网格布局，可以明确指定每个控件所在的行和列
    pageSizeGroup->setStyleSheet(groupBoxStyle);
    sizeLayout->setVerticalSpacing(10);
    sizeLayout->setHorizontalSpacing(5);

    QLabel *widthLabel = new QLabel(tr("W:"));
    widthLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);    // 水平方向设置最小宽度，垂直方向优先采用推荐尺寸
    widthLabel->setMinimumWidth(15);
    widthLabel->setFont(QFont("Microsoft YaHei", 11));

    widthSpinBox = new QSpinBox;
    widthSpinBox->setRange(100, 10000);                      // 设置尺寸范围
    widthSpinBox->setSuffix("px");                           // 设置单位
    widthSpinBox->setFixedSize(75, 30);                      // 设置输入框大小

    // 高度设置
    QLabel *heightLabel = new QLabel(tr("H:"));
    heightLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    heightLabel->setMinimumWidth(15);
    heightLabel->setFont(QFont("Microsoft YaHei", 11));

    heightSpinBox = new QSpinBox;
    heightSpinBox->setRange(100, 10000);
    heightSpinBox->setSuffix("px");
    heightSpinBox->setFixedSize(75, 30);

    sizeLayout->addWidget(widthLabel, 0, 0, Qt::AlignRight);        // 宽度标签占位sizeLayout第一行第一列，并设置为右对齐方式
    sizeLayout->addWidget(widthSpinBox, 0, 1);                      // 宽度输入框占位sizeLayout第一行第二列
    sizeLayout->addWidget(heightLabel, 0, 2, Qt::AlignRight);       // 高度标签占位sizeLayout第一行第三列，并设置为右对齐方式
    sizeLayout->addWidget(heightSpinBox, 0, 3);                     // 高度输入框占位sizeLayout第一行第四列

    connect(widthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
            [this](int w) {
                QSize newSize(w, heightSpinBox->value());
                emit pageSizeChanged(newSize);
            });
    connect(heightSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
            [this](int h) {
                QSize newSize(widthSpinBox->value(), h);
                emit pageSizeChanged(newSize);
            });

    // 预设尺寸选择
    pageSizeCombo = new QComboBox;
    pageSizeCombo->setFixedHeight(30);
    pageSizeCombo->addItem(tr("A4 Equivalent (1050 px x 1500 px)"), QSize(1050, 1500));        // 添加预设尺寸A4选项
    pageSizeCombo->addItem(tr("A3 Equivalent (1500 px x 2100 px)"), QSize(1500, 2100));        // 添加预设尺寸A3选项
    pageSizeCombo->addItem(tr("A5 Equivalent (750 px x 1050 px)"), QSize(750, 1050));          // 添加预设尺寸A5选项
    sizeLayout->addWidget(pageSizeCombo, 1, 0, 1, 4);                // 整个pageSizeCombo占位sizeLayout的第二行四列

    connect(pageSizeCombo, QOverload<int>::of(&QComboBox::activated), this, &PropertyPanel::onPageSizeChanged);

    // 方向选择
    QHBoxLayout *orientationLayout = new QHBoxLayout;
    orientationGroup = new QButtonGroup(pageSizeGroup);
    QRadioButton *portraitBtn = new QRadioButton(tr("Vertical"));             // 竖向单选按钮
    QRadioButton *landscapeBtn = new QRadioButton(tr("Horizontal"));          // 横向单选按钮
    orientationGroup->addButton(portraitBtn, 0);
    orientationGroup->addButton(landscapeBtn, 1);
    portraitBtn->setChecked(true);

    orientationLayout->addWidget(portraitBtn);
    orientationLayout->addWidget(landscapeBtn);
    sizeLayout->addLayout(orientationLayout, 2, 0, 1, 4);                     // 整个orientationLayout占位sizeLayout的第三行四列

    connect(orientationGroup, &QButtonGroup::idClicked, [this](int id) {
        onOrientationChanged(id == 1);
    });

    pageLayout->addWidget(pageSizeGroup);

    // 清除默认风格
    QPushButton *clearStyleButton = new QPushButton(tr("clear default style"));
    clearStyleButton->setFixedHeight(35);
    clearStyleButton->setFont(QFont("Microsoft YaHei", 11, QFont::Bold));

    connect(clearStyleButton, &QPushButton::clicked, this, &PropertyPanel::onClearDefaultStyle);
    pageLayout->addWidget(clearStyleButton);

    pageLayout->addStretch();                               // 让空白区域自动扩展，填满布局中未被控件占用的空间

    // ==============================图形样式设置==========================
    shapeTabWidget = new QTabWidget(this);                  // 创建图形属性配置Tab容器
    shapeTabWidget->setStyleSheet(tabStyle);
    QWidget *styleTab = new QWidget;                        // 创建图形样式设置Tab页
    QWidget *textTab = new QWidget;                         // 创建文本样式设置Tab页
    QWidget *adjustTab = new QWidget;                       // 创建调整图形设置Tab页

    // ----------------------样式设置--------------------
    shapeTabWidget->addTab(styleTab, tr("Style"));
    QVBoxLayout *styleLayout = new QVBoxLayout(styleTab);
    styleLayout->setSpacing(5);
    styleLayout->addSpacing(5);
    styleLayout->setContentsMargins(10, 5, 10, 5);

    // 图形填充颜色区域
    QVBoxLayout *fillColorMainLayout = new QVBoxLayout;
    fillColorMainLayout->setContentsMargins(0, 5, 0, 5);

    QHBoxLayout *fillColorLayout = new QHBoxLayout;
    QLabel *fillColorLabel = new QLabel(tr("Fill Color"));
    fillColorLabel->setFont(QFont("Microsoft YaHei", 12, QFont::Bold));
    fillColorLayout->addWidget(fillColorLabel);
    fillColorLayout->addStretch();
    fillColorButton = new QPushButton;
    fillColorButton->setFixedSize(50, 30);
    fillColorButton->setStyleSheet(QString("background-color: %1;").arg(currentFillColor.name()));
    connect(fillColorButton, &QPushButton::clicked, this, &PropertyPanel::onSelectFillColor);
    fillColorLayout->addWidget(fillColorButton);

    fillColorMainLayout->addLayout(fillColorLayout);

    // 图形填充系统颜色
    shapeFillColorGroup = new QGroupBox(tr(""));
    QGridLayout *shapeFillColorLayout = new QGridLayout(shapeFillColorGroup);
    shapeFillColorLayout->setContentsMargins(5, 5, 5, 10);
    shapeFillColorLayout->setSpacing(5);
    addColorButtonToLayout(shapeFillColorLayout, shapeFillColors, true);
    fillColorMainLayout->addWidget(shapeFillColorGroup);
    shapeFillColorGroup->setStyleSheet(groupBoxStyle);

    styleLayout->addLayout(fillColorMainLayout);

    // 边框设置
    QVBoxLayout *borderStyleLayout = new QVBoxLayout;
    borderStyleLayout->setContentsMargins(0, 5, 0, 5);
    borderStyleLayout->setSpacing(5);

    // 边框颜色
    QHBoxLayout *borderColorLayout = new QHBoxLayout;
    QLabel *borderColorLabel = new QLabel(tr("Border Color"));
    borderColorLabel->setFont(QFont("Microsoft YaHei", 12, QFont::Bold));
    borderColorLayout->addWidget(borderColorLabel);
    borderColorLayout->addStretch();
    borderColorButton = new QPushButton;
    borderColorButton->setFixedSize(50, 30);
    borderColorButton->setStyleSheet(QString("background-color: %1;").arg(currentBorderColor.name()));
    connect(borderColorButton, &QPushButton::clicked, this, &PropertyPanel::onSelectBorderColor);
    borderColorLayout->addWidget(borderColorButton);

    borderStyleLayout->addLayout(borderColorLayout);
    styleLayout->addLayout(borderStyleLayout);

    // 使用哪种线绘制边框
    QHBoxLayout *borderLineLayout = new QHBoxLayout;
    borderStyleComboBox = new QComboBox;
    borderStyleComboBox->setFixedSize(170, 30);

    // 添加实线项
    borderStyleComboBox->addItem(createLineStyleIcon(Qt::SolidLine), tr("SolidLine"),
                                 QVariant::fromValue(Qt::SolidLine));
    //  添加虚线项
    borderStyleComboBox->addItem(createLineStyleIcon(Qt::DashLine), tr("DashLine"), QVariant::fromValue(Qt::DashLine));
    // 添加点线项
    borderStyleComboBox->addItem(createLineStyleIcon(Qt::DotLine), tr("DotLine"), QVariant::fromValue(Qt::DotLine));
    // 添加虚点线项
    borderStyleComboBox->addItem(createLineStyleIcon(Qt::DashDotLine), tr("DashDotLine"),
                                 QVariant::fromValue(Qt::DashDotLine));
    // 添加虚线双点线项
    borderStyleComboBox->addItem(createLineStyleIcon(Qt::DashDotDotLine), tr("DashDotDotLine"),
                                 QVariant::fromValue(Qt::DashDotDotLine));

    borderWidthSpinBox = new QSpinBox;                   // 边框线条大小输入框
    borderWidthSpinBox->setRange(1, 10);
    borderWidthSpinBox->setSuffix("px");
    borderWidthSpinBox->setFixedSize(70, 30);
    borderWidthSpinBox->setValue(2);

    borderLineLayout->addWidget(borderStyleComboBox);
    borderLineLayout->addWidget(borderWidthSpinBox);
    styleLayout->addLayout(borderLineLayout);
    styleLayout->addStretch();

    connect(borderStyleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PropertyPanel::onBorderStyleChanged);
    connect(borderWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &PropertyPanel::onBorderWidthChanged);

    // ----------------------文本设置--------------------
    shapeTabWidget->addTab(textTab, tr("Text"));
    QVBoxLayout *textLayout = new QVBoxLayout(textTab);
    textLayout->setSpacing(10);
    textLayout->addSpacing(10);
    textLayout->setContentsMargins(10, 5, 10, 5);

    // 字体大小
    QHBoxLayout *fontSizeLayout = new QHBoxLayout;
    QLabel *fontSizeLabel = new QLabel(tr("Font"));
    fontSizeLabel->setFont(QFont("Microsoft YaHei", 12, QFont::Bold));
    fontSizeLayout->addWidget(fontSizeLabel);
    fontSizeLayout->addStretch();

    fontSizeSpinBox = new QSpinBox;
    fontSizeSpinBox->setRange(2, 60);
    fontSizeSpinBox->setSuffix("px");
    fontSizeSpinBox->setFixedSize(80, 30);
    fontSizeSpinBox->setValue(9);
    fontSizeLayout->addWidget(fontSizeSpinBox);

    connect(fontSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &PropertyPanel::onFontSizeChanged);
    textLayout->addLayout(fontSizeLayout);

    // 字体
    QHBoxLayout *fontFamilyLayout = new QHBoxLayout;
    fontFamilyComboBox = new QComboBox;
    fontFamilyComboBox->setFixedHeight(35);
    fontFamilyComboBox->setFont(QFont("Microsoft YaHei", 10));
    fontFamilyComboBox->addItem("Arial");
    fontFamilyComboBox->addItem("Times New Roman");
    fontFamilyComboBox->addItem("Courier New");
    fontFamilyComboBox->addItem("Verdana");
    fontFamilyComboBox->addItem("Tahoma");
    fontFamilyComboBox->addItem("Georgia");
    fontFamilyComboBox->addItem("Microsoft YaHei");
    fontFamilyLayout->addWidget(fontFamilyComboBox);

    connect(fontFamilyComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int index) {
                QString fontFamily = fontFamilyComboBox->itemText(index);
                emit fontFamilyChanged(fontFamily);
            });
    textLayout->addLayout(fontFamilyLayout);

    // 字体样式
    QHBoxLayout *textStyleButtonLayout = new QHBoxLayout;
    textStyleButtonLayout->setSpacing(5);
    boldButton = new QPushButton;
    italicButton = new QPushButton;
    underlineButton = new QPushButton;
    leftAlignButton = new QPushButton;
    centerAlignButton = new QPushButton;
    rightAlignButton = new QPushButton;

    boldButton->setIcon(QIcon(":/images/bold.png"));
    boldButton->setIconSize(QSize(18, 18));            // 图标大小
    boldButton->setFixedSize(36, 36);                  // 按钮大小
    boldButton->setCheckable(true);                    // 设置按钮为可选中状态
    boldButton->setStyleSheet("QPushButton { border: 1px solid; padding: 1px; }"
                              "QPushButton:checked { background-color: #D0D0D0; }");

    italicButton->setIcon(QIcon(":/images/italic.png"));
    italicButton->setIconSize(QSize(18, 18));
    italicButton->setFixedSize(36, 36);
    italicButton->setCheckable(true);
    italicButton->setStyleSheet("QPushButton { border: 1px solid; padding: 1px; }"
                                "QPushButton:checked { background-color: #D0D0D0; }");

    underlineButton->setIcon(QIcon(":/images/underline.png"));
    underlineButton->setIconSize(QSize(18, 18));
    underlineButton->setFixedSize(36, 36);
    underlineButton->setCheckable(true);
    underlineButton->setStyleSheet("QPushButton { border: 1px solid; padding: 1px; }"
                                   "QPushButton:checked { background-color: #D0D0D0; }");

    leftAlignButton->setIcon(QIcon(":/images/left.png"));
    leftAlignButton->setIconSize(QSize(18, 18));
    leftAlignButton->setFixedSize(36, 36);
    leftAlignButton->setCheckable(true);
    leftAlignButton->setStyleSheet("QPushButton { border: 1px solid; padding: 1px; }"
                                   "QPushButton:checked { background-color: #D0D0D0; }");

    centerAlignButton->setIcon(QIcon(":/images/center.png"));
    centerAlignButton->setIconSize(QSize(18, 18));
    centerAlignButton->setFixedSize(36, 36);
    centerAlignButton->setCheckable(true);
    centerAlignButton->setStyleSheet("QPushButton { border: 1px solid; padding: 1px; }"
                                     "QPushButton:checked { background-color: #D0D0D0; }");

    rightAlignButton->setIcon(QIcon(":/images/right.png"));
    rightAlignButton->setIconSize(QSize(18, 18));
    rightAlignButton->setFixedSize(36, 36);
    rightAlignButton->setCheckable(true);
    rightAlignButton->setStyleSheet("QPushButton { border: 1px solid; padding: 1px; }"
                                    "QPushButton:checked { background-color: #D0D0D0; }");

    textStyleButtonLayout->addWidget(boldButton);
    textStyleButtonLayout->addWidget(italicButton);
    textStyleButtonLayout->addWidget(underlineButton);
    textStyleButtonLayout->addWidget(leftAlignButton);
    textStyleButtonLayout->addWidget(centerAlignButton);
    textStyleButtonLayout->addWidget(rightAlignButton);

    connect(boldButton, &QPushButton::clicked, this, &PropertyPanel::onTextStyleButtonClicked);
    connect(italicButton, &QPushButton::clicked, this, &PropertyPanel::onTextStyleButtonClicked);
    connect(underlineButton, &QPushButton::clicked, this, &PropertyPanel::onTextStyleButtonClicked);
    connect(leftAlignButton, &QPushButton::clicked, this, &PropertyPanel::onTextStyleButtonClicked);
    connect(centerAlignButton, &QPushButton::clicked, this, &PropertyPanel::onTextStyleButtonClicked);
    connect(rightAlignButton, &QPushButton::clicked, this, &PropertyPanel::onTextStyleButtonClicked);

    textLayout->addLayout(textStyleButtonLayout);

    // 字体颜色
    QHBoxLayout *textColorLayout = new QHBoxLayout;
    QLabel *textColorLabel = new QLabel(tr("Font Color"));
    textColorLabel->setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
    textColorLayout->addWidget(textColorLabel);
    textColorLayout->addStretch();
    fontColorButton = new QPushButton;
    fontColorButton->setFixedSize(50, 30);
    fontColorButton->setStyleSheet(QString("background-color: %1;").arg(currentFontColor.name()));
    textColorLayout->addWidget(fontColorButton);

    connect(fontColorButton, &QPushButton::clicked, this, &PropertyPanel::onSelectFontColor);
    textLayout->addLayout(textColorLayout);

    // 添加分割线
    QFrame *textLine = new QFrame(this);
    textLine->setFrameShape(QFrame::HLine);
    textLine->setFrameShadow(QFrame::Sunken);
    textLayout->addWidget(textLine);

    textLayout->addStretch();

    //  ----------------------调整图形设置--------------------
    shapeTabWidget->addTab(adjustTab, tr("Adjust"));
    QVBoxLayout *adjustLayout = new QVBoxLayout(adjustTab);
    adjustLayout->setSpacing(5);
    adjustLayout->addSpacing(5);
    adjustLayout->setContentsMargins(10, 5, 10, 5);

    QHBoxLayout *moveMoreLayout = new QHBoxLayout;
    moveTopButton = new QPushButton(tr("Move Top"));
    moveTopButton->setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
    moveTopButton->setFixedSize(120, 30);
    moveBottomButton = new QPushButton(tr("Move Bottom"));
    moveBottomButton->setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
    moveBottomButton->setFixedSize(120, 30);
    moveMoreLayout->addWidget(moveTopButton);
    moveMoreLayout->addWidget(moveBottomButton);

    adjustLayout->addLayout(moveMoreLayout);

    QHBoxLayout *moveOneLayout = new QHBoxLayout;
    moveUpButton = new QPushButton(tr("Move Up"));
    moveUpButton->setFixedSize(120, 30);
    moveUpButton->setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
    moveDownButton = new QPushButton(tr("Move Down"));
    moveDownButton->setFixedSize(120, 30);
    moveDownButton->setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
    moveOneLayout->addWidget(moveUpButton);
    moveOneLayout->addWidget(moveDownButton);

    adjustLayout->addLayout(moveOneLayout);
    adjustLayout->addStretch();

    stackedWidget->addWidget(pageSettingWidget);                   // 将页面属性设置Tab容器添加到stackedWidget中
    stackedWidget->addWidget(shapeTabWidget);                      // 将图形属性设置Tab容器添加到stackedWidget中
    mainLayout->addWidget(stackedWidget);                          // 将stackedWidget添加到mainLayout布局中
    setLayout(mainLayout);                                         // 设置当前窗口的内容布局

    stackedWidget->setCurrentIndex(0);                             // 设置stackedWidget的初始索引
}

void PropertyPanel::initWithDrawArea(DrawArea *drawArea) {
    m_drawArea = drawArea;                                             // 设置当前画布
    currentBackgroundColor = drawArea->getCurrentBackgroundColor();    // 设置当前画布的背景色
    updateButtonColor(backgroundColorButton, currentBackgroundColor);

    QSize pxSize = drawArea->getPageSizeInPixels();
    widthSpinBox->setValue(pxSize.width());
    heightSpinBox->setValue(pxSize.height());

    // 如果当前背景色不在系统颜色列表中，则将其添加到最近使用列表中
    if (!bgSystemColors.contains(currentBackgroundColor)) {
        updateRecentColors(currentBackgroundColor);
    }
}

void PropertyPanel::onSelectBackgroundColor() {
    QColorDialog dialog(this);
    dialog.setWindowTitle(tr("select background color"));
    dialog.setCurrentColor(currentBackgroundColor);

    // 设置可以选择颜色的透明度，并使用Qt的标准颜色对话框
    dialog.setOptions(QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);

    if (dialog.exec() == QDialog::Accepted) {
        currentBackgroundColor = dialog.currentColor();
        updateButtonColor(backgroundColorButton, currentBackgroundColor);    // 更新按钮颜色
        updateRecentColors(currentBackgroundColor);                          // 更新最近使用颜色
        emit backgroundColorChanged(currentBackgroundColor);                 // 发送背景颜色变化信号
    }
}

void PropertyPanel::onSelectBorderColor() {
    QColorDialog dialog;
    dialog.setWindowTitle(tr("Select Border Color"));
    dialog.setCurrentColor(currentBorderColor);

    dialog.setOptions(QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);
    if (dialog.exec() == QDialog::Accepted) {
        currentBorderColor = dialog.currentColor();
        updateButtonColor(borderColorButton, currentBorderColor);
        emit borderColorChanged(currentBorderColor);
    }
}

void PropertyPanel::onSelectFillColor() {
    QColorDialog dialog;
    dialog.setWindowTitle(tr("Select Fill Color"));
    dialog.setCurrentColor(currentFillColor);

    dialog.setOptions(QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);
    if (dialog.exec() == QDialog::Accepted) {
        currentFillColor = dialog.currentColor();
        updateButtonColor(fillColorButton, currentFillColor);
        emit fillColorChanged(currentFillColor);
    }
}

void PropertyPanel::onSelectFontColor() {
    QColorDialog dialog;
    dialog.setWindowTitle(tr("Select Font Color"));
    dialog.setCurrentColor(currentFontColor);

    dialog.setOptions(QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);
    if (dialog.exec() == QDialog::Accepted) {
        currentFontColor = dialog.currentColor();
        updateButtonColor(fontColorButton, currentFontColor);
        emit fontColorChanged(currentFontColor);
    }
}

QPushButton *PropertyPanel::createColorButton(const QColor &color, bool isFillColor) {
    QPushButton *btn = new QPushButton;
    btn->setFixedSize(20, 20);             // 设置按钮大小
    btn->setStyleSheet(QString("background-color: %1; border: 1px solid gray;").arg(color.name()));

    if (isFillColor) {
        // 图形填充颜色
        connect(btn, &QPushButton::clicked, [this, color]() {
            currentFillColor = color;
            updateButtonColor(fillColorButton, color);
            emit fillColorChanged(color);
        });
    } else {
        // 背景颜色
        connect(btn, &QPushButton::clicked, [this, color]() {
            currentBackgroundColor = color;
            updateButtonColor(backgroundColorButton, color);
            updateRecentColors(color);
            emit backgroundColorChanged(color);
        });
    }
    return btn;
}

void PropertyPanel::onClearDefaultStyle() {
    gridCheckBox->setChecked(false);               // 关闭网格
    QSize defaultSize(1050, 1500);                 // 默认页面大小

    pageSizeCombo->setCurrentIndex(0);
    orientationGroup->button(0)->setChecked(true); // 设置页面方向为纵向
    widthSpinBox->setValue(defaultSize.width());
    heightSpinBox->setValue(defaultSize.height());
    currentBackgroundColor = Qt::white;
    updateButtonColor(backgroundColorButton, currentBackgroundColor);      // 更新背景颜色按钮

    emit backgroundColorChanged(currentBackgroundColor);
    emit pageSizeChanged(defaultSize);
    emit pageOrientationChanged(false);
}

void PropertyPanel::onPageSizeChanged(int index) {
    QSize size = pageSizeCombo->itemData(index).toSize();      // 从下拉选择框中获取页面大小
    orientationGroup->button(0)->setChecked(true);
    widthSpinBox->blockSignals(true);
    widthSpinBox->setValue(size.width());
    widthSpinBox->blockSignals(false);
    heightSpinBox->blockSignals(true);
    heightSpinBox->setValue(size.height());
    heightSpinBox->blockSignals(false);

    emit pageSizeChanged(size);
    emit pageOrientationChanged(false);
}

void PropertyPanel::onOrientationChanged(bool isLandscape) {
    emit pageOrientationChanged(isLandscape);
}

void PropertyPanel::addColorButtonToLayout(QGridLayout *layout, QList<QColor> &colors, bool isFillColor) {
    // 将一组颜色按钮添加到网格布局
    for (int i = 0; i < colors.size(); ++i) {
        QPushButton *colorBtn = createColorButton(colors[i], isFillColor);
        layout->addWidget(colorBtn, i / 8, i % 8);           // 一行显示8个颜色按钮
    }
}

void PropertyPanel::updateButtonColor(QPushButton *button, const QColor &color) {
    // 文字颜色自动适配,深色背景用白色文字，浅色背景用黑色文字
    button->setStyleSheet(QString("background-color: %1; color: %2; border: 1px solid gray;")
                                  .arg(color.name())
                                  .arg(color.lightness() > 128 ? "black" : "white"));
}

void PropertyPanel::updateRecentColors(const QColor &color) {
    if (!color.isValid()) return;

    recentColors.removeAll(color);        // 先移除已存在的颜色
    recentColors.prepend(color);          // 将新颜色添加到列表开头

    // 如果列表长度超过8，移除最后一个颜色
    if (recentColors.size() > 8) {
        recentColors.removeLast();
    }

    refreshRecentColorButtons();          // 更新最近使用颜色按钮
}

void PropertyPanel::refreshRecentColorButtons() {
    // 确保容器和布局存在
    if (!recentColorGroup || !recentColorGroup->layout()) return;

    // 清除现有的最近使用颜色按钮
    QLayoutItem *child;
    while ((child = recentColorGroup->layout()->takeAt(0)) != nullptr) {  // 循环删除布局项
        if (child->widget()) {
            child->widget()->deleteLater();       // 避免立即删除可能正在使用的控件
        }
        delete child;                             // 删除布局项
    }

    //  如果列表为空，隐藏最近使用颜色组并返回
    if (recentColors.isEmpty()) {
        recentColorGroup->hide();
        return;
    }

    // 显示最近使用颜色组并更新布局
    recentColorGroup->show();
    QGridLayout *recentColorLayout = qobject_cast<QGridLayout *>(recentColorGroup->layout());
    if (!recentColorLayout) return;

    addColorButtonToLayout(recentColorLayout, recentColors, false);   // 重新在最近使用颜色列表中添加所有最近使用颜色按钮

    recentColorLayout->setColumnStretch(8, 1);          // 设置第8列可伸缩，以吸收多余的宽度，让颜色按钮靠左对齐
}

void PropertyPanel::updatePanel(bool hasSelection) {
    if (hasSelection) {         // 如果有选中的图形，stackedWidget的当前索引设置为1，显示图形属性配置面板
        stackedWidget->setCurrentIndex(1);
        ShapeBase *shape = m_drawArea->getSelectedShape();
        if (shape) {
            blockAllSignals(true);
            updateFromShape(shape);
            blockAllSignals(false);
        }
    } else {                   // 如果没有选中的图形，stackedWidget的当前索引设置为0，显示页面属性配置面板
        stackedWidget->setCurrentIndex(0);
    }
}

void PropertyPanel::updateFromShape(const ShapeBase *shape) {
    if (!shape) return;

    // 更新边框属性和图形填充颜色
    borderWidthSpinBox->setValue(shape->getPenWidth());
    borderColorButton->setStyleSheet(QString("background-color: %1;").arg(shape->getBorderColor().name()));
    borderStyleComboBox->setCurrentIndex(borderStyleComboBox->findData(QVariant::fromValue(shape->getBorderStyle())));
    fillColorButton->setStyleSheet(QString("background-color: %1;").arg(shape->getFillColor().name()));

    // 更新文本属性
    fontColorButton->setStyleSheet(QString("background-color: %1;").arg(shape->getFontColor().name()));
    fontSizeSpinBox->setValue(shape->getFontSize());
    fontFamilyComboBox->setCurrentIndex(fontFamilyComboBox->findText(shape->getFontFamily()));
    boldButton->setChecked(shape->isFontBold());
    italicButton->setChecked(shape->isFontItalic());
    underlineButton->setChecked(shape->isFontUnderline());

    QAbstractButton *alignButton = nullptr;             // 使用基类按钮指针提高代码通用性，初始化为nullptr避免野指针风险
    switch (shape->getTextAlignment()) {                // 获取当前文本对齐方式
        case Qt::AlignLeft:
            alignButton = leftAlignButton;
            break;
        case Qt::AlignRight:
            alignButton = rightAlignButton;
            break;
        case Qt::AlignCenter:
            alignButton = centerAlignButton;
            break;
        default:
            alignButton = centerAlignButton;
    }
    if (alignButton) alignButton->setChecked(true);     // 将匹配到的按钮设为选中状态
}

void PropertyPanel::blockAllSignals(bool block) {
    borderWidthSpinBox->blockSignals(block);
    borderColorButton->blockSignals(block);
    borderStyleComboBox->blockSignals(block);
    fillColorButton->blockSignals(block);
    fontColorButton->blockSignals(block);
    fontSizeSpinBox->blockSignals(block);
    fontFamilyComboBox->blockSignals(block);
    boldButton->blockSignals(block);
    italicButton->blockSignals(block);
    underlineButton->blockSignals(block);
    leftAlignButton->blockSignals(block);
    centerAlignButton->blockSignals(block);
    rightAlignButton->blockSignals(block);
}

void PropertyPanel::onBorderWidthChanged(int width) {
    emit borderWidthChanged(width);
}

void PropertyPanel::onBorderStyleChanged(int index) {
    Qt::PenStyle style = static_cast<Qt::PenStyle>(borderStyleComboBox->itemData(index).toInt());
    emit borderStyleChanged(style);
}

void PropertyPanel::onFontSizeChanged(int size) {
    emit fontSizeChanged(size);
}

void PropertyPanel::onFontFamilyChanged(const QString &family) {
    emit fontFamilyChanged(family);
}

void PropertyPanel::onTextStyleButtonClicked() {
    QPushButton *button = qobject_cast<QPushButton *>(sender());     // 获取发出当前信号的对象指针
    if (!button) return;

    if (button == boldButton) {
        emit textBoldChanged(button->isChecked());
    } else if (button == italicButton) {
        emit textItalicChanged(button->isChecked());
    } else if (button == underlineButton) {
        emit textUnderlineChanged(button->isChecked());
    } else if (button == leftAlignButton) {
        leftAlignButton->setChecked(true);
        centerAlignButton->setChecked(false);
        rightAlignButton->setChecked(false);
        emit textAlignmentChanged(Qt::AlignLeft | Qt::AlignVCenter);
    } else if (button == centerAlignButton) {
        leftAlignButton->setChecked(false);
        centerAlignButton->setChecked(true);
        rightAlignButton->setChecked(false);
        emit textAlignmentChanged(Qt::AlignCenter);
    } else if (button == rightAlignButton) {
        leftAlignButton->setChecked(false);
        centerAlignButton->setChecked(false);
        rightAlignButton->setChecked(true);
        emit textAlignmentChanged(Qt::AlignRight | Qt::AlignVCenter);
    }
}

QIcon PropertyPanel::createLineStyleIcon(Qt::PenStyle style, const QColor &color, int size) {
    QPixmap pixmap(size, 16);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    QPen pen;
    pen.setStyle(style);
    pen.setColor(color);
    pen.setWidth(2);
    painter.setPen(pen);

    painter.drawLine(5, 8, size + 5, 8);
    painter.end();

    return QIcon(pixmap);
}