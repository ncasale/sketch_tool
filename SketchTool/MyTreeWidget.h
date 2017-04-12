#ifndef MYTREEWIDGET_H
#define MYTREEWIDGET_H
#include <QTreeWidget>
#include "mytreewidgetitem.h"

class MyTreeWidget : public QTreeWidget
{
public:
    explicit MyTreeWidget(QWidget *parent = Q_NULLPTR);

    //Tree building functions
    void addRoot(QString);
    void addChild(QString);



private:


};

#endif // MYTREEWIDGET_H
