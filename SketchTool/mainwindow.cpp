#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MyGLWidget.h"
#include <iostream>
#include <QtCore>
#include <QtGui>
#include <QTextEdit>
#include <QSplitter>
#include <QVBoxLayout>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::on_actionSave_triggered
 * This function is called whenever the actionSave_triggered signal is emitted
 */
void MainWindow::on_actionSave_triggered()
{
    MyGLWidget* gl_widget = this->findChild<MyGLWidget*>(QString("gl_widget"));
    gl_widget->saveFile();
}

/**
 * @brief MainWindow::on_actionClear_triggered
 * This function is called whenever the actionClear_triggered signal is emitted
 */
void MainWindow::on_actionClear_triggered()
{
    MyGLWidget* gl_widget = this->findChild<MyGLWidget*>(QString("gl_widget"));
    MyTreeWidget* tree_widget = this->findChild<MyTreeWidget*>(QString("treeWidget"));
    tree_widget->deleteAllItemsInTree();
    gl_widget->clearScene();
}

/**
 * @brief MainWindow::on_actionOpen_triggered
 * This function is called whenever the actionOpen_triggered signal is emitted
 */
void MainWindow::on_actionOpen_triggered()
{
    MyGLWidget* gl_widget = this->findChild<MyGLWidget*>(QString("gl_widget"));
    gl_widget->openFile();
}

/**
 * @brief MainWindow::on_actionSave_As_triggered
 * This function is called whenever the actionSave_As_triggered signal is
 * emitted
 */
void MainWindow::on_actionSave_As_triggered()
{
    MyGLWidget* gl_widget = this->findChild<MyGLWidget*>(QString("gl_widget"));
    gl_widget->saveAs();
}

/**
 * @brief MainWindow::on_actionGenerate_Scenegraph_triggered
 * Begins the generation of the scenegraph tree view located in the right-side
 * pane of the GUI
 */
void MainWindow::on_actionGenerate_Scenegraph_triggered()
{
    MyTreeWidget* tree_widget = this->findChild<MyTreeWidget*>(QString("treeWidget"));
    tree_widget->deleteAllItemsInTree();
    tree_widget->generateScenegraphTree();
}

/**
 * @brief MainWindow::on_actionResetCamera_triggered
 * Resets the camera to its original position
 */
void MainWindow::on_actionResetCamera_triggered()
{
    MyGLWidget* gl_widget = this->findChild<MyGLWidget*>("gl_widget");
    gl_widget->revertCamera();
}
