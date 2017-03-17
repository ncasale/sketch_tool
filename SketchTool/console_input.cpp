#include "console_input.h"
#include <QPlainTextEdit>
#include "MyGLWidget.h"
#include <iostream>

using namespace std;

/**
 * @brief ConsoleInput::keyPressEvent
 * Processes Key Press Events that occur in the console input widget. This
 * function adds aditional functionality to the QLineEdit::keyPressEvent
 * function
 *
 * @param e
 * The key press event we are processing
 */
void ConsoleInput::keyPressEvent(QKeyEvent* e)
{
    switch(e->key())
    {
    case Qt::Key_Return:
    {
        //If enter is hit - want to process what is currently in the console_input
        if(this->hasFocus())
        {
            //Take what is currently in console
            QString console_command = this->text();

            //Clear the console
            this->clear();

            //Get console output widget
            QWidget* temp = this->parent()->findChild<QWidget*>("console_output");
            QPlainTextEdit* console_output_widget = (QPlainTextEdit*) temp;

            //Process the command
            string output_text = process_command(console_command);

            //Output output string
            console_output_widget->moveCursor(QTextCursor::End);
            console_output_widget->insertPlainText(QString(output_text.c_str()));
            console_output_widget->moveCursor(QTextCursor::End);
        }
        break;
    }
    }

    QLineEdit::keyPressEvent(e);
}

/**
 * @brief ConsoleInput::process_command
 * This fucntion processes a command passed to the console_input and executes it.
 *
 * @return
 * Returns a string that will be output to the console_output
 */
string ConsoleInput::process_command(QString& command_qstring)
{
    //Let's find gl_widget in case we need it
    MyGLWidget* gl_widget = this->parent()->findChild<MyGLWidget*>("gl_widget");

    string command_string = command_qstring.toStdString();
    string command = command_string.substr(0, command_string.find_first_of(' '));

    if(command == "test")
    {
        return "Success!\n";
    }
    else if(command == "set_x")
    {
        gl_widget->set_x_axis();
        return "Selected Axis: X\n";
    }
    else if(command == "set_y")
    {
        gl_widget->set_y_axis();
        return "Selected Axis: Y\n";
    }
    else if(command == "set_z")
    {
        gl_widget->set_z_axis();
        return "Selected Axis: Z\n";
    }
    else if(command == "fps")
    {
        float fr = gl_widget->getFrameRate();
        return "FrameRate: " + to_string(fr) + " FPS\n";
    }
    else if(command == "translate")
    {
        //Begin parsing for additional parameters
        string params = command_string.substr(command_string.find_first_of(' ') + 1, command_string.npos);
        //Get x-translation amount
        float x = atof(params.substr(0, params.find_first_of(' ')).c_str());
        params = params.substr(params.find_first_of(' ') + 1, params.npos);
        //Get y-translation amount
        float y = atof(params.substr(0, params.find_first_of(' ')).c_str());
        params = params.substr(params.find_first_of(' ') + 1, params.npos);
        //Get z-translation amount
        float z = atof(params.substr(0, params.find_first_of(' ')).c_str());

        std::cout << "X: " << std::to_string(x) << " Y: " << to_string(y) << " Z: " << to_string(z) << endl;

        //Perform tranlsation
        gl_widget->parametrizedTranslation(x, y, z);

        return "Translation Successful";
    }
    else return "Invalid Command";
}
