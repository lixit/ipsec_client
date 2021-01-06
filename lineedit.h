#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QAction>
#include <QLineEdit>
#include <QToolButton>

class LineEdit : public QLineEdit {
    Q_OBJECT

public:
    LineEdit(QWidget *parent = nullptr);

    void showDelete();
    void hideDelete();

private slots:

private:
    QToolButton *buttonDelete;
};

#endif // LINEEDIT_H
