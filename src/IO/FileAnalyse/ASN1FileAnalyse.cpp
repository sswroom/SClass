#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "IO/FileAnalyse/ASN1FileAnalyse.h"

IO::FileAnalyse::ASN1FileAnalyse::ASN1FileAnalyse(NN<IO::StreamData> fd, Optional<Net::ASN1Names> names)
{
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
	this->names = names;

}

IO::FileAnalyse::ASN1FileAnalyse::~ASN1FileAnalyse()
{
	this->fd.Delete();
	this->names.Delete();
}

Text::CStringNN IO::FileAnalyse::ASN1FileAnalyse::GetFormatName()
{
	return CSTR("ASN1");
}

UOSInt IO::FileAnalyse::ASN1FileAnalyse::GetFrameCount()
{
	return 1;
}

Bool IO::FileAnalyse::ASN1FileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	if (index != 0)
		return false;
	sb->AppendC(UTF8STRC("size="));
	sb->AppendU64(this->fd->GetDataSize());
	return true;
}

UOSInt IO::FileAnalyse::ASN1FileAnalyse::GetFrameIndex(UInt64 ofst)
{
	if (ofst < this->fd->GetDataSize())
		return 0;
	else
		return INVALID_INDEX;
}

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::ASN1FileAnalyse::GetFrameDetail(UOSInt index)
{
	if (index != 0)
		return nullptr;
	Data::ByteBuffer buff((UOSInt)this->fd->GetDataSize());
	this->fd->GetRealData(0, buff.GetSize(), buff);
	NN<IO::FileAnalyse::FrameDetail> frame;
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(0, buff.GetSize()));
	Net::ASN1Util::PDUAnalyse(frame, buff, 0, buff.GetSize(), this->names);
	return frame;
}

Bool IO::FileAnalyse::ASN1FileAnalyse::IsError()
{
	return false;
}

Bool IO::FileAnalyse::ASN1FileAnalyse::IsParsing()
{
	return false;
}

Bool IO::FileAnalyse::ASN1FileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}
