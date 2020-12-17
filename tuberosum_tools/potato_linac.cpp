#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>

#include "potato_linac.h"

using namespace std;

Potato_Linac::Potato_Linac(string arg1) {
  /**
   * Creates a Linac object if provided path contains
   * properly data to be used
   *
   * @param  arg1 Provided Linac's path
   */
  struct stat buffer;
  bool exists(stat(arg1.data(), &buffer) == 0);

  if (exists)
  {
    bool is_json(stat((arg1 + "/info.json").data(), &buffer) == 0);

    if (is_json) // Check if contains a info.json
    {
      Json::Value root;
      ifstream json_descriptor(arg1 + "/info.json", std::ifstream::binary);

      if (json_descriptor.good()) // Check if it is parseable
      {
        // Add all data from json file into Linac class var members
        json_descriptor >>
            this->linac_info; // dump json data into a class member
        json_descriptor.close();

        this->linac_path = arg1.data(); // store Mach. path in a class member

        // ALMOST Convert all var into Machine class members
        setAvalE ();

        for (auto item: this->getAvalE ())
          {
            this->AvaliableBeams.push_back(
                Potato_Linac::Beam(this, item));
          }
/*
        Json::Value avaliable_E = linac_info["energies"];

        for (Json::ValueIterator v_itr = avaliable_E.begin();
             v_itr != avaliable_E.end(); v_itr++) {
          this->AvaliableBeams.push_back(
              Potato_Linac::Beam(this, v_itr->asString()));
          // cout<<"\t"<<v_itr->asString ()<<endl;
        }
*/
        // Change to true when all above properly cheked.
        this->set_enabled(true);
        //!< @todo Globally force software to avoid using unable linacs or beams
      } else
        cerr << "Couldn't parse " + arg1 + "/info.json file" << endl;
    }
  }
}

Potato_Linac::Potato_Linac(const char *arg1)
    : Potato_Linac(static_cast<string>(arg1)) {
  /**
   * @param  arg1 Provided Linac's path
   *
   * @overload
   * @deprecated Beacasuse we have to evolve to CXX-11 and adopt std::string
   * @delegating This is a delegating, overloaded and deprecated constructor.
   * Don't use it unless extremly needed.
   */
}

Potato_Linac::Beam::Beam(Potato_Linac *parent, string arg1) {
  /**
   * @param parent Allocation of parent (Linac) object
   * @param arg1 Beam label provided
   *        Used to properly find beam descriptors files under
   * path/to/Linac/Beam/
   * @overload
   * @see Potato_Linac
   */

  this->beam_label = arg1;
  this->parent_machine_path = parent->getPath();

  this->beam_path = this->parent_machine_path + "/" + this->beam_label;

  struct stat buffer;
  bool is_path(stat(this->beam_path.data(), &buffer) == 0);

  if (is_path) {
    string photon_beam_files[] = {"TPR.txt", "Sc_Sp.txt", "WedgeFactor.txt",
                                  "WedgeProfiles.txt"};

    // Test existence of every needed file before continue
    for (auto filename : photon_beam_files) {
      // cout<< "Testing if "<<this->beam_path + "/" + bb<<" exists"<<endl;
      bool file_exists(stat((this->beam_path + "/" + filename).data(), &buffer) == 0);
      if (!file_exists) {
        // Something went wrong --> LOG
        // cout<<"Coudn't find "<< filename<<" in " << this->beam_path;
        break;
      }
    }

    this->setBeam ();
  }
}

Potato_Linac::Beam::Beam(
    Potato_Linac *parent) {
  /**
   * @param  parent Allocation of parent (Linac) object
   * @overload
   * @deprecated
   * @see Potato_Linac
   */
  this->beam_label = "unknown";
  this->parent_machine_path = parent->getPath();
}

Potato_Linac::Beam::Beam() {
} //!< If you reached this line..., I hope it was accidentally

Potato_Linac::Beam::~Beam() {}

string Potato_Linac::Beam::get_parent_path() {
  /**
   * Typically, one level above the beam path (see get_beam_path).
   * This directory contains the _info.json_ file, and directories
   * of each avaliable beam energy, such as the following tree:
   *
   *            ./
   *             |--- MachineId1/
   *             |	|--- info.json
   *             |	|--- BeamName1/
   *             |	|	|--- TPR.txt
   *             |	|	|--- *.txt
   *             |	*
   *             |	|--- BeamNameN
   *             *
   *             |--- MachineIdN
   *
   * @return beam_label
   * @see get_beam_path
   */
  return this->parent_machine_path;
}

string Potato_Linac::Beam::get_beam_path() {
  /**
   * The path in which beam descriptor files are located. This directory
   * __must__ include several files, such as:
   *
   *  - *TPR.txt*
   *  - *Sc_Sp.txt*
   *  - *WedgeFactors.txt*
   *  - *WedgeProfiles.txt*
   *  - *OffAxis.txt*
   *
   * which are the linac's beam descriptor files (according to *AAPM TG-114*)
   * @return beam_label
   * @deprecated
   * @see get_parent_path
   */
  return this->beam_path;
}

string Potato_Linac::Beam::getE_label() {
  /**
   * A beam energy label gives infromation about the current
   * used beam, typical values could be:
   *
   *  - 6MeV, F06, X06: for __6 MeV__ photon beams
   *  - 8MV,  E08: for __8 MeV__ electron beams
   *
   * which are defined in the _info.json_ file at linac's beam path
   * @return beam_label
   * @see get_beam_path
   */
  return this->beam_label;
}

const vector<vector<double>> &Potato_Linac::Beam::getTPR() { return TPR; }

const vector<vector<double>> &Potato_Linac::Beam::getSc_Sp() { return Sc_Sp; }

const vector<vector<double>> &Potato_Linac::Beam::getWF() { return WF; }

const vector<vector<double>> &Potato_Linac::Beam::getWP() { return WP; }

const vector<vector<double>> &Potato_Linac::Beam::getOAR() { return OAR; }

double Potato_Linac::Beam::getSc(const double &arg1) {
  /**
   * @param  arg1 Field size
   * @return Sc interpolated at any field size
   */
  return W_interpolate1D(arg1, getSc_Sp(), 1);
}

double Potato_Linac::Beam::getSp(const double &arg1) {
  /**
   * @param  arg1 Field size
   * @return Sp interpolated at any field size
   */
  return W_interpolate1D(arg1, getSc_Sp(), 2);
}

double Potato_Linac::Beam::getScp(const double &arg1) {
  /**
   * @param  arg1 field size
   * @return Scp interpolated at any field size
   */
  return getSc(arg1) * getSp(arg1);
}

double Potato_Linac::Beam::getTPR(const double &arg1, const double &arg2) {
  /**
   * @param  arg1 field size
   * @param  arg2 depth
   * @return TPR interpolated at any field size & depth
   */
  return W_interpolate2D(arg1, arg2, getTPR());
}

double Potato_Linac::Beam::getWF(const double &arg1, const double &arg2) {
  /**
   * @param  arg1 field size
   * @param  arg2 IN distance
   * @return WF interpolated at any field size & distance
   */
  return W_interpolate2D(arg1, arg2, getWF());
}

double Potato_Linac::Beam::getWP(const double &arg1, const double &arg2) {
  /**
   * @param  arg1 field size
   * @param  arg2 CR distance
   * @return WP interpolated at any field size & distance
   */
  return W_interpolate2D(arg1, arg2, getWP());
}

double Potato_Linac::Beam::getOAR(const double &arg1, const double &arg2) {
  /**
   * @param  arg1 IN distance
   * @param  arg2 CR distance
   * @return OAR interpolated at any distance from center
   */
  return W_interpolate2D(arg1, arg2, getOAR());
}

// SET methods
void Potato_Linac::Beam::setParameter(string arg1,
                                      vector<vector<double>> &arg2) {
  /**
   * @param[in]  arg1 Parameter name, matching filename for read from
   * @param[out] arg2 container Class member to store parameter in
   */
  readMatrix(this->get_beam_path() + arg1, arg2);

  if (arg2.size() == 0 || arg2.size() == 0) {
    cerr << this->get_beam_path() + arg1 << "...couldn't be read properly"
         << endl;
    this->enabled = false;
    // error loading machine
  }
}

void Potato_Linac::Beam::setTPR() {
  /**
   * Stores _TPR.txt_ data into beam TPR class member
   *
   * @see setParameter(string arg1, vector<vector<double>>& arg2)
   */
  this->setParameter("/TPR.txt", this->TPR);
}

void Potato_Linac::Beam::setSc_Sp() {
  /**
   * Stores _Sc_Sp.txt_ data into beam Sc_Sp class member
   *
   * @see setParameter(string arg1,vector<vector<double>> &arg2)
   */
  this->setParameter("/Sc_Sp.txt", this->Sc_Sp);
}

void Potato_Linac::Beam::setWF() {
  /**
   * Stores _WedgeFactors.txt_ data into beam WF class member
   *
   * @see setParameter(string arg1,vector<vector<double>> &arg2)
   */
  this->setParameter("/WedgeFactor.txt", this->WF);
}

void Potato_Linac::Beam::setWP() {
  /**
   * Stores _WedgeProfiles.txt_ data into beam WP class member
   *
   * @see setParameter(string arg1,vector<vector<double>> &arg2)
   */
  this->setParameter("/WedgeProfiles.txt", this->WP);
}

void Potato_Linac::Beam::setOAR() {
  /**
   * Stores _OffAxis.txt_ data into beam OAR class member
   *
   * @see setParameter(string arg1,vector<vector<double>> &arg2)
   */
  this->setParameter("/OffAxis.txt", this->OAR);
}

void Potato_Linac::Beam::setBeam() {
  /**
   * Stores all *.txt data into the respective beam class member
   * @todo Implement some exception warranties
   */
  this->setSc_Sp();   this->setTPR();   this->setWF();
  this->setWP();      this->setOAR();
}

vector<Potato_Linac::Beam> Potato_Linac::getAvalBeams() {
  /**
   * @return All beams already instantiated by constructor (if avaliable)
   */
  return this->AvaliableBeams;
}

bool Potato_Linac::is_enabled() {
  /** based on AAPM TG-114 requirements criteria:
   *  - must have properly loaded parameters descriptor files
   *  - must have avaliable info about MLC type, typical SSD (DFS), etc.
   *
   * @return whether the Linac is enabled for calculations or not
   */
  return this->enabled;
}

void Potato_Linac::set_enabled(bool arg1) {
  /**
   * Set the enabled property of the machine to specified value
   *
   * @param arg1 Specified enabled value for linac
   */
  this->enabled = arg1;
}

void Potato_Linac::setPath(const char *arg1) {
  /**
   * Set the actual path were the machines files are located
   *
   * @param arg1 Path to be set
   */
  this->linac_path = static_cast<string>(arg1);
}

void Potato_Linac::setPath(string arg1) {
  /**
   * @return the actual path where the machines files are located
   * @overload
   */
  this->linac_path = arg1;
}

std::string Potato_Linac::getPath() {
  /**
   * @return the actual path where the machines files are located
   */
  return this->linac_path;
}

vector<string> Potato_Linac::getAvalE() {
  /**
   * @return All beams already instantiated by constructor (if avaliable)
   */
  return this->avalE;
}

void Potato_Linac::setAvalE() {
  /**
   * Extracts list of avaliable beam's energies from linac_info object
   */

  if (this->linac_info){
     Json::Value tmp_value = linac_info["energies"];

     for (Json::ValueIterator v_itr = tmp_value.begin();
          v_itr != tmp_value.end(); v_itr++) {
       this->avalE.push_back(v_itr.name());
       this->refactor_this = v_itr->asDouble();
     }
  }
}
