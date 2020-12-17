#pragma once


#include <iostream>
#include <vector>

//functions declarations
void printVectorOfVectors(std::vector<std::vector<double> >&);


#ifdef FRIED_TOOLS_IMPLEMENTATION

//functions definitions
void printVectorOfVectors(std::vector<std::vector<double> >& toprint)
{
    for(int i = 0 ; i < toprint.size() ; i++){
	for(int ii = 0 ; ii < toprint[i].size() ; ii++){
		std::cout << toprint[i][ii] << " ";
	}
	std::cout << std::endl << std::endl;
	}
}

#endif