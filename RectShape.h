#ifndef RECTSHAPE_H
#define RECTSHAPE_H

#include "PolygonShape.h"

class RectShape : public PolygonShape {
public:
    explicit RectShape(const QRectF &rect);

    ShapeBase *clone() const override;

    QVector<QPointF> getMagneticPoints() const override;

    QString getShapeType() const override { return "Rect"; }

private:
    void updatePolygon() override;

};

#endif  // RECTSHAPE_H
