#include <QApplication>
#include <QMainWindow>
#include "openglwindow.h"
int main(int argc, char *argv[])
{
    //create a main application within which this window will be
   QApplication app(argc, argv);

   //add our window to this application
   OpenGLWindow window;

   /*

   //Add another window for our menu
   QMainWindow *menu_window = new QMainWindow();
   //Create a central widget for menu_window
   QWidget *central_widget = new QWidget(menu_window);
   //Create menu
   QMenuBar *menu_bar = new QMenuBar(central_widget);
   //Create our menus on menu bar
   QMenu* file_menu = new QMenu("File");
   file_menu->addAction("Save");
   //Add File menu to Menu bar
   menu_bar->addMenu(file_menu);
   menu_window->setCentralWidget(central_widget);
   menu_window->show();
   */

   //show this window
   window.show();

   //give control to the application
   return app.exec();
}
