#ifndef _SM_IO_SMAKE
#define _SM_IO_SMAKE
#include "Data/ArrayList.h"
#include "Data/ArrayListStrUTF8.h"
#include "Data/StringUTF8Map.h"
#include "IO/IWriter.h"
#include "IO/ParsedObject.h"
#include "Sync/Mutex.h"
#include "Sync/ParallelTask.h"
#include "Text/StringBuilderUTF.h"

namespace IO
{
	class SMake : public IO::ParsedObject
	{
	private:
		typedef struct
		{
			const UTF8Char *cmd;
			Bool *errorState;
			SMake *me;
		} CompileReq;
	public:
		typedef struct
		{
			const UTF8Char *name;
			const UTF8Char *value;
		} ConfigItem;

		typedef struct
		{
			const UTF8Char *name;
			const UTF8Char *srcFile;
			const UTF8Char *compileCfg;
			Data::ArrayList<const UTF8Char *> *subItems;
			Data::ArrayList<const UTF8Char *> *libs;
		} ProgramItem;

	private:
		Data::StringUTF8Map<ConfigItem*> *cfgMap;
		Data::StringUTF8Map<ProgramItem*> *progMap;
		Data::StringUTF8Map<Int64> *fileTimeMap;
		Sync::Mutex *errorMsgMut;
		const UTF8Char *errorMsg;
		const UTF8Char *basePath;
		IO::IWriter *messageWriter;
		IO::IWriter *cmdWriter;
		const UTF8Char *debugObj;
		Sync::ParallelTask *tasks;

		void AppendCfgItem(Text::StringBuilderUTF *sb, const UTF8Char *val);
		void AppendCfgPath(Text::StringBuilderUTF *sb, const UTF8Char *path);
		void AppendCfg(Text::StringBuilderUTF *sb, const UTF8Char *compileCfg);

		Bool ExecuteCmd(const UTF8Char *cmd);
		Bool LoadConfigFile(const UTF8Char *cfgFile);

		Bool ParseSource(Data::ArrayListStrUTF8 *objList, Data::ArrayListStrUTF8 *libList, Data::ArrayListStrUTF8 *procList, Data::ArrayListStrUTF8 *headerList, Int64 *latestTime, const UTF8Char *sourceFile);
		Bool ParseHeader(Data::ArrayListStrUTF8 *objList, Data::ArrayListStrUTF8 *libList, Data::ArrayListStrUTF8 *procList, Data::ArrayListStrUTF8 *headerList, Int64 *latestTime, const UTF8Char *headerFile, const UTF8Char *sourceFile);
		Bool ParseProgInternal(Data::ArrayListStrUTF8 *objList, Data::ArrayListStrUTF8 *libList, Data::ArrayListStrUTF8 *procList, Data::ArrayListStrUTF8 *headerList, Int64 *latestTime, Bool *progGroup, const ProgramItem *prog);

		static void CompileTask(void *userObj);
		void CompileObject(Bool *errorState, const UTF8Char *cmd);
		Bool CompileProgInternal(ProgramItem *prog, Bool asmListing);

		void SetErrorMsg(const UTF8Char *msg);
	public:
		SMake(const UTF8Char *cfgFile, UOSInt threadCnt, IO::IWriter *messageWriter);
		virtual ~SMake();

		virtual IO::ParsedObject::ParserType GetParserType();

		Bool IsLoadFailed();
		Bool GetErrorMsg(Text::StringBuilderUTF *sb);
		void SetMessageWriter(IO::IWriter *messageWriter);
		void SetCommandWriter(IO::IWriter *cmdWriter);
		void SetDebugObj(const UTF8Char *debugObj);
		void SetThreadCnt(UOSInt threadCnt);

		Data::ArrayList<ConfigItem*> *GetConfigList();
		Bool HasProg(const UTF8Char *progName);
		Bool CompileProg(const UTF8Char *progName, Bool asmListing);
		Bool ParseProg(Data::ArrayListStrUTF8 *objList, Data::ArrayListStrUTF8 *libList, Data::ArrayListStrUTF8 *procList, Data::ArrayListStrUTF8 *headerList, Int64 *latestTime, Bool *progGroup, const UTF8Char *progName);

		void CleanFiles();

		UOSInt GetProgList(Data::ArrayList<const UTF8Char*> *progList); //No release
		Bool IsProgGroup(const UTF8Char *progName);
		const ProgramItem *GetProgItem(const UTF8Char *progName);
	};
}

#endif
