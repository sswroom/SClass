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
			};
		private:
			static Int64 __stdcall TReadInt64(UInt8 *pVal);
			static Int64 __stdcall TReadMInt64(UInt8 *pVal);
			static Int32 __stdcall TReadInt32(UInt8 *pVal);
			static Int32 __stdcall TReadMInt32(UInt8 *pVal);
			static Int16 __stdcall TReadInt16(UInt8 *pVal);
			static Int16 __stdcall TReadMInt16(UInt8 *pVal);

			typedef Int64 (__stdcall *RInt64Func)(UInt8* pVal);
			typedef Int32 (__stdcall *RInt32Func)(UInt8* pVal);
			typedef Int16 (__stdcall *RInt16Func)(UInt8* pVal);

			static Bool ParseType(ParseEnv *env);
			static UTF8Char *ToFuncName(UTF8Char *sbuff, const UTF8Char *funcName);
		public:
			ELFParser();
			virtual ~ELFParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	}
}
#endif
