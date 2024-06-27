#ifndef _SM_PARSER_FILEPARSER_ELFPARSER
#define _SM_PARSER_FILEPARSER_ELFPARSER
#include "IO/EXEFile.h"
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class ELFParser : public IO::FileParser
		{
		private:
			static Int64 __stdcall TReadInt64(UnsafeArray<const UInt8> pVal);
			static Int64 __stdcall TReadMInt64(UnsafeArray<const UInt8> pVal);
			static Int32 __stdcall TReadInt32(UnsafeArray<const UInt8> pVal);
			static Int32 __stdcall TReadMInt32(UnsafeArray<const UInt8> pVal);
			static Int16 __stdcall TReadInt16(UnsafeArray<const UInt8> pVal);
			static Int16 __stdcall TReadMInt16(UnsafeArray<const UInt8> pVal);

			typedef Int64 (CALLBACKFUNC RInt64Func)(UnsafeArray<const UInt8> pVal);
			typedef Int32 (CALLBACKFUNC RInt32Func)(UnsafeArray<const UInt8> pVal);
			typedef Int16 (CALLBACKFUNC RInt16Func)(UnsafeArray<const UInt8> pVal);
		public:
			ELFParser();
			virtual ~ELFParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
