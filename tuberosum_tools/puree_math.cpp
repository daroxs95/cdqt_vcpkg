#include "puree_math.h"


//functions definitions
int readMatrix(const char *filename , std::vector<std::vector<double> >& result)// with arg (std::string filename) must have (-std=gnu++11) flag in compiler options 
    {
    int cols = 0, rows = 0;

    // Read numbers from file into buffer.
    std::ifstream infile;
    infile.open(filename);
    
    while (! infile.eof() )
    {
        std::string line;
        std::getline(infile, line);

        std::stringstream stream(line);
        
        if (!(stream.peek() == decltype(stream)::traits_type::eof()))
        {
            result.push_back(std::vector<double>());
            result[rows].reserve(cols);

            int temp_cols = 0;
            while(! stream.eof())//check another methods of comparing string to empty
            {
                temp_cols++;
                double temp_cell;
                stream >> temp_cell;
                result[rows].push_back(temp_cell);//revisar si se puede hacer sin usar mucho el push_back
            }
            if (temp_cols > cols)
                cols = temp_cols;
            
            rows++;
        }
        
        
    }

    infile.close();
    
    return 0;
};

int readMatrix(const char *filename , Eigen::MatrixXd& result)// con arg de entrada (std::string filename) debe tener (-std=gnu++11) de flag en el compiler 
    {
    int cols = 0, rows = 0;
    double* buff = new (std::nothrow) double[MAXBUFSIZE];
    
    if (buff == 0)
        return 1; //"Error: memory could not be allocated";

    // Read numbers from file into buffer.
    std::ifstream infile;
    infile.open(filename);
    while (! infile.eof())
        {
        std::string line;
        std::getline(infile, line);//no se si tenga q ponerle std delante

        int temp_cols = 0;
        std::stringstream stream(line);
        while(! stream.eof() && !(stream.peek() == decltype(stream)::traits_type::eof()))//check another methods of comparing string to empty
            stream >> buff[cols*rows+temp_cols++];

            if (temp_cols == 0)
                continue;

            if (cols == 0)
                cols = temp_cols;

            rows++;
        }

    infile.close();

    // Populate matrix with numbers.
    //MatrixXd result;
    result.resize(rows,cols);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result(i,j) = buff[ cols*i+j ]; //treating the matrix index like in std arrays
    
    delete [] buff;
    return 0;
};

int readMatrix(std::string arg1, std::vector<std::vector<double> >& arg2)//check if can be on templates
{
    return readMatrix (arg1.data(), arg2);
}

double interpolate(double x, double y , const double x_i[2], const double y_i[2], const double f_i[2][2],int tipo) 
{   
    double result;
    switch (tipo)
    {
    case BILINEAR:

        //double fy_i[2];
        //fy_i[0] = weightedAverage(x,x_i, initializer_list<double>({f_i[0][0] , f_i[1][0]}).begin());
        //fy_i[1] = weightedAverage(x,x_i, initializer_list<double>({f_i[0][1] , f_i[1][1]}).begin());
        result = weightedAverage(y, y_i, std::initializer_list<double>({ weightedAverage(x,x_i, std::initializer_list<double>({f_i[0][0] , f_i[1][0]}).begin()) , weightedAverage(x,x_i, std::initializer_list<double>({f_i[0][1] , f_i[1][1]}).begin())}).begin() );

        break;

    default:
        result = 0;
        break;
    }
    return result;
}

double interpolate(double x, const double x_i[2], const double f_i[2], int tipo) 
{   
    double result;
    switch (tipo)
    {
    case LINEAR:

        result = weightedAverage(x,x_i, std::initializer_list<double>({f_i[0] , f_i[1]}).begin());

        break;

    default:
        result = 0;
        break;
    }
    return result;
}


inline double weightedAverage(double x, const double x_i[2], const double f_i[2])
{
    double dx = x_i[1] - x_i[0];
    return ((f_i[0] * (x_i[1] - x)/dx) + (f_i[1] * (x - x_i[0])/dx));
}

int findInterval(int pos, int from, int to , double x,const std::vector<double>& xi)
{    
    if (x == xi[pos])
        return pos;
    if (x > xi[pos])
    {
        if (x < xi[pos + 1])
            return pos;
        else
            return (findInterval((int)((pos + to)/2), pos , to ,x,xi));
            
    }
    if (x < xi[pos])
    {   
        if (x > xi[pos-1])//xi[pos -1] should exist and pos -1 be non negative(to check)
            return (pos-1);
        else
            return (findInterval((int)((pos + from)/2), from , pos , x , xi));
    }
    return -1; //super duper error in implementation if this point is reached
}

int findInterval(int pos, int from, int to , double x,const Eigen::VectorXd& xi)//no he probado si funciona
{   if (x == xi(pos))
        return pos;
    if (x > xi(pos))
    {
        if (x < xi(pos + 1))
            return pos;
        else
            return (findInterval((int)((pos + to)/2), pos , to ,x,xi));
            
    }
    if (x < xi(pos))
    {   
        if (x > xi[pos-1])//xi[pos -1] should exist and pos -1 be non negative(to check)
            return (pos-1);
        else
            return (findInterval((int)((pos + from)/2), from , pos , x , xi));
    }
    return -1; //super duper error in implementation if this point is reached
}