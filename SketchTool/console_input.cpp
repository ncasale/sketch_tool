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
        if(gl_widget->isNodeSelected())
        {
            //Begin parsing for additional parameters
            string params = command_string.substr(command_string.find_first_of(' ') + 1, command_string.npos);
            //Get x-translation amount
            string first_param = params.substr(0, params.find_first_of(' '));


            if(first_param[0] == 'x' || first_param[0] == 'X')
            {
                //Adjust camera - select axis
                gl_widget->adjustCameraToSelectedNode();
                gl_widget->set_x_axis();
                gl_widget->setTranslationState();

            }
            else if(first_param[0] == 'y' || first_param[0] == 'Y')
            {
                //Adjust camera - select axis
                gl_widget->adjustCameraToSelectedNode();
                gl_widget->set_y_axis();
                gl_widget->setTranslationState();

            }
            else if(first_param[0] == 'z' || first_param[0] == 'Z')
            {
                //Adjust camera - select axis
                gl_widget->adjustCameraToSelectedNode();
                gl_widget->set_z_axis();
                gl_widget->setTranslationState();

            }
            else
            {
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
            }

            gl_widget->setFocus();
            return "Translation Successful\n";
        }
        else
            return "No Node Selected for Translation!\n";

    }
    else if(command == "rotate")
    {
        //Begin parsing for additional parameters
        string params = command_string.substr(command_string.find_first_of(' ') + 1, command_string.npos);

        //Get x-translation amount
        string first_param = params.substr(0, params.find_first_of(' '));


        if(first_param[0] == 'x' || first_param[0] == 'X')
        {
            //Adjust camera - select axis
            gl_widget->adjustCameraToSelectedNode();
            gl_widget->set_x_axis();
            gl_widget->setRotationState();

        }
        else if(first_param[0] == 'y' || first_param[0] == 'Y')
        {
            //Adjust camera - select axis
            gl_widget->adjustCameraToSelectedNode();
            gl_widget->set_y_axis();
            gl_widget->setRotationState();

        }
        else if(first_param[0] == 'z' || first_param[0] == 'Z')
        {
            //Adjust camera - select axis
            gl_widget->adjustCameraToSelectedNode();
            gl_widget->set_z_axis();
            gl_widget->setRotationState();

        }
        else
        {
            //Get the angle of rotation
            float angle = atof(params.substr(0, params.find_first_of(' ')).c_str());
            params = params.substr(params.find_first_of(' ') + 1, params.npos);
            //Rotating about X?
            float x = atof(params.substr(0, params.find_first_of(' ')).c_str());
            params = params.substr(params.find_first_of(' ') + 1, params.npos);
            //Rotating about Y?
            float y = atof(params.substr(0, params.find_first_of(' ')).c_str());
            params = params.substr(params.find_first_of(' ') + 1, params.npos);
            //Rotating about Z?
            float z = atof(params.substr(0, params.find_first_of(' ')).c_str());

            //Value check
            if(x != 1.0f && x != 0.0f)
                return "Invalid parameter: x";
            else
            {
                //
            }


            //Perform rotation
            gl_widget->parametrizedRotation(angle,x, y, z);
        }

        gl_widget->setFocus();
        return "Rotation Successful!\n";


    }
    else if(command == "scale")
    {
        //Begin parsing for additional parameters
        string params = command_string.substr(command_string.find_first_of(' ') + 1, command_string.npos);

        string first_param = params.substr(0, params.find_first_of(' '));


        if(first_param[0] == 'x' || first_param[0] == 'X')
        {
            //Adjust camera - select axis
            gl_widget->adjustCameraToSelectedNode();
            gl_widget->set_x_axis();
            gl_widget->setScaleState();

        }
        else if(first_param[0] == 'y' || first_param[0] == 'Y')
        {
            //Adjust camera - select axis
            gl_widget->adjustCameraToSelectedNode();
            gl_widget->set_y_axis();
            gl_widget->setScaleState();

        }
        else if(first_param[0] == 'z' || first_param[0] == 'Z')
        {
            //Adjust camera - select axis
            gl_widget->adjustCameraToSelectedNode();
            gl_widget->set_z_axis();
            gl_widget->setScaleState();

        }
        else if(first_param == "all")
        {
            //Adjust camera - select all axes
            gl_widget->adjustCameraToSelectedNode();
            gl_widget->setAllAxes();
            gl_widget->setScaleState();
        }
        else
        {

            //X Scale
            float x = atof(params.substr(0, params.find_first_of(' ')).c_str());
            params = params.substr(params.find_first_of(' ') + 1, params.npos);
            //Y Scale
            float y = atof(params.substr(0, params.find_first_of(' ')).c_str());
            params = params.substr(params.find_first_of(' ') + 1, params.npos);
            //Z Scale
            float z = atof(params.substr(0, params.find_first_of(' ')).c_str());

            //Perform rotation
            gl_widget->parametrizedScale(x, y, z);
        }

        gl_widget->setFocus();
        return "Scale Successful!";

    }
    else if(command == "test_circle")
    {
        //Use to test the circle tracing functionality -- unit circle values
        QPointF p1;
        p1.setX(1.0+1.0);
        p1.setY(0.0);
        QPointF p2;
        p2.setX(sqrt(3.0)/2.0 + 1.0);
        p2.setY(0.5);
        QPointF p3;
        p3.setX(sqrt(2.0)/2.0 + 1.0);
        p3.setY(sqrt(2.0)/2.0);
        QPointF p4;
        p4.setX(0.5 + 1.0);
        p4.setY(sqrt(3.0)/2.0);
        QPointF p5;
        p5.setX(0.0 + 1.0);
        p5.setY(1.0);


        //Add all points to mouse_path
        vector<QPointF> path = {p1,p2, p3, p4};
        gl_widget->setMousePath(path);

        Circle c = gl_widget->detectCircle();

        return to_string(c.get_error());


    }
    else if(command == "revert_camera")
    {
        //Reverts camera to original position
        gl_widget->revertCamera();
    }
    else if(command == "select")
    {
        //Selects a particular node from the scenegraph if it exists
        string params = command_string.substr(command_string.find_first_of(' ') + 1, command_string.npos);

        string first_param = params.substr(0, params.find_first_of(' '));

        gl_widget->selectNode(first_param);

        return "\n";

    }
    else return "Invalid Command";
}
