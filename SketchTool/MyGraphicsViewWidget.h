#ifndef MYGRAPHICSVIEWWIDGET_H
#define MYGRAPHICSVIEWWIDGET_H
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QtCore>
#include <QtGui>
#include "NodeItem.h"
#include <vector>
#include "SGraphItemInfo.h"

class MyGraphicsViewWidget : public QWidget
{
public:
    MyGraphicsViewWidget();
    using QWidget::QWidget;

    void generateScenegraph();

    vector<SGraphItemInfo> generateItems();

    void drawItem(SGraphItemInfo);

protected:

private:
    //Scene to draw upon
    QGraphicsScene* scene;

    //Graphics view into which we'll put the scene
    QGraphicsView* graphics_view;

    //Brushes and pens used for drawing
    QBrush group_node_brush;
    QBrush transform_node_brush;
    QBrush leaf_node_brush;
    QPen black_pen;

    int node_size = 50;

    vector<QGraphicsEllipseItem*> ellipses;
    //Scenegraph to traverse and draw
    //sgraph::Scenegraph* scenegraph;
};

#endif // MYGRAPHICSVIEWWIDGET_H
