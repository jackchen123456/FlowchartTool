#ifndef DIAMONDSHAPE_H
#define DIAMONDSHAPE_H

#include "PolygonShape.h"

class DiamondShape : public PolygonShape {
public:
    explicit DiamondShape(const QRectF &rect);

    ShapeBase *clone() const override;

    QVector<QPointF> getMagneticPoints() const override;

    QString getShapeType() const override { return "Diamond"; }

private:
    void updatePolygon() override;

};

#endif  // DIAMOND_SHAPE_H