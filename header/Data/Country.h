#ifndef _SM_DATA_COUNTRY
#define _SM_DATA_COUNTRY
namespace Data
{
	class Country
	{
	public:
		typedef struct
		{
			const Char *name;
			const Char code2[3];
			const Char code3[4];
			const Char numCode[4];
			Int32 mcc;
			Int32 phoneCode;
		} CountryInfo;
	private:
		static CountryInfo countries[];

	public:
	};
};
#endif
