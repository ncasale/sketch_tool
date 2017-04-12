/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <console_input.h>
#include <myglwidget.h>
#include <mytreewidget.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionClose;
    QAction *actionSave;
    QAction *actionClear;
    QAction *actionOpen;
    QAction *actionSave_As;
    QAction *actionGenerate_Scenegraph;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QSplitter *splitter;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSplitter *splitter_2;
    MyGLWidget *gl_widget;
    QWidget *scenegraphWidget;
    QHBoxLayout *horizontalLayout_2;
    MyTreeWidget *treeWidget;
    QPlainTextEdit *console_output;
    ConsoleInput *console_input;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuScenegraph;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(779, 694);
        MainWindow->setFocusPolicy(Qt::StrongFocus);
        actionClose = new QAction(MainWindow);
        actionClose->setObjectName(QStringLiteral("actionClose"));
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/Images/toolbar_icons/save-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon);
        actionClear = new QAction(MainWindow);
        actionClear->setObjectName(QStringLiteral("actionClear"));
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/Images/toolbar_icons/folder-open-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen->setIcon(icon1);
        actionSave_As = new QAction(MainWindow);
        actionSave_As->setObjectName(QStringLiteral("actionSave_As"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/Images/toolbar_icons/save_as.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave_As->setIcon(icon2);
        actionGenerate_Scenegraph = new QAction(MainWindow);
        actionGenerate_Scenegraph->setObjectName(QStringLiteral("actionGenerate_Scenegraph"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/Images/toolbar_icons/graph.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionGenerate_Scenegraph->setIcon(icon3);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        splitter = new QSplitter(centralWidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(splitter->sizePolicy().hasHeightForWidth());
        splitter->setSizePolicy(sizePolicy);
        splitter->setOrientation(Qt::Vertical);
        widget = new QWidget(splitter);
        widget->setObjectName(QStringLiteral("widget"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(20);
        sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy1);
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        splitter_2 = new QSplitter(widget);
        splitter_2->setObjectName(QStringLiteral("splitter_2"));
        splitter_2->setOrientation(Qt::Horizontal);
        gl_widget = new MyGLWidget(splitter_2);
        gl_widget->setObjectName(QStringLiteral("gl_widget"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(4);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(gl_widget->sizePolicy().hasHeightForWidth());
        gl_widget->setSizePolicy(sizePolicy2);
        splitter_2->addWidget(gl_widget);
        scenegraphWidget = new QWidget(splitter_2);
        scenegraphWidget->setObjectName(QStringLiteral("scenegraphWidget"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(1);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(scenegraphWidget->sizePolicy().hasHeightForWidth());
        scenegraphWidget->setSizePolicy(sizePolicy3);
        horizontalLayout_2 = new QHBoxLayout(scenegraphWidget);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        treeWidget = new MyTreeWidget(scenegraphWidget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(13, QStringLiteral("14"));
        __qtreewidgetitem->setText(12, QStringLiteral("13"));
        __qtreewidgetitem->setText(11, QStringLiteral("12"));
        __qtreewidgetitem->setText(10, QStringLiteral("11"));
        __qtreewidgetitem->setText(9, QStringLiteral("10"));
        __qtreewidgetitem->setText(8, QStringLiteral("9"));
        __qtreewidgetitem->setText(7, QStringLiteral("8"));
        __qtreewidgetitem->setText(6, QStringLiteral("7"));
        __qtreewidgetitem->setText(5, QStringLiteral("6"));
        __qtreewidgetitem->setText(4, QStringLiteral("5"));
        __qtreewidgetitem->setText(3, QStringLiteral("4"));
        __qtreewidgetitem->setText(2, QStringLiteral("3"));
        __qtreewidgetitem->setText(1, QStringLiteral("2"));
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        treeWidget->setColumnCount(14);

        horizontalLayout_2->addWidget(treeWidget);

        splitter_2->addWidget(scenegraphWidget);

        horizontalLayout->addWidget(splitter_2);

        splitter->addWidget(widget);
        console_output = new QPlainTextEdit(splitter);
        console_output->setObjectName(QStringLiteral("console_output"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(10);
        sizePolicy4.setHeightForWidth(console_output->sizePolicy().hasHeightForWidth());
        console_output->setSizePolicy(sizePolicy4);
        console_output->setMaximumSize(QSize(16777215, 100));
        splitter->addWidget(console_output);
        console_input = new ConsoleInput(splitter);
        console_input->setObjectName(QStringLiteral("console_input"));
        QSizePolicy sizePolicy5(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(console_input->sizePolicy().hasHeightForWidth());
        console_input->setSizePolicy(sizePolicy5);
        splitter->addWidget(console_input);

        verticalLayout->addWidget(splitter);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 779, 17));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuFile->setFocusPolicy(Qt::StrongFocus);
        menuScenegraph = new QMenu(menuBar);
        menuScenegraph->setObjectName(QStringLiteral("menuScenegraph"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuScenegraph->menuAction());
        menuFile->addAction(actionSave);
        menuFile->addAction(actionSave_As);
        menuFile->addAction(actionOpen);
        menuFile->addSeparator();
        menuFile->addAction(actionClear);
        menuScenegraph->addAction(actionGenerate_Scenegraph);
        mainToolBar->addAction(actionSave);
        mainToolBar->addAction(actionSave_As);
        mainToolBar->addAction(actionOpen);
        mainToolBar->addAction(actionGenerate_Scenegraph);
        mainToolBar->addSeparator();

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Sketch Tool", 0));
        actionClose->setText(QApplication::translate("MainWindow", "Close", 0));
        actionSave->setText(QApplication::translate("MainWindow", "Save", 0));
        actionClear->setText(QApplication::translate("MainWindow", "Clear", 0));
        actionOpen->setText(QApplication::translate("MainWindow", "Open", 0));
        actionSave_As->setText(QApplication::translate("MainWindow", "Save As", 0));
        actionGenerate_Scenegraph->setText(QApplication::translate("MainWindow", "Generate Scenegraph", 0));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0));
        menuScenegraph->setTitle(QApplication::translate("MainWindow", "Scenegraph", 0));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
