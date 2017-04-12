#include "mytreewidgetitem.h"

MyTreeWidgetItem::MyTreeWidgetItem(QTreeWidget *parent)
    :QTreeWidgetItem(parent)
{
    //Do stuff here
    this->setText(0,"Test");
}
