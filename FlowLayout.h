﻿#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include <QWidget>
#include <QLayout>
#include <QRect>
#include <QStyle>

class FlowLayout : public QLayout {
    Q_OBJECT
public:
    explicit FlowLayout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);

    explicit FlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);

    ~FlowLayout();

    void addItem(QLayoutItem *item) override;

    int horizontalSpacing() const;

    int verticalSpacing() const;

    Qt::Orientations expandingDirections() const override;

    bool hasHeightForWidth() const override;

    int heightForWidth(int) const override;

    int count() const override;

    QLayoutItem *itemAt(int index) const override;

    QSize minimumSize() const override;

    void setGeometry(const QRect &rect) override;

    QSize sizeHint() const override;

    QLayoutItem *takeAt(int index) override;

private:
    int doLayout(const QRect &rect, bool testOnly) const;  // 计算布局
    int smartSpacing(QStyle::PixelMetric pm) const;        // 间距计算

    QList<QLayoutItem *> itemList;       // 存储所有子项
    int m_hSpace;
    int m_vSpace;

};

#endif  // FLOWLAYOUT_H