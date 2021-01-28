#ifndef _SM_MATH_COMPLEXNUMBER
#define _SM_MATH_COMPLEXNUMBER

namespace Math
{
	class ComplexNumber
	{
	private:
		Double realPart;
		Double imaginaryPart;

	public:
		ComplexNumber();
		ComplexNumber(Double val);
		ComplexNumber(Double realPart, Double imaginaryPart);
		ComplexNumber(const ComplexNumber &val);
		~ComplexNumber();

		const ComplexNumber operator +(const ComplexNumber &val);
		const ComplexNumber operator -(const ComplexNumber &val);
		const ComplexNumber operator *(const ComplexNumber &val);

		Double GetRealPart();
		Double GetImaginaryPart();
	};
};
#endif
