#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/DWGFileAnalyse.h"
#include "IO/FileAnalyse/EBMLFileAnalyse.h"
#include "IO/FileAnalyse/EDIDFileAnalyse.h"
#include "IO/FileAnalyse/EXEFileAnalyse.h"
#include "IO/FileAnalyse/FGDBFileAnalyse.h"
#include "IO/FileAnalyse/FLVFileAnalyse.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "IO/FileAnalyse/JPGFileAnalyse.h"
#include "IO/FileAnalyse/JMVL01FileAnalyse.h"
#include "IO/FileAnalyse/LNKFileAnalyse.h"
#include "IO/FileAnalyse/MDBFileAnalyse.h"
#include "IO/FileAnalyse/MPEGFileAnalyse.h"
#include "IO/FileAnalyse/NFDumpFileAnalyse.h"
#include "IO/FileAnalyse/PCapFileAnalyse.h"
#include "IO/FileAnalyse/PCapngFileAnalyse.h"
#include "IO/FileAnalyse/PNGFileAnalyse.h"
#include "IO/FileAnalyse/QTFileAnalyse.h"
#include "IO/FileAnalyse/RAR5FileAnalyse.h"
#include "IO/FileAnalyse/RIFFFileAnalyse.h"
#include "IO/FileAnalyse/SHPFileAnalyse.h"
#include "IO/FileAnalyse/SPKFileAnalyse.h"
#include "IO/FileAnalyse/SMTCFileAnalyse.h"
#include "IO/FileAnalyse/TIFFFileAnalyse.h"
#include "IO/FileAnalyse/TSFileAnalyse.h"
#include "IO/FileAnalyse/TXTFileAnalyse.h"
#include "IO/FileAnalyse/ZIPFileAnalyse.h"

IO::FileAnalyse::IFileAnalyse::~IFileAnalyse()
{
}

Bool IO::FileAnalyse::IFileAnalyse::GetFrameDetail(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	NotNullPtr<IO::FileAnalyse::FrameDetail> frame;
	if (!this->GetFrameDetail(index).SetTo(frame))
	{
		return false;
	}
	frame->ToString(sb);
	frame.Delete();
	return true;
}

Optional<IO::FileAnalyse::IFileAnalyse> IO::FileAnalyse::IFileAnalyse::AnalyseFile(NotNullPtr<IO::StreamData> fd)
{
	UInt8 buff[256];
	UOSInt buffSize;
	NotNullPtr<IO::FileAnalyse::IFileAnalyse> analyse;
	NotNullPtr<Text::String> fileName = fd->GetFullFileName();
	buffSize = fd->GetRealData(0, 256, BYTEARR(buff));
	if (buffSize >= 4 && ReadMInt32(&buff[0]) == 0x000001ba)
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::MPEGFileAnalyse(fd));
	}
	else if (buffSize >= 12 && *(Int32*)&buff[0] == *(Int32*)"RIFF")
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::RIFFFileAnalyse(fd));
	}
	else if (buffSize >= 8 && (ReadInt32(&buff[4]) == *(Int32*)"ftyp" || ReadInt32(&buff[4]) == *(Int32*)"moov"))
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::QTFileAnalyse(fd));
	}
	else if (buff[0] == 'F' && buff[1] == 'L' && buff[2] == 'V' && buff[3] == 1)
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::FLVFileAnalyse(fd));
	}
	else if (buff[0] == 0x47 && buff[188] == 0x47)
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::TSFileAnalyse(fd));
	}
	else if (buff[4] == 0x47 && buff[196] == 0x47)
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::TSFileAnalyse(fd));
	}
	else if (buff[0] == 0x0c && buff[1] == 0xa5 && buff[2] == 1 && buff[3] == 0)
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::NFDumpFileAnalyse(fd));
	}
	else if (buff[0] == 0xff && buff[1] == 0xd8 && buff[2] == 0xff && (buff[3] == 0xe0 || buff[3] == 0xe1))
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::JPGFileAnalyse(fd));
	}
	else if (buff[0] == 0x89 && buff[1] == 0x50 && buff[2] == 0x4e && buff[3] == 0x47 && buff[4] == 0x0d && buff[5] == 0x0a && buff[6] == 0x1a && buff[7] == 0x0a)
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::PNGFileAnalyse(fd));
	}
	else if (ReadUInt32(buff) == 0xa1b2c3d4 || ReadMUInt32(buff) == 0xa1b2c3d4)
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::PCapFileAnalyse(fd));
	}
	else if (ReadNInt32(buff) == 0x0a0d0d0a)
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::PCapngFileAnalyse(fd));
	}
	else if (buff[0] == 0x4D && buff[1] == 0x5A)
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::EXEFileAnalyse(fd));
	}
	else if (ReadInt32(&buff[0]) == 0x21726152 && ReadInt32(&buff[4]) == 0x0001071A)
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::RAR5FileAnalyse(fd));
	}
	else if (ReadMInt32(buff) == 0x1A45DFA3)
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::EBMLFileAnalyse(fd));
	}
	else if (buffSize >= 16 && *(Int32*)&buff[0] == *(Int32*)"Smpf")
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::SPKFileAnalyse(fd));
	}
	else if (ReadUInt32(buff) == 3 && ReadUInt64(&buff[24]) == fd->GetDataSize())
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::FGDBFileAnalyse(fd));
	}
	else if (ReadUInt32(buff) == 0x100 && (Text::StrStartsWithC(&buff[4], 252, UTF8STRC("Standard Jet DB")) || Text::StrStartsWithC(&buff[4], 252, UTF8STRC("Standard ACE DB"))))
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::MDBFileAnalyse(fd));
	}
	else if (buff[0] == 'A' && buff[1] == 'C' && buff[2] == '1' && buff[3] == '0')
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::DWGFileAnalyse(fd));
	}
	else if (buffSize >= 22 && buff[0] == 0x78 && buff[1] == 0x78 && buff[2] == 0x11 && buff[3] == 0x01 && buff[20] == 13 && buff[21] == 10)
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::JMVL01FileAnalyse(fd));
	}
	else if (buffSize >= 18 && buff[0] == 0x78 && buff[1] == 0x78 && buff[2] == 0x0D && buff[3] == 0x01 && buff[16] == 13 && buff[17] == 10)
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::JMVL01FileAnalyse(fd));
	}
	else if (buffSize >= 100 && ReadMInt32(buff) == 9994 && ReadInt32(&buff[28]) == 1000 && (ReadMUInt32(&buff[24]) << 1) == fd->GetDataSize())
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::SHPFileAnalyse(fd));
	}
	else if (buffSize >= 76 && ReadUInt32(&buff[0]) == 0x4C && ReadUInt32(&buff[4]) == 0x00021401 && ReadUInt32(&buff[8]) == 0 && ReadUInt32(&buff[12]) == 0xC0 && ReadUInt32(&buff[16]) == 0x46000000)
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::LNKFileAnalyse(fd));
	}
	else if (ReadMInt32(buff) == 0x504B0304)
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::ZIPFileAnalyse(fd));
	}
	else if (buffSize >= 128 && ReadNUInt64(buff) == 0xffffffffffff00LL && (((UOSInt)buff[126] + 1) << 7) <= fd->GetDataSize() && (fd->GetDataSize() & 127) == 0)
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::EDIDFileAnalyse(fd));
	}
	else if (buffSize >= 128 && buff[0] == 'I' && buff[1] == 'I' && (ReadUInt16(&buff[2]) == 42 || ReadUInt16(&buff[2]) == 43))
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::TIFFFileAnalyse(fd));
	}
	else if (buffSize >= 128 && buff[0] == 'M' && buff[1] == 'M' && (ReadMUInt16(&buff[2]) == 42 || ReadMUInt16(&buff[2]) == 43))
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::TIFFFileAnalyse(fd));
	}
	else if (buffSize >= 25 && *(Int32*)&buff[0] == *(Int32*)"SmTC" && buff[24] == 0)
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::SMTCFileAnalyse(fd));
	}
	else if (fileName->Equals(UTF8STRC("README")) || fileName->Equals(UTF8STRC("LICENSE")) || fileName->EndsWith(UTF8STRC(".txt")))
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::TXTFileAnalyse(fd));
	}
	else if (fileName->EndsWith(UTF8STRC(".vcs")) && buffSize >= 15 && Text::StrStartsWithC(buff, buffSize, UTF8STRC("BEGIN:VCALENDAR")))
	{
		NEW_CLASSNN(analyse, IO::FileAnalyse::TXTFileAnalyse(fd));
	}
	else
	{
		return 0;
	}

	if (analyse->IsError())
	{
		analyse.Delete();
		return 0;
	}
	return analyse;
}

void IO::FileAnalyse::IFileAnalyse::AddFilters(NotNullPtr<IO::FileSelector> selector)
{
	selector->AddFilter(CSTR("*.mpg"), CSTR("MPEG System Stream"));
	selector->AddFilter(CSTR("*.avi"), CSTR("RIFF AVI"));
	selector->AddFilter(CSTR("*.wav"), CSTR("RIFF WAVE"));
	selector->AddFilter(CSTR("*.mov"), CSTR("Quicktime File"));
	selector->AddFilter(CSTR("*.mp4"), CSTR("MPEG-4 File"));
	selector->AddFilter(CSTR("*.flv"), CSTR("FLV file"));
	selector->AddFilter(CSTR("*.ts"), CSTR("TS file"));
	selector->AddFilter(CSTR("*.mts"), CSTR("MTS file"));
	selector->AddFilter(CSTR("*.m2ts"), CSTR("M2TS file"));
	selector->AddFilter(CSTR("*.pcap"), CSTR("PCAP file"));
	selector->AddFilter(CSTR("*.pcapng"), CSTR("PCAPNG file"));
	selector->AddFilter(CSTR("*.exe"), CSTR("Executable file"));
	selector->AddFilter(CSTR("*.rar"), CSTR("RAR file"));
	selector->AddFilter(CSTR("*.lnk"), CSTR("Shell Link file"));
	selector->AddFilter(CSTR("*.spk"), CSTR("SPackage file"));
	selector->AddFilter(CSTR("*.dwg"), CSTR("AutoCAD Drawing file"));
	selector->AddFilter(CSTR("*.zip"), CSTR("ZIP file"));
	selector->AddFilter(CSTR("*.jar"), CSTR("JAR file"));
	selector->AddFilter(CSTR("*.tif"), CSTR("TIFF file"));
	selector->AddFilter(CSTR("*.tf8"), CSTR("TIFF file"));
	selector->AddFilter(CSTR("*.btf"), CSTR("TIFF file"));
}
