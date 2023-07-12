#include "Stdafx.h"
#include "IO/VBoxVMInfo.h"

IO::VBoxVMInfo::VBoxVMInfo(Text::CString name)
{
	this->name = Text::String::New(name);
	this->state = State::Unknown;
	this->stateSince = 0;
}

IO::VBoxVMInfo::~VBoxVMInfo()
{
	this->name->Release();
}

NotNullPtr<Text::String> IO::VBoxVMInfo::GetName() const
{
	return this->name;
}

IO::VBoxVMInfo::State IO::VBoxVMInfo::GetState() const
{
	return this->state;
}

Data::Timestamp IO::VBoxVMInfo::GetStateSince() const
{
	return this->stateSince;
}

void IO::VBoxVMInfo::SetState(State state)
{
	this->state = state;
}

void IO::VBoxVMInfo::SetStateSince(Data::Timestamp stateSince)
{
	this->stateSince = stateSince;
}

Text::CString IO::VBoxVMInfo::StateGetName(State val)
{
	switch (val)
	{
	case State::Unknown:
		return CSTR("Unknown");
	case State::PoweredOff:
		return CSTR("PoweredOff");
	default:
		return CSTR_NULL;
	}
}
