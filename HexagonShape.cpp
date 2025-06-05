#include "HexagonShape.h"
#include <QPainter>
#include <QtMath>

HexagonShape::HexagonShape(const QRectF &rect) : PolygonShape(rect) {
    updatePolygon();
}

void HexagonShape::updatePolygon() {
    m_polygon.clear();
    QVector<QPointF> unitPoints;

    // // 从 -90°开始（顶部中点），按 60°间隔生成 6 个顶点，形成顶点朝上的单位正六边形
    for (int i = 0; i < 6; ++i) {
        double angle = qDegreesToRadians(-90.0 + i * 60.0);
        unitPoints << QPointF(qCos(angle), qSin(angle));
    }

    // 获取单位正六边形的外接矩形
    QRectF unitBound = QPolygonF(unitPoints).boundingRect();

    QVector<QPointF> mappedPoints;

    // 将单位正六边形映射到m_rect区域
    for (const auto &pt: unitPoints) {
        // 计算点在单位矩形中的比例位置
        qreal xRatio = (pt.x() - unitBound.left()) / unitBound.width();
        qreal yRatio = (pt.y() - unitBound.top()) / unitBound.height();
        qreal x = m_rect.left() + xRatio * m_rect.width();
        qreal y = m_rect.top() + yRatio * m_rect.height();
        mappedPoints << QPointF(x, y);
    }

    QPointF center = m_rect.center();
    m_polygon << applyRotation(center, mappedPoints, m_rotationAngle);
}

ShapeBase *HexagonShape::clone() const {
    HexagonShape *hexagon = new HexagonShape(*this);
    hexagon->setUuid(this->getUuid());
    return hexagon;
}

QVector<QPointF> HexagonShape::getMagneticPoints() const {
    QVector<QPointF> points;
    if (m_polygon.isEmpty())
        return points;

    // 6个角点
    for (int i = 0; i < 6; ++i) {
        points << m_polygon[i];
    }

    // 6条边的中点
    for (int i = 0; i < 6; ++i) {
        int next = (i + 1) % 6;
        points << (m_polygon[i] + m_polygon[next]) / 2;
    }

    return points;
}