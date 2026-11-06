#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>
#include "llvm/IR/Verifier.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/NoFolder.h"
#include "llvm/Support/raw_ostream.h"

using namespace std;
using namespace llvm;

#define NUMBER 256

static unique_ptr<LLVMContext> TheContext;
static unique_ptr<IRBuilder<NoFolder>> Builder;
static unique_ptr<Module> TheModule;

static void InitializeModule()
{
    TheContext = std::make_unique<LLVMContext>();
    TheModule = std::make_unique<Module>("MyModule", *TheContext);
    Builder = std::make_unique<IRBuilder<NoFolder>>(*TheContext);
}

//===----------------------------------------------------------------------===//
// AST nodes
//===----------------------------------------------------------------------===//
class GenericASTNode {
public:
    virtual ~GenericASTNode() = default;
    virtual void toString() {};
    virtual Value *codegen() = 0;
};

class NumberASTNode : public GenericASTNode {
  int Val;

public:
    NumberASTNode(int Val)
    {
        this->Val = Val;
    }

    void toString() override { printf("%d", Val); }
    Value* codegen() override {
        return ConstantInt::get(Type::getInt32Ty(*TheContext), Val, true);
    }
};

class BinaryExprAST : public GenericASTNode {
    char Op;
    unique_ptr<GenericASTNode> LHS, RHS;

public:
    BinaryExprAST(char Op, unique_ptr<GenericASTNode> LHS, unique_ptr<GenericASTNode> RHS)
    {
        this->Op = Op;
        this->LHS = std::move(LHS);
        this->RHS = std::move(RHS);
    }

    void toString() override {
        printf("("); LHS->toString(); printf(" %c ", Op);
        RHS->toString(); printf(")");
    }
    Value* codegen() override {
        Value *L = LHS->codegen(), *R = RHS->codegen();
        if (!L || !R) return nullptr;
        switch (Op) {
        case '+': return Builder->CreateAdd(L, R, "addtmp");
        case '-': return Builder->CreateSub(L, R, "subtmp");
        case '*': return Builder->CreateMul(L, R, "multmp");
        case '/': return Builder->CreateSDiv(L, R, "divtmp");
        case '%': return Builder->CreateSRem(L, R, "remtmp");
        default: return nullptr;
        }
    }
};

bool CodeGenTopLevel(unique_ptr<GenericASTNode> AST_Root)
{
    // Create an anonymous function with no parameters
    vector<Type *> ArgumentsTypes(0);
    FunctionType *FT = FunctionType::get(Type::getInt32Ty(*TheContext), ArgumentsTypes, false);
    Function *F = Function::Create(FT, Function::ExternalLinkage, "main", TheModule.get());

    // Create a label 'entry' and set it to the current position in the builder
    BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", F);
    Builder->SetInsertPoint(BB);

    // Generate the code for the body of the function and return the result
    if (Value *RetVal = AST_Root->codegen()) {
        Builder->CreateRet(RetVal);
    } else {
        F->eraseFromParent();
        return false;
    }
    if (verifyFunction(*F, &errs())) {
        F->eraseFromParent();
        return false;
    }

    auto Filename = "output.ll";
    std::error_code EC;
    raw_fd_ostream dest(Filename, EC);

    if (EC) {
        errs() << "Could not open file: " << EC.message();
        return false;
    }

    F->print(errs());
    F->print(dest);
    F->eraseFromParent();
    return true;
}

//===----------------------------------------------------------------------===//
// Parser
//===----------------------------------------------------------------------===//
int yylex();
int symbol;
extern int yylval;

unique_ptr<GenericASTNode> Z();
unique_ptr<GenericASTNode> E_AS(); // Addition and subtraction
unique_ptr<GenericASTNode> E_MDR(); // Multiplication, Division and Remainder
unique_ptr<GenericASTNode> T();

void next_symbol()
{
	symbol = yylex();
}

unique_ptr<GenericASTNode> ParseError(const char *message)
{
    fprintf(stderr, "Syntax error: %s\n", message);
    return nullptr;
}

unique_ptr<GenericASTNode> Z()
{
    auto expression = E_AS();
    if (!expression) return nullptr;
    if (symbol == '=') next_symbol();
    while (symbol == '\n') next_symbol();
    if (symbol != 0) return ParseError("unexpected token after expression");
    return expression;
}

unique_ptr<GenericASTNode> E_AS()
{
    auto lhs = E_MDR();
    if (!lhs) return nullptr;
    while (symbol == '+' || symbol == '-') {
        char op = symbol;
        next_symbol();
        auto rhs = E_MDR();
        if (!rhs) return nullptr;
        lhs = make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs));
    }
    return lhs;
}

unique_ptr<GenericASTNode> E_MDR()
{
    auto lhs = T();
    if (!lhs) return nullptr;
    while (symbol == '*' || symbol == '/' || symbol == '%') {
        char op = symbol;
        next_symbol();
        auto rhs = T();
        if (!rhs) return nullptr;
        lhs = make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs));
    }
    return lhs;
}

unique_ptr<GenericASTNode> T()
{
    if (symbol == NUMBER) {
        auto number = make_unique<NumberASTNode>(yylval);
        next_symbol();
        return number;
    }
    if (symbol == '(') {
        next_symbol();
        auto expression = E_AS();
        if (!expression) return nullptr;
        if (symbol != ')') return ParseError("expected ')'");
        next_symbol();
        return expression;
    }
    return ParseError("expected an integer or '('");
}

//===----------------------------------------------------------------------===//
// main function
//===----------------------------------------------------------------------===//
int main()
{
    InitializeModule();

    next_symbol();
    auto root = Z();
    if (!root) return 1;
    return CodeGenTopLevel(std::move(root)) ? 0 : 1;
}