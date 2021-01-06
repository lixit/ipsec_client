#ifndef PASSWORDLINEEDIT_H
#define PASSWORDLINEEDIT_H

#include <QAction>
#include <QLineEdit>
#include <QToolButton>

class PasswordLineEdit : public QLineEdit {
    Q_OBJECT

public:
    PasswordLineEdit(QWidget *parent = nullptr);

    void showEye();
    void hideEye();

private slots:
//    void onPressed();
//    void onReleased();
    void reactToToggle();

protected:
//    void enterEvent(QEvent *event) override;
//    void leaveEvent(QEvent *event) override;
//    void focusInEvent(QFocusEvent *event) override;
//    void focusOutEvent(QFocusEvent *event) override;

private:
    QToolButton *button;
    QToolButton *buttonDelete;
    bool toggled;
};

#endif // PASSWORDLINEEDIT_H
