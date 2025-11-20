#ifndef _SM_TEXT_UNICODECHARACTERDATA
#define _SM_TEXT_UNICODECHARACTERDATA
#include "Data/ArrayListNN.hpp"
#include "IO/PackageFile.h"
#include "IO/Stream.h"

namespace Text
{
	class UnicodeCharacterData
	{
	public:
		struct UnicodeData
		{
			UInt32 code;
			NN<Text::String> characterName;
			NN<Text::String> unicode10Name;
			UInt32 uppercaseMapping;
			UInt32 lowercaseMapping;
			UInt32 titlecaseMapping;
		};

	private:
		Data::ArrayListNN<UnicodeData> dataList;

		NN<UnicodeData> GetOrNewUnicodeData(UInt32 code);
	public:
		UnicodeCharacterData();
		~UnicodeCharacterData();

		void LoadUnicodeDataFile(Text::CStringNN filePath);
		void LoadUnicodeDataFile(NN<IO::Stream> stm);
		void LoadUCDPackage(NN<IO::PackageFile> pkg);
		void LoadUCDZip(Text::CStringNN ucdZipFile);
		Optional<UnicodeData> GetUnicodeData(UInt32 code) const;
	};
}
#endif
