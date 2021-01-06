#include "listwidgetitem.h"

ListWidgetItem::ListWidgetItem(int t, QListWidget *parent)
    :QListWidgetItem(parent),
      time(t)
{

}

bool ListWidgetItem::operator<(const ListWidgetItem &other)  const
{
    return this->time < other.time;
}
