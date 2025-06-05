#include "DiamondShape.h"
#include <QPainter>
#include <QPainterPath>

DiamondShape::DiamondShape(const QRectF &rect) : PolygonShape(rect) {
    updatePolygon();
}

void DiamondShape::updatePolygon() {
    m_polygon.clear();
    QRectF rect = m_rect;
    QVector<QPointF> mappedPoints;
    mappedPoints << QPointF(rect.center().x(), rect.top())
                 << QPointF(rect.right(), rect.center().y())
                 << QPointF(rect.center().x(), rect.bottom())
                 << QPointF(rect.left(), rect.center().y());
    QPointF center = rect.center();
    m_polygon = applyRotation(center, mappedPoints, m_rotationAngle);
}

ShapeBase *DiamondShape::clone() const {
    DiamondShape *diamond = new DiamondShape(*this);
    diamond->setUuid(this->getUuid());
    return diamond;
}

QVector<QPointF> DiamondShape::getMagneticPoints() const {
    QVector<QPointF> points;
    QRectF rect = boundingRect();

    // 4个角点
    points << QPointF(rect.center().x(), rect.top())
           << QPointF(rect.right(), rect.center().y())
           << QPointF(rect.center().x(), rect.bottom())
           << QPointF(rect.left(), rect.center().y());

    // 4条边的中点
    points << (points[0] + points[1]) / 2
           << (points[1] + points[2]) / 2
           << (points[2] + points[3]) / 2
           << (points[3] + points[0]) / 2;

    return points;
}