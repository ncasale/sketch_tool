#include "MyGraphicsViewWidget.h"
#include <QGraphicsEllipseItem>
#include <vector>
#include "MyGLWidget.h"

MyGraphicsViewWidget::MyGraphicsViewWidget()
{


//    graphics_view->show();
    
}

void MyGraphicsViewWidget::generateScenegraph()
{
    scene = new QGraphicsScene(this);

    //Find our graphics view child
    graphics_view = this->findChild<QGraphicsView*>("scenegraphView");

    //Set scene for graphics view
    graphics_view->setScene(scene);

    //Set up brushes and pens for drawing
    group_node_brush = QBrush(Qt::red);
    transform_node_brush = QBrush(Qt::blue);
    leaf_node_brush = QBrush(Qt::green);
    black_pen = QPen(Qt::black);

    black_pen.setWidth(2);

    //Traverse the scenegraph and generate items to draw
    vector<SGraphItemInfo> items = generateItems();

    //Draw all items
    for(auto info : items)
    {
        drawItem(info);
    }
    //QGraphicsEllipseItem* ellipse = scene->addEllipse(10, 10, 20, 20, black_pen, group_node_brush);
}


vector<SGraphItemInfo> MyGraphicsViewWidget::generateItems()
{
    //Function will recurse through scenegraph nodes and have each draw
    //itself in the scenegraph selection pane

    vector<SGraphItemInfo> items;

    //Get Scenegraph
    MyGLWidget* gl_widget = this->parent()->findChild<MyGLWidget*>("gl_widget");
    sgraph::Scenegraph* sgraph = gl_widget->getScenegraph();

    //Traverse scenegraph to generate items
    items = sgraph->drawScenegraphPane();
    /*for(auto item : sgraph->drawScenegraphPane())
    {
        items.push_back(item);
    }*/

    return items;

}

void MyGraphicsViewWidget::drawItem(SGraphItemInfo info)
{
    QBrush brush;
    //Convert info into item and draw
    if(info.getNodeName() == "group")
        brush = group_node_brush;
    else if(info.getNodeName() == "transform")
        brush = transform_node_brush;
    else
        brush = leaf_node_brush;

    QGraphicsEllipseItem* ellipse = scene->addEllipse(info.getDrawX(), info.getDrawY(), node_size, node_size, black_pen,brush);
    ellipses.push_back(ellipse);
}
