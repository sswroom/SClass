#ifndef _SM_MATH_LANCZOSFILTER
#define _SM_MATH_LANCZOSFILTER
namespace Math
{
	class LanczosFilter
	{
	private:
		Double nTap;
		Double iTap;
	public:
		LanczosFilter(UOSInt nTap);
		~LanczosFilter();

		Double Weight(Double phase);
	};
}
#endif
