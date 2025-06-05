#include "LineBaseShape.h"
#include <QPainter>
#include <QPainterPath>
#include <QPainterPathStroker>

LineBaseShape::LineBaseShape(const QPointF &start, const QPointF &end)
        : m_start(start), m_end(end) {}

bool LineBaseShape::containPoint(const QPointF &point) const {
    // 1. 创建一条从起点到终点的路径
    QPainterPath path;
    path.moveTo(m_start);  // 路径起点
    path.lineTo(m_end);    // 路径终点（直线段）

    // 2. 创建一个“笔触”工具，设置检测宽度
    QPainterPathStroker stroker;
    stroker.setWidth(10);  // 设置检测区域的宽度

    // 3. 生成线段的“加粗轮廓”，并检测点是否在其中
    return stroker.createStroke(path).contains(point);
}

void LineBaseShape::moveBy(qreal dx, qreal dy) {
    m_start += QPointF(dx, dy);
    m_end += QPointF(dx, dy);
}

QRectF LineBaseShape::boundingRect() const {
    return QRectF(m_start, m_end).normalized();
}

QVector<QPointF> LineBaseShape::calculateHandles() const {
    QVector<QPointF> handles;
    handles << m_start
            << (m_start + m_end) / 2
            << m_end;

    return handles;
}

int LineBaseShape::hitHandle(const QPointF &point) const {
    auto handles = calculateHandles();
    for (int i = 0; i < handles.size(); ++i) {
        QRectF handleRect(handles[i].x() - HANDLE_SIZE / 2.0, handles[i].y() - HANDLE_SIZE / 2.0, HANDLE_SIZE,
                          HANDLE_SIZE);
        if (handleRect.contains(point))
            return i;
    }
    return -1;
}

void LineBaseShape::resizeBy(qreal dx, qreal dy, int handleIndex) {
    if (handleIndex == 0) {
        m_start += QPointF(dx, dy);
    } else if (handleIndex == 2) {
        m_end += QPointF(dx, dy);
    } else if (handleIndex == 1) {
        moveBy(dx, dy);
    }
}

QVector<QPointF> LineBaseShape::getMagneticPoints() const {
    return {m_start, m_end};
}

void LineBaseShape::setEndPoint(int index, const QPointF &point) {
    if (index == 0) {
        m_start = point;
    } else if (index == 1) {
        m_end = point;
    }
}

void LineBaseShape::setEndPointBinding(int index, ShapeBase *shape, int magIdx) {
    if (!shape || magIdx < 0) return;

    if (index == 0) {
        startBinding = {shape, magIdx};
    } else if (index == 1) {
        endBinding = {shape, magIdx};
    }
}

void LineBaseShape::clearEndPointBinding(int index) {
    if (index == 0) {
        startBinding = {nullptr, -1};
    } else if (index == 1) {
        endBinding = {nullptr, -1};
    }
}

LineBaseShape::EndPointBinding LineBaseShape::getEndPointBinding(int index) const {
    if (index == 0)
        return startBinding;
    else if (index == 1)
        return endBinding;
    else
        return {nullptr, -1};
}

void LineBaseShape::updateEndPointByBinding(int index) {
    if (index != 0 && index != 1) return;

    EndPointBinding binding = getEndPointBinding(index);      // 根据索引获取端点绑定信息

    if (!binding.targetShape) {
        clearEndPointBinding(index);
        return;
    }

    auto points = binding.targetShape->getMagneticPoints();   // 获取绑定图形的磁力点
    if (binding.magneticIndex < 0 || binding.magneticIndex >= points.size()) {
        clearEndPointBinding(index);
        return;
    }
    setEndPoint(index, points[binding.magneticIndex]);       // 设置端点位置
}