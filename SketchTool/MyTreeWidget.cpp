#include "MyTreeWidget.h"

MyTreeWidget::MyTreeWidget(QWidget *parent)
    :QTreeWidget(parent)
{
    //Set Column count
    this->setColumnCount(2);

    //Add a node
    addRoot("Hello");
}

void MyTreeWidget::addRoot(QString name)
{
    //Make a new item
    MyTreeWidgetItem *item  = new MyTreeWidgetItem(this);
    item->setText(0, name);
    this->addTopLevelItem(item);
}
