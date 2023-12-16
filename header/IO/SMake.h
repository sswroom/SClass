#ifndef _SM_IO_SMAKE
#define _SM_IO_SMAKE
#include "Data/ArrayList.h"
#include "Data/ArrayListNN.h"
#include "Data/ArrayListStringNN.h"
#include "Data/ArrayListStrUTF8.h"
#include "Data/FastStringMap.h"
#include "Data/StringMap.h"
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
			NotNullPtr<Text::String> cmd;
			SMake *me;
		} CompileReq;
	public:
		typedef struct
		{
			NotNullPtr<Text::String> name;
			NotNullPtr<Text::String> value;
		} ConfigItem;

		class ProgramItem
		{
		public:
			NotNullPtr<Text::String> name;
			Text::String *srcFile;
			Text::String *compileCfg;
			Data::ArrayListStringNN subItems;
			Data::ArrayListStringNN libs;
			Bool compiled;
		};

	private:
		Data::StringMap<ConfigItem*> cfgMap;
		Data::FastStringMap<ProgramItem*> progMap;
		Data::FastStringMap<Int64> fileTimeMap;
		Sync::Mutex errorMsgMut;
		Text::String *errorMsg;
		NotNullPtr<Text::String> basePath;
		IO::Writer *messageWriter;
		IO::Writer *cmdWriter;
		Text::String *debugObj;
		Sync::ParallelTask *tasks;
		Bool error;
		Bool asyncMode;
		Data::ArrayListStringNN linkCmds;
		Data::ArrayListNN<const ProgramItem> testProgs;

		void AppendCfgItem(NotNullPtr<Text::StringBuilderUTF8> sb, Text::CStringNN val);
		void AppendCfgPath(NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString path);
		void AppendCfg(NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString compileCfg);

		Bool ExecuteCmd(Text::CString cmd);
		Bool LoadConfigFile(Text::CStringNN cfgFile);

		Bool ParseSource(NotNullPtr<Data::FastStringMap<Int32>> objList, NotNullPtr<Data::FastStringMap<Int32>> libList, NotNullPtr<Data::FastStringMap<Int32>> procList, Optional<Data::ArrayListStringNN> headerList, OutParam<Int64> latestTime, Text::CStringNN sourceFile, NotNullPtr<Text::StringBuilderUTF8> tmpSb);
		Bool ParseHeader(NotNullPtr<Data::FastStringMap<Int32>> objList, NotNullPtr<Data::FastStringMap<Int32>> libList, NotNullPtr<Data::FastStringMap<Int32>> procList, Optional<Data::ArrayListStringNN> headerList, OutParam<Int64> latestTime, NotNullPtr<Text::String> headerFile, Text::CStringNN sourceFile, NotNullPtr<Text::StringBuilderUTF8> tmpSb);
		Bool ParseObject(NotNullPtr<Data::FastStringMap<Int32>> objList, NotNullPtr<Data::FastStringMap<Int32>> libList, NotNullPtr<Data::FastStringMap<Int32>> procList, Optional<Data::ArrayListStringNN> headerList, OutParam<Int64> latestTime, NotNullPtr<Text::String> objectFile, Text::CStringNN sourceFile, NotNullPtr<Text::StringBuilderUTF8> tmpSb);
		Bool ParseProgInternal(NotNullPtr<Data::FastStringMap<Int32>> objList, NotNullPtr<Data::FastStringMap<Int32>> libList, NotNullPtr<Data::FastStringMap<Int32>> procList, Optional<Data::ArrayListStringNN> headerList, OutParam<Int64> latestTime, OutParam<Bool> progGroup, NotNullPtr<const ProgramItem> prog, NotNullPtr<Text::StringBuilderUTF8> tmpSb);

		static void __stdcall CompileTask(void *userObj);
		static void __stdcall TestTask(void *userObj);
		void CompileObject(Text::CStringNN cmd);
		void CompileObject(NotNullPtr<Text::String> cmd);
		Bool CompileProgInternal(NotNullPtr<const ProgramItem> prog, Bool asmListing, Bool enableTest);
		Bool TestProg(NotNullPtr<const ProgramItem> prog, NotNullPtr<Text::StringBuilderUTF8> sb);

		void SetErrorMsg(Text::CString msg);
	public:
		SMake(Text::CStringNN cfgFile, UOSInt threadCnt, IO::Writer *messageWriter);
		virtual ~SMake();

		virtual IO::ParserType GetParserType() const;

		void ClearLinks();
		void ClearStatus();
		Bool IsLoadFailed() const;
		Bool HasError() const;
		Bool GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> sb) const;
		void SetMessageWriter(IO::Writer *messageWriter);
		void SetCommandWriter(IO::Writer *cmdWriter);
		void SetDebugObj(Text::CString debugObj);
		void SetThreadCnt(UOSInt threadCnt);
		void SetAsyncMode(Bool asyncMode);

		void AsyncPostCompile();

		NotNullPtr<const Data::ArrayList<ConfigItem*>> GetConfigList() const;
		Bool HasProg(Text::CStringNN progName) const;
		Bool CompileProg(Text::CStringNN progName, Bool asmListing);
		Bool ParseProg(NotNullPtr<Data::FastStringMap<Int32>> objList, NotNullPtr<Data::FastStringMap<Int32>> libList, NotNullPtr<Data::FastStringMap<Int32>> procList, Optional<Data::ArrayListStringNN> headerList, OutParam<Int64> latestTime, OutParam<Bool> progGroup, NotNullPtr<Text::String> progName);

		void CleanFiles();

		UOSInt GetProgList(NotNullPtr<Data::ArrayList<Text::String*>> progList); //No release
		Bool IsProgGroup(Text::CStringNN progName) const;
		const ProgramItem *GetProgItem(Text::CStringNN progName) const;
	};
}

#endif
