#ifndef __STATEMENT_H
#define __STATEMENT_H

#include <cstddef>
#include <iostream>
#include <cmath>
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Casting.h"
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "base.h"
#include "arithmeticexpression.h"
#include "lvalue.h"
class StatementBlock : public Statement
{
    public:
    std::vector<Statement*>statements;
        StatementBlock(std::vector<Statement*>& statements)
        {
            this->statements = statements;
        }
        ~StatementBlock()
        {
            for(Statement* statement : statements) delete statement;
            statements.clear();
        }
        void Execute(Context* context) override
        {
            for(Statement* statement : statements) statement->Execute(context);
        }
        llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) override
        {
                for(Statement* statement : statements) statement->codegen(context, builder, symbolTable);
                return nullptr;
        }
};

class StatementWrite: public Statement
{
    Types text;
    public:
        StatementWrite(Types text)
        {
            this->text = text;
        }
        ~StatementWrite()
        {
        }
        void Execute(Context* context) override
        {
            std::visit([](auto&& value) {
                std::cout << value;
            }, text);
        }
        llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) override {
             llvm::Function* printfFunc = builder.GetInsertBlock()->getParent()->getParent()->getFunction("printf");
                if (!printfFunc) {
                    std::vector<llvm::Type*> printfArgs = {llvm::PointerType::get(*context, 0)};
                    llvm::FunctionType* printfType = llvm::FunctionType::get(builder.getInt32Ty(), printfArgs, true);
                    printfFunc = llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", builder.GetInsertBlock()->getParent()->getParent());
                }

                std::string formatStr;
                llvm::Value* value = nullptr;
                std::visit([&](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, int>) {
                        formatStr = "%d";
                        value = builder.getInt32(arg);
                    } else if constexpr (std::is_same_v<T, float>) {
                        formatStr = "%g";
                        value = llvm::ConstantFP::get(builder.getFloatTy(), static_cast<double>(arg));
                    } else if constexpr (std::is_same_v<T, bool>) {
                        formatStr = "%d";
                        value = builder.getInt1(arg);
                    } else if constexpr (std::is_same_v<T, std::string>) {
                        formatStr = "%s";
                        value = builder.CreateGlobalString(arg);
                    }
                }, text);

                llvm::Value* formatStrPtr = builder.CreateGlobalString(formatStr);
                std::vector<llvm::Value*> args = {formatStrPtr};

                if (value) {
                    if (llvm::isa<llvm::ConstantFP>(value) && value->getType()->isFloatTy()) {
                        // Продвигаем float до double для printf
                        llvm::Value* promoted = builder.CreateFPExt(value, builder.getDoubleTy());
                        args.push_back(promoted);
                    } else {
                        args.push_back(value);
                    }
                }

                builder.CreateCall(printfFunc, args);
                return nullptr;
        }
};
class StatementWriteVar: public Statement
{
    ArithmeticExpression* expression;
    public:
        StatementWriteVar(ArithmeticExpression* expression)
        {
            this->expression = expression;
        }
        ~StatementWriteVar()
        {
            delete expression;
        }
        void Execute(Context* context) override
        {
            Types value = expression->Evaluate(context);
            std::visit([](auto&& arg) {
                std::cout << arg;
            }, value);
        }
        llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) override {
            llvm::Function* printfFunc = builder.GetInsertBlock()->getParent()->getParent()->getFunction("printf");
            if (!printfFunc) {
                std::vector<llvm::Type*> printfArgs = {llvm::PointerType::get(*context, 0)};
                llvm::FunctionType* printfType = llvm::FunctionType::get(builder.getInt32Ty(), printfArgs, true);
                printfFunc = llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", builder.GetInsertBlock()->getParent()->getParent());
            }

            llvm::Value* value = expression->codegen(context, builder, symbolTable);
            std::string formatStr;

            if (value->getType()->isIntegerTy(32)) {
                formatStr = "%d";
            } else if (value->getType()->isFloatTy()) {
                formatStr = "%g";
                // Convert float to double for printf
                value = builder.CreateFPExt(value, builder.getDoubleTy());
            } else if (value->getType()->isIntegerTy(1)) {
                formatStr = "%d";
            } else if (value->getType()->isPointerTy()) {
                formatStr = "%s";
            }

            llvm::Value* formatStrPtr = builder.CreateGlobalString(formatStr);
            std::vector<llvm::Value*> args = {formatStrPtr, value};
            builder.CreateCall(printfFunc, args);
            return nullptr;
        }
};
class StatementRead: public Statement
{
    LocationValue* lvalue;
    public:
        StatementRead(LocationValue* lvalue)
        {
            this->lvalue = lvalue;
        }
        ~StatementRead()
        {
            delete lvalue;
        }
        void Execute(Context* context) override
        {
            std::string tmp;
            std::cin >> tmp;
            lvalue->Set(tmp, context);
        }
        llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) override {
            llvm::Function* scanfFunc = builder.GetInsertBlock()->getParent()->getParent()->getFunction("scanf");
            if (!scanfFunc) {
                std::vector<llvm::Type*> scanfArgs = {llvm::PointerType::get(*context, 0)};
                llvm::FunctionType* scanfType = llvm::FunctionType::get(builder.getInt32Ty(), scanfArgs, true);
                scanfFunc = llvm::Function::Create(scanfType, llvm::Function::ExternalLinkage, "scanf", builder.GetInsertBlock()->getParent()->getParent());
            }

            llvm::Value* formatStrPtr = builder.CreateGlobalString("%s");
            llvm::Value* buffer = builder.CreateAlloca(llvm::Type::getInt8Ty(*context), builder.getInt32(256));
            std::vector<llvm::Value*> args = {formatStrPtr, buffer};
            builder.CreateCall(scanfFunc, args);

            llvm::Value* lhsPtr = symbolTable->lookup(lvalue->GetName())->allocaInst;
            builder.CreateStore(buffer, lhsPtr);
            return nullptr;
        }
};

class StatementAssign: public Statement
{
    LocationValue* lvalue;
    ArithmeticExpression* expression;
    public:
        StatementAssign(LocationValue* lvalue, ArithmeticExpression* expression)
        {
            this->lvalue = lvalue;
            this->expression = expression;
        }
        ~StatementAssign()
        {
            delete lvalue;
            delete expression;
        }
        void Execute(Context* context) override
        {
            Types tmp = expression->Evaluate(context);
            lvalue->Set(tmp, context);
        }
        llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) override {
            llvm::Value* rhs = expression->codegen(context, builder, symbolTable);
            llvm::Value* lhsPtr = symbolTable->lookup(lvalue->GetName())->allocaInst;
            builder.CreateStore(rhs, lhsPtr);
            return nullptr;
        }
};

class StatementAssignStr: public Statement
{
    LocationValue* lvalue;
    std::string text;
    public:
        StatementAssignStr(LocationValue* lvalue, std::string* text)
        {
            this->lvalue = lvalue;
            this->text = *text;
        }
        ~StatementAssignStr()
        {
            delete lvalue;
        }
        void Execute(Context* context) override
        {
            lvalue->Set(text, context);
        }
        llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) override {
            llvm::Function* currentFunc = builder.GetInsertBlock()->getParent();
            llvm::Module* module = currentFunc->getParent();

            // Create a global string constant
            llvm::Constant* str = llvm::ConstantDataArray::getString(*context, text);
            llvm::GlobalVariable* gvar = new llvm::GlobalVariable(
                *module,
                str->getType(),
                true,
                llvm::GlobalValue::PrivateLinkage,
                str,
                ".str"
            );

            // Get the pointer to the string
            llvm::Value* strPtr = builder.CreateConstGEP1_64(str->getType(), gvar, 0, "strptr");

            // Store the string pointer in the variable
            llvm::Value* lhsPtr = symbolTable->lookup(lvalue->GetName())->allocaInst;
            builder.CreateStore(strPtr, lhsPtr);

            return nullptr;
        }
};

class StatementReadInt: public Statement
{
    LocationValue* lvalue;
    public:
        StatementReadInt(LocationValue* lvalue)
        {
            this->lvalue= lvalue;
        }
        ~StatementReadInt()
        {
            delete lvalue;
        }
        void Execute(Context* context) override
        {
            int tmp;
            std::cin >> tmp;
            lvalue->Set(tmp, context);
        }
        llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) override {
            llvm::Function* scanfFunc = builder.GetInsertBlock()->getParent()->getParent()->getFunction("scanf");
            if (!scanfFunc) {
                std::vector<llvm::Type*> scanfArgs = {llvm::PointerType::get(*context, 0)};
                llvm::FunctionType* scanfType = llvm::FunctionType::get(builder.getInt32Ty(), scanfArgs, true);
                scanfFunc = llvm::Function::Create(scanfType, llvm::Function::ExternalLinkage, "scanf", builder.GetInsertBlock()->getParent()->getParent());
            }

            llvm::Value* formatStrPtr = builder.CreateGlobalString("%d");
            llvm::Value* lhsPtr = symbolTable->lookup(lvalue->GetName())->allocaInst;
            std::vector<llvm::Value*> args = {formatStrPtr, lhsPtr};
            builder.CreateCall(scanfFunc, args);
            return nullptr;
        }
};

class StatementReadFloat: public Statement
{
    LocationValue* lvalue;
    public:
        StatementReadFloat(LocationValue* lvalue)
        {
            this->lvalue = lvalue;
        }
        ~StatementReadFloat()
        {
            delete lvalue;
        }
        void Execute(Context* context) override
        {
            std::string tmp;
            std::cin >> tmp;
            lvalue->Set(parseFloat(tmp), context);
        }
        llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) override {
            llvm::Function* scanfFunc = builder.GetInsertBlock()->getParent()->getParent()->getFunction("scanf");
            if (!scanfFunc) {
                std::vector<llvm::Type*> scanfArgs = {llvm::PointerType::get(*context, 0)};
                llvm::FunctionType* scanfType = llvm::FunctionType::get(builder.getInt32Ty(), scanfArgs, true);
                scanfFunc = llvm::Function::Create(scanfType, llvm::Function::ExternalLinkage, "scanf", builder.GetInsertBlock()->getParent()->getParent());
            }

            // Create a temporary float variable to store the scanf result
            llvm::Value* tempFloat = builder.CreateAlloca(builder.getFloatTy(), nullptr, "temp_float");
            llvm::Value* formatStrPtr = builder.CreateGlobalString("%f");
            std::vector<llvm::Value*> args = {formatStrPtr, tempFloat};
            builder.CreateCall(scanfFunc, args);

            // Load the value from the temporary variable
            llvm::Value* loadedFloat = builder.CreateLoad(builder.getFloatTy(), tempFloat, "loaded_float");
            
            // Store the loaded value into the target variable
            llvm::Value* lhsPtr = symbolTable->lookup(lvalue->GetName())->allocaInst;
            builder.CreateStore(loadedFloat, lhsPtr);
            
            return nullptr;
        }
    private:
    float parseFloat(const std::string& number){
        size_t ePos = number.find('E');

        if (ePos == std::string::npos){
            return std::stof(number);
        }

        return std::stof(number.substr(0,ePos)) * std::pow(10, std::stoi(number.substr(ePos + 1)));
    }
};

class StatementWhile: public Statement
{
    BooleanExpression* condition;
    Statement* statement;
    public:
        StatementWhile(BooleanExpression* condition, Statement* statement)
        {
            this->condition = condition;
            this->statement = statement;
        }
        ~StatementWhile()
        {
            delete condition;
            delete statement;
        }
        void Execute(Context* context) override
        {
            while (condition->Evaluate(context)) {
                statement->Execute(context);
            }
        }
        llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) override {
            llvm::Function* parentFunc = builder.GetInsertBlock()->getParent();

            llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(*context, "loop.cond", parentFunc);
            llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(*context, "loop.body", parentFunc);
            llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(*context, "loop.end", parentFunc);

            builder.CreateBr(condBlock);

            builder.SetInsertPoint(condBlock);
            llvm::Value* cond = condition->codegen(context, builder, symbolTable);

            // Convert condition to boolean if needed
            if (cond->getType()->isIntegerTy(32)) {
                cond = builder.CreateICmpNE(cond, llvm::Constant::getNullValue(cond->getType()), "cond.bool");
            } else if (cond->getType()->isFloatTy()) {
                cond = builder.CreateFCmpONE(cond, llvm::ConstantFP::get(cond->getType(), 0.0), "cond.bool");
            }

            builder.CreateCondBr(cond, bodyBlock, endBlock);

            builder.SetInsertPoint(bodyBlock);
            statement->codegen(context, builder, symbolTable);
            builder.CreateBr(condBlock);

            builder.SetInsertPoint(endBlock);
            return nullptr;
        }
};


class StatementIfElseIfElse: public Statement
{
    std::vector<BooleanExpression*> conditions;
    std::vector<Statement*> blocks;
    Statement* elseblock;
    public:
        StatementIfElseIfElse(std::vector<BooleanExpression*> conditions, std::vector<Statement*> blocks, Statement* elseblock)
        {
            this->conditions = conditions;
            this->blocks = blocks;
            this->elseblock = elseblock;
        }
        StatementIfElseIfElse(std::vector<BooleanExpression*> conditions, std::vector<Statement*> blocks)
        {
            this->conditions = conditions;
            this->blocks = blocks;
            this->elseblock = nullptr;
        }
        ~StatementIfElseIfElse()
        {
            for(BooleanExpression* condition : conditions) delete condition;
            for(Statement* block : blocks) delete block;
            delete elseblock;
        }
        void Execute(Context* context) override
        {
            for (size_t i = 0; i < conditions.size(); i++) {
                if(conditions[i]){
                    if (conditions[i]->Evaluate(context)) {
                        if(blocks[i]){
                            blocks[i]->Execute(context);
                            return;
                        }
                    }
                }
            }
            if(elseblock){
                elseblock->Execute(context);
            }
        }
    llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) override {
        llvm::Function* parentFunc = builder.GetInsertBlock()->getParent();
        llvm::BasicBlock* currentBlock = builder.GetInsertBlock();
        llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context, "if.end", parentFunc);

        if (conditions.empty()) {
            if (elseblock) {
                elseblock->codegen(context, builder, symbolTable);
            }
            builder.CreateBr(mergeBlock);
            return nullptr;
        }

        std::vector<llvm::BasicBlock*> condBlocks;
        std::vector<llvm::BasicBlock*> thenBlocks;

        for (size_t i = 0; i < conditions.size(); i++) {
            condBlocks.push_back(llvm::BasicBlock::Create(*context, "if.cond", parentFunc));
            thenBlocks.push_back(llvm::BasicBlock::Create(*context, "if.then", parentFunc));
        }

        llvm::BasicBlock* elseBlock = elseblock ? llvm::BasicBlock::Create(*context, "if.else", parentFunc) : mergeBlock;

        builder.SetInsertPoint(currentBlock);
        builder.CreateBr(condBlocks[0]);

        for (size_t i = 0; i < conditions.size(); i++) {
            builder.SetInsertPoint(condBlocks[i]);
            llvm::Value* cond = conditions[i]->codegen(context, builder, symbolTable);

            if (cond->getType() != builder.getInt1Ty()) {
                cond = builder.CreateICmpNE(cond, llvm::Constant::getNullValue(cond->getType()), "cond.bool");
            }

            llvm::BasicBlock* nextBlock = (i + 1 < conditions.size()) ? condBlocks[i + 1] : elseBlock;
            builder.CreateCondBr(cond, thenBlocks[i], nextBlock);

            builder.SetInsertPoint(thenBlocks[i]);
            if (blocks[i]) {
                blocks[i]->codegen(context, builder, symbolTable);
            }
            builder.CreateBr(mergeBlock);
        }

        if (elseblock) {
            builder.SetInsertPoint(elseBlock);
            elseblock->codegen(context, builder, symbolTable);
            builder.CreateBr(mergeBlock);
        }

        builder.SetInsertPoint(mergeBlock);
        return nullptr;
    }
};

// class StatementFor: public Statement
// {
//     BooleanExpression* condition;
//     Statement* statement;
//     ArithmeticExpression* step;
//     LocationValue* iterator;
//     public:
//         StatementFor(LocationValue* iterator, BooleanExpression* condition, Statement* statement,ArithmeticExpression* step)
//         {
//             this->condition = condition;
//             this->statement = statement;
//             this->step = step;
//             this->iterator = iterator;
//         }
//         StatementFor(LocationValue* iterator, BooleanExpression* condition, Statement* statement)
//         {
//             this->condition = condition;
//             this->statement = statement;
//             this->step = new ArithmeticExpressionPlus(iterator, new ArithmeticExpressionConst(1));
//             this->iterator = iterator;
//         }
//     ~StatementFor()
//         {
//             delete condition;
//             delete statement;
//             delete iterator;
//             delete step;
//         }
//         void Execute(Context* context) override
//         {
//             while (condition->Evaluate(context)) {
//                 statement->Execute(context);
//                 Types newIterator = step->Evaluate(context);
//                 iterator->Set(newIterator, context);
//             }
//         }
// };
template<typename T>
class DeclarationStatement : public Statement
{
    public:
        std::vector<std::string> names;
        DeclarationStatement(const std::vector<std::string>& names)
        {
            this->names = names;
        }
        ~DeclarationStatement()
        {
            names.clear();
        }
        void Execute(Context* context) override
        {
            for (int i = 0; i < names.size(); i++) {
                context->values.emplace(std::make_pair(names[i], T{}));
            }
        }
        llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) override {
            // Get the appropriate LLVM type
            llvm::Type* ty = nullptr;
            VarType varType;

            if constexpr (std::is_same_v<T, int>) {
                ty = builder.getInt32Ty();
                varType = VarType::Int;
            } else if constexpr (std::is_same_v<T, float>) {
                ty = builder.getFloatTy();
                varType = VarType::Float;
            } else if constexpr (std::is_same_v<T, bool>) {
                ty = builder.getInt1Ty();
                varType = VarType::Bool;
            } else if constexpr (std::is_same_v<T, std::string>) {
                ty = llvm::PointerType::get(builder.getInt8Ty(), 0);
                varType = VarType::String;
            } else {
                throw std::runtime_error("Unsupported variable type");
            }

            llvm::Function* currentFunc = builder.GetInsertBlock()->getParent();
            llvm::IRBuilder<> entryBuilder(&currentFunc->getEntryBlock(), currentFunc->getEntryBlock().begin());

            // Declare each variable
            for (int i = 0; i < names.size(); i++) {
                if (symbolTable->lookup(names[i])) {
                    throw std::runtime_error("Variable '" + names[i] + "' already declared");
                }

                llvm::AllocaInst* alloca = entryBuilder.CreateAlloca(ty, nullptr, names[i]);
                symbolTable->addVariable(names[i], varType, alloca);

                if constexpr (std::is_same_v<T, int>) {
                    entryBuilder.CreateStore(llvm::ConstantInt::get(ty, 0), alloca);
                } else if constexpr (std::is_same_v<T, float>) {
                    entryBuilder.CreateStore(llvm::ConstantFP::get(ty, 0.0), alloca);
                } else if constexpr (std::is_same_v<T, bool>) {
                    entryBuilder.CreateStore(llvm::ConstantInt::get(ty, 0), alloca);
                } else if constexpr (std::is_same_v<T, std::string>) {
                    llvm::Value* emptyStr = entryBuilder.CreateGlobalString("");
                    entryBuilder.CreateStore(emptyStr, alloca);
                }
            }
            return nullptr;
        }
};

class StatementModule : public Statement
{
    Statement* declarations;
    Statement* statements;
    public:
        StatementModule(Statement* declarations, Statement* statements)
        {
            this->declarations = declarations;
            this->statements = statements;
        }
        ~StatementModule()
        {
            delete declarations;
            delete statements;
        }
        void Execute(Context* context) override
        {
            declarations->Execute(context);
            statements->Execute(context);
        }
        llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) override
        {
            declarations->codegen(context, builder, symbolTable);
            statements->codegen(context, builder, symbolTable);
            return nullptr;
        }
};

#endif
