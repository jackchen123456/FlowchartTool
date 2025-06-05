#include "PentagonShape.h"
#include <QPainter>
#include <QtMath>
#include <QDebug>

PentagonShape::PentagonShape(const QRectF &rect) : PolygonShape(rect) {
    updatePolygon();
}

void PentagonShape::updatePolygon() {
    m_polygon.clear();
    QVector<QPointF> unitPoints;

    for (int i = 0; i < 5; ++i) {
        double angle = qDegreesToRadians(-90.0 + i * 72.0);
        unitPoints << QPointF(qCos(angle), qSin(angle));
    }

    QRectF unitBound = QPolygonF(unitPoints).boundingRect();

    QVector<QPointF> mappedPoints;
    for (const auto &pt: unitPoints) {
        qreal xRatio = (pt.x() - unitBound.left()) / unitBound.width();
        qreal yRatio = (pt.y() - unitBound.top()) / unitBound.height();
        qreal x = m_rect.left() + xRatio * m_rect.width();
        qreal y = m_rect.top() + yRatio * m_rect.height();
        mappedPoints << QPointF(x, y);
    }

    QPointF center = m_rect.center();
    m_polygon << applyRotation(center, mappedPoints, m_rotationAngle);
}

ShapeBase *PentagonShape::clone() const {
    PentagonShape *pentagon = new PentagonShape(*this);
    pentagon->setUuid(this->getUuid());
    return pentagon;
}

QVector<QPointF> PentagonShape::getMagneticPoints() const {
    QVector<QPointF> points;
    if (m_polygon.isEmpty())
        return points;

    // 5个角点
    for (int i = 0; i < 5; ++i) {
        points << m_polygon[i];
    }

    // 5条边的中点
    for (int i = 0; i < 5; ++i) {
        int next = (i + 1) % 5;
        points << (m_polygon[i] + m_polygon[next]) / 2;
    }

    return points;
}