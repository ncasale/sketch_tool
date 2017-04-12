#ifndef MYTREEWIDGET_H
#define MYTREEWIDGET_H
#include <QTreeWidget>
#include "mytreewidgetitem.h"
#include "sgraph/Scenegraph.h"

class MyTreeWidget : public QTreeWidget
{
public:
    explicit MyTreeWidget(QWidget *parent = Q_NULLPTR);

    //Tree building functions
    void addRoot(QString, QString);
    void addChild(MyTreeWidgetItem* , QString, QString);
    void generateScenegraphTree();
    MyTreeWidgetItem* findItemInTree(QString name);

    //Tree deletion
    void deleteAllItemsInTree();
    void itemDoubleClicked(QTreeWidgetItem*, int);



private:


};

#endif // MYTREEWIDGET_H
