#include "passwordlineedit.h"
#include <QDebug>

PasswordLineEdit::PasswordLineEdit(QWidget *parent)
    :QLineEdit(parent)
{

    setEchoMode(QLineEdit::Password);

    //deley 1000 miliseconds before hide the password
    setStyleSheet(QString::fromUtf8("QLineEdit { lineedit-password-mask-delay: 1000 }"));

    QAction *actionDelete = addAction(QIcon(":/images/icon_delete.png"), QLineEdit::TrailingPosition);
    QAction *action = addAction(QIcon(":/images/icon_eyeOff.png"), QLineEdit::TrailingPosition);

    toggled = false;
    button = qobject_cast<QToolButton *>(action->associatedWidgets().last());
    button->setCheckable(true);
    button->hide();
    button->setCursor(QCursor(Qt::PointingHandCursor));

    buttonDelete = qobject_cast<QToolButton *>(actionDelete->associatedWidgets().last());
    buttonDelete->setCheckable(true);
    buttonDelete->hide();
    buttonDelete->setCursor(QCursor(Qt::PointingHandCursor));

//    connect(button, &QToolButton::pressed, this, &PasswordLineEdit::onPressed);
//    connect(button, &QToolButton::released, this, &PasswordLineEdit::onReleased);
    connect(button, &QToolButton::pressed, this, &PasswordLineEdit::reactToToggle);
    connect(buttonDelete, &QToolButton::pressed, this, &PasswordLineEdit::clear);
//    connect(button, SIGNAL(toggled(bool )), this, &PasswordLineEdit::reactToToggle);

    //setClearButtonEnabled(true);


}

void PasswordLineEdit::showEye()
{
    button->show();
    buttonDelete->show();
}

void PasswordLineEdit::hideEye()
{
    button->hide();
    buttonDelete->hide();
}

//void PasswordLineEdit::onPressed() {
//    QToolButton *button = qobject_cast<QToolButton *>(sender());
//    button->setIcon(QIcon(":/images/eyeOn.png"));
//    setEchoMode(QLineEdit::Normal);
//}

//void PasswordLineEdit::onReleased() {
//    QToolButton *button = qobject_cast<QToolButton *>(sender());
//    button->setIcon(QIcon(":/images/eye_off.svg"));
//    setEchoMode(QLineEdit::Password);
//}

void PasswordLineEdit::reactToToggle()
{
    if (toggled) {
        QToolButton *button = qobject_cast<QToolButton *>(sender());
        button->setIcon(QIcon(":/images/icon_eyeOff.png"));
        setEchoMode(QLineEdit::Password);

        QFont font;
        font.setPointSize(10);
        setFont(font);

        toggled = false;
    } else {
        QToolButton *button = qobject_cast<QToolButton *>(sender());
        button->setIcon(QIcon(":/images/icon_eyeOn.png"));
        setEchoMode(QLineEdit::Normal);

        QFont font;
        font.setPointSize(14);
        setFont(font);

        toggled = true;
    }
}

//void PasswordLineEdit::enterEvent(QEvent *event) {
//    button->show();
//    QLineEdit::enterEvent(event);
//}

//void PasswordLineEdit::leaveEvent(QEvent *event) {
//    button->hide();
//    QLineEdit::leaveEvent(event);
//}

//void PasswordLineEdit::focusInEvent(QFocusEvent *event) {
//    button->show();
//    QLineEdit::focusInEvent(event);
//}

//void PasswordLineEdit::focusOutEvent(QFocusEvent *event) {
//    button->hide();
//    QLineEdit::focusOutEvent(event);
//}
