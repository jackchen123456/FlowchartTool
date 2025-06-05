#ifndef PENTAGONSHAPE_H
#define PENTAONSHAPE_H

#include "PolygonShape.h"

class PentagonShape : public PolygonShape {
public:
    explicit PentagonShape(const QRectF &rect);

    ShapeBase *clone() const override;

    QVector<QPointF> getMagneticPoints() const override;

    QString getShapeType() const override { return "Pentagon"; }

private:
    void updatePolygon() override;

};

#endif  // PENTAGONSHAPE_H