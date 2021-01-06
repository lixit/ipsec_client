#ifndef LISTWIDGETITEM_H
#define LISTWIDGETITEM_H

#include <QListWidgetItem>


class ListWidgetItem : public QListWidgetItem
{
    Q_OBJECT
public:
    ListWidgetItem(int t = 0, QListWidget *parent = nullptr);
    bool operator<(const ListWidgetItem &other) const;

private:
    int time;
};
#endif // LISTWIDGETITEM_H
