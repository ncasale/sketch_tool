#include "MyTreeWidget.h"
#include "MyGLWidget.h"

MyTreeWidget::MyTreeWidget(QWidget *parent)
    :QTreeWidget(parent)
{
    //Connect double click signal
    connect(this, &QTreeWidget::itemDoubleClicked, this, &MyTreeWidget::itemDoubleClicked);
}

void MyTreeWidget::addRoot(QString name, QString type)
{
    //Make a new item
    MyTreeWidgetItem *item  = new MyTreeWidgetItem(this);
    item->setText(0, name);
    item->setText(1,type);
    this->addTopLevelItem(item);

    //Add some sample children
    addChild(item, "One", "Group");
    addChild(item, "Two", "Transform");
}

void MyTreeWidget::addChild(MyTreeWidgetItem *parent, QString name, QString type)
{
    //Make a child
    MyTreeWidgetItem *child = new MyTreeWidgetItem();
    child->setText(0,name);
    child->setText(1,type);
    parent->addChild(child);
}

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

void MyTreeWidget::deleteAllItemsInTree()
{
    this->clear();
}

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
