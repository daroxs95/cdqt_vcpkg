#ifndef POTATO_LINAC_H
#define POTATO_LINAC_H

#include <iostream>
#include <json/json.h>
#include <string>
#include <vector>

#include "peel_wrappers.h"
#include "puree_math.h"

using namespace std;

class Potato_Linac {
  /**
   * Object model of a Linac machine
   *
   * Used to instantiate several objects of Linac's type
   * when performing calculations to speed the change of
   * the calculation machine and its descriptor parameters
   * For instance, several machines could exists:
   *  _PreciseXXXX_, _SynergyXXXX_, _TrueBeamXXXX_
   */

public:
  Potato_Linac(string);       //!< Recomended constructor
  Potato_Linac(const char *); //!< Alternative constructor
  ~Potato_Linac();            //!< Destructor

  vector<string> getAvalE();  //!< Get Linac's avaliable Energy

  bool is_enabled();      //!< Get machine avaliability to perform calculations
  void set_enabled(bool); //!< Set Linac avaiability for calculations
  void setPath(const char *); //!< Set Linac base path (alternative)
  void setPath(string);       //!< Set Linac base path (default)
  string getPath();           //!< Get the current Linac path
  void setAvalE();            //!< Set Linac's avaliable Energy


  class Beam {
    /**
     * Object model of a Linac's beam
     *
     * Several beams could be avaliable for the
     * same Linac. For instance, a single machine
     * (Linac Precise XXXX) could have:
     *  X06, X15, E06, E08, E15
     * and those could be used combined in a single treatment
     */

  public:
    Beam(Potato_Linac *, string arg1); //!< Recommended constructor
    Beam(Potato_Linac *);              //!< Alternative constructor
    Beam();                            //!< Default constructor
    ~Beam();                           //!< Destructor

    double getSc(const double &);  //!< Get Collimator Scattering Factor
    double getSp(const double &);  //!< Get Phantom Scattering Factor
    double getScp(const double &); //!< Get Scattering Factor

    double getTPR(const double &, const double &); //!< Get Tissue Phantom Ratio
    double getWF(const double &, const double &);  //!< Get Wedge Factor
    double getWP(const double &, const double &);  //!< Get Wedge Profile
    double getOAR(const double &, const double &); //!< Get Off-Axis Ratio

    void setParameter(string arg1, vector<vector<double>> &arg2);
    //!< Set an specified parameter into the specified container

    void setTPR();   //!< Loads the TPR member
    void setSc_Sp(); //!< Loads the Sc_Sp member
    void setWF();    //!< Loads the WF member
    void setWP();    //!< Loads the WP member
    void setOAR();   //!< Loads the OAR member
    void setBeam();  //!< Loads all above data

    string getE_label();      //!< The current Beam (energy) label
    string get_parent_path(); //!< The current Beam (energy) path to parent
                              //!< Potato_Linac
    string
    get_beam_path(); //!< The current Beam (energy) path to its descriptor files

  private:
    bool enabled;
    string parent_machine_path;
    string beam_path;
    string beam_label;

    const vector<vector<double>> &getTPR();
    const vector<vector<double>> &getSc_Sp();
    const vector<vector<double>> &getWF();
    const vector<vector<double>> &getWP();
    const vector<vector<double>> &getOAR();

  protected:
    // beam descriptor parameters (according to AAPM TG-114)
    vector<vector<double>> TPR;   //!< Tissue-Phantom-Ratio of Beam
    vector<vector<double>> Sc_Sp; //!< Scatter Factor of Beam
    vector<vector<double>> WF;    //!< Wedge Factor of Beam
    vector<vector<double>> WP;    //!< Wedge Profile of Beam
    vector<vector<double>> OAR;   //!< Off-Axis Ratio of Beam
  };

  vector<Potato_Linac::Beam> getAvalBeams(); //!< Obtain all avaliable beams

private:
  bool enabled;      //!< A label to define usability of linac
  string linac_path; //!< The path were the Machine files exists
  Json::Value linac_info; //!< All linac info read from "info.json"
  unsigned short int  number_Energies; //! # of beam energies in linac
  vector <string> avalE;
  
  //andy chequea esto
  double refactor_this = 0;

protected:
  vector<Beam>
      AvaliableBeams; //!< container with beam energies of current Potato_Linac
};

#endif // POTATO_LINAC_H
