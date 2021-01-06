#include "lineedit.h"

LineEdit::LineEdit(QWidget *parent)
    :QLineEdit(parent)
{
    QAction *actionDelete = addAction(QIcon(":/images/icon_delete.png"), QLineEdit::TrailingPosition);

    buttonDelete = qobject_cast<QToolButton *>(actionDelete->associatedWidgets().last());
    buttonDelete->setCheckable(true);
    buttonDelete->hide();
    buttonDelete->setCursor(QCursor(Qt::PointingHandCursor));

    connect(buttonDelete, &QToolButton::pressed, this, &LineEdit::clear);

    QFont font;
    font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
    font.setPointSize(14);
    setMinimumSize(QSize(360, 50));
    setFont(font);
    setStyleSheet(QString::fromUtf8("QLineEdit{ border-width: 1px; border-style: solid; border-color: white white #D1D7E0 white; }"));
    setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
}


void LineEdit::showDelete()
{
    buttonDelete->show();
}

void LineEdit::hideDelete()
{
    buttonDelete->hide();
}
