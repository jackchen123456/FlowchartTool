#ifndef LINEBASESHAPE_H
#define LINEBASESHAPE_H

#include "ShapeBase.h"
#include <QLineF>

class LineBaseShape : public ShapeBase {
public:
    // 端点绑定信息
    struct EndPointBinding {
        ShapeBase *targetShape = nullptr;                   // 绑定的图形
        int magneticIndex = -1;                             // 绑定的图形的磁力点索引

        EndPointBinding(ShapeBase *shape = nullptr, int index = -1)
                : targetShape(shape), magneticIndex(index) {}
    };

    LineBaseShape(const QPointF &start, const QPointF &end);

    virtual ~LineBaseShape() {};

    bool containPoint(const QPointF &point) const override;

    void moveBy(qreal dx, qreal dy) override;

    QRectF boundingRect() const override;

    QVector<QPointF> calculateHandles() const override;

    int hitHandle(const QPointF &point) const override;

    void resizeBy(qreal dx, qreal dy, int handleIndex) override;

    bool isShapeCanRotate() const override { return false; }

    QVector<QPointF> getMagneticPoints() const override;

    void setStart(const QPointF &start) { m_start = start; }

    QPointF getStart() const { return m_start; }

    void setEnd(const QPointF &end) { m_end = end; }

    QPointF getEnd() const { return m_end; }

    void setEndPoint(int index, const QPointF &point);            // 设置索引与端点的对应关系：0为线段起点，1为线段终点
    void setEndPointBinding(int index, ShapeBase *shape, int magIdx);   // 设置端点与图形的绑定关系
    void clearEndPointBinding(int index);                               // 清空端点与图形的绑定关系
    EndPointBinding getEndPointBinding(int index) const;                // 根据索引获取端点与图形的绑定关系
    void updateEndPointByBinding(int index);                            // 根据绑定关系更新端点位置

protected:
    QPointF m_start;
    QPointF m_end;
    EndPointBinding startBinding;                                // 线段起点的绑定信息
    EndPointBinding endBinding;                                  // 线段终点的绑定信息

};

#endif  // LINEBASESHAPE_H
