
#ifndef __ARITHMETICEXPRESSION_H
#define __ARITHMETICEXPRESSION_H

#include "base.h"
    template<typename T>
    class ArithmeticExpressionConst : public ArithmeticExpression<T>
    {
        T value;
        public:
            ArithmeticExpressionConst(T value) : ArithmeticExpression<T>()
            {
                this->value = value;
            }
            ~ArithmeticExpressionConst()
            {
            }
            int Evaluate(Context* context) override
            {
                return value;
            };
    };

    template<typename T>
    BINARYOP(ArithmeticExpressionPlus, ArithmeticExpression<T>, ArithmeticExpression<T>, T, + )
    template<typename T>
    BINARYOP(ArithmeticExpressionMinus, ArithmeticExpression<T>, ArithmeticExpression<T>, T, - )
    template<typename T>
    BINARYOP(ArithmeticExpressionMult, ArithmeticExpression<T>, ArithmeticExpression<T>, T, * )
             // WARN: should be double but for now T
    template<typename T>
    BINARYOP(ArithmeticExpressionDiv, ArithmeticExpression<T>, ArithmeticExpression<T>, T, / )
    template<typename T>
    BINARYOP(ArithmeticExpressionDivInt, ArithmeticExpression<T>, ArithmeticExpression<T>, T, / )
    template<typename T>
    BINARYOP(ArithmeticExpressionDivMod, ArithmeticExpression<T>, ArithmeticExpression<T>, T, % )

    template<typename T>
    UNARYOP(ArithmeticExpressionNeg, ArithmeticExpression<T>, ArithmeticExpression<T>, T, - )

#endif
