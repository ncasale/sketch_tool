#ifndef NODE_ITEM_H
#define NODE_ITEM_H
#include <QGraphicsScene>
#include <QGraphicsItem>

class NodeItem : public QGraphicsItem
{
public:
    NodeItem(QGraphicsItem *parent = 0);


protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);

private:


};


#endif // NODE_ITEM_H
