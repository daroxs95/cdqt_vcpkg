#define PEEL_WRAPPERS_IMPLEMENTATIONS

#include "checkdoseqt.h"
#include <chrono>
#include <iostream>
#include <QApplication>

using namespace std;
using namespace std::chrono;
int main(int argc, char *argv[])
{
  auto start_stopwatch0 = steady_clock::now();
  QApplication a(argc, argv); /*
                              ** It creates a QApplication object:
                              ** This object manages application-wide resources
                              ** and is necessary to run any Qt program that uses
                              ** Qt Widgets. It constructs an application object
                              ** with argc command line arguments run in argv.
                              */
  cout << ">>> QApp created in: "<<
          chrono::duration_cast<chrono::microseconds>
          (steady_clock::now() - start_stopwatch0).count ()
       << " us"<< endl;

    auto start_stopwatch = steady_clock::now();
    CheckDoseQt w;              /*
                                ** It creates the CheckDoseQt object.
                                ** This is the object for which
                                ** the wizard created the class and the UI file.
                                */
    cout << ">>> CheckDoseQt widget instanciated in: "<<
            chrono::duration_cast<chrono::microseconds>
            (steady_clock::now() - start_stopwatch).count ()
         << " us"<< endl;

    auto start_stopwatch1 = steady_clock::now();
    w.show();                   // Shows the widget on the screen in its own window

    cout << ">>> CheckDoseQt widget showed in: "<<
            chrono::duration_cast<chrono::microseconds>
            (steady_clock::now() - start_stopwatch1).count ()
         << " us"<< endl;

    return a.exec();           /*
                               ** Makes the QApplication enter its event loop.
                               ** When a Qt application is running, events are generated
                               ** and sent to the widgets of the application.
                               */
}
