#ifndef MYGRAPHICSVIEWWIDGET_H
#define MYGRAPHICSVIEWWIDGET_H
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "node_item.h"

class MyGraphicsViewWidget : public QWidget
{
public:
    using QWidget::QWidget;
    MyGraphicsViewWidget();

protected:

private:
    //Scene to draw upon
    QGraphicsScene* scene;

    //Graphics view into which we'll put the scene
    QGraphicsView* graphics_view;
};

#endif // MYGRAPHICSVIEWWIDGET_H
