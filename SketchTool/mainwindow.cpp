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
