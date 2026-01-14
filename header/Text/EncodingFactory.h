#ifndef _SM_TEXT_ENCODINGFACTORY
#define _SM_TEXT_ENCODINGFACTORY
#include "Data/ArrayListInt32.h"
#include "Data/FastStringMapNN.hpp"

namespace Text
{
	class EncodingFactory
	{
	public:
		typedef struct
		{
			UInt32 codePage;
			const Char *dotNetName;
			const Char *desc;
			const Char *internetNames[12];
		} EncodingInfo;

	private:
		Data::FastStringMapNN<EncodingInfo> encMap;
		static EncodingInfo encInfo[];
	public:
		EncodingFactory();
		~EncodingFactory();
		
		UInt32 GetCodePage(Text::CStringNN shortName);

		static UnsafeArray<UTF8Char> GetName(UnsafeArray<UTF8Char> buff, UInt32 codePage);
		static UnsafeArray<UTF8Char> GetInternetName(UnsafeArray<UTF8Char> buff, UInt32 codePage);
		static UnsafeArray<UTF8Char> GetDotNetName(UnsafeArray<UTF8Char> buff, UInt32 codePage);
		static UInt32 GetSystemCodePage();
		static UInt32 GetSystemLCID();
		static void GetCodePages(NN<Data::ArrayListNative<UInt32>> codePages);
	};
}
#endif
