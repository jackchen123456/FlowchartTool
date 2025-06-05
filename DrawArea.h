#ifndef DRAWAREA_H
#define DRAWAREA_H

#include "LineBaseShape.h"
#include "MyTextEdit.h"
#include <QWidget>
#include <QPointF>
#include <vector>
#include <QMenu>
#include <QAction>
#include <memory>
#include <stack>
#include <QFileDialog>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QSvgGenerator>
#include <QSvgRenderer>

// 存储所有图形状态
struct ShapeState {
    std::vector<ShapeBase *> shapes;                       //  图形数组
    QUuid selectedShapeId;                                 //  当前选中的图形Uuid

    ShapeState() = default;

    ShapeState(const ShapeState &) = delete;               // 禁止拷贝构造

    ShapeState &operator=(const ShapeState &) = delete;    // 禁止赋值构造

    ~ShapeState() {
        for (auto shape: shapes) {
            delete shape;
        }
        shapes.clear();
    }
};

class DrawArea : public QWidget {
    Q_OBJECT

public:
    DrawArea(QWidget *parent = nullptr);

    ~DrawArea();

    QColor getCurrentBackgroundColor() const { return currentBackgroundColor; }    // 获取当前页面背景颜色

    void setCurrentBackgroundColor(const QColor &color);        // 设置当前页面背景颜色

    void setPageSize(const QSize &size);                        // 设置页面大小

    QSize getPageSizeInPixels() const;                          // 获取页面大小（像素单位）

    void setPageOrientation(bool isLandscape);                  // 设置页面方向

    void setGridVisible(bool visible);                          // 设置网格可见性

    bool getGridVisible() const { return m_gridVisible; }       // 获取网格可见性

    ShapeBase *getSelectedShape() const { return selectedShape; }                  // 获取当前选中的图形

    const std::vector<ShapeBase *> &getAllShapes() const { return shapes; }        // 获取所有图形

    bool newFile();                                             // 新建文件

    bool openFile();                                            // 打开文件

    bool saveFile();                                            // 保存文件

    bool saveAsFile();                                          // 另存为文件

    bool exportToPng();                                         // 导出为PNG格式

    bool exportToSvg();                                         // 导出为SVG格式

    bool loadFromSvg(const QString &filePath);                  // 从SVG文件中加载图形

    bool canUndo() const { return !undoStack.empty(); }         // 是否可撤销

    bool canRedo() const { return !redoStack.empty(); }         // 是否可重做

    bool canCopy() const { return selectedShape != nullptr; }   // 是否可复制

    bool canPaste() const { return clipboardShape != nullptr; } // 是否可粘贴

    bool canDuplicate() const { return selectedShape != nullptr; } // 是否可复用

    bool canCut() const { return selectedShape != nullptr; }    // 是否可剪切

    bool canDelete() const;                                     // 是否可删除

    bool canMoveTop() const;                                    // 是否可置顶

    bool canMoveBottom() const;                                 // 是否可置底

    bool canMoveUp() const;                                     // 是否可上移

    bool canMoveDown() const;                                   // 是否可下移

signals:
    void selectionChanged(bool hasSelection);                   // 图形选中状态改变信号

    void pageSizeChanged(const QSize &size);                    // 页面大小改变信号

    void pageOrientationChanged(bool isLandscape);              // 页面方向改变信号

    void shapeOrderChanged();                                   // 图形顺序改变信号

    void gridVisibilityChanged(bool visible);                   // 网格可见性改变信号

    void fileChanged(const QString &filePath);                  // 文件改变信号

    void undoStateChanged();                                    // 撤销状态改变信号

    void redoStateChanged();                                    // 重做状态改变信号

    void deleteSelectedShapeChanged();                          // 删除选中图形并且图形选中状态改变信号

public slots:
    void moveSelectedShapeToTop();                              // 移动选中图形到顶层对应的槽函数

    void moveSelectedShapeToBottom();                           // 移动选中图形到底层对应的槽函数

    void moveSelectedShapeUp();                                 // 移动选中图形上移一层对应的槽函数

    void moveSelectedShapeDown();                               // 移动选中图形下移一层对应的槽函数

    void undo();                                                // 撤销对应的槽函数

    void redo();                                                // 重做对应的槽函数

    void selectAll();                                           // 全选对应的槽函数

    void copySelectedShape();                                   // 复制选中图形对应的槽函数

    void cutSelectedShape();                                    // 剪切选中图形对应的槽函数

    void pasteShape(const QPointF &pos);                        // 粘贴剪切板中的图形对应的槽函数

    void duplicateSelectedShape();                              // 复用选中图形对应的槽函数

    void deleteSelectedShape();                                 // 删除选中图形对应的槽函数

protected:
    void paintEvent(QPaintEvent *event) override;                  // 绘制事件

    void mousePressEvent(QMouseEvent *event) override;             // 鼠标按下事件

    void mouseMoveEvent(QMouseEvent *event) override;              // 鼠标移动事件

    void mouseReleaseEvent(QMouseEvent *event) override;           // 鼠标释放事件

    void dragEnterEvent(QDragEnterEvent *event) override;          // 拖拽进入事件

    void dropEvent(QDropEvent *event) override;                    // 拖拽事件

    void mouseDoubleClickEvent(QMouseEvent *event) override;       // 鼠标双击事件

    void contextMenuEvent(QContextMenuEvent *event) override;      // 右键菜单事件

    void keyPressEvent(QKeyEvent *event) override;                 // 键盘按下事件

    void resizeEvent(QResizeEvent *event) override;                // 窗口大小改变事件

    void enterEvent(QEvent *event) override;                       // 鼠标进入控件事件

    void leaveEvent(QEvent *event) override;                       // 鼠标离开控件事件

private:
    void drawGrid(QPainter &painter, const QRect &pageRect);       // 绘制网格

    void drawRotationButton(QPainter &painter, ShapeBase *shape);  // 绘制旋转按钮

    bool isInRotateButton(const ShapeBase *shape, const QPointF &pos) const;    // 判断是否在旋转按钮内

    void drawMagneticPoints(QPainter &painter);           // 绘制磁力点

    void updateAllLineBindings();                         // 更新所有线段端点与图形的绑定关系

    void clearSelection();                                // 清除图形选择状态

    void emitSelectionChanged();                          // 触发选择状态改变

    void saveToUndoStack();                               // 保存撤销操作前的状态

    void restoreFromUndoStack();                          // 恢复撤销操作状态

    void restoreFromRedoStack();                          // 恢复重做操作状态

    std::unique_ptr<ShapeState> createCurrentState() const;  // 创建当前图形状态

    void restoreState(const ShapeState *state);           // 恢复图形状态

    void clearState(ShapeState &state);                   // 清空图形状态

    void clearAll();                                      // 清空所有图形

    bool saveToSvg(const QString &filePath);              // 保存为svg

    bool saveToPng(const QString &filePath);              // 保存为png

    void serializeToXml(QXmlStreamWriter &writer);        // 序列化到xml

    bool deserializeFromXml(QXmlStreamReader &reader);    // 从xml解序列化

    void setCurrentFilePath(const QString &filePath);     // 设置当前文件路径

    bool maybeSave();                                     // 是否保存文件

private:
    QColor currentBackgroundColor;                        // 当前页面背景颜色
    QString currentFontFamily;                            // 当前图形文本字体

    bool isClicked = false;                               // 是否单击（未拖动）
    bool fromMultiSelected = false;                       // 是否多选

    std::vector<ShapeBase *> shapes;                      // 存储当前所有图形
    ShapeBase *selectedShape = nullptr;                   // 当前选中的图形

    QPointF lastMousePos;
    bool isDragging = false;                              // 是否在拖动
    bool isResizing = false;                              // 是否在调整大小
    int resizingHandle;                                   // 正在调整图形的控制点

    MyTextEdit *textEdit;                                 // 编辑图形时的文本框
    ShapeBase *editingShape = nullptr;                    // 是否在编辑图形文本内容

    ShapeBase *hoveredShape = nullptr;                    // 鼠标是否悬停在该图形上

    QSize m_pageSize;                                     // 页面尺寸
    bool m_isLandscape = false;                           // 页面是否为横向
    bool m_gridVisible = false;                           // 是否显示网格

    std::unique_ptr<ShapeBase> clipboardShape;            // 剪贴板
    std::stack<std::unique_ptr<ShapeState>> undoStack;    // 撤销栈
    std::stack<std::unique_ptr<ShapeState>> redoStack;    // 重做栈

    QString currentFilePath;                              // 当前文件路径
    bool isModified = false;                              // 文件是否被修改
    QString lastSaveFormat;                               // 当前文件保存格式

    int draggingLineHandle;                               // 正在拖动线段的控制点
    // 存储连接图形的线段指针（这里将线段的放大缩小也视为拖动线段，因为只移动一个点，不是整个图形拖动）
    LineBaseShape *draggingLine = nullptr;
    QPointF lastMagneticPoint;                            // 最近的磁力点
    bool isMagneticActive = false;                        // 是否激活自动吸附

};

#endif  // DRAWAREA_H