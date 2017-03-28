#include "mygraphicsviewwidget.h"

MyGraphicsViewWidget::MyGraphicsViewWidget()
{
    scene = new QGraphicsScene(-200,-200,400,400);

    graphics_view = new QGraphicsView(scene);

    graphics_view->setRenderHint(QPainter::Antialiasing);
    graphics_view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    graphics_view->setBackgroundBrush(QColor(230, 200, 167));

    graphics_view->show();
}
