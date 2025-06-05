#include "RectShape.h"

RectShape::RectShape(const QRectF &rect) : PolygonShape(rect) {
    updatePolygon();
}

void RectShape::updatePolygon() {
    m_polygon.clear();
    QVector<QPointF> mappedPoints;
    mappedPoints << m_rect.topLeft() << m_rect.topRight() << m_rect.bottomRight() << m_rect.bottomLeft();

    QPointF center = m_rect.center();
    m_polygon = applyRotation(center, mappedPoints, m_rotationAngle);
}

ShapeBase *RectShape::clone() const {
    RectShape *rect = new RectShape(*this);
    rect->setUuid(this->getUuid());
    return rect;
}

QVector<QPointF> RectShape::getMagneticPoints() const {
    QVector<QPointF> points;
    QRectF rect = boundingRect();

    points << rect.topLeft() << rect.topRight() << rect.bottomLeft() << rect.bottomRight()
           << QPointF(rect.center().x(), rect.top())
           << QPointF(rect.center().x(), rect.bottom())
           << QPointF(rect.left(), rect.center().y())
           << QPointF(rect.right(), rect.center().y());

    return points;
}