//Not finish
#ifndef _SM_MATH_EQUATION
#define _SM_MATH_EQUATION
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListNN.hpp"
#include "Text/String.h"

namespace Math
{
	class Equation
	{
	private:
		Data::ArrayListNN<Data::ArrayListInt32> equationLeft;
		Data::ArrayListNN<Data::ArrayListInt32> equationRight;
		Data::ArrayListInt32 equationSign;
		Data::ArrayListNN<Text::String> equationVari;

		// Type, Value, Type = 0: Int value, 1: Float value, 2: Variable, 3: Function, 4:Operators
		// Operators: 0 = +, 1 = -, 2 = *, 3 = /, 4 = Neg

		Int32 ParseEquation(NN<Data::ArrayListInt32> equOut, UTF8Char *equation, OSInt nChars, UTF8Char **endPos);
	public:
		Equation();
		~Equation();

		UOSInt AddEquation(UTF8Char *equation); //Index
		void Solve(UTF8Char *variName, UTF8Char *outBuff);
		UTF8Char *GetEquationStr(UTF8Char *buff, Int32 index);
	};
}
#endif
