#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/EXEFile.h"
#include "Parser/FileParser/COMParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::COMParser::COMParser()
{
}

Parser::FileParser::COMParser::~COMParser()
{
}

Int32 Parser::FileParser::COMParser::GetName()
{
	return *(Int32*)"COMP";
}

void Parser::FileParser::COMParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::EXEFile)
	{
		selector->AddFilter((const UTF8Char*)"*.com", (const UTF8Char*)"COM File");
	}
}

IO::ParserType Parser::FileParser::COMParser::GetParserType()
{
	return IO::ParserType::EXEFile;
}

IO::ParsedObject *Parser::FileParser::COMParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	if (!fd->GetFullName()->EndsWithICase((const UTF8Char*)".COM"))
	{
		return 0;
	}
	if (fd->GetDataSize() >= 65280)
	{
		return 0;
	}

	Manage::Dasm::DasmX86_16_Regs regs;
	regs.EAX = 0;
	regs.ECX = (UInt16)fd->GetDataSize();
	regs.EBX = 0;
	regs.EDX = 0;
	regs.ESI = 0;
	regs.EDI = 0;
	regs.ESP = 0x0000fffe;
	regs.EBP = 0;
	regs.IP = 256;
	regs.CS = 0x80;
	regs.DS = 0x80;
	regs.ES = 0x80;
	regs.SS = 0x80;
	IO::EXEFile *exef;
	NEW_CLASS(exef, IO::EXEFile(fd->GetFullName()));
	UOSInt codeLen;
	exef->AddDOSEnv((UOSInt)fd->GetDataSize() + 256, &regs, 0x80);
	UInt8 *codePtr = exef->GetDOSCodePtr(&codeLen);
	exef->SetDOSHasPSP(true);
	fd->GetRealData(0, codeLen - 256, &codePtr[256]);
	codePtr[0] = 0xcd;
	codePtr[1] = 0x20;
	*(UInt16*)&codePtr[2] = 0x9fff;
	codePtr[4] = 0;
	codePtr[5] = 0x9a;
	*(UInt32*)&codePtr[6] = 0xf01dfeee;
	*(UInt32*)&codePtr[10] = 0;
	*(UInt32*)&codePtr[14] = 0;
	*(UInt32*)&codePtr[18] = 0;
	*(UInt16*)&codePtr[22] = 0;
	codePtr[24] = 1;
	codePtr[25] = 1;
	codePtr[26] = 1;
	codePtr[27] = 0;
	codePtr[28] = 2;
	codePtr[29] = 0xff;
	codePtr[30] = 0xff;
	codePtr[31] = 0xff;
	codePtr[32] = 0xff;
	codePtr[33] = 0xff;
	codePtr[34] = 0xff;
	codePtr[35] = 0xff;
	codePtr[36] = 0xff;
	codePtr[37] = 0xff;
	codePtr[38] = 0xff;
	codePtr[39] = 0xff;
	codePtr[40] = 0xff;
	codePtr[41] = 0xff;
	codePtr[42] = 0xff;
	codePtr[43] = 0xff;
	*(UInt16*)&codePtr[44] = 0;
	*(UInt32*)&codePtr[46] = 0;
	*(UInt16*)&codePtr[50] = 20;
	*(UInt32*)&codePtr[52] = 0x18;
	*(UInt32*)&codePtr[56] = 0xffffffff;
	*(UInt32*)&codePtr[60] = 0;
	*(UInt16*)&codePtr[64] = 0x206;
	OSInt i = 66;
	while (i < 80)
		codePtr[i++] = 0;
	codePtr[80] = 0xcd;
	codePtr[81] = 0x21;
	codePtr[82] = 0xcb;
	i = 83;
	while (i < 128)
		codePtr[i++] = 0;
	while (i < 256)
		codePtr[i++] = 0;
	return exef;
}
