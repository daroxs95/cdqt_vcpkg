#pragma once //compiler dependant

#include <fstream>
#include <vector>//do not know if needed
#include "puree_math.h"

//functions declarations
int W_findInterval(double ,const std::vector<double>&, int to_ignore = 0);// returns the position in xi that holds the closest value to x, being minor
int W_findInterval(double ,const Eigen::VectorXd&, int to_ignore = 0);// returns the position in xi that holds the closest value to x, being minor
double W_interpolate2D(double x, double y,const std::vector<std::vector<double> >&, int type = BILINEAR);// y -> rows , x -> columns , of the VV
double W_interpolate1D(const double y,const std::vector<std::vector<double> >&, const int column = 1, int type = LINEAR);//y -> rows, of the VV, columns are dircrete and represent in which column of the data will seek f(y) 

#ifdef PEEL_WRAPPERS_IMPLEMENTATIONS
//functions definitions
double W_interpolate2D(double x, double y,const std::vector<std::vector<double> >& sourceVectors, int type)
{   
    double xi[2]; //check if possible do this with pointers only
	double yi[2];
	double fi[2][2];
    
    std::vector<double> y_column(sourceVectors.size());// this is bad implementation, check if doable with pointer too
    
    for (int i=0; i< sourceVectors.size(); i++)
    {
        y_column[i]=sourceVectors[i][0];
    }

    int closest_x = W_findInterval(x , sourceVectors[0],1);
    if (closest_x < 0 )
        return -1; // something went wrong finding the interval
    int closest_y = W_findInterval(y , y_column,1);
    if (closest_y < 0)
        return -1; // something went wrong finding the interval

    xi[0] = sourceVectors[0][closest_x];
    xi[1] = sourceVectors[0][closest_x + 1];
    yi[0] = sourceVectors[closest_y][0];
    yi[1] = sourceVectors[closest_y + 1][0];

    fi[0][0] = sourceVectors[closest_y][closest_x];
    fi[0][1] = sourceVectors[closest_y][closest_x+1];
    fi[1][0] = sourceVectors[closest_y+1][closest_x];
    fi[1][1] = sourceVectors[closest_y+1][closest_x+1];



	return interpolate(x,y,xi,yi,fi,type);
}

double W_interpolate1D(const double y,const std::vector<std::vector<double> >& sourceVectors, const int column, int type)
{   
    double yi[2]; //check if possible do this with pointers only
	double fi[2];
    
    std::vector<double> y_column(sourceVectors.size());// this is bad implementation, check if doable with pointer too
    
    for (int i=0; i< sourceVectors.size(); i++)
    {
        y_column[i]=sourceVectors[i][0];
    }

    int closest_y = W_findInterval(y , y_column,0);

    if (closest_y < 0)
        return -1; // something went wrong finding the interval
    if (closest_y == sourceVectors.size()-1)// this is the best i can think, pretty much easy
        return sourceVectors.back()[column];

    yi[0] = sourceVectors[closest_y][0];
    yi[1] = sourceVectors[closest_y + 1][0];

    fi[0] = sourceVectors[closest_y][column];
    fi[1] = sourceVectors[closest_y+1][column];

	return interpolate(y,yi,fi, type);
}

int W_findInterval(double x,const std::vector<double>& xi, int to_ignore)
{
    if (x > xi.back() || x < xi[to_ignore])
        return -1; // the element is not in the interval

    int pos = (int)((x - xi[to_ignore]) / (xi.back() - xi[to_ignore]) * (xi.size() - to_ignore - 1));// check if this can be inserted in the function call directly  
    pos = findInterval(pos , to_ignore , xi.size() - 1, x , xi);


    return pos; 
}

#endif