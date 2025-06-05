#ifndef ARROWSHAPE_H
#define ARROWSHAPE_H

#include "LineBaseShape.h"

class ArrowShape : public LineBaseShape {
public:
    ArrowShape(const QPointF &start, const QPointF &end);

    void draw(QPainter &painter) override;

    ShapeBase *clone() const override;

    QString getShapeType() const override { return "Arrow"; }

private:
    void drawArrowHead(QPainter &painter, const QPointF &start, const QPointF &end);     // 绘制箭头头部

};

#endif // ARROWSHAPE_H