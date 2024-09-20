#ifndef SEQ_INFO_H
#define SEQ_INFO_H

#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
namespace sseq
{
    //语义类型
    enum Stype{
        Default = 0,
        VarDeclare=1,
        VarAssign=2,
        IfCond=3,
        ForLoop=4,
        WhileLoop=5,
    };
    enum Postype{
        PDefault = 0,
        Function = 1,
        IfBranch=2,//待定
        ElseBranch=3,//待定
        ForBody = 4,
        WhileBody = 5

    };
    
// 初始状态根据静态分析工具的输出，能得到每个点的位置，其余信息在分析过程中进行补充
struct point{
    std::string filepath;//文件路径
    // std::string func;//函数名
    int line;//行号
    Stype type=Default;
    Postype pos = PDefault;
    std::string var_name;//变量名--若静态分析工具输出，则记录
    

    point(std::string fp,int line):filepath{fp},line{line}\
    {var_name = "";}
};
class SeqInfo
{
public:
    //接收路径信息
    SeqInfo(std::string fn) :file_name{ fn } \
     { parse_file();}

    void parse_file()
    {
        std::ifstream ofs(file_name);
        if (!ofs.is_open())
        {
            std::cout << "[error]open file:" << file_name << " failed!\n";
            exit(-1);
        }
        std::cout << "open file:" << file_name << "!\n";
        while (!ofs.eof())
        {
            std::string point_type, file_path;
            int ptype,line;
            ofs >> point_type;
            // 静态分析工具给出的信息
            if (point_type == "VarDeclare")
            {   
                ofs >> file_path >>line;
                path.push_back({file_path,line});
            }
            else
            {
                if(!point_type.empty()) 
                    std::cout << "[error]invalid path point type" << point_type<<" !\n";
                continue;
            }
        }
        ofs.close();
    }


    std::string file_name;
    std::vector<point> path;

 
};


}  // namespace sseq

#endif