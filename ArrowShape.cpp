#include "ArrowShape.h"
#include <QPainter>
#include <QtMath>

ArrowShape::ArrowShape(const QPointF &start, const QPointF &end)
        : LineBaseShape(start, end) {}

void ArrowShape::drawArrowHead(QPainter &painter, const QPointF &start, const QPointF &end) {
    painter.save();
    const double arrowSize = 20.0;                            // 箭头头部的大小
    QLineF line(start, end);

    double angle = std::atan2(line.dy(), line.dx());          // 计算线段与X轴的夹角（弧度）

    // 计算箭头两翼坐标
    QPointF arrowP1 = end - QPointF(std::cos(angle - M_PI / 6.0) * arrowSize,
                                    std::sin(angle - M_PI / 6.0) * arrowSize);    // 线段方向向左偏 30°的点
    QPointF arrowP2 = end - QPointF(std::cos(angle + M_PI / 6.0) * arrowSize,
                                    std::sin(angle + M_PI / 6.0) * arrowSize);    // 线段方向向右偏 30°的点

    QPolygonF arrowHead;
    arrowHead << end << arrowP1 << arrowP2;
    painter.setBrush(m_borderColor);
    painter.setPen(Qt::NoPen);            // 取消边框线
    painter.drawPolygon(arrowHead);
    painter.restore();
}

void ArrowShape::draw(QPainter &painter) {
    painter.save();

    painter.setPen(QPen(m_borderColor, m_penWidth, m_borderStyle));
    painter.drawLine(m_start, m_end);
    drawArrowHead(painter, m_start, m_end);

    if (!m_text.isEmpty()) {
        painter.save();
        QPointF mid = (m_start + m_end) / 2;
        QRectF rect(mid.x() - 40, mid.y() - 15, 80, 30);

        painter.setPen(QPen(m_fontColor));
        QFont font(m_font);
        painter.setFont(font);
        painter.drawText(rect, m_textAlignment, m_text);
        painter.restore();
    }

    if (m_selected) {
        painter.save();
        painter.setPen(QPen(Qt::blue, 1, Qt::DashLine));
        painter.setBrush(Qt::white);

        // 绘制控制点
        for (const auto &pt: calculateHandles()) {
            painter.drawRect(QRectF(pt.x() - HANDLE_SIZE / 2, pt.y() - HANDLE_SIZE / 2, HANDLE_SIZE, HANDLE_SIZE));
        }
        painter.restore();
    }

    painter.restore();
}

ShapeBase *ArrowShape::clone() const {
    ArrowShape *arrow = new ArrowShape(*this);
    arrow->setUuid(this->getUuid());
    return arrow;
}