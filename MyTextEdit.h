#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H

#include <QTextEdit>

class MyTextEdit : public QTextEdit {
    Q_OBJECT

public:
    explicit MyTextEdit(QWidget *parent = nullptr);

signals:
    void editingFinished();                // 输入完成信号

protected:
    void focusOutEvent(QFocusEvent *event) override;        // 失去焦点事件

    void keyPressEvent(QKeyEvent *event) override;          // 键盘按下事件

};

#endif // MYTEXTEDIT_H