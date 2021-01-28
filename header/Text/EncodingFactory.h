#ifndef _SM_TEXT_ENCODINGFACTORY
#define _SM_TEXT_ENCODINGFACTORY
#include "Data/StringUTF8Map.h"
#include "Data/ArrayListInt32.h"

namespace Text
{
	class EncodingFactory
	{
	public:
		typedef struct
		{
			Int32 codePage;
			const Char *dotNetName;
			const Char *desc;
			const Char *internetNames[12];
		} EncodingInfo;

	private:
		Data::StringUTF8Map<EncodingInfo*> *encMap;
		static EncodingInfo encInfo[];
	public:
		EncodingFactory();
		~EncodingFactory();
		
		Int32 GetCodePage(const UTF8Char *shortName);

		static UTF8Char *GetName(UTF8Char *buff, Int32 codePage);
		static UTF8Char *GetInternetName(UTF8Char *buff, Int32 codePage);
		static UTF8Char *GetDotNetName(UTF8Char *buff, Int32 codePage);
		static Int32 GetSystemCodePage();
		static Int32 GetSystemLCID();
		static void GetCodePages(Data::ArrayList<Int32> *codePages);
	};
}
#endif
