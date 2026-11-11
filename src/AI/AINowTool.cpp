#include "Stdafx.h"
#include "AI/AINowTool.h"

AI::AINowTool::AINowTool()
{
}

AI::AINowTool::~AINowTool()
{
}

Text::CStringNN AI::AINowTool::GetName() const
{
	return CSTR("Now");
}

void AI::AINowTool::GetDescription(NN<Text::StringBuilderUTF8> sb) const
{
	sb->Append(CSTR("Get the current date and time"));
}

void AI::AINowTool::GetParameters(NN<Net::OpenAIToolParameters> parameters) const
{
}

Bool AI::AINowTool::Invoke(NN<Data::VariObject> input, NN<Data::VariObject> output)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr = Data::Timestamp::Now().ToString(sbuff);
	NN<Text::String> data = Text::String::NewP(sbuff, sptr);
	output->SetItemStr(U8STR("data"), data);
	data->Release();
	return true;
}
