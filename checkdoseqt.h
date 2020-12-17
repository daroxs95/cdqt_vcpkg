#ifndef CHECKDOSEQT_H
#define CHECKDOSEQT_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include <Eigen/Dense>

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QTableWidget>
#include <QDialog>
#include <QMainWindow>
#include <QTableWidgetItem>

#include "tuberosum_tools/potato_linac.h"

using namespace Eigen;

QT_BEGIN_NAMESPACE
namespace Ui { class CheckDoseQt; }
QT_END_NAMESPACE

class CheckDoseQt : public QMainWindow
{
    Q_OBJECT

public:
    CheckDoseQt(QWidget *parent = nullptr); /* Declare a constructor that has a default argument called parent.
                                            ** The value 0 indicates that the widget has no parent
                                            ** (it is a top-level widget).
                                            */
    ~CheckDoseQt();


private slots:

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_numberOfFields_valueChanged(int arg1);

    void on_globalEnergy_currentIndexChanged(const QString &arg1);

    void on_treatmentMach_currentIndexChanged(const QString &arg1);

    void on_calculateButton_clicked();

    void on_beamSummaryTable_itemChanged(QTableWidgetItem *item);


private:
    Ui::CheckDoseQt *ui;

    int VerifyData();

    int CheckItemValue(QTableWidgetItem *);

    int MachineLoad(const QString &arg1);

    Potato_Linac* DefaultCalcLinac;

};
#endif // CHECKDOSEQT_H
