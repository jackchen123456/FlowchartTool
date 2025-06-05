#ifndef SHAPEBASE_H
#define SHAPEBASE_H

#include <QPainter>
#include <QColor>
#include <QRectF>
#include <QPointF>
#include <QVector>
#include <QUuid>

class ShapeBase {
public:
    ShapeBase();

    virtual ~ShapeBase() {}                             // 使用virtual时为了确保多态删除时能调用派生类的析构函数

    QUuid getUuid() const { return m_id; }

    void setUuid(const QUuid &uuid) { m_id = uuid; }

    void setPenWidth(int width) { m_penWidth = width; }

    int getPenWidth() const { return m_penWidth; }

    void setBorderColor(const QColor &color) { m_borderColor = color; }

    QColor getBorderColor() const { return m_borderColor; }

    void setFillColor(const QColor &color) { m_fillColor = color; }

    QColor getFillColor() const { return m_fillColor; }

    qreal getRotation() const { return m_rotationAngle; }

    virtual void setRotation(qreal angle);

    void setSelected(bool selected) { m_selected = selected; }

    bool isSelected() const { return m_selected; }

    virtual void draw(QPainter &painter) = 0;

    virtual bool containPoint(const QPointF &point) const = 0; // 判断某个点是否在图形中
    virtual void moveBy(qreal dx, qreal dy) = 0;               // 图形移动

    virtual QRectF boundingRect() const = 0;                   // 返回图形的边界矩形，可用于碰撞检测，判断两个图形是否相交
    virtual QVector<QPointF> calculateHandles() const = 0;     // 计算图形的控制点

    virtual int hitHandle(const QPointF &point) const = 0;     // 获取图形的控制点

    virtual void resizeBy(qreal dx, qreal dy, int handleIndex) = 0;         // 图形缩放

    virtual QVector<QPointF> getMagneticPoints() const = 0;    // 获取图形的磁力点
    virtual QString getShapeType() const = 0;

    virtual QPointF rotationButtonPosition() const;            // 设置旋转按钮位置
    virtual bool isShapeCanRotate() const = 0;                 // 判断图形是否可旋转

    virtual ShapeBase *clone() const = 0;                      // 克隆图形

    void setBorderStyle(Qt::PenStyle style) { m_borderStyle = style; }

    Qt::PenStyle getBorderStyle() const { return m_borderStyle; }

    void setText(const QString &text) { m_text = text; }

    QString getText() const { return m_text; }

    void setFont(const QFont &newFont) { m_font = newFont; }

    const QFont &getFont() const { return m_font; }

    void setFontSize(int size) { m_font.setPointSize(size); }

    int getFontSize() const { return m_font.pointSize(); }

    void setFontFamily(const QString &family) { m_font.setFamily(family); }

    QString getFontFamily() const { return m_font.family(); }

    void setFontBold(bool bold) { m_font.setBold(bold); }

    bool isFontBold() const { return m_font.bold(); }

    void setFontItalic(bool italic) { m_font.setItalic(italic); }

    bool isFontItalic() const { return m_font.italic(); }

    void setFontUnderline(bool underline) { m_font.setUnderline(underline); }

    bool isFontUnderline() const { return m_font.underline(); }

    void setFontColor(const QColor &color) { m_fontColor = color; }

    QColor getFontColor() const { return m_fontColor; }

    void setTextAlignment(Qt::Alignment alignment) { m_textAlignment = alignment; }

    Qt::Alignment getTextAlignment() const { return m_textAlignment; }

public:
    static constexpr qreal HANDLE_SIZE = 10.0;          // 设置控制点的大小
    static constexpr qreal MAGNETIC_RANGE = 8.0;        // 磁力吸附范围

protected:
    QUuid m_id;

    // 图形属性
    int m_penWidth;
    QColor m_borderColor;
    QColor m_fillColor;
    Qt::PenStyle m_borderStyle;

    // 图形文本属性
    QString m_text;
    QColor m_fontColor;
    QFont m_font;
    Qt::Alignment m_textAlignment;

    bool m_selected;                                     // 图形是否被选中
    qreal m_rotationAngle;                               // 图形的旋转角度

    virtual void updateShape() {};                                                                             // 更新图形
    void normalizeAngle();                                                                                     // 角度归一化
    static QVector<QPointF> applyRotation(const QPointF &center, const QVector<QPointF> &points, qreal angle); // 应用旋转
};

#endif // SHAPEBASE_H