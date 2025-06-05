#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DrawArea.h"
#include "ShapeLibraryWidget.h"
#include "PropertyPanel.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:
    void updateActions();                // 更新菜单栏和工具栏的部分action状态

private:
    ShapeLibraryWidget *shapeLibrary;    // 图形库
    DrawArea *drawArea;                  // 绘图区域
    PropertyPanel *propertyPanel;        // 属性面板

    QAction *newFileAction;              // 新建文件
    QAction *openFileAction;             // 打开文件
    QAction *saveFileAction;             // 保存文件
    QAction *saveAsFileAction;           // 另存为文件
    QAction *exportPngAction;            // 导出为PNG格式
    QAction *exportSvgAction;            // 导出为SVG格式

    QAction *zoomInAction;               // 放大
    QAction *zoomOutAction;              // 缩小
    QAction *zoomResetAction;            // 恢复初始大小

    QAction *undoAction;                 // 撤销
    QAction *redoAction;                 // 重做
    QAction *copyAction;                 // 复制
    QAction *cutAction;                  // 剪切
    QAction *pasteAction;                // 粘贴
    QAction *deleteAction;               // 删除
    QAction *selectAllAction;            // 全选

    QAction *moveTopAction;              // 移动到最上层
    QAction *moveBottomAction;           // 移动到最下层
    QAction *moveUpAction;               // 上移一层
    QAction *moveDownAction;             // 下移一层

    void setupMenuBar();                 // 菜单栏
    void setupToolBar();                 // 工具栏
    void setupDockWidgets();             // 浮动窗口

    // 菜单栏样式
    QString menuBarStyle =
            "QMenuBar {"
            "    background-color: #F1F3F4;"
            "    font-family: Microsoft YaHei;"
            "    font-size: 10pt;"
            "    font-weight: bold;"
            "    margin-left: 10px;"
            "}"
            "QMenuBar:selected {"
            "    color: #0c58bb;"
            "}";

};

#endif  // MAINWINDOW_H
