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
			CppEnv(NN<Text::CodeProject> proj, Optional<IO::ConfigFile> cfg);
			~CppEnv();

			void AddIncludePath(Text::CStringNN includePath);
			UnsafeArrayOpt<UTF8Char> GetIncludeFilePath(UnsafeArray<UTF8Char> buff, Text::CStringNN includeFile, Text::String *sourceFile);

			void InitEnvStatus(NN<Text::Cpp::CppParseStatus> status);

			static CppEnv *LoadVSEnv(Text::VSProject::VisualStudioVersion vsv);
			static CppEnv *LoadVSEnv();
			static UnsafeArrayOpt<UTF8Char> GetVCInstallDir(UnsafeArray<UTF8Char> sbuff, Text::VSProject::VisualStudioVersion vsv);
			static UnsafeArrayOpt<UTF8Char> GetWindowsSdkDir(UnsafeArray<UTF8Char> sbuff);
			static Bool IsCompilerExist(Text::VSProject::VisualStudioVersion vsv);
//			static CompilerType GetSystemCompiler(WChar *includePath);
		};
	};
};
#endif
