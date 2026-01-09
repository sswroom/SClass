#include "Stdafx.h"
#include "IO/ProtocolBuffersMessage.h"

// https://wiki.openstreetmap.org/wiki/PBF_Format
// https://protobuf.dev/programming-guides/encoding/

IO::ProtocolBuffersMessage::ProtocolBuffersMessage(Text::CStringNN name)
{
}

IO::ProtocolBuffersMessage::~ProtocolBuffersMessage()
{
}

Bool IO::ProtocolBuffersMessage::ParseMsssage(UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
	return false;
}
