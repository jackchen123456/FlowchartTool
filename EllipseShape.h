#ifndef ELLIPSESHAPE_H
#define ELLIPSESHAPE_H

#include "PolygonShape.h"

class EllipseShape : public PolygonShape {
public:
    explicit EllipseShape(const QRectF &rect);

    ShapeBase *clone() const override;

    QVector<QPointF> getMagneticPoints() const override;

    QString getShapeType() const override { return "Ellipse"; }

private:
    void updatePolygon() override;

};

#endif  // ELLIPSESHAPE_H