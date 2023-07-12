#ifndef _SM_TEXT_CPP_CPPENV
#define _SM_TEXT_CPP_CPPENV
#include "Data/ArrayListStringNN.h"
#include "IO/ConfigFile.h"
#include "Text/VSProject.h"
#include "Text/Cpp/CppCodeParser.h"

namespace Text
{
	namespace Cpp
	{
		class CppEnv
		{
		private:
			Data::ArrayListStringNN includePaths;
			Text::VSProject::VisualStudioVersion vsv;
			Text::CodeProject::ProjectType pt;
			Text::String *baseFile;
			
			Bool InitVSEnv(Text::VSProject::VisualStudioVersion vsv);
		public:
			CppEnv(Text::VSProject::VisualStudioVersion vsv);
			CppEnv(Text::CodeProject *proj, IO::ConfigFile *cfg);
			~CppEnv();

			void AddIncludePath(Text::CString includePath);
			UTF8Char *GetIncludeFilePath(UTF8Char *buff, Text::CString includeFile, Text::String *sourceFile);

			void InitEnvStatus(Text::Cpp::CppParseStatus *status);

			static CppEnv *LoadVSEnv(Text::VSProject::VisualStudioVersion vsv);
			static CppEnv *LoadVSEnv();
			static UTF8Char *GetVCInstallDir(UTF8Char *sbuff, Text::VSProject::VisualStudioVersion vsv);
			static UTF8Char *GetWindowsSdkDir(UTF8Char *sbuff);
			static Bool IsCompilerExist(Text::VSProject::VisualStudioVersion vsv);
//			static CompilerType GetSystemCompiler(WChar *includePath);
		};
	};
};
#endif
