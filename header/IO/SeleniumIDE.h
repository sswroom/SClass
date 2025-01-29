#ifndef _SM_IO_SELENIUMIDE
#define _SM_IO_SELENIUMIDE
#include "Data/ArrayListNN.h"
#include "IO/ParsedObject.h"
#include "Text/JSON.h"

namespace IO
{
	struct SeleniumTarget
	{
		NN<Text::String> name;
		NN<Text::String> value;
	};

	class SeleniumCommand
	{
	private:
		Optional<Text::String> id;
		Optional<Text::String> comment;
		Optional<Text::String> command;
		Optional<Text::String> target;
		Optional<Text::String> value;
		Data::ArrayListNN<SeleniumTarget> targets;

	public:
		SeleniumCommand(NN<Text::JSONObject> command);
		~SeleniumCommand();

		Optional<Text::String> GetId() const { return this->id; }
		Optional<Text::String> GetComment() const { return this->comment; }
		Optional<Text::String> GetCommand() const { return this->command; }
		Optional<Text::String> GetTarget() const { return this->target; }
		Optional<Text::String> GetValue() const { return this->value; }
		Optional<SeleniumTarget> GetTargetItem(UOSInt index) const { return this->targets.GetItem(index); }
		static void __stdcall SeleniumTargetFree(NN<SeleniumTarget> target);
	};

	class SeleniumTest
	{
	private:
		Optional<Text::String> id;
		Optional<Text::String> name;
		Data::ArrayListNN<SeleniumCommand> commands;

	public:
		SeleniumTest(NN<Text::JSONObject> test);
		~SeleniumTest();

		Optional<Text::String> GetId() const { return this->id; }
		Optional<Text::String> GetName() const { return this->name; }
		Optional<SeleniumCommand> GetCommand(UOSInt index) const { return this->commands.GetItem(index); }
	};

	class SeleniumIDE : public IO::ParsedObject
	{
	private:
		Optional<Text::String> id;
		Optional<Text::String> version;
		Optional<Text::String> name;
		Optional<Text::String> url;
		Data::ArrayListNN<SeleniumTest> tests;
		//suites
		//urls
		//plugins

	public:
		SeleniumIDE(NN<Text::String> sourceName, NN<Text::JSONObject> side);
		virtual ~SeleniumIDE();

		virtual IO::ParserType GetParserType() const;
		Optional<Text::String> GetId() const { return this->id; }
		Optional<Text::String> GetVersion() const { return this->version; }
		Optional<Text::String> GetName() const { return this->name; }
		Optional<Text::String> GetURL() const { return this->url; }
		Optional<SeleniumTest> GetTest(UOSInt index) const { return this->tests.GetItem(index); }
	};
}
#endif
