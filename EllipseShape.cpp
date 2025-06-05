#include "EllipseShape.h"
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

EllipseShape::EllipseShape(const QRectF &rect) : PolygonShape(rect) {
    updatePolygon();
}

void EllipseShape::updatePolygon() {
    m_polygon.clear();

    const int numSegments = 60;          // 多边形点数量
    QRectF rect = m_rect;
    QPointF center = rect.center();
    qreal radiusX = rect.width() / 2.0;
    qreal radiusY = rect.height() / 2.0;

    // 创建椭圆点
    for (int i = 0; i < numSegments; ++i) {
        qreal angle = qDegreesToRadians(i * (360.0 / numSegments));      // 每6度创建一个点
        qreal x = radiusX * qCos(angle);
        qreal y = radiusY * qSin(angle);
        QPointF pt = QPointF(x, y) + center;
        m_polygon << pt;
    }

    m_polygon = applyRotation(center, m_polygon, m_rotationAngle);
}

ShapeBase *EllipseShape::clone() const {
    EllipseShape *ellipse = new EllipseShape(*this);
    ellipse->setUuid(this->getUuid());
    return ellipse;
}

QVector<QPointF> EllipseShape::getMagneticPoints() const {
    QVector<QPointF> points;
    QRectF rect = boundingRect();

    // 4个主要方向点
    points << QPointF(rect.center().x(), rect.top())
           << QPointF(rect.right(), rect.center().y())
           << QPointF(rect.center().x(), rect.bottom())
           << QPointF(rect.left(), rect.center().y());

    // 4个45度方向点
    qreal rx = rect.width() / 2;
    qreal ry = rect.height() / 2;
    points << rect.center() + QPointF(rx * qCos(M_PI_4), -ry * qSin(M_PI_4))
           << rect.center() + QPointF(rx * qCos(M_PI_4), ry * qSin(M_PI_4))
           << rect.center() + QPointF(-rx * qCos(M_PI_4), ry * qSin(M_PI_4))
           << rect.center() + QPointF(-rx * qCos(M_PI_4), -ry * qSin(M_PI_4));

    return points;
}