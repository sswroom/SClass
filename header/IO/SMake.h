#ifndef _SM_IO_SMAKE
#define _SM_IO_SMAKE
#include "AnyType.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListNN.h"
#include "Data/ArrayListStringNN.h"
#include "Data/ArrayListStrUTF8.h"
#include "Data/FastStringMap.h"
#include "Data/FastStringMapNN.h"
#include "Data/StringMapNN.h"
#include "IO/ParsedObject.h"
#include "IO/Writer.h"
#include "Sync/Mutex.h"
#include "Sync/ParallelTask.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	class SMake : public IO::ParsedObject
	{
	private:
		typedef struct
		{
			NN<Text::String> cmd;
			SMake *me;
		} CompileReq;
	public:
		typedef struct
		{
			NN<Text::String> name;
			NN<Text::String> value;
		} ConfigItem;

		class ProgramItem
		{
		public:
			NN<Text::String> name;
			Text::String *srcFile;
			Text::String *compileCfg;
			Data::ArrayListStringNN subItems;
			Data::ArrayListStringNN libs;
			Bool compiled;
		};

	private:
		Data::StringMapNN<ConfigItem> cfgMap;
		Data::FastStringMapNN<ProgramItem> progMap;
		Data::FastStringMap<Int64> fileTimeMap;
		Sync::Mutex errorMsgMut;
		Text::String *errorMsg;
		NN<Text::String> basePath;
		IO::Writer *messageWriter;
		IO::Writer *cmdWriter;
		Text::String *debugObj;
		Sync::ParallelTask *tasks;
		Bool error;
		Bool asyncMode;
		Data::ArrayListStringNN linkCmds;
		Data::ArrayListNN<const ProgramItem> testProgs;

		void AppendCfgItem(NN<Text::StringBuilderUTF8> sb, Text::CStringNN val);
		void AppendCfgPath(NN<Text::StringBuilderUTF8> sb, Text::CString path);
		void AppendCfg(NN<Text::StringBuilderUTF8> sb, Text::CString compileCfg);

		Bool ExecuteCmd(Text::CStringNN cmd);
		Bool LoadConfigFile(Text::CStringNN cfgFile);

		Text::PString ParseCond(Text::PString str1, OutParam<Bool> valid);
		Bool ParseSource(NN<Data::FastStringMap<Int32>> objList, NN<Data::FastStringMap<Int32>> libList, NN<Data::FastStringMap<Int32>> procList, Optional<Data::ArrayListStringNN> headerList, OutParam<Int64> latestTime, Text::CStringNN sourceFile, NN<Text::StringBuilderUTF8> tmpSb);
		Bool ParseHeader(NN<Data::FastStringMap<Int32>> objList, NN<Data::FastStringMap<Int32>> libList, NN<Data::FastStringMap<Int32>> procList, Optional<Data::ArrayListStringNN> headerList, OutParam<Int64> latestTime, NN<Text::String> headerFile, Text::CStringNN sourceFile, NN<Text::StringBuilderUTF8> tmpSb);
		Bool ParseObject(NN<Data::FastStringMap<Int32>> objList, NN<Data::FastStringMap<Int32>> libList, NN<Data::FastStringMap<Int32>> procList, Optional<Data::ArrayListStringNN> headerList, OutParam<Int64> latestTime, NN<Text::String> objectFile, Text::CStringNN sourceFile, NN<Text::StringBuilderUTF8> tmpSb);
		Bool ParseProgInternal(NN<Data::FastStringMap<Int32>> objList, NN<Data::FastStringMap<Int32>> libList, NN<Data::FastStringMap<Int32>> procList, Optional<Data::ArrayListStringNN> headerList, OutParam<Int64> latestTime, OutParam<Bool> progGroup, NN<const ProgramItem> prog, NN<Text::StringBuilderUTF8> tmpSb);

		static void __stdcall CompileTask(AnyType userObj);
		void CompileObject(Text::CStringNN cmd);
		void CompileObject(NN<Text::String> cmd);
		Bool CompileProgInternal(NN<const ProgramItem> prog, Bool asmListing, Bool enableTest);
		Bool TestProg(NN<const ProgramItem> prog, NN<Text::StringBuilderUTF8> sb);

		void SetErrorMsg(Text::CString msg);
	public:
		SMake(Text::CStringNN cfgFile, UOSInt threadCnt, IO::Writer *messageWriter);
		virtual ~SMake();

		virtual IO::ParserType GetParserType() const;

		void ClearLinks();
		void ClearStatus();
		Bool IsLoadFailed() const;
		Bool HasError() const;
		Bool GetLastErrorMsg(NN<Text::StringBuilderUTF8> sb) const;
		void SetMessageWriter(IO::Writer *messageWriter);
		void SetCommandWriter(IO::Writer *cmdWriter);
		void SetDebugObj(Text::CString debugObj);
		void SetThreadCnt(UOSInt threadCnt);
		void SetAsyncMode(Bool asyncMode);

		void AsyncPostCompile();

		NN<const Data::ArrayListNN<ConfigItem>> GetConfigList() const;
		Bool HasProg(Text::CStringNN progName) const;
		Bool CompileProg(Text::CStringNN progName, Bool asmListing);
		Bool ParseProg(NN<Data::FastStringMap<Int32>> objList, NN<Data::FastStringMap<Int32>> libList, NN<Data::FastStringMap<Int32>> procList, Optional<Data::ArrayListStringNN> headerList, OutParam<Int64> latestTime, OutParam<Bool> progGroup, NN<Text::String> progName);

		void CleanFiles();

		UOSInt GetProgList(NN<Data::ArrayListNN<Text::String>> progList); //No release
		Bool IsProgGroup(Text::CStringNN progName) const;
		Optional<const ProgramItem> GetProgItem(Text::CStringNN progName) const;
	};
}

#endif
