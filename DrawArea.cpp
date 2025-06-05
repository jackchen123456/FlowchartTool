#include "DrawArea.h"
#include "RectShape.h"
#include "EllipseShape.h"
#include "DiamondShape.h"
#include "PentagonShape.h"
#include "HexagonShape.h"
#include "ArrowShape.h"
#include "LineShape.h"
#include <QPainter>
#include <QPaintEvent>
#include <QMimeData>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QtMath>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <map>
#include <QDebug>
#include <iostream>
#include <memory>

DrawArea::DrawArea(QWidget *parent)
        : QWidget(parent), currentBackgroundColor(Qt::white), resizingHandle(-1), draggingLineHandle(-1) {
    setMouseTracking(true);                                    // 启用鼠标跟踪功能
    setAutoFillBackground(true);                               // 设置自动填充绘图区域背景
    setAcceptDrops(true);                                      // 启用拖拽功能
    setFocusPolicy(Qt::StrongFocus);                            // 设置焦点策略为StrongFocus，使得组件具有键盘焦点
    setFocus();                                                // 设置焦点
    setFixedSize(2000, 1800);                                  // 设置绘图区域固定大小

    m_pageSize = QSize(1050, 1500);                             // 初始化页面大小

    textEdit = new MyTextEdit(this);                           // 创建自定义的多行文本编辑框
    textEdit->setFont(QFont("Arial", 12));
    textEdit->hide();                                          // 隐藏多行文本编辑框
    // 绑定多行文本编辑框的编辑完成信号
    connect(textEdit, &MyTextEdit::editingFinished, this, [this]() {
        if (editingShape) {
            editingShape->setText(textEdit->toPlainText());    // 将多行文本编辑框的内容赋给当前选中的图形
            textEdit->hide();
            editingShape = nullptr;                            // 重置编辑的图形指针
            update();
        }
    });
}

DrawArea::~DrawArea() {
    for (auto shape: shapes) {
        delete shape;                             // 释放每个图形的内存，这里会调用图形各自的析构函数
    }
    shapes.clear();                               // 安全释放后清空容器指针
}

void DrawArea::setCurrentBackgroundColor(const QColor &color) {
    currentBackgroundColor = color;
    isModified = true;                           // 设置文档已被修改
    update();
}

void DrawArea::setPageSize(const QSize &size) {
    if (m_pageSize != size) {
        m_pageSize = size;
        isModified = true;
        update();
        emit pageSizeChanged(size);
    }
}

void DrawArea::setPageOrientation(bool isLandscape) {
    if (m_isLandscape != isLandscape) {
        m_isLandscape = isLandscape;
        isModified = true;
        update();
        emit pageOrientationChanged(isLandscape);
    }
}

QSize DrawArea::getPageSizeInPixels() const {
    return QSize(m_pageSize.width(), m_pageSize.height());
}

void DrawArea::setGridVisible(bool visible) {
    if (m_gridVisible != visible) {
        m_gridVisible = visible;
        update();
        emit gridVisibilityChanged(visible);
    }
}

bool DrawArea::isInRotateButton(const ShapeBase *shape, const QPointF &pos) const {
    if (!shape) return false;        // 空指针直接返回false
    QRectF btnRect(shape->rotationButtonPosition(), QSizeF(20, 20));   // 在图形的位置偏右上角（10, -10）上绘制一个旋转按钮区域
    return btnRect.contains(pos);
}

void DrawArea::drawGrid(QPainter &painter, const QRect &pageRect) {
    if (m_gridVisible) {                        // 如果网格设置为可见，则绘制网格
        painter.save();
        painter.setClipRect(pageRect);          // 设置剪裁区域为页面区域
        painter.setPen(QPen(Qt::gray, 1, Qt::SolidLine));

        for (int y = 0; y < height(); y += 10) {
            painter.drawLine(0, y, width(), y);
        }

        for (int x = 0; x < width(); x += 10) {
            painter.drawLine(x, 0, x, height());
        }
        painter.restore();
    }
}

void DrawArea::drawRotationButton(QPainter &painter, ShapeBase *shape) {
    QPointF pos = shape->rotationButtonPosition();
    QPixmap icon(":/images/rotate.png");
    icon = icon.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    painter.drawPixmap(pos, icon);
}

void DrawArea::drawMagneticPoints(QPainter &painter) {
    // 如果当前图形为鼠标悬停，且可以旋转（线段类型设置为不可旋转），则绘制磁力点
    if (hoveredShape && hoveredShape->isShapeCanRotate()) {
        painter.save();
        painter.setPen(QPen(Qt::blue, 1));
        auto points = hoveredShape->getMagneticPoints();         // 获取当前图形的磁力点
        for (const auto &point: points) {
            painter.drawLine(point - QPointF(3, 3), point + QPointF(3, 3));
            painter.drawLine(point - QPointF(3, -3), point + QPointF(3, -3));
        }
        painter.restore();
    }
}

void DrawArea::clearSelection() {
    for (auto s: shapes) {
        s->setSelected(false);
    }
    fromMultiSelected = false;            // 多选状态设为false
    selectedShape = nullptr;              // 重置当前选中的图形设为nullptr
    draggingLine = nullptr;               // 重置正在拖拽的线设为nullptr
    draggingLineHandle = -1;
}

void DrawArea::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);                                 // 不使用event参数，避免编译器警告

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);   // 开启抗锯齿

    QRect pageRect = QRect(50, 50, m_pageSize.width(), m_pageSize.height());   // 相对于绘图区域偏移(50, 50)绘制页面
    painter.save();
    painter.fillRect(pageRect, currentBackgroundColor);
    painter.setPen(QPen(Qt::gray, 2));
    painter.drawRect(pageRect);
    painter.restore();

    // 绘制网格
    drawGrid(painter, pageRect);

    painter.setClipRect(pageRect);
    for (auto shape: shapes) {
        painter.save();
        shape->draw(painter);      // 绘制图形
        painter.restore();
    }

    // 绘制当前鼠标悬停所在图形的磁力点
    drawMagneticPoints(painter);

    // 绘制线吸附图形命中的磁力点
    if (isMagneticActive) {
        painter.save();
        painter.setPen(QPen(Qt::red, 2));
        painter.setBrush(Qt::red);
        painter.drawEllipse(lastMagneticPoint, 6, 6);
        painter.restore();
    }
}

void DrawArea::mousePressEvent(QMouseEvent *event) {
    setFocus();                 // 设置控件按压为焦点
    QPointF pos = event->pos();
    isDragging = false;
    isResizing = false;
    resizingHandle = -1;
    isClicked = true;           // 默认是单击状态

    saveToUndoStack();          // 保存按压之前的状态到撤销栈

    // 判断图形是否多选
    int selectedCount = 0;
    for (auto shape: shapes) {
        if (shape->isSelected()) selectedCount++;
    }
    bool isMultiSelect = (selectedCount >= 1);

    // 先处理左键按压
    if (event->button() == Qt::LeftButton) {
        // 判断是否按下了Ctrl或Shift键
        bool ctrlOrShift = (event->modifiers() & Qt::ControlModifier) || (event->modifiers() & Qt::ShiftModifier);
        bool clickedOnSelected = false;      // 记录是否点击到图形

        // 从后往前遍历，后添加的图形越在上层
        for (auto it = shapes.rbegin(); it != shapes.rend(); ++it) {
            ShapeBase *shape = *it;
            if (!shape) continue;

            // 如果图形被选中
            if (shape->isSelected()) {
                int handle = shape->hitHandle(pos);      // 获取选中图形的控制点
                if (handle >= 0) {
                    if (dynamic_cast<LineBaseShape *>(shape)) {              // 判断是否为线段类型
                        draggingLine = static_cast<LineBaseShape *>(shape);
                        draggingLineHandle = (handle == 0 ? 0 : 1);          // 判断控制点是起点还是终点，并设置为拖动控制点
                    }
                    selectedShape = shape;              // 设置选中的图形为当前图形
                    isResizing = true;
                    resizingHandle = handle;
                    lastMousePos = pos;
                    update();
                    emitSelectionChanged();
                    return;
                }
            }

            // 判断当前点击是否在图形内
            if (shape->containPoint(pos)) {
                clickedOnSelected = shape->isSelected();
                selectedShape = shape;

                if (ctrlOrShift) {
                    // 多选，切换当前图形的选中状态
                    shape->setSelected(!shape->isSelected());
                } else if (isMultiSelect && shape->isSelected()) {
                    // 从多选状态点击：先不改变选中状态，等到鼠标释放时再处理
                    fromMultiSelected = true;
                } else {
                    // 单选，取消所有图形的选中状态，只选中当前图形
                    clearSelection();
                    shape->setSelected(true);
                    selectedShape = shape;
                }

                isDragging = true;
                lastMousePos = pos;
                hoveredShape = nullptr;      // 取消图形悬停状态
                update();
                emitSelectionChanged();
                return;
            }
        }

        // 点中空白区域，取消所有选中状态
        if (!clickedOnSelected) {
            clearSelection();
            hoveredShape = nullptr;
            update();
            emitSelectionChanged();
        }
    } else if (event->button() == Qt::RightButton) {        // 处理右键按压
        for (auto it = shapes.rbegin(); it != shapes.rend(); ++it) {
            ShapeBase *shape = *it;
            if (!shape) continue;

            // 如果当前点击在图形内
            if (shape->containPoint(pos)) {
                // 清除所有图形选中状态，只设置当前图形为选中状态
                clearSelection();
                selectedShape = shape;
                shape->setSelected(true);
                update();
                emitSelectionChanged();
                return;
            }
        }

        clearSelection();
        update();
        emitSelectionChanged();
    }
}

void DrawArea::mouseMoveEvent(QMouseEvent *event) {
    // 如果是正在拖动的线段
    if (draggingLine) {
        QPointF mousePos = event->pos();
        QPointF nearestPt;                                      // 最接近的点
        ShapeBase *nearestShape = nullptr;                      // 最接近的图形
        int nearestIndex = -1;                                  // 最接近的图形磁力点索引
        qreal minDist = std::numeric_limits<qreal>::max();      // 最短距离
        for (auto shape: shapes) {
            if (shape == draggingLine) continue;                // 如果是正在拖动的线段，则跳过
            auto magPoints = shape->getMagneticPoints();
            for (int i = 0; i < magPoints.size(); ++i) {
                qreal dist = QLineF(mousePos, magPoints[i]).length();
                if (dist < minDist) {                           // 如果当前点到鼠标的距离小于最小距离，则更新最小距离和最近点
                    minDist = dist;
                    nearestPt = magPoints[i];
                    nearestShape = shape;
                    nearestIndex = i;
                }
            }
        }
        // 如果最近点距离小于磁吸范围，则将拖动线段endpoint设置为最近点，并且绑定图形和最近磁力点
        if (minDist < ShapeBase::MAGNETIC_RANGE) {
            draggingLine->setEndPoint(draggingLineHandle, nearestPt);
            draggingLine->setEndPointBinding(draggingLineHandle, nearestShape, nearestIndex);
            lastMagneticPoint = nearestPt;
            isMagneticActive = true;          // 设置磁吸状态为吸附状态
        } else {
            // 将拖动线段endpoint设置为鼠标位置（这里被拖动的线段端点位置实时变化，类似于拖动线段控制点进行放大缩小的效果）
            draggingLine->setEndPoint(draggingLineHandle, mousePos);
            draggingLine->clearEndPointBinding(draggingLineHandle);
            isMagneticActive = false;        // 设置磁吸状态为未吸附状态
        }
        update();
        return;
    }

    // 判断鼠标是否移动到图形上，并且图形为未选中状态
    QPointF pos = event->pos();
    ShapeBase *newHovered = nullptr;
    for (ShapeBase *shape: shapes) {
        if (shape->boundingRect().contains(pos) && shape->isSelected() == false) {
            newHovered = shape;
            break;
        }
    }

    if (hoveredShape != newHovered) {
        hoveredShape = newHovered;         // 设置该图形为鼠标悬停状态
        update();
    }

    // 如果移动距离超过阈值，则认为拖动，设置单击为false
    if ((pos - lastMousePos).manhattanLength() > QApplication::startDragDistance()) {
        isClicked = false;
    }

    // 图形缩放
    if (isResizing) {
        QPointF offset = pos - lastMousePos;
        selectedShape->resizeBy(offset.x(), offset.y(), resizingHandle);
        isModified = true;
        lastMousePos = pos;
        updateAllLineBindings();
        update();
    } else if (isDragging) {            // 图形移动
        QPointF offset = pos - lastMousePos;

        // 移动所有选中的图形
        if (fromMultiSelected) {
            for (auto shape: shapes) {
                if (shape->isSelected()) {
                    shape->moveBy(offset.x(), offset.y());
                }
            }
        } else if (selectedShape) {    // 移动当前选中的图形
            selectedShape->moveBy(offset.x(), offset.y());
        }

        isModified = true;
        lastMousePos = pos;
        updateAllLineBindings();      // 更新所有线段的绑定点
        update();
    }
}

void DrawArea::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (draggingLine) {
            draggingLine = nullptr;        // 将指向正在拖动的线段指针设置为空指针
            draggingLineHandle = -1;
            isMagneticActive = false;
        }

        // 如果是单击且从多选状态点击，释放后只保留当前点击的图形为选中状态
        if (isClicked && fromMultiSelected && selectedShape) {
            for (auto shape: shapes) {
                shape->setSelected(shape == selectedShape);
            }
        }

        isDragging = false;
        isResizing = false;
        fromMultiSelected = false;
        isClicked = false;
        update();
        emitSelectionChanged();
    }
}

void DrawArea::dragEnterEvent(QDragEnterEvent *event) {
    // 当用户拖动图形进入控件区域时，检查拖拽数据是否包含文本格式
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();       // 接受拖放操作
    }
}

void DrawArea::dropEvent(QDropEvent *event) {
    QPointF pos = event->pos();
    saveToUndoStack();             // 保存拖拽前的状态到撤销栈

    QString type = event->mimeData()->text();     // 从拖放事件的MIME数据中提取文本内容
    ShapeBase *shape = nullptr;
    if (type == "Rect") {
        shape = new RectShape(QRectF(pos, QSizeF(80, 80)));
    } else if (type == "Ellipse") {
        shape = new EllipseShape(QRectF(pos, QSizeF(80, 80)));
    } else if (type == "Diamond") {
        shape = new DiamondShape(QRectF(pos, QSizeF(80, 80)));
    } else if (type == "Pentagon") {
        shape = new PentagonShape(QRectF(pos, QSizeF(80, 80)));
    } else if (type == "Hexagon") {
        shape = new HexagonShape(QRectF(pos, QSizeF(80, 80)));
    } else if (type == "Arrow") {
        shape = new ArrowShape(pos, pos + QPointF(80, 0));
    } else if (type == "Line") {
        shape = new LineShape(pos, pos + QPointF(80, 0));
    }

    if (shape) {
        shapes.push_back(shape);        // 将拖入的图形添加到图形数组中

        // 取消所有图形选中状态，设置当前图形选中
        clearSelection();
        shape->setSelected(true);
        selectedShape = shape;
        isModified = true;
        update();
        emitSelectionChanged();
    }

    event->acceptProposedAction();        // 接受拖拽操作
}

void DrawArea::mouseDoubleClickEvent(QMouseEvent *event) {
    QPointF pos = event->pos();
    saveToUndoStack();                   // 保存当前状态到撤销栈中

    for (auto it = shapes.rbegin(); it != shapes.rend(); ++it) {
        ShapeBase *shape = *it;
        if (!shape) continue;

        if (shape->containPoint(pos)) {
            editingShape = shape;                // 设置当前图形为正在编辑的图形
            QRectF rect = shape->boundingRect();

            // 创建文本框但不立即显示
            textEdit->setGeometry(rect.toRect());                                 // 设置编辑框位置和大小
            textEdit->setText(shape->getText());                                  // 填充图形中的现有文本
            textEdit->setStyleSheet("MyTextEdit { border: none; background-color: transparent;}");  // 透明无边框样式
            textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);         // 禁用纵向滚动条
            textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);       // 禁用横向滚动条
            textEdit->show();                                                     // 显示编辑框
            textEdit->setFocus();                                                 // 获取焦点
            isModified = true;
            break;
        }
    }
}

void DrawArea::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);
    if (selectedShape) {
        QAction *copyAct = menu.addAction(tr("copy"));
        copyAct->setShortcut(QKeySequence::Copy);

        QAction *cutAct = menu.addAction(tr("cut"));
        cutAct->setShortcut(QKeySequence::Cut);

        QAction *pasteAct = menu.addAction(tr("paste"));
        pasteAct->setShortcut(QKeySequence::Paste);

        QAction *duplicateAct = menu.addAction(tr("duplicate"));
        duplicateAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));

        QAction *deleteAct = menu.addAction(tr("delete"));
        deleteAct->setShortcut(QKeySequence::Delete);

        pasteAct->setEnabled(clipboardShape != nullptr);            // 剪切板不为空时才启用粘贴
        QAction *chosen = menu.exec(event->globalPos());            // 在指定位置显示菜单并获取用户选择（通常是鼠标右键点击位置）
        if (!chosen) return;
        if (chosen == copyAct) {
            copySelectedShape();
        } else if (chosen == cutAct) {
            cutSelectedShape();
        } else if (chosen == pasteAct) {
            pasteShape(event->pos());
        } else if (chosen == duplicateAct) {
            duplicateSelectedShape();
        } else if (chosen == deleteAct) {
            deleteSelectedShape();
        }
    } else {
        // 空白区域右键菜单
        QAction *undoAct = menu.addAction(tr("undo"));
        undoAct->setShortcut(QKeySequence::Undo);
        QAction *redoAct = menu.addAction(tr("redo"));
        redoAct->setShortcut(QKeySequence::Redo);
        QAction *pasteHereAct = menu.addAction(tr("pasteHere"));
        pasteHereAct->setShortcut(QKeySequence::Paste);
        QAction *selectAllAct = menu.addAction(tr("selectAll"));
        selectAllAct->setShortcut(QKeySequence::SelectAll);

        undoAct->setEnabled(!undoStack.empty());
        redoAct->setEnabled(!redoStack.empty());
        pasteHereAct->setEnabled(clipboardShape != nullptr);
        selectAllAct->setEnabled(!shapes.empty());

        QAction *chosen = menu.exec(event->globalPos());
        if (!chosen) return;
        if (chosen == undoAct) {
            undo();
        } else if (chosen == redoAct) {
            redo();
        } else if (chosen == pasteHereAct) {
            pasteShape(event->pos());
        } else if (chosen == selectAllAct) {
            selectAll();
        }
    }
}

void DrawArea::copySelectedShape() {
    if (!selectedShape) return;
    saveToUndoStack();
    clipboardShape.reset(selectedShape->clone());
}

void DrawArea::cutSelectedShape() {
    if (!selectedShape) return;
    saveToUndoStack();
    clipboardShape.reset(selectedShape->clone());
    deleteSelectedShape();
}

void DrawArea::pasteShape(const QPointF &pos) {
    if (!clipboardShape) return;                      // 剪切板没有内容，直接退出
    saveToUndoStack();

    ShapeBase *newShape = clipboardShape->clone();    // 从剪切板拷贝图形

    // 粘贴时偏移一点，避免与原图形重叠
    newShape->moveBy(pos.x() - newShape->boundingRect().x() + 20,
                     pos.y() - newShape->boundingRect().y() + 20);
    shapes.push_back(newShape);                      // 将新图形添加到图形数组中
    clearSelection();                                 // 清空图形选中状态
    newShape->setSelected(true);
    selectedShape = newShape;
    isModified = true;                               // 设置文档已被修改
    update();
    emitSelectionChanged();
}

void DrawArea::duplicateSelectedShape() {
    if (!selectedShape) return;
    saveToUndoStack();
    ShapeBase *newShape = selectedShape->clone();

    // 复用时偏移一点，避免与原图形重叠
    newShape->moveBy(20, 20);
    shapes.push_back(newShape);
    clearSelection();
    newShape->setSelected(true);
    selectedShape = newShape;
    isModified = true;
    update();
    emitSelectionChanged();
}

void DrawArea::deleteSelectedShape() {
    if (!selectedShape) return;
    saveToUndoStack();

    // 先解除所有绑定到该图形的线条
    for (auto shape: shapes) {
        if (auto line = dynamic_cast<LineBaseShape *>(shape)) {
            auto startBinding = line->getEndPointBinding(0);
            // 如果该线条的起点绑定的是当前选中的图形，则解除绑定
            if (startBinding.targetShape == selectedShape) {
                line->clearEndPointBinding(0);
            }
            // 如果该线条的终点绑定的是当前选中的图形，则解除绑定
            auto endBinding = line->getEndPointBinding(1);
            if (endBinding.targetShape == selectedShape) {
                line->clearEndPointBinding(1);
            }
        }
    }

    auto it = std::find(shapes.begin(), shapes.end(), selectedShape);     // 在图形数组中寻找当前选中的图形
    if (it != shapes.end()) {
        delete *it;                          // 释放迭代器指向的图形对象内存
        shapes.erase(it);                    // 从容器中移除该迭代器
        selectedShape = nullptr;
        isModified = true;
        update();

        emitSelectionChanged();
        emit deleteSelectedShapeChanged();
    }
}

void DrawArea::keyPressEvent(QKeyEvent *event) {
    if (!selectedShape && event->matches(QKeySequence::Paste)) {
        pasteShape(QPointF(500, 500));
        return;
    }

    if (event->matches(QKeySequence::Undo)) {
        restoreFromUndoStack();
    } else if (event->matches(QKeySequence::Redo)) {
        restoreFromRedoStack();
    } else if (event->matches(QKeySequence::Copy)) {
        copySelectedShape();
    } else if (event->matches(QKeySequence::Cut)) {
        cutSelectedShape();
    } else if (event->matches(QKeySequence::Paste)) {
        QPointF pos = selectedShape->boundingRect().center();
        pasteShape(pos);
    } else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_D) {
        duplicateSelectedShape();
    } else if (event->matches(QKeySequence::Delete)) {
        deleteSelectedShape();
    } else if (event->matches(QKeySequence::SelectAll)) {
        selectAll();
    }
}

void DrawArea::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);     // 确保基础功能正常
    update();                        // 在窗口大小变化时触发界面重绘
}

void DrawArea::enterEvent(QEvent *event) {
    setMouseTracking(true);         // 启用鼠标跟踪
    QWidget::enterEvent(event);
}

void DrawArea::leaveEvent(QEvent *event) {
    hoveredShape = nullptr;
    update();
    QWidget::leaveEvent(event);
}

void DrawArea::saveToUndoStack() {
    // 保存当前状态
    undoStack.push(createCurrentState());

    // 清空redoStack
    while (!redoStack.empty()) {
        clearState(*(redoStack.top()));
        redoStack.pop();
    }
    emit undoStateChanged();
}

void DrawArea::undo() {
    restoreFromUndoStack();
}

void DrawArea::redo() {
    restoreFromRedoStack();
}

// 生成当前绘图区域的独立状态副本，用于实现撤销/重做、状态保存等功能
std::unique_ptr<ShapeState> DrawArea::createCurrentState() const {
    auto state = std::make_unique<ShapeState>();

    // 深拷贝所有图形
    for (auto shape: shapes) {
        ShapeBase *cloned = shape->clone();
        state->shapes.push_back(cloned);
        if (shape == selectedShape) {
            state->selectedShapeId = shape->getUuid();     // 记录当前选中图形的唯一标识（UUID）
        }
    }
    return state;
}

void DrawArea::restoreState(const ShapeState *state) {
    // 清理当前所有图形
    for (auto shape: shapes) {
        delete shape;
    }
    shapes.clear();
    selectedShape = nullptr;

    // 从保存的状态中深拷贝图形
    for (auto shape: state->shapes) {
        ShapeBase *cloned = shape->clone();
        if (cloned->getUuid() == state->selectedShapeId) {
            selectedShape = cloned;
            selectedShape->setSelected(true);
        }
        shapes.push_back(cloned);
    }

    // 恢复绑定关系
    updateAllLineBindings();

    update();
    isModified = true;
    emitSelectionChanged();
}

void DrawArea::clearState(ShapeState &state) {
    for (auto shape: state.shapes) {
        delete shape;
    }
    state.shapes.clear();
}

void DrawArea::restoreFromUndoStack() {
    if (undoStack.empty()) return;                // 边界检查：撤销栈为空则退出

    // 保存当前状态到重做栈
    redoStack.push(createCurrentState());

    // 恢复撤销栈中的状态
    std::unique_ptr<ShapeState> state = std::move(undoStack.top());       // 转移指针所有权
    undoStack.pop();                              // 弹出栈顶元素

    restoreState(state.get());                    // 恢复状态
    emit undoStateChanged();
    emitSelectionChanged();
}

void DrawArea::restoreFromRedoStack() {
    if (redoStack.empty()) return;

    // 保存当前状态到撤销栈
    undoStack.push(createCurrentState());

    std::unique_ptr<ShapeState> state = std::move(redoStack.top());
    redoStack.pop();

    restoreState(state.get());
    emit redoStateChanged();
    emitSelectionChanged();
}

bool DrawArea::canDelete() const {
    if (selectedShape != nullptr) {
        return true;
    }

    for (const auto &shape: shapes) {
        if (shape->isSelected()) {
            return true;
        }
    }

    return false;
}

bool DrawArea::canMoveTop() const {
    if (!selectedShape || shapes.empty()) {
        return false;
    }

    return (selectedShape != shapes.back());
}

bool DrawArea::canMoveBottom() const {
    if (!selectedShape || shapes.empty()) {
        return false;
    }

    return (selectedShape != shapes.front());
}

bool DrawArea::canMoveUp() const {
    if (!selectedShape || shapes.empty()) {
        return false;
    }

    auto it = std::find(shapes.begin(), shapes.end(), selectedShape);
    return (it != shapes.end() && it != shapes.end() - 1);   // 选中的图形不在容器的尾部（即所有图形顶部），则可以上移
}

bool DrawArea::canMoveDown() const {
    if (!selectedShape || shapes.empty()) {
        return false;
    }

    auto it = std::find(shapes.begin(), shapes.end(), selectedShape);
    return (it != shapes.begin());                           // 选中的图形不在容器首部，则可以下移
}

void DrawArea::selectAll() {
    for (auto shape: shapes) {
        shape->setSelected(true);
    }

    fromMultiSelected = true;
    update();
    emitSelectionChanged();
}

void DrawArea::emitSelectionChanged() {
    bool hasSelection = false;
    for (auto shape: shapes) {
        if (shape->isSelected()) {
            hasSelection = true;
            break;
        }
    }

    emit selectionChanged(hasSelection);
}

void DrawArea::moveSelectedShapeToTop() {
    if (!canMoveTop()) return;
    saveToUndoStack();

    for (auto it = shapes.begin(); it != shapes.end(); ++it) {
        if (*it == selectedShape) {                     // 当前图形为选中的图形
            auto shape = std::move(*it);                // 转移指针所有权
            shapes.erase(it);                           // 从数组中删除当前图形
            shapes.push_back(std::move(shape));         // 移动到末尾
            update();
            emit shapeOrderChanged();
            break;
        }
    }
}

void DrawArea::moveSelectedShapeToBottom() {
    if (!canMoveBottom()) return;
    saveToUndoStack();

    for (auto it = shapes.begin(); it != shapes.end(); ++it) {
        if (*it == selectedShape) {
            auto shape = std::move(*it);
            shapes.erase(it);
            shapes.insert(shapes.begin(), std::move(shape));   // 插入到最前面
            update();
            emit shapeOrderChanged();
            break;
        }
    }
}

void DrawArea::moveSelectedShapeUp() {
    if (!canMoveUp()) return;
    saveToUndoStack();

    for (auto it = shapes.begin(); it != shapes.end(); ++it) {
        if (*it == selectedShape) {
            if (it == shapes.end() - 1) break;

            // 交换当前指针和下一个指针
            std::iter_swap(it, it + 1);
            update();
            emit shapeOrderChanged();
            break;
        }
    }
}

void DrawArea::moveSelectedShapeDown() {
    if (!canMoveDown()) return;
    saveToUndoStack();

    for (auto it = shapes.begin(); it != shapes.end(); ++it) {
        if (*it == selectedShape) {
            if (it == shapes.begin()) break;

            // 交换当前指针和前一个指针
            std::iter_swap(it, it - 1);
            update();
            emit shapeOrderChanged();
            break;
        }
    }
}

bool DrawArea::newFile() {
    if (!maybeSave()) {
        return false;
    }

    clearAll();                           // 清空所有图形和状态
    setCurrentFilePath(QString());        // 重置文件路径
    isModified = false;
    return true;
}

bool DrawArea::openFile() {
    if (!maybeSave()) return false;

    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), tr("SVG (*.svg)"));
    if (!filePath.isEmpty()) {
        return loadFromSvg(filePath);
    }

    return false;
}

bool DrawArea::saveFile() {
    if (currentFilePath.isEmpty()) {          // 当前文件路径为空时，调用另存为文件
        return saveAsFile();
    }

    return saveToSvg(currentFilePath);        // 当前文件路径不为空时，保存为SVG文件
}

bool DrawArea::saveAsFile() {
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), QString(), tr("SVG (*.svg);;PNG (*.png)"));
    if (!filePath.isEmpty()) {
        if (filePath.endsWith(".svg", Qt::CaseInsensitive)) {
            lastSaveFormat = "svg";
            return saveToSvg(filePath);
        } else if (filePath.endsWith(".png", Qt::CaseInsensitive)) {
            lastSaveFormat = "png";
            return saveToPng(filePath);
        }
    }

    return false;
}

bool DrawArea::exportToPng() {
    QString filePath = QFileDialog::getSaveFileName(this, tr("Export to PNG"), QString(), tr("PNG (*.png)"));
    if (!filePath.isEmpty()) {
        return saveToPng(filePath);
    }

    return false;
}

bool DrawArea::exportToSvg() {
    QString filePath = QFileDialog::getSaveFileName(this, tr("Export to SVG"), QString(), tr("SVG (*.svg)"));
    if (!filePath.isEmpty()) {
        return saveToSvg(filePath);
    }

    return false;
}

void DrawArea::clearAll() {
    for (auto shape: shapes) {
        delete shape;
    }

    shapes.clear();
    selectedShape = nullptr;
    currentFilePath.clear();
    isModified = false;
    update();
}

bool DrawArea::saveToSvg(const QString &filePath) {
    QFile file(filePath);
    // 以只写和文本模式打开文件
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QXmlStreamWriter writer(&file);                  // 创建XML写入器，绑定到已打开的文件
    writer.setAutoFormatting(true);                  // 设置自动格式化
    writer.writeStartDocument();                     // 写入文档头，标识这是一个XML文档
    writer.writeStartElement("svg");                 // 写入<svg> 标签，定义SVG根元素
    writer.writeAttribute("xmlns", "http://www.w3.org/2000/svg");    // 设置命名空间（必须）
    writer.writeAttribute("width", QString::number(width()));        // 设置SVG画布尺寸
    writer.writeAttribute("height", QString::number(height()));

    // 序列化所有图形
    serializeToXml(writer);
    writer.writeEndElement();                        // 结束<svg>标签，结束SVG根元素
    writer.writeEndDocument();                       // 结束XML文档

    setCurrentFilePath(filePath);                    // 设置当前文件路径
    isModified = false;                              // 文件保存成功，修改标志位设为false
    return true;
}

bool DrawArea::saveToPng(const QString &filePath) {
    QPixmap pixmap(m_pageSize);                             // 创建一个透明画布
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);          // 创建一个画笔抗锯齿

    QRect pageRect = QRect(0, 0, m_pageSize.width(), m_pageSize.height());
    painter.fillRect(pageRect, currentBackgroundColor);
    // 绘制所有图形
    for (auto shape: shapes) {
        painter.save();
        shape->draw(painter);
        painter.restore();
    }
    return pixmap.save(filePath, "PNG");
}

void DrawArea::serializeToXml(QXmlStreamWriter &writer) {
    writer.writeAttribute("backgroundColor", currentBackgroundColor.name());     // 写入背景颜色
    writer.writeStartElement("shapes");                                          // 写入<shapes>

    for (auto shape: shapes) {
        writer.writeStartElement("shape");                                       // 写入<shape>
        writer.writeAttribute("type", shape->getShapeType());                    // 写入图形类型type
        if (shape == nullptr) continue;

        // 保存基本属性
        if (LineBaseShape *line = dynamic_cast<LineBaseShape *>(shape)) {
            writer.writeAttribute("startX", QString::number(line->getStart().x()));
            writer.writeAttribute("startY", QString::number(line->getStart().y()));
            writer.writeAttribute("endX", QString::number(line->getEnd().x()));
            writer.writeAttribute("endY", QString::number(line->getEnd().y()));
        } else {
            writer.writeAttribute("x", QString::number(shape->boundingRect().x()));
            writer.writeAttribute("y", QString::number(shape->boundingRect().y()));
            writer.writeAttribute("width", QString::number(shape->boundingRect().width()));
            writer.writeAttribute("height", QString::number(shape->boundingRect().height()));
            writer.writeAttribute("rotation", QString::number(shape->getRotation()));
        }

        // 保存样式属性
        writer.writeAttribute("penWidth", QString::number(shape->getPenWidth()));
        writer.writeAttribute("borderColor", shape->getBorderColor().name());
        writer.writeAttribute("fillColor", shape->getFillColor().name());
        writer.writeAttribute("borderStyle", QString::number(shape->getBorderStyle()));

        // 保存文本属性
        writer.writeAttribute("text", shape->getText());
        writer.writeAttribute("fontFamily", shape->getFontFamily());
        writer.writeAttribute("fontSize", QString::number(shape->getFontSize()));
        writer.writeAttribute("fontBold", QString::number(shape->isFontBold()));
        writer.writeAttribute("fontItalic", QString::number(shape->isFontItalic()));
        writer.writeAttribute("fontUnderline", QString::number(shape->isFontUnderline()));
        writer.writeAttribute("fontColor", shape->getFontColor().name());
        writer.writeAttribute("textAlignment", QString::number(shape->getTextAlignment()));

        writer.writeEndElement();                                    // 结束<shape>
    }
    writer.writeEndElement();                                        // 结束<shapes>
}

bool DrawArea::loadFromSvg(const QString &filePath) {
    QFile file(filePath);
    // 以只读和文本模式打开文件
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QXmlStreamReader reader(&file);                                // 创建一个XML读取器
    clearAll();

    while (!reader.atEnd()) {
        reader.readNext();                                         // 读取下一个元素
        if (reader.isStartElement()) {
            if (reader.name() == "svg") {
                QString bgColor = reader.attributes().value("backgroundColor").toString();   // 读取背景颜色
                if (!bgColor.isEmpty()) {
                    currentBackgroundColor = QColor(bgColor);
                }
                continue;
            } else if (reader.name() == "shapes") {
                continue;
            } else if (reader.name() == "shape") {                 // 读取<shape>
                if (!deserializeFromXml(reader)) {
                    return false;
                }
            }
        }
    }
    if (reader.hasError()) {                                      // 读取失败
        return false;
    }

    setCurrentFilePath(filePath);
    isModified = false;
    update();
    return true;
}

bool DrawArea::deserializeFromXml(QXmlStreamReader &reader) {
    QString type = reader.attributes().value("type").toString();       // 获取当前XML元素的type属性值
    ShapeBase *shape = nullptr;

    if (type == "Rect") {
        shape = new RectShape(QRectF(
                reader.attributes().value("x").toDouble(),
                reader.attributes().value("y").toDouble(),
                reader.attributes().value("width").toDouble(),
                reader.attributes().value("height").toDouble()
        ));
    } else if (type == "Ellipse") {
        shape = new EllipseShape(QRectF(
                reader.attributes().value("x").toDouble(),
                reader.attributes().value("y").toDouble(),
                reader.attributes().value("width").toDouble(),
                reader.attributes().value("height").toDouble()
        ));
    } else if (type == "Diamond") {
        shape = new DiamondShape(QRectF(
                reader.attributes().value("x").toDouble(),
                reader.attributes().value("y").toDouble(),
                reader.attributes().value("width").toDouble(),
                reader.attributes().value("height").toDouble()
        ));
    } else if (type == "Pentagon") {
        shape = new PentagonShape(QRectF(
                reader.attributes().value("x").toDouble(),
                reader.attributes().value("y").toDouble(),
                reader.attributes().value("width").toDouble(),
                reader.attributes().value("height").toDouble()
        ));
    } else if (type == "Hexagon") {
        shape = new HexagonShape(QRectF(
                reader.attributes().value("x").toDouble(),
                reader.attributes().value("y").toDouble(),
                reader.attributes().value("width").toDouble(),
                reader.attributes().value("height").toDouble()
        ));
    } else if (type == "Line") {
        shape = new LineShape(QPointF(
                reader.attributes().value("startX").toDouble(),
                reader.attributes().value("startY").toDouble()
        ), QPointF(
                reader.attributes().value("endX").toDouble(),
                reader.attributes().value("endY").toDouble()
        ));
    } else if (type == "Arrow") {
        shape = new ArrowShape(QPointF(
                reader.attributes().value("startX").toDouble(),
                reader.attributes().value("startY").toDouble()
        ), QPointF(
                reader.attributes().value("endX").toDouble(),
                reader.attributes().value("endY").toDouble()
        ));
    }

    if (shape) {
        shape->setRotation(reader.attributes().value("rotation").toDouble());
        shape->setPenWidth(reader.attributes().value("penWidth").toInt());
        shape->setBorderColor(QColor(reader.attributes().value("borderColor").toString()));
        shape->setFillColor(QColor(reader.attributes().value("fillColor").toString()));
        shape->setBorderStyle(static_cast<Qt::PenStyle>(reader.attributes().value("borderStyle").toInt()));

        shape->setText(reader.attributes().value("text").toString());
        shape->setFontFamily(reader.attributes().value("fontFamily").toString());
        shape->setFontSize(reader.attributes().value("fontSize").toInt());
        shape->setFontBold(reader.attributes().value("fontBold").toInt());
        shape->setFontItalic(reader.attributes().value("fontItalic").toInt());
        shape->setFontUnderline(reader.attributes().value("fontUnderline").toInt());
        shape->setFontColor(QColor(reader.attributes().value("fontColor").toString()));
        shape->setTextAlignment(static_cast<Qt::Alignment>(reader.attributes().value("textAlignment").toInt()));

        shapes.push_back(shape);
        return true;
    }
    return false;
}

void DrawArea::setCurrentFilePath(const QString &filePath) {
    currentFilePath = filePath;
    emit fileChanged(filePath);
}

bool DrawArea::maybeSave() {
    // 检查当前文件是否被修改
    if (isModified) {
        QMessageBox msgBox(this);           // 弹出消息对话框，提示用户保存文件
        msgBox.setWindowTitle(tr("Flowchart"));
        msgBox.setText(tr("The file has been modified.\nDo you want to save your changes?"));
        msgBox.setIcon(QMessageBox::Warning);

        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        msgBox.setButtonText(QMessageBox::Save, tr("Save"));            // 保存文件
        msgBox.setButtonText(QMessageBox::Discard, tr("Discard"));      // 不保存文件
        msgBox.setButtonText(QMessageBox::Cancel, tr("Cancel"));        // 取消

        int ret = msgBox.exec();           // 显示一个模态对话框，并阻塞当前线程直到用户做出选择

        if (ret == QMessageBox::Save) {
            return saveFile();
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void DrawArea::updateAllLineBindings() {
    for (auto shape: shapes) {
        if (auto line = dynamic_cast<LineBaseShape *>(shape)) {       // 判断图形是否为线段类型
            for (int i = 0; i < 2; ++i) {
                // 获取第i个端点的绑定信息
                auto binding = line->getEndPointBinding(i);           // 获取第i个端点的绑定信息
                if (binding.targetShape) {
                    bool targetExists = false;
                    for (auto s: shapes) {
                        if (s == binding.targetShape) {               // 判断绑定图形是否在当前图形列表中
                            targetExists = true;
                            break;
                        }
                    }
                    if (targetExists)
                        line->updateEndPointByBinding(i);     // 更新绑定的端点
                    else
                        line->clearEndPointBinding(i);        // 清除绑定
                }
            }
        }
    }
}