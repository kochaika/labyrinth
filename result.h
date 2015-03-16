#include <vector>
#include <string>
#include <fstream>

#ifndef RESULT_H
#define RESULT_H

class Results
{
public:
    Results(std::string file_path);
    void LoadResults();
    void AddResult(std::string name, double time);
    std::vector<std::pair<std::string,double> > GetResults();
private:
    std::string path;
    std::vector<std::pair<std::string,double> > data;
};

#endif // RESULT_H
