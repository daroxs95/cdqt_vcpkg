#pragma once //compiler dependant

//#include <iostream>//do not know if needed, needed in cpp
#include <fstream>
#include <vector>//do not know if needed
#include <Eigen/Dense>
//#include <string>//do not know if needed


#define MAXBUFSIZE  ((int) 1e5)
#define BILINEAR 2
#define LINEAR 3

//functions declarations
int readMatrix(const char * , std::vector<std::vector<double> >& );
int readMatrix(const char * , Eigen::MatrixXd& );
int readMatrix(std::string , std::vector<std::vector<double> >& );



double interpolate(double x, double y , const double x_i[2], const double y_i[2], const double f_i[2][2], int tipo = BILINEAR);// here f_i[a][b]=f(x_i[a],y_i[b]), check thi against the x, y orders in wrappers in peel_wrappers
double interpolate(double x, const double x_i[2], const double f_i[2], int tipo = LINEAR);// here f_i[a][b]=f(x_i[a],y_i[b])

inline double weightedAverage(double x, const double x_i[2], const double f_i[2]);// TODO: make description

int findInterval(int pos,int from, int to, double x,const std::vector<double>& xi);// returns the position in xi that holds the closest value to x, being minor, starting in pos, and taking into account elements of xi from "from" to "to" 
int findInterval(int pos,int from, int to, double x,const Eigen::VectorXd& xi);// returns the position in xi that holds the closest value to x, being minor, starting in pos, and taking into account elements of xi from "from" to "to" 
