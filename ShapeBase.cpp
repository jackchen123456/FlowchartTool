#include "ShapeBase.h"
#include <QPointF>
#include <QtMath>

ShapeBase::ShapeBase() : m_penWidth(2), m_borderColor(Qt::black), m_borderStyle(Qt::SolidLine), m_fillColor(Qt::white),
                         m_fontColor(Qt::black), m_text(""), m_font(QFont("Arial", 9)),
                         m_textAlignment(Qt::AlignCenter), m_selected(false), m_rotationAngle(0.0){
    m_id = QUuid::createUuid();
}

void ShapeBase::setRotation(qreal angle) {
    m_rotationAngle = angle;
    normalizeAngle();
    updateShape();
}

QPointF ShapeBase::rotationButtonPosition() const {
    QRectF rect = boundingRect();
    return rect.topRight() + QPointF(10, -10);
}

QVector<QPointF> ShapeBase::applyRotation(const QPointF &center, const QVector<QPointF> &points, qreal angle) {
    QVector<QPointF> rotate;
    qreal angleRad = qDegreesToRadians(angle);      // 将角度转为弧度
    qreal cosA = qCos(angleRad);
    qreal sinA = qSin(angleRad);

    for (const QPointF &pt: points) {
        // 计算点相对于中心的偏移量,将点转换到以center为原点的局部坐标系
        qreal dx = pt.x() - center.x();
        qreal dy = pt.y() - center.y();

        // 旋转后再转换回全局坐标系
        qreal x = center.x() + dx * cosA - dy * sinA;
        qreal y = center.y() + dx * sinA + dy * cosA;
        rotate << QPointF(x, y);
    }
    return rotate;
}

// 规范化角度值，确保角度 m_rotationAngle 始终落在 [-180°, 180°] 的范围内
void ShapeBase::normalizeAngle() {
    while (m_rotationAngle < -180.0)
        m_rotationAngle += 360.0;
    while (m_rotationAngle > 180.0)
        m_rotationAngle -= 360.0;
}

