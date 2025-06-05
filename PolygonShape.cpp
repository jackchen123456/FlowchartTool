#include "PolygonShape.h"
#include <QPainter>
#include <QTransform>

PolygonShape::PolygonShape(const QRectF &rect) : m_rect(rect.normalized()) {}

void PolygonShape::draw(QPainter &painter) {
    painter.save();

    // 绘制多边形
    painter.setPen(QPen(m_borderColor, m_penWidth, m_borderStyle));
    painter.setBrush(m_fillColor);
    painter.drawPolygon(m_polygon);

    // 绘制文本
    if (!m_text.isEmpty()) {
        painter.save();
        QRectF rect = boundingRect();
        painter.setPen(QPen(m_fontColor));
        QFont font(m_font);
        painter.setFont(font);
        painter.drawText(rect, m_textAlignment | Qt::TextWordWrap, m_text);
        painter.restore();
    }

    if (m_selected) {
        painter.save();
        // 绘制外接矩形边框
        painter.setPen(QPen(Qt::blue, 1, Qt::DashLine));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(boundingRect());

        // 绘制控制点
        painter.setPen(QPen(Qt::blue, 1));
        painter.setBrush(Qt::white);
        for (const auto &pt: calculateHandles()) {
            painter.drawRect(QRectF(pt.x() - HANDLE_SIZE / 2.0, pt.y() - HANDLE_SIZE / 2.0, HANDLE_SIZE, HANDLE_SIZE));
        }
        painter.restore();
    }

    painter.restore();
}

// 判断给定点 point 是否位于多边形内部
bool PolygonShape::containPoint(const QPointF &point) const {
    // Qt::OddEvenFill：从点发射一条无限长射线，统计与多边形边的交点数量。奇数在内，偶数在外
    return m_polygon.containsPoint(point, Qt::OddEvenFill);
}

void PolygonShape::moveBy(qreal dx, qreal dy) {
    m_rect.translate(dx, dy);
    updatePolygon();
}

QRectF PolygonShape::boundingRect() const {
    return m_polygon.boundingRect();        // 多边形的最小外接矩形，用于渲染优化、碰撞检查、图形交互
}

QVector<QPointF> PolygonShape::calculateHandles() const {
    QVector<QPointF> handles;
    QRectF rect = m_rect;
    QVector<QPointF> rawHandles = {
            rect.topLeft(),                             // 左上角
            QPointF(rect.center().x(), rect.top()),     // 上边中点
            rect.topRight(),                            // 右上角
            QPointF(rect.left(), rect.center().y()),    // 左边中点
            QPointF(rect.right(), rect.center().y()),   // 右边中点
            rect.bottomLeft(),                          // 左下角
            QPointF(rect.center().x(), rect.bottom()),  // 下边中点
            rect.bottomRight()                          // 右下角
    };

    // 坐标系转换
    QTransform transform;
    transform.translate(rect.center().x(), rect.center().y());     // 平移到中心
    transform.rotate(m_rotationAngle);                             // 旋转
    transform.translate(-rect.center().x(), -rect.center().y());   // 恢复到原位

    for (const QPointF &pt: rawHandles)
        handles << transform.map(pt);                              // 对每个控制点应用旋转变换

    return handles;
}

int PolygonShape::hitHandle(const QPointF &point) const {
    auto handles = calculateHandles();

    for (int i = 0; i < handles.size(); ++i) {
        QRectF handleRect(handles[i].x() - HANDLE_SIZE / 2.0, handles[i].y() - HANDLE_SIZE / 2.0, HANDLE_SIZE,
                          HANDLE_SIZE);
        if (handleRect.contains(point))
            return i;
    }
    return -1;
}

void PolygonShape::resizeBy(qreal dx, qreal dy, int handleIndex) {
    if (handleIndex < 0 || handleIndex > 7) return;

    QPointF center = m_rect.center();             // 多边形的中心点

    QTransform toLocal;
    toLocal.translate(center.x(), center.y());
    toLocal.rotate(-m_rotationAngle);            // 旋转到原位，先消除旋转影响
    toLocal.translate(-center.x(), -center.y());
    QPointF delta = toLocal.map(QPointF(dx, dy)) - toLocal.map(QPointF(0, 0));     // 计算偏移量

    QRectF localRect = toLocal.mapRect(m_rect);
    QPointF topLeft = localRect.topLeft();
    QPointF bottomRight = localRect.bottomRight();

    switch (handleIndex) {
        case 0:                                 // 左上角
            topLeft += delta;
            break;
        case 1:                                 // 上边中点
            topLeft.setY(topLeft.y() + delta.y());
            break;
        case 2:                                 //  右上角
            topLeft.setY(topLeft.y() + delta.y());
            bottomRight.setX(bottomRight.x() + delta.x());
            break;
        case 3:                                // 左边中点
            topLeft.setX(topLeft.x() + delta.x());
            break;
        case 4:                                // 右边中点
            bottomRight.setX(bottomRight.x() + delta.x());
            break;
        case 5:                                // 左下角
            topLeft.setX(topLeft.x() + delta.x());
            bottomRight.setY(bottomRight.y() + delta.y());
            break;
        case 6:                               // 下边中点
            bottomRight.setY(bottomRight.y() + delta.y());
            break;
        case 7:                               // 右下角
            bottomRight += delta;
            break;
    }

    QRectF newLocalRect(topLeft, bottomRight);

    QTransform toGlobal;
    toGlobal.translate(center.x(), center.y());
    toGlobal.rotate(m_rotationAngle);        // 将修改后的矩形恢复最终旋转状态
    toGlobal.translate(-center.x(), -center.y());
    m_rect = toGlobal.mapRect(newLocalRect);
    updatePolygon();
}