#include "Stdafx.h"
#include "IO/SeleniumIDE.h"

IO::SeleniumCommand::SeleniumCommand(NN<Text::JSONObject> command)
{
	this->id = command->GetObjectNewString(CSTR("id"));
	this->comment = command->GetObjectNewString(CSTR("comment"));
	this->command = command->GetObjectNewString(CSTR("command"));
	this->target = command->GetObjectNewString(CSTR("target"));
	this->value = command->GetObjectNewString(CSTR("value"));
	NN<Text::JSONArray> targets;
	if (command->GetObjectArray(CSTR("targets")).SetTo(targets))
	{
		NN<SeleniumTarget> starget;
		NN<Text::JSONBase> targetBase;
		NN<Text::JSONArray> target;
		UOSInt i = 0;
		UOSInt j = targets->GetArrayLength();
		while (i < j)
		{
			if (targets->GetArrayValue(i).SetTo(targetBase) && targetBase->GetType() == Text::JSONType::Array)
			{
				target = NN<Text::JSONArray>::ConvertFrom(targetBase);
				if (target->GetArrayLength() == 2)
				{
					starget = MemAllocNN(SeleniumTarget);
					starget->value = Text::String::OrEmpty(target->GetArrayString(0))->Clone();
					starget->name = Text::String::OrEmpty(target->GetArrayString(1))->Clone();
					this->targets.Add(starget);
				}
			}
			i++;
		}
	}
}

IO::SeleniumCommand::~SeleniumCommand()
{
	OPTSTR_DEL(this->id);
	OPTSTR_DEL(this->comment);
	OPTSTR_DEL(this->command);
	OPTSTR_DEL(this->target);
	OPTSTR_DEL(this->value);
	this->targets.MemFreeAll();
}

IO::SeleniumTest::SeleniumTest(NN<Text::JSONObject> test)
{
	this->id = test->GetObjectNewString(CSTR("id"));
	this->name = test->GetObjectNewString(CSTR("name"));
	NN<Text::JSONArray> commands;
	NN<Text::JSONObject> command;
	NN<SeleniumCommand> cmd;
	if (test->GetObjectArray(CSTR("commands")).SetTo(commands))
	{
		UOSInt i = 0;
		UOSInt j = commands->GetArrayLength();
		while (i < j)
		{
			if (commands->GetArrayObject(i).SetTo(command))
			{
				NEW_CLASSNN(cmd, SeleniumCommand(command));
				this->commands.Add(cmd);
			}
			i++;
		}
	}
}

IO::SeleniumTest::~SeleniumTest()
{
	OPTSTR_DEL(this->id);
	OPTSTR_DEL(this->name);
	this->commands.DeleteAll();
}

IO::SeleniumIDE::SeleniumIDE(NN<Text::String> sourceName, NN<Text::JSONObject> side) : IO::ParsedObject(sourceName)
{
	this->id = side->GetObjectNewString(CSTR("id"));
	this->version = side->GetObjectNewString(CSTR("version"));
	this->name = side->GetObjectNewString(CSTR("name"));
	this->url = side->GetObjectNewString(CSTR("url"));
	NN<Text::JSONArray> tests;
	NN<Text::JSONObject> test;
	NN<SeleniumTest> tst;
	if (side->GetObjectArray(CSTR("tests")).SetTo(tests))
	{
		UOSInt i = 0;
		UOSInt j = tests->GetArrayLength();
		while (i < j)
		{
			if (tests->GetArrayObject(i).SetTo(test))
			{
				NEW_CLASSNN(tst, SeleniumTest(test));
				this->tests.Add(tst);
			}
			i++;
		}
	}
}

IO::SeleniumIDE::~SeleniumIDE()
{
	OPTSTR_DEL(this->id);
	OPTSTR_DEL(this->version);
	OPTSTR_DEL(this->name);
	OPTSTR_DEL(this->url);
	this->tests.DeleteAll();
}

IO::ParserType IO::SeleniumIDE::GetParserType() const
{
	return IO::ParserType::SeleniumIDE;
}
