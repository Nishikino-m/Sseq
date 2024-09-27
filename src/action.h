#ifndef FCOSAL_ACTION_H
#define FCOSAL_ACTION_H

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "seq_info.h"
#include "tool.h"
namespace sseq
{
class SeqASTVisitor : public clang::RecursiveASTVisitor<SeqASTVisitor>
{
    private:
        clang::ASTContext *_ctx;
        SeqInfo &seq_info;
    public:
    explicit SeqASTVisitor(clang::ASTContext *ctx,
                            SeqInfo &seq_i)
        : _ctx(ctx),  seq_info{ seq_i }
    {
    }
    ~SeqASTVisitor(){
    }

    bool isInSystemHeader(const clang::SourceLocation &loc);
    bool MatchPoint(point &p, clang::Stmt *stmt, std::string filePath);
    clang::SourceRange get_sourcerange(clang::Stmt *stmt);
    clang::SourceRange get_decl_sourcerange(clang::Decl *stmt);
    bool VisitFunctionDecl(clang::FunctionDecl *func_decl);
};
class SeqASTConsumer : public clang::ASTConsumer
{
  public:
    explicit SeqASTConsumer(clang::CompilerInstance &CI,
                             SeqInfo &seq_i)
        :  _visitor(&CI.getASTContext(), seq_i)
    {
    }

    virtual void HandleTranslationUnit(clang::ASTContext &ctx)
    {
        _visitor.TraverseDecl(ctx.getTranslationUnitDecl());
    }

  private:
    SeqASTVisitor _visitor;
};

class SeqFrontendAction : public clang::ASTFrontendAction
{
  public:
    SeqFrontendAction(SeqInfo &seq_i) : seq_info{ seq_i }
    {}
    virtual std::unique_ptr<clang::ASTConsumer>
        CreateASTConsumer(clang::CompilerInstance &compiler,
                          llvm::StringRef in_file)
    {
        // _SM = compiler.getSourceManager();
        return std::make_unique<SeqASTConsumer>(
            compiler, seq_info);
    }
    //写文件 
    void EndSourceFileAction() override
    {
        // clang::SourceManager &SM = _rewriter.getSourceMgr();
        // std::error_code ec;
        // std::string file_name =
        //     SM.getFileEntryForID(SM.getMainFileID())->getName().str();
        // replace_suffix(file_name, "_InstSlice");
        // llvm::raw_fd_stream fd(file_name, ec);
        // _rewriter.getEditBuffer(SM.getMainFileID()).write(fd);
    }

  private:
    SeqInfo &seq_info;
};


class SeqFactory : public clang::tooling::FrontendActionFactory
{
  public:
    SeqFactory(SeqInfo &seq_i) : seq_info{ seq_i } {
        std::cout<<"Bug type is "<<seq_i.warningType<<"\n";
    }
    std::unique_ptr<clang::FrontendAction> create() override
    {
        return std::make_unique<SeqFrontendAction>(seq_info);
    }

  private:
    SeqInfo &seq_info;
};
}  // namespace sseq
#endif