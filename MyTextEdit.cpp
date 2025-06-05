#include "MyTextEdit.h"
#include <QFocusEvent>
#include <QKeyEvent>

MyTextEdit::MyTextEdit(QWidget *parent)
        : QTextEdit(parent) {}

void MyTextEdit::focusOutEvent(QFocusEvent *event) {
    emit editingFinished();
    QTextEdit::focusOutEvent(event);      // 必须调用父类的focusOutEvent，否则可能破坏Qt默认的焦点链
}

void MyTextEdit::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Return && event->modifiers() == Qt::NoModifier) {     // 仅响应无修饰键
        emit editingFinished();           // 纯回车键触发信号
    }
    QTextEdit::keyPressEvent(event);      // 调用父类默认处理
}