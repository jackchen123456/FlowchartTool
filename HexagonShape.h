#ifndef HEXAGONSHAPE_H
#define HEXAGONSHAPE_H

#include "PolygonShape.h"

class HexagonShape : public PolygonShape {
public:
    explicit HexagonShape(const QRectF &rect);

    ShapeBase *clone() const override;

    QVector<QPointF> getMagneticPoints() const override;

    QString getShapeType() const override { return "Hexagon"; }

private:
    void updatePolygon() override;

};

#endif  // HEXAGONSHAPE_H