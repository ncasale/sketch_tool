#ifndef MYTREEWIDGETITEM_H
#define MYTREEWIDGETITEM_H
#include <QTreeWidget>
#include <QTreeWidgetItem>

class MyTreeWidgetItem : public QTreeWidgetItem
{
public:
    explicit MyTreeWidgetItem(QWidget* parent = 0);

    /*//Setters
    void setNode(sgraph::INode* n) {node = n;}
    void setNodeType(sgraph::NodeType t) {node_type = t;}
    void setNodeAndType(sgraph::INode* n, sgraph::NodeType t) {node = n; node_type = t;}

    //Getters
    sgraph::INode* getNode() {return node;}
    sgraph::NodeType getNodeType() {return node_type;}*/

private:
    /*
    //Node attached to this item
    sgraph::INode* node;
    //Type of node
    sgraph::NodeType node_type;
    */

};



#endif // MYTREEWIDGETITEM_H
