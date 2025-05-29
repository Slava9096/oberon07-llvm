
#ifndef __STATEMENT_H
#define __STATEMENT_H

#include <cstddef>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>

#include "base.h"
#include "arithmeticexpression.h"
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
        }
        void Execute(Context* context) override
        {
        Types value = expression->Evaluate(context);

        std::visit([](auto&& arg) {
            std::cout << arg;
        }, value);
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
};

class StatementAssignStr: public Statement
{
    LocationValue* lvalue;
    std::string* text;
    public:
        StatementAssignStr(LocationValue* lvalue, std::string* text)
        {
            this->lvalue = lvalue;
            this->text = text;
        }
        ~StatementAssignStr()
        {
            delete lvalue;
            delete text;
        }
        void Execute(Context* context) override
        {
            lvalue->Set(*text, context);
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
};

class StatementReadDouble: public Statement
{
    LocationValue* lvalue;
    public:
        StatementReadDouble(LocationValue* lvalue)
        {
            this->lvalue= lvalue;
        }
        ~StatementReadDouble()
        {
            delete lvalue;
        }
        void Execute(Context* context) override
        {
            std::string tmp;
            std::cin >> tmp;
            lvalue->Set(parseDouble(tmp), context);
        }
    private:
    double parseDouble(const std::string& number){
        size_t ePos = number.find('E');

        if (ePos == std::string::npos){
            return std::stod(number);
        }

        return std::stod(number.substr(0,ePos)) * std::pow(10, std::stoi(number.substr(ePos + 1)));
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
};

class StatementFor: public Statement
{
    BooleanExpression* condition;
    Statement* statement;
    ArithmeticExpression* step;
    LocationValue* iterator;
    public:
        StatementFor(LocationValue* iterator, BooleanExpression* condition, Statement* statement,ArithmeticExpression* step)
        {
            this->condition = condition;
            this->statement = statement;
            this->step = step;
            this->iterator = iterator;
        }
        StatementFor(LocationValue* iterator, BooleanExpression* condition, Statement* statement)
        {
            this->condition = condition;
            this->statement = statement;
            this->step = new ArithmeticExpressionPlus(iterator, new ArithmeticExpressionConst(1));
            this->iterator = iterator;
        }
    ~StatementFor()
        {
            delete condition;
            delete statement;
            delete iterator;
            delete step;
        }
        void Execute(Context* context) override
        {
            while (condition->Evaluate(context)) {
                statement->Execute(context);
                Types newIterator = step->Evaluate(context);
                iterator->Set(newIterator, context);
            }
        }
};
template<typename T>
class DeclarationStatement : public Statement
{
    public:
        std::string name;
        DeclarationStatement(const std::string& name)
        {
            this->name = name;
        }
        ~DeclarationStatement()
        {
        }
        void Execute(Context* context)
        {
            context->values.emplace(std::make_pair(name, T{}));
        }
};

#endif
