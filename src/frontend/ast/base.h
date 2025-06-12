#ifndef __BASE_H
#define __BASE_H

#include <llvm/IR/Instructions.h>
#include <map>
#include <string>
#include <variant>

#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"


using Types = std::variant<int, float, bool, std::string>;
enum class VarType { Int, Float, Bool, String };

class Context
{
    public:
        std::map<std::string, Types> values;
        Context() {}
        ~Context() {}
};

class Symbol {
public:
    std::string name;
    VarType type;
    llvm::AllocaInst* allocaInst;
    bool isInitialized;

    Symbol(const std::string& name, VarType type) : name(name), type(type), allocaInst(nullptr), isInitialized(false) {}

    ~Symbol() = default;
};

class SymbolTable {
private:
    std::map<std::string, Symbol*> symbols;
    SymbolTable* parent;

public:
    SymbolTable(SymbolTable* parent = nullptr) : parent(parent) {}

    ~SymbolTable() {
        for (auto& pair : symbols) {
            delete pair.second;
        }
    }

    bool addSymbol(const std::string& name, VarType type) {
        if (symbols.find(name) != symbols.end()) {
            return false;  // Symbol already exists in current scope
        }
        symbols[name] = new Symbol(name, type);
        return true;
    }

    bool addVariable(const std::string& name, VarType varType, llvm::AllocaInst* alloca) {
        if (lookup(name)) {
            return false;
        }

        Symbol* symbol = new Symbol(name, varType);
        symbol->allocaInst = alloca;
        symbol->isInitialized = true;

        symbols[name] = symbol;
        return true;
    }

    Symbol* lookup(const std::string& name) {
        auto it = symbols.find(name);
        if (it != symbols.end()) {
            return it->second;
        }
        if (parent) {
            return parent->lookup(name);
        }
        return nullptr;
    }

    bool isInitialized(const std::string& name) {
        Symbol* symbol = lookup(name);
        return symbol ? symbol->isInitialized : false;
    }

    VarType getType(const std::string& name) {
        Symbol* symbol = lookup(name);
        return symbol ? symbol->type : VarType::Int;  // Default to Int if not found
    }

    SymbolTable* createScope() {
        return new SymbolTable(this);
    }
};

class Statement
{
    public:
        Statement();
        virtual ~Statement();
        virtual void Execute(Context* context) = 0;
        virtual llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) = 0;
};

class BooleanExpression
{
    public:
        BooleanExpression();
        virtual ~BooleanExpression();
        virtual bool Evaluate(Context* context) = 0;
        virtual llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) = 0;
};

class RelationalExpression : public BooleanExpression
{
    public:
        RelationalExpression();
        ~RelationalExpression();
};

class ArithmeticExpression
{
    public:
        ArithmeticExpression(){};
        virtual ~ArithmeticExpression(){};
        virtual Types Evaluate(Context* context) = 0;
        virtual llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) = 0;
};

class LocationValue : public ArithmeticExpression
{
    public:
        LocationValue(){};
        virtual ~LocationValue(){};
        virtual void Set(Types value, Context* context) = 0;
        virtual std::string GetName() const = 0;
};

#define BINARYOP(NAME, BASETYPE, CONTENTTYPE, RETURNTYPE, OPERATION, CODEGEN) \
    class NAME : public BASETYPE { \
            CONTENTTYPE* l; \
            CONTENTTYPE* r; \
        public: \
            NAME(CONTENTTYPE* l, CONTENTTYPE* r) : BASETYPE(){ \
                this->l = l; \
                this->r = r; \
            } \
            ~NAME(){ \
                delete l; \
                delete r; \
            } \
            RETURNTYPE Evaluate(Context* context) override{ \
                return std::visit(OPERATION{}, l->Evaluate(context), r->Evaluate(context)); \
            } \
            llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) override { \
                llvm::Value* lhs = l->codegen(context, builder, symbolTable); \
                llvm::Value* rhs = r->codegen(context, builder, symbolTable); \
                return CODEGEN; \
            } \
        };


#define UNARYOP(NAME, BASETYPE, CONTENTTYPE, RETURNTYPE, OPERATION, CODEGEN) \
    class NAME : public BASETYPE { \
            CONTENTTYPE* x; \
        public: \
            NAME(CONTENTTYPE* x) : BASETYPE() { \
                this->x = x; \
            }\
            ~NAME(){ \
                delete x; \
            }\
            RETURNTYPE Evaluate(Context* context) override{ \
                return std::visit(OPERATION{}, x->Evaluate(context)); \
            } \
            llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) override { \
                llvm::Value* val = x->codegen(context, builder, symbolTable); \
                return CODEGEN; \
            } \
        };

#endif
