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
		virtual ObjectType GetObjectType() const = 0;
	};

	class CodeFile : public CodeObject
	{
	public:
		virtual ~CodeFile();
		virtual ObjectType GetObjectType() const;

		virtual Text::String *GetFileName() const = 0;
	};

	class CodeContainer : public CodeObject
	{
	public:
		virtual ~CodeContainer();

		virtual ObjectType GetObjectType() const;

		virtual Text::String *GetContainerName() const = 0;
		virtual UOSInt GetChildCount() const = 0;
		virtual CodeObject *GetChildObj(UOSInt index) const = 0;
	};

	class CodeProjectCfg : public IO::ConfigFile
	{
	private:
		Text::String *cfgName;
	public:
		CodeProjectCfg(Text::String *name);
		virtual ~CodeProjectCfg();

		Text::String *GetCfgName() const;
	};

	class CodeProject : public IO::ParsedObject, public CodeContainer
	{
	public:
		typedef enum
		{
			PROJT_VSPROJECT
		} ProjectType;
	private:
		Data::ArrayList<CodeProjectCfg*> cfgList;
	public:
		CodeProject(Text::CString name);
		virtual ~CodeProject();

		void AddConfig(Text::CodeProjectCfg *cfg);
		UOSInt GetConfigCnt() const;
		CodeProjectCfg *GetConfig(UOSInt index) const;

		virtual ProjectType GetProjectType() const = 0;
		virtual IO::ParserType GetParserType() const;
	};
}
#endif
