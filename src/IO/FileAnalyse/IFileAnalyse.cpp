#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/EBMLFileAnalyse.h"
#include "IO/FileAnalyse/EXEFileAnalyse.h"
#include "IO/FileAnalyse/FGDBFileAnalyse.h"
#include "IO/FileAnalyse/FLVFileAnalyse.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "IO/FileAnalyse/JPGFileAnalyse.h"
#include "IO/FileAnalyse/MDBFileAnalyse.h"
#include "IO/FileAnalyse/MPEGFileAnalyse.h"
#include "IO/FileAnalyse/NFDumpFileAnalyse.h"
#include "IO/FileAnalyse/PCapFileAnalyse.h"
#include "IO/FileAnalyse/PCapngFileAnalyse.h"
#include "IO/FileAnalyse/PNGFileAnalyse.h"
#include "IO/FileAnalyse/QTFileAnalyse.h"
#include "IO/FileAnalyse/RAR5FileAnalyse.h"
#include "IO/FileAnalyse/RIFFFileAnalyse.h"
#include "IO/FileAnalyse/SPKFileAnalyse.h"
#include "IO/FileAnalyse/TSFileAnalyse.h"

IO::FileAnalyse::IFileAnalyse::~IFileAnalyse()
{
}

Bool IO::FileAnalyse::IFileAnalyse::GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::FrameDetail *frame = this->GetFrameDetail(index);
	if (frame == 0)
	{
		return false;
	}
	frame->ToString(sb);
	DEL_CLASS(frame);
	return true;
}

IO::FileAnalyse::IFileAnalyse *IO::FileAnalyse::IFileAnalyse::AnalyseFile(IO::IStreamData *fd)
{
	UInt8 buff[256];
	UOSInt buffSize;
	IO::FileAnalyse::IFileAnalyse *analyse = 0;
	buffSize = fd->GetRealData(0, 256, buff);
	if (buffSize >= 4 && ReadMInt32(&buff[0]) == 0x000001ba)
	{
		NEW_CLASS(analyse, IO::FileAnalyse::MPEGFileAnalyse(fd));
	}
	else if (buffSize >= 12 && *(Int32*)&buff[0] == *(Int32*)"RIFF")
	{
		NEW_CLASS(analyse, IO::FileAnalyse::RIFFFileAnalyse(fd));
	}
	else if (buffSize >= 8 && (ReadInt32(&buff[4]) == *(Int32*)"ftyp" || ReadInt32(&buff[4]) == *(Int32*)"moov"))
	{
		NEW_CLASS(analyse, IO::FileAnalyse::QTFileAnalyse(fd));
	}
	else if (buff[0] == 'F' && buff[1] == 'L' && buff[2] == 'V' && buff[3] == 1)
	{
		NEW_CLASS(analyse, IO::FileAnalyse::FLVFileAnalyse(fd));
	}
	else if (buff[0] == 0x47 && buff[188] == 0x47)
	{
		NEW_CLASS(analyse, IO::FileAnalyse::TSFileAnalyse(fd));
	}
	else if (buff[4] == 0x47 && buff[196] == 0x47)
	{
		NEW_CLASS(analyse, IO::FileAnalyse::TSFileAnalyse(fd));
	}
	else if (buff[0] == 0x0c && buff[1] == 0xa5 && buff[2] == 1 && buff[3] == 0)
	{
		NEW_CLASS(analyse, IO::FileAnalyse::NFDumpFileAnalyse(fd));
	}
	else if (buff[0] == 0xff && buff[1] == 0xd8 && buff[2] == 0xff && (buff[3] == 0xe0 || buff[3] == 0xe1))
	{
		NEW_CLASS(analyse, IO::FileAnalyse::JPGFileAnalyse(fd));
	}
	else if (buff[0] == 0x89 && buff[1] == 0x50 && buff[2] == 0x4e && buff[3] == 0x47 && buff[4] == 0x0d && buff[5] == 0x0a && buff[6] == 0x1a && buff[7] == 0x0a)
	{
		NEW_CLASS(analyse, IO::FileAnalyse::PNGFileAnalyse(fd));
	}
	else if (ReadUInt32(buff) == 0xa1b2c3d4 || ReadMUInt32(buff) == 0xa1b2c3d4)
	{
		NEW_CLASS(analyse, IO::FileAnalyse::PCapFileAnalyse(fd));
	}
	else if (ReadNInt32(buff) == 0x0a0d0d0a)
	{
		NEW_CLASS(analyse, IO::FileAnalyse::PCapngFileAnalyse(fd));
	}
	else if (buff[0] == 0x4D && buff[1] == 0x5A)
	{
		NEW_CLASS(analyse, IO::FileAnalyse::EXEFileAnalyse(fd));
	}
	else if (ReadInt32(&buff[0]) == 0x21726152 && ReadInt32(&buff[4]) == 0x0001071A)
	{
		NEW_CLASS(analyse, IO::FileAnalyse::RAR5FileAnalyse(fd));
	}
	else if (ReadMInt32(buff) == 0x1A45DFA3)
	{
		NEW_CLASS(analyse, IO::FileAnalyse::EBMLFileAnalyse(fd));
	}
	else if (buffSize >= 16 && *(Int32*)&buff[0] == *(Int32*)"Smpf")
	{
		NEW_CLASS(analyse, IO::FileAnalyse::SPKFileAnalyse(fd));
	}
	else if (ReadUInt32(buff) == 3 && ReadUInt64(&buff[24]) == fd->GetDataSize())
	{
		NEW_CLASS(analyse, IO::FileAnalyse::FGDBFileAnalyse(fd));
	}
	else if (ReadUInt32(buff) == 0x100 && (Text::StrEquals((const UTF8Char*)"Standard Jet DB", &buff[4]) || Text::StrEquals((const UTF8Char*)"Standard ACE DB", &buff[4])))
	{
		NEW_CLASS(analyse, IO::FileAnalyse::MDBFileAnalyse(fd));
	}

	if (analyse && analyse->IsError())
	{
		DEL_CLASS(analyse);
		analyse = 0;
	}
	return analyse;
}

void IO::FileAnalyse::IFileAnalyse::AddFilters(IO::IFileSelector *selector)
{
	selector->AddFilter((const UTF8Char*)"*.mpg", (const UTF8Char*)"MPEG System Stream");
	selector->AddFilter((const UTF8Char*)"*.avi", (const UTF8Char*)"RIFF AVI");
	selector->AddFilter((const UTF8Char*)"*.wav", (const UTF8Char*)"RIFF WAVE");
	selector->AddFilter((const UTF8Char*)"*.mov", (const UTF8Char*)"Quicktime File");
	selector->AddFilter((const UTF8Char*)"*.mp4", (const UTF8Char*)"MPEG-4 File");
	selector->AddFilter((const UTF8Char*)"*.flv", (const UTF8Char*)"FLV file");
	selector->AddFilter((const UTF8Char*)"*.ts", (const UTF8Char*)"TS file");
	selector->AddFilter((const UTF8Char*)"*.mts", (const UTF8Char*)"MTS file");
	selector->AddFilter((const UTF8Char*)"*.m2ts", (const UTF8Char*)"M2TS file");
	selector->AddFilter((const UTF8Char*)"*.pcap", (const UTF8Char*)"PCAP file");
	selector->AddFilter((const UTF8Char*)"*.pcapng", (const UTF8Char*)"PCAPNG file");
	selector->AddFilter((const UTF8Char*)"*.exe", (const UTF8Char*)"Executable file");
	selector->AddFilter((const UTF8Char*)"*.rar", (const UTF8Char*)"RAR file");
	selector->AddFilter((const UTF8Char*)"*.spk", (const UTF8Char*)"SPackage file");
}
