
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
    std::string text;
    public:
        StatementWrite(std::string text)
        {
            this->text = text;
        }
        ~StatementWrite()
        {
        }
        void Execute(Context* context) override
        {
            std::cout << text;
        }
};
class StatementRead: public Statement
{
    LocationValue<std::string>* lvalue;
    public:
        StatementRead(LocationValue<std::string>* lvalue)
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

template<typename T>
class StatementAssign: public Statement
{
    LocationValue<T>* lvalue;
    ArithmeticExpression<T>* expression;
    public:
        StatementAssign(LocationValue<T>* lvalue, ArithmeticExpression<T>* expression)
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
            T tmp = expression->Evaluate(context);
            lvalue->Set(tmp, context);
        }
};

class StatementAssignStr: public Statement
{
    LocationValue<std::string>* lvalue;
    std::string* text;
    public:
        StatementAssignStr(LocationValue<std::string>* lvalue, std::string* text)
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


class StatementWriteInt: public Statement
{
    ArithmeticExpression<int>* expression;
    public:
        StatementWriteInt(ArithmeticExpression<int>* expression)
        {
            this->expression = expression;
        }
        ~StatementWriteInt()
        {
            delete expression;
        }
        void Execute(Context* context) override
        {
            std::cout << ( expression->Evaluate(context) );
        }
};

class StatementReadInt: public Statement
{
    LocationValue<int>* lvalue;
    public:
        StatementReadInt(LocationValue<int>* lvalue)
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

class StatementWriteDouble: public Statement
{
    ArithmeticExpression<double>* expression;
    public:
        StatementWriteDouble(ArithmeticExpression<double>* expression)
        {
            this->expression = expression;
        }
        ~StatementWriteDouble()
        {
            delete expression;
        }
        void Execute(Context* context) override
        {
            std::cout << ( expression->Evaluate(context) );
        }
};

class StatementReadDouble: public Statement
{
    LocationValue<double>* lvalue;
    public:
        StatementReadDouble(LocationValue<double>* lvalue)
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
    ArithmeticExpression<int>* step;
    LocationValue<int>* iterator;
    public:
        StatementFor(LocationValue<int>* iterator, BooleanExpression* condition, Statement* statement,ArithmeticExpression<int>* step)
        {
            this->condition = condition;
            this->statement = statement;
            this->step = step;
            this->iterator = iterator;
        }
        StatementFor(LocationValue<int>* iterator, BooleanExpression* condition, Statement* statement)
        {
            this->condition = condition;
            this->statement = statement;
            this->step = new ArithmeticExpressionPlus<int>(iterator, new ArithmeticExpressionConst<int>(1));
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
                int newIterator = step->Evaluate(context);
                iterator->Set(newIterator, context);
            }
        }
};


#endif
