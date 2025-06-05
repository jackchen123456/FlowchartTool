#include "LineShape.h"
#include <QPainter>

LineShape::LineShape(const QPointF &start, const QPointF &end) : LineBaseShape(start, end) {}

void LineShape::draw(QPainter &painter) {
    painter.save();

    painter.setPen(QPen(m_borderColor, m_penWidth, m_borderStyle));
    painter.drawLine(m_start, m_end);

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

// 深拷贝
ShapeBase *LineShape::clone() const {
    LineShape *line = new LineShape(*this);
    line->setUuid(this->getUuid());     // 复制UUID
    return line;
}