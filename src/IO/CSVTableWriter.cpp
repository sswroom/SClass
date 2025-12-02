#include "Stdafx.h"
#include "IO/CSVTableWriter.h"
IO::CSVTableWriter::CSVTableWriter(NN<IO::SeekableStream> stm) : TableWriter(stm), bos(stm, 16384)
{
	this->lineBegin = true;
}

IO::CSVTableWriter::~CSVTableWriter()
{
	this->EndWrite();
}

void IO::CSVTableWriter::NextRow()
{
	this->stm->Write(CSTR("\r\n").ToByteArray());
	this->lineBegin = true;
}

void IO::CSVTableWriter::NextColNull()
{
	if (lineBegin)
	{
		lineBegin = false;
	}
	else
	{
		this->stm->Write(CSTR(",").ToByteArray());
	}
}

void IO::CSVTableWriter::NextColStr(NN<Text::String> s)
{
	NextColNull();
	NN<Text::String> nns = Text::String::NewCSVRec(UnsafeArray<const UTF8Char>(s->v));
	this->stm->Write(nns->ToByteArray());
	nns->Release();
}

void IO::CSVTableWriter::NextColStr(Text::CStringNN s)
{
	NextColNull();
	NN<Text::String> nns = Text::String::NewCSVRec(s.v);
	this->stm->Write(nns->ToByteArray());
	nns->Release();
}

void IO::CSVTableWriter::NextColI32(Int32 v)
{
	UTF8Char sbuff[12];
	UnsafeArray<UTF8Char> sptr;
	NextColNull();
	sptr = Text::StrInt32(sbuff, v);
	this->stm->Write(Data::ByteArrayR(sbuff, (UOSInt)(sptr - sbuff)));
}

void IO::CSVTableWriter::NextColU64(UInt64 v)
{
	UTF8Char sbuff[25];
	UnsafeArray<UTF8Char> sptr;
	NextColNull();
	sptr = Text::StrUInt64(sbuff, v);
	this->stm->Write(Data::ByteArrayR(sbuff, (UOSInt)(sptr - sbuff)));
}

void IO::CSVTableWriter::NextColTS(Data::Timestamp ts)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NextColNull();
	sptr = ts.ToLocalTime().ToStringNoZone(sbuff);
	this->stm->Write(Data::ByteArrayR(sbuff, (UOSInt)(sptr - sbuff)));
}

void IO::CSVTableWriter::EndWrite()
{
	this->bos.Flush();
}
