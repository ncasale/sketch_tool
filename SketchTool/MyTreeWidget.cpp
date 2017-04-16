#include "MyTreeWidget.h"
#include "MyGLWidget.h"

MyTreeWidget::MyTreeWidget(QWidget *parent)
    :QTreeWidget(parent)
{
    //Connect double click signal
    connect(this, &QTreeWidget::itemDoubleClicked, this, &MyTreeWidget::itemDoubleClicked);
}

/**
 * @brief MyTreeWidget::generateScenegraphTree
 * Grabs the scenegraph from the gl_widget's view and recurses through it to
 * generate information about each of the nodes. The information for each node
 * is contained within a GeneratedItem object. A vector of these objects is
 * returned by the recursion and is then used to draw a tree representation
 * of the scenegraph in the right-side pane of the GUI
 */
void MyTreeWidget::generateScenegraphTree()
{
    //Get the scenegraph by accessing GL_Widget's view. Gl_widget is sibling
    //my parent
    MyGLWidget *gl_widget = this->parent()->parent()->findChild<MyGLWidget*>(QString("gl_widget"));
    Scenegraph *scenegraph = gl_widget->getScenegraph();

    //Create list of node items to create
    vector<INode::GeneratedItem> node_items;
    scenegraph->generateScenegraphTreeView(node_items);

    //Iterate through list and generate nodes
    for(auto node : node_items)
    {
        //Create tree item for node
        MyTreeWidgetItem *item = new MyTreeWidgetItem();
        item->setNode(node.node);
        item->setNodeType(node.type);

        //Assign item a parent
        if(node.parent == NULL || node.node->getName() == "Root of scene graph")
        {
            //This is the root, skip this item
            delete item;

        }
        else
        {
            //Not root, if parent is root, make this item top level
            if(node.parent == scenegraph->getRoot() || node.parent->getName() == "Root of scene graph")
            {
                string name = node.node->getName();
                item->setText(0, QString(name.c_str()));
                this->addTopLevelItem(item);
            }
            else
            {
                //If parent isn't root, find parent in tree
                MyTreeWidgetItem* parent = findItemInTree(QString(node.parent->getName().c_str()));
                parent->addChild(item);
                item->setText(0,QString(node.node->getName().c_str()));
            }

            //Set the node type in column 1
            switch(node.type)
            {
            case NodeType::GROUP:
                item->setText(1, "Group");
                break;
            case NodeType::TRANSFORM:
                item->setText(1, "Transform");
                break;
            case NodeType::LEAF:
                item->setText(1, "Leaf");
                break;
            }
        }

    }



    return;
}

/**
 * @brief MyTreeWidget::findItemInTree
 * Will find a particular MyTreeWidgetItem contained within the tree
 *
 * @param name
 * The name of the item to find
 *
 * @return
 * The MyTreeWidgetItem if it exists, NULL otherwise.
 */
MyTreeWidgetItem* MyTreeWidget::findItemInTree(QString name)
{
    //Search tree for item with matching name
    QList<QTreeWidgetItem*> list = this->findItems(name,Qt::MatchExactly | Qt::MatchRecursive, 0);

    //If list not empty, return first match
    if(!list.empty())
    {
        return (MyTreeWidgetItem*)list[0];
    }
    else
    {
        return NULL;
    }
}

/**
 * @brief MyTreeWidget::deleteAllItemsInTree
 * Clears all items in the tree, leaving an empty tree.
 */
void MyTreeWidget::deleteAllItemsInTree()
{
    this->clear();
}

/**
 * @brief MyTreeWidget::itemDoubleClicked
 * A Qt Slotted function that is called whenever an item within th tree is
 * double clicked.
 *
 * @param item
 * The item that was double clicked
 *
 * @param column
 * The column of that item that was double clicked -- not used for our purposes
 */
void MyTreeWidget::itemDoubleClicked(QTreeWidgetItem* item, int column)
{
    MyTreeWidgetItem* clicked_item = (MyTreeWidgetItem*) item;

    if(clicked_item->getNodeType() == NodeType::LEAF)
    {
        //This is an object we can select. Find GLWidget which is the sibling
        //of the tree's parent
        MyGLWidget* gl_widget = this->parent()->parent()->findChild<MyGLWidget*>("gl_widget");
        gl_widget->selectNode(clicked_item->getNode()->getName());

    }


}
