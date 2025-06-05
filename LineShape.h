#ifndef LINESHAPE_H
#define LINESHAPE_H

#include "LineBaseShape.h"

class LineShape : public LineBaseShape {
public:
    LineShape(const QPointF &start, const QPointF &end);

    void draw(QPainter &painter) override;

    ShapeBase *clone() const override;

    QString getShapeType() const override { return "Line"; }

};

#endif  // LINESHAPE_H