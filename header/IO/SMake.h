#ifndef _SM_IO_SMAKE
#define _SM_IO_SMAKE
#include "Data/ArrayList.h"
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
			Text::String *cmd;
			Bool *errorState;
			SMake *me;
		} CompileReq;
	public:
		typedef struct
		{
			Text::String *name;
			Text::String *value;
		} ConfigItem;

		class ProgramItem
		{
		public:
			Text::String *name;
			Text::String *srcFile;
			Text::String *compileCfg;
			Data::ArrayList<Text::String *> subItems;
			Data::ArrayList<Text::String *> libs;
		};

	private:
		Data::StringMap<ConfigItem*> cfgMap;
		Data::FastStringMap<ProgramItem*> progMap;
		Data::FastStringMap<Int64> fileTimeMap;
		Sync::Mutex errorMsgMut;
		Text::String *errorMsg;
		Text::String *basePath;
		IO::Writer *messageWriter;
		IO::Writer *cmdWriter;
		Text::String *debugObj;
		Sync::ParallelTask *tasks;

		void AppendCfgItem(Text::StringBuilderUTF8 *sb, Text::CString val);
		void AppendCfgPath(Text::StringBuilderUTF8 *sb, Text::CString path);
		void AppendCfg(Text::StringBuilderUTF8 *sb, Text::CString compileCfg);

		Bool ExecuteCmd(Text::CString cmd);
		Bool LoadConfigFile(Text::CString cfgFile);

		Bool ParseSource(Data::FastStringMap<Int32> *objList, Data::FastStringMap<Int32> *libList, Data::FastStringMap<Int32> *procList, Data::ArrayListString *headerList, Int64 *latestTime, Text::CString sourceFile, Text::StringBuilderUTF8 *tmpSb);
		Bool ParseHeader(Data::FastStringMap<Int32> *objList, Data::FastStringMap<Int32> *libList, Data::FastStringMap<Int32> *procList, Data::ArrayListString *headerList, Int64 *latestTime, Text::String *headerFile, Text::CString sourceFile, Text::StringBuilderUTF8 *tmpSb);
		Bool ParseProgInternal(Data::FastStringMap<Int32> *objList, Data::FastStringMap<Int32> *libList, Data::FastStringMap<Int32> *procList, Data::ArrayListString *headerList, Int64 *latestTime, Bool *progGroup, const ProgramItem *prog, Text::StringBuilderUTF8 *tmpSb);

		static void __stdcall CompileTask(void *userObj);
		void CompileObject(Bool *errorState, Text::CString cmd);
		Bool CompileProgInternal(ProgramItem *prog, Bool asmListing, Bool enableTest);

		void SetErrorMsg(Text::CString msg);
	public:
		SMake(Text::CString cfgFile, UOSInt threadCnt, IO::Writer *messageWriter);
		virtual ~SMake();

		virtual IO::ParserType GetParserType() const;

		Bool IsLoadFailed() const;
		Bool GetLastErrorMsg(Text::StringBuilderUTF8 *sb) const;
		void SetMessageWriter(IO::Writer *messageWriter);
		void SetCommandWriter(IO::Writer *cmdWriter);
		void SetDebugObj(Text::CString debugObj);
		void SetThreadCnt(UOSInt threadCnt);

		const Data::ArrayList<ConfigItem*> *GetConfigList() const;
		Bool HasProg(Text::CString progName) const;
		Bool CompileProg(Text::CString progName, Bool asmListing);
		Bool ParseProg(Data::FastStringMap<Int32> *objList, Data::FastStringMap<Int32> *libList, Data::FastStringMap<Int32> *procList, Data::ArrayListString *headerList, Int64 *latestTime, Bool *progGroup, Text::String *progName);

		void CleanFiles();

		UOSInt GetProgList(Data::ArrayList<Text::String*> *progList); //No release
		Bool IsProgGroup(Text::CString progName) const;
		const ProgramItem *GetProgItem(Text::CString progName) const;
	};
}

#endif
