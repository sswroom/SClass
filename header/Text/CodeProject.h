#ifndef _SM_TEXT_CODEPROJECT
#define _SM_TEXT_CODEPROJECT
#include "Data/ArrayList.h"
#include "IO/ConfigFile.h"
#include "IO/ParsedObject.h"

namespace Text
{
	class CodeObject
	{
	public:
		typedef enum
		{
			OT_FILE,
			OT_CONTAINER
		} ObjectType;

		virtual ~CodeObject();
		virtual ObjectType GetObjectType() = 0;
	};

	class CodeFile : public CodeObject
	{
	public:
		virtual ~CodeFile();
		virtual ObjectType GetObjectType();

		virtual const UTF8Char *GetFileName() = 0;
	};

	class CodeContainer : public CodeObject
	{
	public:
		virtual ~CodeContainer();

		virtual ObjectType GetObjectType();

		virtual const UTF8Char *GetContainerName() = 0;
		virtual OSInt GetChildCount() = 0;
		virtual CodeObject *GetChildObj(OSInt index) = 0;
	};

	class CodeProjectCfg : public IO::ConfigFile
	{
	private:
		const UTF8Char *cfgName;
	public:
		CodeProjectCfg(const UTF8Char *name);
		virtual ~CodeProjectCfg();

		const UTF8Char *GetCfgName();
	};

	class CodeProject : public IO::ParsedObject, public CodeContainer
	{
	public:
		typedef enum
		{
			PROJT_VSPROJECT
		} ProjectType;
	private:
		Data::ArrayList<CodeProjectCfg*> *cfgList;
	public:
		CodeProject(const UTF8Char *name);
		virtual ~CodeProject();

		void AddConfig(Text::CodeProjectCfg *cfg);
		OSInt GetConfigCnt();
		CodeProjectCfg *GetConfig(OSInt index);

		virtual ProjectType GetProjectType() = 0;
		virtual IO::ParsedObject::ParserType GetParserType();
	};
};
#endif
