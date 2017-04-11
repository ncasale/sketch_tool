#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionSave_triggered();

    void on_actionSave2_triggered();

    void on_actionClear_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_As_triggered();

    void on_actionGenerate_Scenegraph_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
