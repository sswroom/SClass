#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ProtoDec/ProtoDecList.h"
#include "IO/ProtoDec/JTT808ProtocolDecoder.h"
#include "IO/ProtoDec/TK109ProtocolDecoder.h"
#include "IO/ProtoDec/TSProtocolDecoder.h"

IO::ProtoDec::ProtoDecList::ProtoDecList()
{
	IO::ProtoDec::IProtocolDecoder *protoDec;
	NEW_CLASS(this->decList, Data::ArrayList<IO::ProtoDec::IProtocolDecoder*>());

	NEW_CLASS(protoDec, IO::ProtoDec::TK109ProtocolDecoder());
	this->decList->Add(protoDec);
	NEW_CLASS(protoDec, IO::ProtoDec::TSProtocolDecoder());
	this->decList->Add(protoDec);
	NEW_CLASS(protoDec, IO::ProtoDec::JTT808ProtocolDecoder());
	this->decList->Add(protoDec);
}

IO::ProtoDec::ProtoDecList::~ProtoDecList()
{
	IO::ProtoDec::IProtocolDecoder *protoDec;
	UOSInt i;
	i = this->decList->GetCount();
	while (i-- > 0)
	{
		protoDec = this->decList->GetItem(i);
		DEL_CLASS(protoDec);
	}
	DEL_CLASS(this->decList);
}

UOSInt IO::ProtoDec::ProtoDecList::GetCount()
{
	return this->decList->GetCount();
}

IO::ProtoDec::IProtocolDecoder *IO::ProtoDec::ProtoDecList::GetItem(UOSInt index)
{
	return this->decList->GetItem(index);
}
