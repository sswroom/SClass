#ifndef _SM_TEXT_CODEPROJECT
#define _SM_TEXT_CODEPROJECT
#include "Data/ArrayListNN.hpp"
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

		virtual NN<Text::String> GetFileName() const = 0;
	};

	class CodeContainer : public CodeObject
	{
	public:
		virtual ~CodeContainer();

		virtual ObjectType GetObjectType() const;

		virtual NN<Text::String> GetContainerName() const = 0;
		virtual UIntOS GetChildCount() const = 0;
		virtual NN<CodeObject> GetChildNoCheck(UIntOS index) const = 0;
		virtual Optional<CodeObject> GetChildObj(UIntOS index) const = 0;
	};

	class CodeProjectCfg : public IO::ConfigFile
	{
	private:
		NN<Text::String> cfgName;
	public:
		CodeProjectCfg(NN<Text::String> name);
		virtual ~CodeProjectCfg();

		NN<Text::String> GetCfgName() const;
	};

	class CodeProject : public IO::ParsedObject, public CodeContainer
	{
	public:
		typedef enum
		{
			PROJT_VSPROJECT
		} ProjectType;
	private:
		Data::ArrayListNN<CodeProjectCfg> cfgList;
	public:
		CodeProject(Text::CStringNN name);
		virtual ~CodeProject();

		void AddConfig(NN<Text::CodeProjectCfg> cfg);
		UIntOS GetConfigCnt() const;
		Optional<CodeProjectCfg> GetConfig(UIntOS index) const;

		virtual ProjectType GetProjectType() const = 0;
		virtual IO::ParserType GetParserType() const;
	};
}
#endif
