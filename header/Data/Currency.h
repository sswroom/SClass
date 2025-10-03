#ifndef _SM_DATA_CURRENCY
#define _SM_DATA_CURRENCY
#if IS_BYTEORDER_LE
#define MAKECURRENCY(a, b, c) (a | (b << 8) | (c << 16))
#else
#define MAKECURRENCY(a, b, c) ((a << 24) | (b << 16) | (c << 8))
#endif
namespace Data
{
	class Currency
	{
	public:
		static UOSInt GetDecimal(UInt32 currency)
		{
			switch (currency)
			{
			case MAKECURRENCY('J', 'P', 'Y'):
				return 0;
			case MAKECURRENCY('X', 'A', 'U'):
				return 6;
			default:
				return 2;
			}
		}
	};
}
#endif
