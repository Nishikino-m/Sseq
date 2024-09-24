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
    //警报类型-目前参考CSA
    //未来是不是可以弄成配置文件？不同的静态分析工具不一致-但是action的一些策略是否需要和警报类型相关呢
    enum Wtype{
        WDefault = 0,
        NullPointerDereference = 1,//Dereference of null pointer
        DivisionByZero=2,//Division by zero
        DeadAssignment = 3,
        DeadInit = 4,// Dead initialization
        DeadNestedAssignment = 5,
        GarbageAssigned = 6, //Assigned value is garbage or undefined
        UninitializedArgument = 7, //Uninitialized argument value
        GarbageOperationResult = 8,//Result of operation is garbage or undefined
        ReturnStackMemory = 9, //Return of address to stack-allocated memory
    };
    //语义类型
    enum Stype{
        Default = 0,
        VarDeclare=1,//如果是这一种，还可以存储变量类型
        VarAssign=2,
        IfCond=3,
        SwitchCond = 4,
        ForLoop=5,// 包括for(a;b;c) 的a b c 三个位置
        WhileLoop=6,// while 条件
        DoWhileLoop=7,// do-while条件
        CallSite=8, //调用其他函数
        Return = 9,
    };
    //位置类型
    enum Postype{
        PDefault = 0,
        Function = 1,//函数内语句
        IfBranch=2,//True分支
        ElseBranch=3,//False分支
        SwitchBranch = 4,
        ForBody = 5,
        WhileBody = 6,
        DoBody = 7,
        Parma = 8, // 函数参数
    };
    
// 初始状态根据静态分析工具的输出，能得到警报类型和每个点的位置，其余信息在分析过程中进行补充
struct point{
    int id;//
    std::string filepath;//文件路径
    int line;//行号
    Wtype w=WDefault;//警报类型
    Stype type=Default;
    Postype pos = PDefault;
    bool isWarning =false;//是否告警点
    bool isInMacro = false;//这个位置是否属于宏
    std::string var_name;//变量名--若静态分析工具输出，则记录
    //变量类型 是否const 指针 引用 数组
    //后续需要输入机器学习算法的话，应该存储原始格式还是直接在此处进行转换

    std::string value;//变量赋值时，等号右侧的内容/类型？

    point(int id, std::string fp,int line):id{id},filepath{fp},line{line}\
    {var_name = "";}
};

class SeqInfo
{
public:
    //接收路径信息
    SeqInfo(std::string fn,std::string fn2) :file_name{ fn },func_list{ fn2 }  \
     { parse_file();parse_func();}

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
            int id,ptype,line;
            ofs >> id;
            // std::cout<<"\nid="<<id<<"\t";
            // 静态分析工具给出的信息
            if (id>0)
            {   
                ofs >> file_path >>line;
                // std::cout<<file_path<<"-"<<line;
                // functions.insert(function);
                path.push_back({id,file_path,line});
            }
            else
            {   if(id==0) 
                    ofs >> warningType;
                // if(!point_type.empty()) 
                //     std::cout << "[error]invalid path point type" << point_type<<" !\n";
                continue;
            }
        }
        ofs.close();
    }

    void parse_func()
    {
        std::ifstream ofs(func_list);
        if (!ofs.is_open())
        {
            std::cout << "[error]open file:" << func_list << " failed!\n";
            exit(-1);
        }
        std::cout << "open file:" << func_list << "!\n";
        while (!ofs.eof())
        {
            std::string func_name;
            ofs >> func_name;
            functions.insert(func_name);
        }
        ofs.close();
    }


    std::string file_name,func_list,warningType;
    std::vector<point> path;//告警路径上的每个点的语义信息
    std::set<std::string> functions;

};


}  // namespace sseq

#endif