#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ProtoDec/ProtoDecList.h"
#include "IO/ProtoDec/JTT808ProtocolDecoder.h"
#include "IO/ProtoDec/TK109ProtocolDecoder.h"
#include "IO/ProtoDec/TSProtocolDecoder.h"

IO::ProtoDec::ProtoDecList::ProtoDecList()
{
	NN<IO::ProtoDec::ProtocolDecoder> protoDec;
	NEW_CLASSNN(protoDec, IO::ProtoDec::TK109ProtocolDecoder());
	this->decList.Add(protoDec);
	NEW_CLASSNN(protoDec, IO::ProtoDec::TSProtocolDecoder());
	this->decList.Add(protoDec);
	NEW_CLASSNN(protoDec, IO::ProtoDec::JTT808ProtocolDecoder());
	this->decList.Add(protoDec);
}

IO::ProtoDec::ProtoDecList::~ProtoDecList()
{
	NN<IO::ProtoDec::ProtocolDecoder> protoDec;
	UIntOS i;
	i = this->decList.GetCount();
	while (i-- > 0)
	{
		protoDec = this->decList.GetItemNoCheck(i);
		protoDec.Delete();
	}
}

UIntOS IO::ProtoDec::ProtoDecList::GetCount() const
{
	return this->decList.GetCount();
}

Optional<IO::ProtoDec::ProtocolDecoder> IO::ProtoDec::ProtoDecList::GetItem(UIntOS index) const
{
	return this->decList.GetItem(index);
}

NN<IO::ProtoDec::ProtocolDecoder> IO::ProtoDec::ProtoDecList::GetItemNoCheck(UIntOS index) const
{
	return this->decList.GetItemNoCheck(index);
}
