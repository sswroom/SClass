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

		virtual Text::String *GetFileName() = 0;
	};

	class CodeContainer : public CodeObject
	{
	public:
		virtual ~CodeContainer();

		virtual ObjectType GetObjectType();

		virtual Text::String *GetContainerName() = 0;
		virtual UOSInt GetChildCount() = 0;
		virtual CodeObject *GetChildObj(UOSInt index) = 0;
	};

	class CodeProjectCfg : public IO::ConfigFile
	{
	private:
		Text::String *cfgName;
	public:
		CodeProjectCfg(Text::String *name);
		virtual ~CodeProjectCfg();

		Text::String *GetCfgName();
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
		CodeProject(Text::CString name);
		virtual ~CodeProject();

		void AddConfig(Text::CodeProjectCfg *cfg);
		UOSInt GetConfigCnt();
		CodeProjectCfg *GetConfig(UOSInt index);

		virtual ProjectType GetProjectType() = 0;
		virtual IO::ParserType GetParserType();
	};
}
#endif
