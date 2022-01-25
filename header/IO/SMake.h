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

		typedef struct
		{
			Text::String *name;
			Text::String *srcFile;
			Text::String *compileCfg;
			Data::ArrayList<Text::String *> *subItems;
			Data::ArrayList<Text::String *> *libs;
		} ProgramItem;

	private:
		Data::StringMap<ConfigItem*> *cfgMap;
		Data::FastStringMap<ProgramItem*> *progMap;
		Data::FastStringMap<Int64> *fileTimeMap;
		Sync::Mutex *errorMsgMut;
		Text::String *errorMsg;
		Text::String *basePath;
		IO::Writer *messageWriter;
		IO::Writer *cmdWriter;
		Text::String *debugObj;
		Sync::ParallelTask *tasks;

		void AppendCfgItem(Text::StringBuilderUTF8 *sb, const UTF8Char *val);
		void AppendCfgPath(Text::StringBuilderUTF8 *sb, const UTF8Char *path);
		void AppendCfg(Text::StringBuilderUTF8 *sb, const UTF8Char *compileCfg);

		Bool ExecuteCmd(const UTF8Char *cmd, UOSInt cmdLen);
		Bool LoadConfigFile(const UTF8Char *cfgFile, UOSInt cfgFileLen);

		Bool ParseSource(Data::ArrayListString *objList, Data::ArrayListString *libList, Data::ArrayListString *procList, Data::ArrayListString *headerList, Int64 *latestTime, const UTF8Char *sourceFile, UOSInt sourceFileLen);
		Bool ParseHeader(Data::ArrayListString *objList, Data::ArrayListString *libList, Data::ArrayListString *procList, Data::ArrayListString *headerList, Int64 *latestTime, Text::String *headerFile, const UTF8Char *sourceFile, UOSInt sourceFileLen);
		Bool ParseProgInternal(Data::ArrayListString *objList, Data::ArrayListString *libList, Data::ArrayListString *procList, Data::ArrayListString *headerList, Int64 *latestTime, Bool *progGroup, const ProgramItem *prog);

		static void __stdcall CompileTask(void *userObj);
		void CompileObject(Bool *errorState, const UTF8Char *cmd, UOSInt cmdLeng);
		Bool CompileProgInternal(ProgramItem *prog, Bool asmListing, Bool enableTest);

		void SetErrorMsg(const UTF8Char *msg, UOSInt msgLen);
	public:
		SMake(const UTF8Char *cfgFile, UOSInt threadCnt, IO::Writer *messageWriter);
		virtual ~SMake();

		virtual IO::ParserType GetParserType();

		Bool IsLoadFailed();
		Bool GetErrorMsg(Text::StringBuilderUTF8 *sb);
		void SetMessageWriter(IO::Writer *messageWriter);
		void SetCommandWriter(IO::Writer *cmdWriter);
		void SetDebugObj(const UTF8Char *debugObj, UOSInt len);
		void SetThreadCnt(UOSInt threadCnt);

		Data::ArrayList<ConfigItem*> *GetConfigList();
		Bool HasProg(const UTF8Char *progName);
		Bool CompileProg(const UTF8Char *progName, Bool asmListing);
		Bool ParseProg(Data::ArrayListString *objList, Data::ArrayListString *libList, Data::ArrayListString *procList, Data::ArrayListString *headerList, Int64 *latestTime, Bool *progGroup, Text::String *progName);

		void CleanFiles();

		UOSInt GetProgList(Data::ArrayList<Text::String*> *progList); //No release
		Bool IsProgGroup(const UTF8Char *progName);
		const ProgramItem *GetProgItem(const UTF8Char *progName);
	};
}

#endif
