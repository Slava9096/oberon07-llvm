#ifndef __RELATIONALEXPRESSION_H
#define __RELATIONALEXPRESSION_H

#include "base.h"

    template<typename T>
    BINARYOP(RelationalExpressionEQ, RelationalExpression, ArithmeticExpression<T>, bool, == )
    template<typename T>
    BINARYOP(RelationalExpressionNEQ, RelationalExpression, ArithmeticExpression<T>, bool, != )
    template<typename T>
    BINARYOP(RelationalExpressionLT, RelationalExpression, ArithmeticExpression<T>, bool, < )
    template<typename T>
    BINARYOP(RelationalExpressionGT, RelationalExpression, ArithmeticExpression<T>, bool, > )
    template<typename T>
    BINARYOP(RelationalExpressionLEQ, RelationalExpression, ArithmeticExpression<T>, bool, <= )
    template<typename T>
    BINARYOP(RelationalExpressionGEQ, RelationalExpression, ArithmeticExpression<T>, bool, >= )

#endif
