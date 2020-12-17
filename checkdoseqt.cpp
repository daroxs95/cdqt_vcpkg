#include "checkdoseqt.h"
#include "./ui_checkdoseqt.h"

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <unsupported/Eigen/Splines>

#include <fstream>
#include <iostream>
#include <string>

#include <chrono>
#include <time.h>

#include "tuberosum_tools/peel_wrappers.h"
#include "tuberosum_tools/potato_linac.h"

using namespace std;
using namespace Eigen;
using namespace std::chrono;

CheckDoseQt::CheckDoseQt(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CheckDoseQt)
{
    ui->setupUi(this);
}

CheckDoseQt::~CheckDoseQt()
{
    delete ui;
}

void CheckDoseQt::on_actionOpen_triggered() {}

void CheckDoseQt::on_actionSave_triggered() {
  QString fileNameSaved = QFileDialog::getSaveFileName(
      this, tr("Save File"), QString(),
      tr("Text Files (*.txt);;C++ Files(*.cpp, *.h)"));
}

void CheckDoseQt::on_numberOfFields_valueChanged(int arg1) {
  /**
   * Create field editable columns
   *
   * Setup the table to match the number of beams
   * selected, creates the beams in table with
   * default global values of machine and energy
   *
   * @internal
   */

  this->ui->beamSummaryTable->setColumnCount(arg1);

  for (int i = 0; i < arg1; i++) {
    // Setup Beam Number (deprecated, should be removed in version 2.0)
      int beamNumberRowInTable = 0; // of 9 possible rows (look at UI)

    QTableWidgetItem *int_item =
        new QTableWidgetItem(QString::number(i+1));
    this->ui->beamSummaryTable->setItem(beamNumberRowInTable, i, int_item);

    // Setup treatment machine value in Table's item
    QTableWidgetItem *treatMach =
        new QTableWidgetItem(this->ui->treatmentMach->currentText());
    int treatMachPosRowInTable = 1; // of 9 possible rows (look at UI)
    this->ui->beamSummaryTable->setItem(treatMachPosRowInTable, i, treatMach);

    // Setup energy value in Table's item
    QTableWidgetItem *selectedEnergy =
        new QTableWidgetItem(this->ui->globalEnergy->currentText());
    int selectedEnergyPosRowInTable = 2; // of 9 possible rows (look at UI)
    this->ui->beamSummaryTable->setItem(selectedEnergyPosRowInTable, i,
                                        selectedEnergy);
  }
}

void CheckDoseQt::on_globalEnergy_currentIndexChanged(const QString &arg1) {
  /**
   * Setup beams energies to match global one
   *
   * Change the energy value in each beam column in table
   * to match the selected global energy
   *
   * @internal
   */
  int total_beam_count = this->ui->beamSummaryTable->columnCount();

  for (int i = 0; i < total_beam_count; i++) {
    // Setup energy value in Table's item
    QTableWidgetItem *selectedEnergy = new QTableWidgetItem(arg1);
    int selectedEnergyPosRowInTable = 2; // of 9 possible rows (look at UI)
    this->ui->beamSummaryTable->setItem(selectedEnergyPosRowInTable, i,
                                        selectedEnergy);
  }
}

void CheckDoseQt::on_treatmentMach_currentIndexChanged(const QString &arg1) {
  /**
   * Setup the current selected machine
   *
   * (Re)Creates the Machine item and setup its value
   */
  int total_beam_count = this->ui->beamSummaryTable->columnCount();
  for (int i = 0; i < total_beam_count; i++) {
    // Setup treatment machine value in Table's item
    QTableWidgetItem *treatMach = new QTableWidgetItem(arg1);
    int treatMachPosRowInTable = 1; // of 9 possible rows (look at UI)
    this->ui->beamSummaryTable->setItem(treatMachPosRowInTable, i, treatMach);
  }
}

int CheckDoseQt::VerifyData() {
  /**
   * Check every hand-editable table's value for
   * range consistency through CheckTableItem
   *
   * @returns 0 if data entered is valid for calculations
   * @returns status of CheckItemValues if item check fails
   * @returns -1 if item doesn't exists
   *
   * @see CheckItemValues
   */

  int col_count = this->ui->beamSummaryTable->columnCount();
  int row_count = this->ui->beamSummaryTable->rowCount();

  for (int col_i = 0; col_i < col_count; col_i++) {
    for (int row_i = 0; row_i < row_count - 1; row_i++) {
      // next line is failing when item is empty
      QTableWidgetItem *an_item =
          this->ui->beamSummaryTable->item(row_i, col_i);

      if (an_item)
        {
          //!< If the item exists, checks if has  value
          int status = CheckItemValue(an_item); // weird
          if (status != 0)
            return status; // value if test fails
        }
      else
          return -1; //!< @returns -1 if the item doesn't exists
    }
  }
  return 0;
}

int CheckDoseQt::CheckItemValue(QTableWidgetItem *an_item) {
  /**
   * Check if the data in a item
   * is valid attending
   * to its corresponding data type
   * determined by the row it occupies.
   * If item cell is empty returns 2.
   *
   * @returns -1 if item doesn't exists
   * @returns  0 if item value is valid
   * @returns  1 if item check value is invalid or out of range
   * @returns  2 if item has empty value
   *
   * @see VerifyData
   */

  // Null checks an_item to avoid segmentation due to invalid arg
  if (!an_item) {
      //!< @todo: Check if still neccessary
    return -1;
  }

  int row = an_item->row();

  int min_row_to_check = 2;
  int max_row_to_check = 8;

  // exclude innecesary columns from checking
  if ((row < min_row_to_check) || (row > max_row_to_check))
    return 0;

  QString tmp_item_value = an_item->text();
  if (tmp_item_value.length() == 0) {
    return 2;
  }

  switch (row) {

    case 2: {
        const int &E_index =
            this->ui->globalEnergy->findText (
              tmp_item_value, Qt::MatchContains);

        if (E_index>-1)
          {
            an_item->setText (this->ui->globalEnergy->itemText (E_index));
            return 0;
          }
        else
          return 1;
    }

  case 3: {
    float min_SSD_value = 80.;
    float max_SSD_value = 120.;
    float current_SSD_value = tmp_item_value.toFloat(nullptr);

    if (!((min_SSD_value < current_SSD_value) &&
          (current_SSD_value < max_SSD_value)))
      return 1;
    return 0;
  }

  case 4: {
    float min_field_size_x = 2.;
    float max_field_size_x = 30.;
    float current_field_size_x = tmp_item_value.toFloat(nullptr);

    if (!((min_field_size_x < current_field_size_x) &&
          (current_field_size_x < max_field_size_x)))
      return 1;
    return 0;
  }

  case 5: {
    float min_field_size_y = 2.;
    float max_field_size_y = 40.;
    float current_field_size_y = tmp_item_value.toFloat(nullptr);

    if (!((min_field_size_y < current_field_size_y) &&
          (current_field_size_y < max_field_size_y)))
      return 1;
    return 0;
  }

  case 6: {
    float min_effec_depth = 0.;
    float max_effec_depth = 20.;
    float current_effec_depth = tmp_item_value.toFloat(nullptr);

    if (!((min_effec_depth < current_effec_depth) &&
          (current_effec_depth < max_effec_depth)))
      return 1;
    return 0;
  }

  case 7: {
    float min_MU = 0.;
    float max_MU = 1000.;
    float current_MU = tmp_item_value.toFloat(nullptr);

    if (!((min_MU < current_MU) &&
          (current_MU <= max_MU)))
      return 1;
    return 0;
  }

  case 8: {
    float min_Dose = 0.;
    float max_Dose = 1000.;
    float current_Dose = tmp_item_value.toFloat(nullptr);

    if (!((min_Dose < current_Dose) && (current_Dose <= max_Dose)))
      return 1;
    return 0;
  }

  break;
  default:
      return 1;
    break;
  }
}

void CheckDoseQt::on_calculateButton_clicked() {
  auto start_stopwatch = steady_clock::now();

  /***  BEGIN TABLE's DATA VERIFICATION*/
  if (VerifyData() != 0) {
    QMessageBox CheckingInfo(this);
    CheckingInfo.setText("Verifique los datos ingresados");
    CheckingInfo.setModal(true); // already modal???
    CheckingInfo.setWindowTitle("Error");
    CheckingInfo.exec();
    CheckingInfo.show();

    cout << "VerifyData returned non-zero value\n"; // take some action
    return; // to exit function when Table's values missmatches
  }
  /***  END TABLE's DATA VERIFICATION*/

  /***  BEGIN EXTERNAL DATA LOAD*/
  if (MachineLoad(this->ui->treatmentMach->currentText())) {
    cerr << "Machine loaded unsucessfully" << endl;
    return;
  }
  /***  END EXTERNAL DATA LOAD*/


  /***  BEGIN DOSE CALCULATION*/
  //!< @bug In early commits it crashed when called item didn't exist
  //!  now it doesn't happen

  double D_0 = .81;
  //!< @todo #URGENT Redefine D_0 (alias rendimiento) as a potato_linac member

  double SSD_0 = 100.;
  //!< @todo #URGENT Redefine SSD_0 (reference conditions' SSD) as a potato_linac member

  double OAR (1.), TF (1.), WF (1.);
  double &WP = TF;
  //!< @todo Define some method to use wedged beams

  /*
   * TG-114 (A1 formula page 16(519))
   *
  D = MU· D_0· S_c(r_c)· S_p(r_d)· TPR(d,rd)· ({(SSD_0 +d_0)/SPD}^2)· OAR(d,x)· TF· WF(d,r,x)
  */

  //!< @todo Functionalize this process
  for (int i=0; i<this->ui->numberOfFields->text ().toInt (); i++)
    {
      double D_p =
          this->ui->beamSummaryTable->item (8,i)->text ().toDouble ();

      double SPD =
          this->ui->beamSummaryTable->item (3,i)->text ().toDouble ();

      double eq_Fsize =
          std::sqrt(
          this->ui->beamSummaryTable->item (4,i)->text ().toDouble ()
          * this->ui->beamSummaryTable->item (5,i)->text ().toDouble () );

      double eff_depth =
          this->ui->beamSummaryTable->item (6,i)->text ().toDouble ();

      double MU =
          this->ui->beamSummaryTable->item (7,i)->text ().toDouble ();

      const int &E_index =
          this->ui->globalEnergy->findText (
            this->ui->beamSummaryTable->item (2,i)->text (), Qt::MatchContains);

      double TPR =
          DefaultCalcLinac->getAvalBeams()[E_index].getTPR(eq_Fsize, eff_depth);
      double S_c =
          DefaultCalcLinac->getAvalBeams()[E_index].getSc(eq_Fsize);
      double S_p =
          DefaultCalcLinac->getAvalBeams()[E_index].getSp(eq_Fsize);

      double D = MU * D_0* S_c* S_p* TPR* OAR* WF* WP*
          (std::pow ((SPD/(SSD_0 + eff_depth)), 2.));

      double discrepancy = (D - D_p)*100/D_p;

      cout << "Calculated DOSE is " << D << endl;

      cout << "The difference is " << discrepancy << " %";

      QTableWidgetItem *answer_item =
          new QTableWidgetItem(QString::number(discrepancy, 'f', 2).append(" %"));
      //!< casting in Qt is a horrible task
      this->ui->beamSummaryTable->setItem(9, i, answer_item);

      cout << "\tI DO CALCULATE" << endl; // take some action
      cout<<"=========End Tests=========="<<endl;
        /*** END TESTS */

    }


  cout << ">>> Calculate button shit done in: "<<
          chrono::duration_cast<chrono::microseconds>
          (steady_clock::now() - start_stopwatch).count ()
       << " us"<< endl;
}


void CheckDoseQt::on_beamSummaryTable_itemChanged(QTableWidgetItem *item) {
  /**
   * Call the CheckItemvalue funtion to determine
   * wheter is valid or not the item value. Align
   * its content and highlight in error case.
   *
   * @arg *item that changed and needs to be checked
   * @internal
   */

  item->setTextAlignment(
      Qt::AlignCenter); // center the item every time it's changed

  if ((CheckItemValue(item)) % 2 == 0) {
    item->setBackground(Qt::color0); // highligth the mistake

  } else {
    item->setBackground(Qt::red); // highligth the mistake
  }

  this->ui->beamSummaryTable->setCurrentCell(item->row() + 1, item->column());
}

int CheckDoseQt::MachineLoad(const QString &arg1) {
  /**
   * Loads data corresponding to the
   * selected machine (passed as &arg1)
   *
   * Normally, this function is intended to
   * be called using some of the energy selector
   * combo box values as arg1
   *
   * @arg arg1 The machine to be loaded (actual Linac ID must be provided)
   */

  //!< @todo Implement this path as a global variable or
  //! or similar outside accesible value until automatic selection
  //! through database is comleted on future release.

  std::string Machine_Installation_Path = "./Machines/";
  std::string& MIP = Machine_Installation_Path;

  //DONE: Part of the following behavior must be implemented
  // in the LoadMachine method

  // Instanciate the calculation machine
  //!< @todo Make this global or similar

  //!< @todo Check the Potato_Linac constructor
  //!< @bug This causes program to crash if arg1 isn't correct
  DefaultCalcLinac = new Potato_Linac(MIP + arg1.toStdString ());

  //!< @todo Need to find a way to check if the DefaultCalcMachine
  //! was properly loaded
  if (!DefaultCalcLinac)
    {
      cerr << " I couldn't create the DefaultCalcLinac, it just exploted "<< endl;
      // Get out of here with returnin error
      return 1;
    }

  return 0; // MACHINE LOADED
}
