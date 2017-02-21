#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MyGLWidget.h"
#include <iostream>
#include <QtCore>
#include <QtGui>
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
    MyGLWidget* cent_widget = (MyGLWidget*)this->centralWidget();
    cent_widget->saveFile();
}


void MainWindow::on_actionClear_triggered()
{
    MyGLWidget* cent_widget = (MyGLWidget*)this->centralWidget();
    cent_widget->clearScene();
}

void MainWindow::on_actionOpen_triggered()
{

}
