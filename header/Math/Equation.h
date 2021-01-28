//Not finish
#ifndef _SM_MATH_EQUATION
#define _SM_MATH_EQUATION
#include "Data/ArrayListInt.h"
#include "Data/ArrayList.h"

namespace Math
{
	class Equation
	{
	private:
		Data::ArrayList<Data::ArrayListInt*> *equationLeft;
		Data::ArrayList<Data::ArrayListInt*> *equationRight;
		Data::ArrayListInt *equationSign;
		Data::ArrayList<WChar*> *equationVari;

		// Type, Value, Type = 0: Int value, 1: Float value, 2: Variable, 3: Function, 4:Operators
		// Operators: 0 = +, 1 = -, 2 = *, 3 = /, 4 = Neg

		Int32 ParseEquation(Data::ArrayListInt *equOut, WChar *equation, OSInt nChars, WChar **endPos);
	public:
		Equation();
		~Equation();

		Int32 AddEquation(WChar *equation); //Index
		void Solve(WChar *variName, WChar *outBuff);
		WChar *GetEquationStr(WChar *buff, Int32 index);
	};
}
#endif
