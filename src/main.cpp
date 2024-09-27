#include "action.h"

#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <iostream>
#include <llvm/Support/CommandLine.h>

static llvm::cl::OptionCategory MyASTSlicer_category("myastslicer options");
static llvm::cl::extrahelp
    common_help(clang::tooling::CommonOptionsParser::HelpMessage);

    //commandLine Arguments
static llvm::cl::opt<std::string> path_info_f("inst", llvm::cl::desc("path_info_file"),
                                llvm::cl::cat(MyASTSlicer_category));
int main(int argc, const char **argv)
{
    int argc_f           = argc - 2;
    auto expected_parser = clang::tooling::CommonOptionsParser::create(
        argc,
        argv,
        MyASTSlicer_category,
        llvm::cl::NumOccurrencesFlag::ZeroOrMore
        );
    if (!expected_parser)
    {
        // Fail gracefully for unsupported options.
        llvm::errs() << expected_parser.takeError();
        return 1;
    }
    clang::tooling::CommonOptionsParser &options_parser = expected_parser.get();
    
    // clang::tooling::ClangTool tool(options_parser.getCompilations(),
    //                                options_parser.getSourcePathList());
    clang::tooling::ClangTool tool(
        options_parser.getCompilations(),
        options_parser.getCompilations().getAllFiles());

//add
    clang::tooling::ArgumentsAdjuster ardj = clang::tooling::getInsertArgumentAdjuster("-I/usr/local/llvm/lib/clang/12.0.0/include");
    tool.appendArgumentsAdjuster(ardj);

    std::cout << "[get semantic sequence]\n";
    std::string path_info_file = path_info_f;
    //./sseq compile_commands.json test_info_1.txt 
    if(path_info_file=="" ) path_info_file =  argv[2];//待修改
    std::string function_list_file =  argv[3];//函数
    sseq::SeqInfo seq_info(path_info_file, function_list_file);
    std::unique_ptr<sseq::SeqFactory> sseq_factory =
        std::make_unique<sseq::SeqFactory>(seq_info);

    tool.run(sseq_factory.get());
    
    std::cout << "[exit]\n";
    for(auto &p:seq_info.path){
        if(!p.node){
            std::cout<<"NullPointer\n";
            continue;
        }
        std::cout<<p.id<<" "<<p.node->getStmtClassName()<<"\n";
    }
    return 0;
}
