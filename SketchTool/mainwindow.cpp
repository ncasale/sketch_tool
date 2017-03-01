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


void MainWindow::on_actionSave_triggered()
{
    MyGLWidget* gl_widget = this->findChild<MyGLWidget*>(QString("gl_widget"));
    gl_widget->saveFile();
}


void MainWindow::on_actionClear_triggered()
{
    MyGLWidget* gl_widget = this->findChild<MyGLWidget*>(QString("gl_widget"));
    gl_widget->clearScene();
}

void MainWindow::on_actionOpen_triggered()
{
    MyGLWidget* gl_widget = this->findChild<MyGLWidget*>(QString("gl_widget"));
    gl_widget->openFile();
}

void MainWindow::on_actionSave_As_triggered()
{
    MyGLWidget* gl_widget = this->findChild<MyGLWidget*>(QString("gl_widget"));
    gl_widget->saveAs();
}
