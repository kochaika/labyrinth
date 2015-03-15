#include "result.h"
#include <QDebug>
Results::Results(std::string file_path)
{
   path = file_path;
}

void Results::LoadResults()
{
    std::ifstream inp;
    double d;
    std::string s;
    inp.open(path+std::string("/results.txt"));
    if(!inp.is_open()) return;
    data.clear();
    for(int i=0; i<5 && !inp.eof(); i++)
    {
        std::getline(inp,s);
        if(inp.eof()){inp.close(); return;}
        inp>>d;
        data.push_back(std::pair<std::string,double>(s,d));
        std::getline(inp,s); //для съедения переноса
    }
    inp.close();
}

std::vector<std::pair<std::string,double>> Results::GetResults()
{
    return data;
}

void Results::AddResult(std::string name, double time)
{
    std::vector<std::pair<std::string,double>> data1;
    std::vector<std::pair<std::string,double>>::iterator it;
    bool flag = false;
    for(it=data.begin();it!=data.end();it++)
    {
        if(((*it).second)<time)
        {
            data1.push_back(*it);
        }
        else
        {
          //  qDebug()<<">"<<(*it).second<<">"<<time;
            if(!flag)
            {
                data1.push_back(std::pair<std::string,double>(name,time));
                flag = true;
            }
            data1.push_back(*it);
        }
    }
    if(!flag) data1.push_back(std::pair<std::string,double>(name,time));
    data.clear();
    if(data1.size()>5)
        data.assign(data1.begin(),data1.begin()+5);
    else
        data = data1;
    std::ofstream outp;
    outp.open(path+std::string("/results.txt"));
    for(it=data.begin();it!=data.end();it++)
    {
        outp<<(*it).first<<'\n'<<(*it).second<<'\n';
    }
    outp.close();
}
