
#include "action.h"
namespace sseq
{

    bool SeqASTVisitor::isInSystemHeader(const clang::SourceLocation &loc) {
        auto &SM = _ctx->getSourceManager();
        if(SM.getFilename(loc)=="") return true;
        std::string filename = SM.getFilename(loc).str();
        if(filename.find("/usr/include/")!=std::string::npos) return true;
        if(filename.find("llvm-12.0.0.obj/")!=std::string::npos) return true; //结合实际情况 当前工具开发使用llvm12
        return false;
    }
    clang::SourceRange SeqASTVisitor::get_decl_sourcerange(clang::Decl *stmt){
        clang::SourceRange SR = stmt->getSourceRange();
        auto &SM = _ctx->getSourceManager();
        auto &LO = _ctx->getLangOpts();
        clang::SourceLocation  loc = SR.getBegin();
        clang::SourceLocation  end = SR.getEnd();
        if(loc.isMacroID()){
            clang::FileID FID = SM.getFileID(loc);
            const clang::SrcMgr::SLocEntry *Entry = &SM.getSLocEntry(FID);
            while (Entry->getExpansion().getExpansionLocStart().isMacroID()) {
                loc = Entry->getExpansion().getExpansionLocStart();
                FID = SM.getFileID(loc);
                Entry = &SM.getSLocEntry(FID);
            }
            loc = SM.getExpansionLoc(loc);
            if(end.isMacroID()){
                FID = SM.getFileID(end);
                const clang::SrcMgr::SLocEntry *Entry = &SM.getSLocEntry(FID);
                while (Entry->getExpansion().getExpansionLocStart().isMacroID()) {
                    end = Entry->getExpansion().getExpansionLocStart();
                    FID = SM.getFileID(end);
                    Entry = &SM.getSLocEntry(FID);
                }
                end = Entry->getExpansion().getExpansionLocEnd();
            }
            else end = SM.getExpansionLoc(end);
        }
        else if(end.isMacroID()){
            clang::FileID FID = SM.getFileID(end);
            const clang::SrcMgr::SLocEntry *Entry = &SM.getSLocEntry(FID);
            while (Entry->getExpansion().getExpansionLocStart().isMacroID()) {
                end = Entry->getExpansion().getExpansionLocStart();
                FID = SM.getFileID(end);
                Entry = &SM.getSLocEntry(FID);
            }
            end = Entry->getExpansion().getExpansionLocEnd();
        }
        
        
        return clang::SourceRange(loc,end);
    }
    //获得stmt的range
    clang::SourceRange SeqASTVisitor::get_sourcerange(clang::Stmt *stmt){
        clang::SourceRange SR = stmt->getSourceRange();
        auto &SM = _ctx->getSourceManager();
        auto &LO = _ctx->getLangOpts();
        clang::SourceLocation  loc = SR.getBegin();
        clang::SourceLocation  end = SR.getEnd();
        if(loc.isMacroID()){
            clang::FileID FID = SM.getFileID(loc);
            const clang::SrcMgr::SLocEntry *Entry = &SM.getSLocEntry(FID);
            while (Entry->getExpansion().getExpansionLocStart().isMacroID()) {
                loc = Entry->getExpansion().getExpansionLocStart();
                FID = SM.getFileID(loc);
                Entry = &SM.getSLocEntry(FID);
            }
            loc = SM.getExpansionLoc(loc);
            if(end.isMacroID()){
                FID = SM.getFileID(end);
                const clang::SrcMgr::SLocEntry *Entry = &SM.getSLocEntry(FID);
                while (Entry->getExpansion().getExpansionLocStart().isMacroID()) {
                    end = Entry->getExpansion().getExpansionLocStart();
                    FID = SM.getFileID(end);
                    Entry = &SM.getSLocEntry(FID);
                }
                end = Entry->getExpansion().getExpansionLocEnd();
            }
            else end = SM.getExpansionLoc(end);
        }
        else if(end.isMacroID()){
            clang::FileID FID = SM.getFileID(end);
            const clang::SrcMgr::SLocEntry *Entry = &SM.getSLocEntry(FID);
            while (Entry->getExpansion().getExpansionLocStart().isMacroID()) {
                end = Entry->getExpansion().getExpansionLocStart();
                FID = SM.getFileID(end);
                Entry = &SM.getSLocEntry(FID);
            }
            end = Entry->getExpansion().getExpansionLocEnd();
        }
        
        return clang::SourceRange(loc,end);
    }
    bool SeqASTVisitor::VisitFunctionDecl(clang::FunctionDecl *func_decl){
        if (!_ctx->getSourceManager().isInMainFile(func_decl->getLocation()))
                return true;
        if (isInSystemHeader(func_decl->getLocation()))
            return true;
            
        clang::SourceManager &SM = _ctx->getSourceManager();
        std::string filePath_abs = SM.getFilename(func_decl->getLocation()).str(); //绝对路径
        std::string filePath =filePath_abs;
        if(filePath.find("src/")!=std::string::npos){
            int pos = filePath.find("src/");
            size_t idx = filePath.find_last_of("/", pos);
        
            if (idx != std::string::npos) {
                // 提取从最后一个 '/' 到字符串末尾的子字符串
                filePath = filePath.substr(idx + 1);
            }
        }
        std::string func_name = func_decl->getNameAsString();
        std::cout<<"\n| Function [" << func_decl->getNameAsString() << "] defined in: " << filePath << "\n";
        if (func_decl == func_decl->getDefinition())
        {
            clang::Stmt *body_stmt = func_decl->getBody();
            for (auto &stmt : body_stmt->children())
            {
                //遍历这个函数中的语句
                //在第一个语句之前插入内容X
                _rewriter.InsertTextBefore(stmt->getBeginLoc(),"XXX\n");
                // 方法1： 在此处判断语句类型 如果是有子语句的语句（如for\if\switch\while)则遍历其子语句查找除法运算和模运算 
                // 可以通过get_stmt_string 可以输出该语句的内容，通过判断是否存在"/"或"%"字符来进行一次筛选
                /*
                if(const clang::BinaryOperator *bop = llvm::dyn_cast<clang::BinaryOperator>(stmt)){ 这样是判断是不是二元运算
                clang::Expr *lhs =  bop->getLHS(),*rhs = bop->getRHS();//获取左右两边的
                clang::BinaryOperator::Opcode op = bop->getOpcode();//运算符，如op==clang::BinaryOperator::Opcode::BO_Comma 可以判断运算符是不是逗号
                if(lhs==nullptr || rhs==nullptr) return false;//防止空指针
                std::string lhs_class = lhs->getStmtClassName(),rhs_class = rhs->getStmtClassName();//需要考虑运算符左右子语句可能也是二元运算。所以可能需要设计递归函数

                */
               //方法2： 可以尝试写一个VisitBinaryOperator，再看看能不能通过这个节点找节点获取到对应的函数节点

               //一些API：get_stmt_string输出语句内容，
               //获取SourceLocation对应的行列：  int line= SM.getPresumedLineNumber(Loc), col = SM.getPresumedColumnNumber(Loc); 
               // 在loc之后插入str： _rewriter.InsertTextAfter(loc,str);
               // 遍历子语句直接auto， IF语句获取为true的部分是ifstmt->getThen()， 其他语句具体查询clang的文档。if while这些语句还要考虑条件，也可以查询文档得知如何获取。
            }
        }

        
        return true;
    }

};