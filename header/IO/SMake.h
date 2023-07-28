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
			Data::ArrayListNN<Text::String> subItems;
			Data::ArrayListNN<Text::String> libs;
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
		Data::ArrayListNN<Text::String> linkCmds;
		Data::ArrayListNN<const ProgramItem> testProgs;

		void AppendCfgItem(Text::StringBuilderUTF8 *sb, Text::CString val);
		void AppendCfgPath(Text::StringBuilderUTF8 *sb, Text::CString path);
		void AppendCfg(Text::StringBuilderUTF8 *sb, Text::CString compileCfg);

		Bool ExecuteCmd(Text::CString cmd);
		Bool LoadConfigFile(Text::CString cfgFile);

		Bool ParseSource(Data::FastStringMap<Int32> *objList, Data::FastStringMap<Int32> *libList, Data::FastStringMap<Int32> *procList, Data::ArrayListStringNN *headerList, Int64 *latestTime, Text::CString sourceFile, NotNullPtr<Text::StringBuilderUTF8> tmpSb);
		Bool ParseHeader(Data::FastStringMap<Int32> *objList, Data::FastStringMap<Int32> *libList, Data::FastStringMap<Int32> *procList, Data::ArrayListStringNN *headerList, Int64 *latestTime, NotNullPtr<Text::String> headerFile, Text::CString sourceFile, NotNullPtr<Text::StringBuilderUTF8> tmpSb);
		Bool ParseProgInternal(Data::FastStringMap<Int32> *objList, Data::FastStringMap<Int32> *libList, Data::FastStringMap<Int32> *procList, Data::ArrayListStringNN *headerList, Int64 *latestTime, Bool *progGroup, NotNullPtr<const ProgramItem> prog, NotNullPtr<Text::StringBuilderUTF8> tmpSb);

		static void __stdcall CompileTask(void *userObj);
		static void __stdcall TestTask(void *userObj);
		void CompileObject(Text::CString cmd);
		void CompileObject(NotNullPtr<Text::String> cmd);
		Bool CompileProgInternal(NotNullPtr<const ProgramItem> prog, Bool asmListing, Bool enableTest);
		Bool TestProg(NotNullPtr<const ProgramItem> prog, NotNullPtr<Text::StringBuilderUTF8> sb);

		void SetErrorMsg(Text::CString msg);
	public:
		SMake(Text::CString cfgFile, UOSInt threadCnt, IO::Writer *messageWriter);
		virtual ~SMake();

		virtual IO::ParserType GetParserType() const;

		void ClearLinks();
		void ClearStatus();
		Bool IsLoadFailed() const;
		Bool HasError() const;
		Bool GetLastErrorMsg(Text::StringBuilderUTF8 *sb) const;
		void SetMessageWriter(IO::Writer *messageWriter);
		void SetCommandWriter(IO::Writer *cmdWriter);
		void SetDebugObj(Text::CString debugObj);
		void SetThreadCnt(UOSInt threadCnt);
		void SetAsyncMode(Bool asyncMode);

		void AsyncPostCompile();

		NotNullPtr<const Data::ArrayList<ConfigItem*>> GetConfigList() const;
		Bool HasProg(Text::CString progName) const;
		Bool CompileProg(Text::CString progName, Bool asmListing);
		Bool ParseProg(Data::FastStringMap<Int32> *objList, Data::FastStringMap<Int32> *libList, Data::FastStringMap<Int32> *procList, Data::ArrayListStringNN *headerList, Int64 *latestTime, Bool *progGroup, Text::String *progName);

		void CleanFiles();

		UOSInt GetProgList(Data::ArrayList<Text::String*> *progList); //No release
		Bool IsProgGroup(Text::CString progName) const;
		const ProgramItem *GetProgItem(Text::CString progName) const;
	};
}

#endif
