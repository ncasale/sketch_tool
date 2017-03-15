#ifndef CONSOLE_INPUT_H
#define CONSOLE_INPUT_H

#include <QObject>
#include<QLineEdit>
#include <QKeyEvent>

using namespace std;

class ConsoleInput : public QLineEdit
{
    Q_OBJECT
public:
    using QLineEdit::QLineEdit;
    void keyPressEvent(QKeyEvent *);
    string process_command(QString&);
private:

};

#endif // CONSOLE_INPUT_H
