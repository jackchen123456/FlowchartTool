#ifndef SHAPELIBRARYWIDGET_H
#define SHAPELIBRARYWIDGET_H

#include "FlowLayout.h"
#include <QWidget>
#include <QMap>

class ShapeLibraryWidget : public QWidget {
	Q_OBJECT

public:
    explicit ShapeLibraryWidget(QWidget *parent = nullptr);

private:
    void setupButtons(FlowLayout *layout, QMap<QString, QString> &iconMap);       // 设置图形按钮

    QMap<QString, QString> m_iconShapeMap;   // 图标名称与图形类型的映射
    QMap<QString, QString> m_iconUMLMap;     // 图标名称与UML图元类型的映射

    // 带有标题的组框小部件样式
    QString groupBoxStyle =
            "QGroupBox {"
            "font: bold 10pt 'Microsoft YaHei';"
            "margin-top: 20px;"
            "padding-top: 5px;"
            "border: 1px solid rgba(210, 210, 210, 128);"
            "border-radius: 3px;"
            "}"
            "QGroupBox::title {"
            "subcontrol-origin: margin;"       // 指定子控件（这里是标题）的位置参考点
            "position: absolute;"              // 使其浮动定位
            "top: -2px;"                       // 将标题相对于其原始位置向上移动2像素
            "left: 3px;"
            "padding: 0 3px;"
            "}";

};

#endif  // SHAPELIBRARYWIDGET_H