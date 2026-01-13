#include "Stdafx.h"
#include "IO/FileAnalyse/ProtocolBuffersFileAnalyse.h"

IO::FileAnalyse::ProtocolBuffersFileAnalyse::ProtocolBuffersFileAnalyse(NN<IO::StreamData> fd, NN<IO::ProtocolBuffersMessage> msg)
{
	this->msg = msg;
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
}

IO::FileAnalyse::ProtocolBuffersFileAnalyse::~ProtocolBuffersFileAnalyse()
{
	this->msg.Delete();
	this->fd.Delete();
}

Text::CStringNN IO::FileAnalyse::ProtocolBuffersFileAnalyse::GetFormatName()
{
	return CSTR("Protobuf");
}

UOSInt IO::FileAnalyse::ProtocolBuffersFileAnalyse::GetFrameCount()
{
	return 1;
}

Bool IO::FileAnalyse::ProtocolBuffersFileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	if (index != 0)
	{
		return false;
	}
	sb->Append(this->msg->GetName());
	return true;
}

UOSInt IO::FileAnalyse::ProtocolBuffersFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	return 0;
}

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::ProtocolBuffersFileAnalyse::GetFrameDetail(UOSInt index)
{
	NN<IO::FileAnalyse::FrameDetail> frame;
	if (index != 0)
	{
		return 0;
	}
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(0, (UOSInt)this->fd->GetDataSize()));
	Data::ByteBuffer packetBuff((UOSInt)this->fd->GetDataSize());
	this->fd->GetRealData(0, (UOSInt)this->fd->GetDataSize(), packetBuff);
	this->msg->ParseMsssage(frame, packetBuff.Arr(), 0, (UOSInt)this->fd->GetDataSize());
	return frame;
}

Bool IO::FileAnalyse::ProtocolBuffersFileAnalyse::IsError()
{
	return false;
}

Bool IO::FileAnalyse::ProtocolBuffersFileAnalyse::IsParsing()
{
	return false;
}

Bool IO::FileAnalyse::ProtocolBuffersFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}

IO::FileAnalyse::ProtocolBuffersFileAnalyseCreator::ProtocolBuffersFileAnalyseCreator(NN<IO::ProtocolBuffersMessage> msg)
{
	this->msg = msg;
}

IO::FileAnalyse::ProtocolBuffersFileAnalyseCreator::~ProtocolBuffersFileAnalyseCreator()
{
	this->msg.Delete();
}

Optional<IO::FileAnalyse::FileAnalyser> IO::FileAnalyse::ProtocolBuffersFileAnalyseCreator::Create(NN<IO::StreamData> fd)
{
	NN<IO::FileAnalyse::FileAnalyser> ret;
	NEW_CLASSNN(ret, IO::FileAnalyse::ProtocolBuffersFileAnalyse(fd, this->msg->Clone()));
	return ret;
}