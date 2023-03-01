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
			struct ParseEnv
			{
				UTF8Char *sbuff;
				const UTF8Char *funcName;
				Bool valid;
				Bool constFunc;
				UOSInt seqId;
				Text::PString substr[32];
				UOSInt tplId;
				Text::PString tplstr[4];
			};
		private:
			static Int64 __stdcall TReadInt64(const UInt8 *pVal);
			static Int64 __stdcall TReadMInt64(const UInt8 *pVal);
			static Int32 __stdcall TReadInt32(const UInt8 *pVal);
			static Int32 __stdcall TReadMInt32(const UInt8 *pVal);
			static Int16 __stdcall TReadInt16(const UInt8 *pVal);
			static Int16 __stdcall TReadMInt16(const UInt8 *pVal);

			typedef Int64 (__stdcall *RInt64Func)(const UInt8* pVal);
			typedef Int32 (__stdcall *RInt32Func)(const UInt8* pVal);
			typedef Int16 (__stdcall *RInt16Func)(const UInt8* pVal);

			static Bool ParseType(ParseEnv *env, Bool firstPart);
		public:
			ELFParser();
			virtual ~ELFParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);

			static UTF8Char *ToFuncName(UTF8Char *sbuff, const UTF8Char *funcName);
		};
	}
}
#endif
