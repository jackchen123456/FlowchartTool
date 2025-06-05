#ifndef POLYGONSHAPE_H
#define POLYGONSHAPE_H

#include "ShapeBase.h"

class PolygonShape : public ShapeBase {
public:
    explicit PolygonShape(const QRectF &rect);

    virtual ~PolygonShape() {};

    void draw(QPainter &painter) override;

    bool containPoint(const QPointF &point) const override;

    void moveBy(qreal dx, qreal dy) override;

    QRectF boundingRect() const override;

    QVector<QPointF> calculateHandles() const override;

    int hitHandle(const QPointF &point) const override;

    void resizeBy(qreal dx, qreal dy, int handleIndex) override;

    bool isShapeCanRotate() const override { return true; }

protected:
    virtual void updatePolygon() = 0;                    // 更新多边形的坐标点集合

    void updateShape() override { updatePolygon(); }     // 更新多边形

protected:
    QRectF m_rect;                       // 存储多边形的外接矩形框
    QPolygonF m_polygon;                 // 存储多边形的坐标点集合

};

#endif
