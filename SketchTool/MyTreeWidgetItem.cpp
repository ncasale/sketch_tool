#include "mytreewidgetitem.h"

MyTreeWidgetItem::MyTreeWidgetItem(QWidget *parent)
    :QTreeWidgetItem(parent)
{
    //Do stuff here
    this->setText(0,"Test");
}
