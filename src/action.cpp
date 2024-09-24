
#include "action.h"
namespace sseq
{

bool SeqASTVisitor::isInSystemHeader(const clang::SourceLocation &loc) {
    auto &SM = _ctx->getSourceManager();
    if(SM.getFilename(loc)=="") return true;
    std::string filename = SM.getFilename(loc).str();
    if(filename.find("/usr/include/")!=std::string::npos) return true;
    if(filename.find("llvm-12.0.0.obj/")!=std::string::npos) return true;
    return false;
}
    //
bool SeqASTVisitor::MatchPoint(point &p, clang::Stmt *stmt){
    // int line_start = SM.getPresumedLineNumber(SR.getBegin());
    // int line_end   = SM.getPresumedLineNumber(SR.getEnd());
    // int col_start, col_end;
    // col_start = SM.getPresumedColumnNumber(SR.getBegin());
    // col_end   = SM.getPresumedColumnNumber(SR.getEnd());
    // int to_slice=1;
    // DefFunc def_func = inst_info.funcs_locs[func_name];
    // for (int i = line_start; i <= line_end; i++)
    // {
    //     if (def_func.lines.find(i) != def_func.lines.end())
    //     {
    //         to_slice = 0;//保留
    //     }
    // }
    return false;
}
bool SeqASTVisitor::VisitFunctionDecl(clang::FunctionDecl *func_decl){
    if (!_ctx->getSourceManager().isInMainFile(func_decl->getLocation()))
            return true;
    if (isInSystemHeader(func_decl->getLocation()))
        return true;
        
    clang::SourceManager &SM = _ctx->getSourceManager();
    std::string filePath_abs = _ctx->getSourceManager().getFilename(func_decl->getLocation()).str(); //绝对路径
    std::string filePath =filePath_abs;
    if(filePath.find("src/")!=std::string::npos){
        int pos = filePath.find("src/");
        size_t idx = filePath.find_last_of("/", pos);
    
        if (idx != std::string::npos) {
            // 提取从最后一个 '/' 到字符串末尾的子字符串
            filePath = filePath.substr(idx + 1);
        }
    }
    // std::cout<<"\n***in VisitFunctionDecl | Function " << func_decl->getNameAsString() << " is defined in file: " << filePath << "\n";
    std::string func_name = func_decl->getNameAsString();
    if (seq_info.functions.find(func_name)
        != seq_info.functions.end())
    {

    std::cout<<"\n| Function [" << func_decl->getNameAsString() << "] defined in: " << filePath << "\n";
        if (func_decl == func_decl->getDefinition())
        {
            clang::Stmt *body_stmt = func_decl->getBody();
            clang::SourceLocation last_stmt,pre_stmt;
            for (auto stmt : body_stmt->children())
            {
                //

            }
        }

    }
    return true;
}

};