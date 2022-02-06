#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/EXEFile.h"
#include "IO/FileStream.h"
#include "Parser/FileParser/FNTParser.h"
#include "Text/Encoding.h"
#include "Text/EncodingFactory.h"
#include "Text/Locale.h"
#include "Text/MyString.h"

IO::EXEFile::EXEFile(Text::String *fileName) : IO::ParsedObject(fileName)
{
	NEW_CLASS(this->propNames, Data::ArrayList<Text::String *>());
	NEW_CLASS(this->propValues, Data::ArrayList<Text::String *>());
	NEW_CLASS(this->importList, Data::ArrayList<ImportInfo*>());
	NEW_CLASS(this->exportList, Data::ArrayList<ExportInfo*>());
	NEW_CLASS(this->resList, Data::ArrayList<ResourceInfo*>());
	this->envDOS = 0;
}

IO::EXEFile::EXEFile(const UTF8Char *fileName) : IO::ParsedObject(fileName)
{
	NEW_CLASS(this->propNames, Data::ArrayList<Text::String *>());
	NEW_CLASS(this->propValues, Data::ArrayList<Text::String *>());
	NEW_CLASS(this->importList, Data::ArrayList<ImportInfo*>());
	NEW_CLASS(this->exportList, Data::ArrayList<ExportInfo*>());
	NEW_CLASS(this->resList, Data::ArrayList<ResourceInfo*>());
	this->envDOS = 0;
}

IO::EXEFile::~EXEFile()
{
	if (this->envDOS)
	{
		if (this->envDOS->b16Codes)
		{
			MemFree(this->envDOS->b16Codes);
			this->envDOS->b16Codes = 0;
		}
		if (this->envDOS->b16Regs)
		{
			MemFree(this->envDOS->b16Regs);
			this->envDOS->b16Regs = 0;
		}
		MemFree(this->envDOS);
		this->envDOS = 0;
	}
	UOSInt i;
	UOSInt j;
	i = this->propNames->GetCount();
	while (i-- > 0)
	{
		this->propNames->GetItem(i)->Release();
		this->propValues->GetItem(i)->Release();
	}
	DEL_CLASS(this->propNames);
	DEL_CLASS(this->propValues);

	i = this->importList->GetCount();
	while (i-- > 0)
	{
		ImportInfo *imp = this->importList->GetItem(i);
		imp->moduleName->Release();
		j = imp->funcs->GetCount();
		while (j-- > 0)
		{
			imp->funcs->GetItem(j)->Release();
		}
		DEL_CLASS(imp->funcs);
		MemFree(imp);
	}
	DEL_CLASS(this->importList);

	i = this->exportList->GetCount();
	while (i-- > 0)
	{
		ExportInfo *exp = this->exportList->GetItem(i);
		exp->funcName->Release();
		MemFree(exp);
	}
	DEL_CLASS(this->exportList);

	i = this->resList->GetCount();
	while (i-- > 0)
	{
		ResourceInfo *res = this->resList->GetItem(i);
		res->name->Release();
		MemFree((void*)res->data);
		MemFree(res);
	}
	DEL_CLASS(this->resList);
}

IO::ParserType IO::EXEFile::GetParserType()
{
	return IO::ParserType::EXEFile;
}

void IO::EXEFile::AddProp(Text::CString name, Text::CString value)
{
	if (name.leng != 0 && value.leng != 0)
	{
		this->propNames->Add(Text::String::New(name));
		this->propValues->Add(Text::String::New(value));
	}
}

UOSInt IO::EXEFile::GetPropCount()
{
	return this->propNames->GetCount();
}

Text::String *IO::EXEFile::GetPropName(UOSInt index)
{
	return this->propNames->GetItem(index);
}

Text::String *IO::EXEFile::GetPropValue(UOSInt index)
{
	return this->propValues->GetItem(index);
}

UOSInt IO::EXEFile::AddImportModule(Text::CString moduleName)
{
	ImportInfo *imp;
	imp = MemAlloc(ImportInfo, 1);
	NEW_CLASS(imp->funcs, Data::ArrayList<Text::String*>());
	imp->moduleName = Text::String::New(moduleName.v, moduleName.leng);
	return this->importList->Add(imp);
}

void IO::EXEFile::AddImportFunc(UOSInt modIndex, Text::CString funcName)
{
	ImportInfo *imp;
	imp = this->importList->GetItem(modIndex);
	if (imp)
	{
		imp->funcs->Add(Text::String::New(funcName));
	}
}

UOSInt IO::EXEFile::GetImportCount()
{
	return this->importList->GetCount();
}

Text::String *IO::EXEFile::GetImportName(UOSInt modIndex)
{
	ImportInfo *imp = this->importList->GetItem(modIndex);
	if (imp)
	{
		return imp->moduleName;
	}
	return 0;
}

UOSInt IO::EXEFile::GetImportFuncCount(UOSInt modIndex)
{
	ImportInfo *imp = this->importList->GetItem(modIndex);
	if (imp)
	{
		return imp->funcs->GetCount();
	}
	return 0;
}

Text::String *IO::EXEFile::GetImportFunc(UOSInt modIndex, UOSInt funcIndex)
{
	ImportInfo *imp = this->importList->GetItem(modIndex);
	if (imp)
	{
		return imp->funcs->GetItem(funcIndex);
	}
	return 0;
}

void IO::EXEFile::AddExportFunc(Text::CString funcName)
{
	ExportInfo *exp = MemAlloc(ExportInfo, 1);
	exp->funcName = Text::String::New(funcName.v, funcName.leng);
	this->exportList->Add(exp);
}

UOSInt IO::EXEFile::GetExportCount()
{
	return this->exportList->GetCount();
}

Text::String *IO::EXEFile::GetExportName(UOSInt index)
{
	ExportInfo *exp = this->exportList->GetItem(index);
	if (exp)
	{
		return exp->funcName;
	}
	return 0;
}


Bool IO::EXEFile::HasDOS()
{
	return this->envDOS != 0;
}

void IO::EXEFile::AddDOSEnv(UOSInt b16CodeLen, Manage::Dasm::DasmX86_16_Regs *b16Regs, UInt16 b16CodeSegm)
{
	if (this->envDOS == 0)
	{
		this->envDOS = MemAlloc(ProgramEnvDOS, 1);
		this->envDOS->b16HasPSP = false;
		this->envDOS->b16Codes = MemAlloc(UInt8, b16CodeLen);
		this->envDOS->b16CodeLen = b16CodeLen;
		this->envDOS->b16CodeSegm = b16CodeSegm;
		this->envDOS->b16Regs = MemAlloc(Manage::Dasm::DasmX86_16_Regs, 1);
		MemCopyNO(this->envDOS->b16Regs, b16Regs, sizeof(Manage::Dasm::DasmX86_16_Regs));
	}
}

UInt8 *IO::EXEFile::GetDOSCodePtr(UOSInt *codeLen)
{
	if (this->envDOS == 0)
		return 0;
	*codeLen = this->envDOS->b16CodeLen;
	return this->envDOS->b16Codes;
}

void IO::EXEFile::SetDOSHasPSP(Bool hasPSP)
{
	if (this->envDOS == 0)
		return;
	this->envDOS->b16HasPSP = hasPSP;
}

void IO::EXEFile::GetDOSInitRegs(Manage::Dasm::DasmX86_16_Regs *regs)
{
	if (this->envDOS == 0)
		return;
	MemCopyNO(regs, this->envDOS->b16Regs, sizeof(Manage::Dasm::DasmX86_16_Regs));
}

UInt16 IO::EXEFile::GetDOSCodeSegm()
{
	if (this->envDOS == 0)
		return 0;
	return this->envDOS->b16CodeSegm;
}

void IO::EXEFile::AddResource(Text::CString name, const UInt8 *data, UOSInt dataSize, UInt32 codePage, ResourceType rt)
{
	ResourceInfo *res = MemAlloc(ResourceInfo, 1);
	res->name = Text::String::New(name);
	res->data = MemAlloc(UInt8, dataSize);
	MemCopyNO((UInt8*)res->data, data, dataSize);
	res->dataSize = dataSize;
	res->codePage = codePage;
	res->rt = rt;
	this->resList->Add(res);
}

UOSInt IO::EXEFile::GetResourceCount()
{
	return this->resList->GetCount();
}

const IO::EXEFile::ResourceInfo *IO::EXEFile::GetResource(UOSInt index)
{
	return this->resList->GetItem(index);
}

Bool IO::EXEFile::GetFileTime(Text::CString fileName, Data::DateTime *fileTimeOut)
{
	IO::FileStream *fs;
	UInt8 buff[64];

	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	if (fs->Read(buff, 64) != 64)
	{
		DEL_CLASS(fs);
		return false;
	}
	if (buff[0] != 'M' || buff[1] != 'Z')
	{
		DEL_CLASS(fs);
		return false;
	}
	UInt32 ofst = ReadUInt32(&buff[60]);
	if ((ofst & 7) != 0 || ofst < 64)
	{
		DEL_CLASS(fs);
		return false;
	}
	fs->SeekFromBeginning(ofst);
	if (fs->Read(buff, 64) != 64)
	{
		DEL_CLASS(fs);
		return false;
	}
	if (buff[0] != 'P' || buff[1] != 'E' || buff[2] != 0 || buff[3] != 0)
	{
		DEL_CLASS(fs);
		return false;
	}
	fileTimeOut->SetUnixTimestamp(ReadUInt32(&buff[8]));
	DEL_CLASS(fs);
	return true;
}

Text::CString IO::EXEFile::GetResourceTypeName(ResourceType rt)
{
	switch (rt)
	{
	case RT_VERSIONINFO:
		return CSTR("VersionInfo");
	case RT_BITMAP:
		return CSTR("Bitmap");
	case RT_MENU:
		return CSTR("Menu");
	case RT_DIALOG:
		return CSTR("Dialog");
	case RT_STRINGTABLE:
		return CSTR("StringTable");
	case RT_ACCELERATOR:
		return CSTR("Accelerator");
	case RT_CURSOR:
		return CSTR("Cursor");
	case RT_ICON:
		return CSTR("Icon");
	case RT_FONT:
		return CSTR("Font");
	case RT_FONTDIR:
		return CSTR("FontDir");
	case RT_RAW_DATA:
		return CSTR("RawData");
	case RT_MESSAGETABLE:
		return CSTR("MessageTable");
	case RT_DLGINCLUDE:
		return CSTR("DlgInclude");
	case RT_PLUGPLAY:
		return CSTR("PlugAndPlay");
	case RT_VXD:
		return CSTR("VXD");
	case RT_ANICURSOR:
		return CSTR("AniCursor");
	case RT_ANIICON:
		return CSTR("AniIcon");
	case RT_HTML:
		return CSTR("HTML");
	case RT_MANIFEST:
		return CSTR("ManiFest");
	case RT_UNKNOWN:
	default:
		return CSTR("Unknown");
	}
}

void IO::EXEFile::GetResourceDesc(const ResourceInfo *res, Text::StringBuilderUTF8 *sb)
{
	UTF8Char u8buff[256];
	UTF8Char *sptr;
	if (res->rt == RT_FONT)
	{
		Parser::FileParser::FNTParser::GetFileDesc(res->data, res->dataSize, sb);
	}
	else if (res->rt == RT_FONTDIR)
	{
		Parser::FileParser::FNTParser::GetFileDirDesc(res->data, res->dataSize, sb);
	}
	else if (res->rt == RT_VERSIONINFO)
	{
		if (res->dataSize >= 92 && ReadUInt16(&res->data[2]) == 52 && Text::StrEquals((Char*)&res->data[4], "VS_VERSION_INFO") && Text::StrEquals((Char*)&res->data[76], "StringFileInfo"))
		{
			UInt32 verSize = ReadUInt16(&res->data[0]);
			UInt32 v;
			UInt32 v2;
			if (verSize <= res->dataSize)
			{
				sb->AppendC(UTF8STRC("Length = "));
				sb->AppendU32(verSize);
				sb->AppendC(UTF8STRC("\r\nValue Length = "));
				sb->AppendU16(ReadUInt16(&res->data[2]));
				sb->AppendC(UTF8STRC("\r\nKey = "));
				sb->AppendSlow((UTF8Char*)&res->data[4]);
				sb->AppendC(UTF8STRC("\r\nSignature = 0x"));
				sb->AppendHex32(ReadUInt32(&res->data[20]));
				sb->AppendC(UTF8STRC("\r\nStruct Version = "));
				sb->AppendU16(ReadUInt16(&res->data[26]));
				sb->AppendC(UTF8STRC("."));
				sb->AppendU16(ReadUInt16(&res->data[24]));
				sb->AppendC(UTF8STRC("\r\nFile Version = "));
				sb->AppendU16(ReadUInt16(&res->data[30]));
				sb->AppendC(UTF8STRC("."));
				sb->AppendU16(ReadUInt16(&res->data[28]));
				sb->AppendC(UTF8STRC("."));
				sb->AppendU16(ReadUInt16(&res->data[34]));
				sb->AppendC(UTF8STRC("."));
				sb->AppendU16(ReadUInt16(&res->data[32]));
				sb->AppendC(UTF8STRC("\r\nProduct Version = "));
				sb->AppendU16(ReadUInt16(&res->data[38]));
				sb->AppendC(UTF8STRC("."));
				sb->AppendU16(ReadUInt16(&res->data[36]));
				sb->AppendC(UTF8STRC("."));
				sb->AppendU16(ReadUInt16(&res->data[42]));
				sb->AppendC(UTF8STRC("."));
				sb->AppendU16(ReadUInt16(&res->data[40]));
				sb->AppendC(UTF8STRC("\r\nFile Flags Mask = 0x"));
				sb->AppendHex32(ReadUInt32(&res->data[44]));
				sb->AppendC(UTF8STRC("\r\nFile Flags = 0x"));
				sb->AppendHex32(ReadUInt32(&res->data[48]));
				sb->AppendC(UTF8STRC("\r\nFile OS = 0x"));
				sb->AppendHex32(v = ReadUInt32(&res->data[52]));
				switch (v)
				{
				case 0x00010001:
					sb->AppendC(UTF8STRC(" (16-bit Windows running on MS-DOS)"));
					break;
				case 0x00010004:
					sb->AppendC(UTF8STRC(" (32-bit Windows running on MS-DOS)"));
					break;
				case 0x00040004:
					sb->AppendC(UTF8STRC(" (Windows NT)"));
					break;
				case 0x00020002:
					sb->AppendC(UTF8STRC(" (16-bit Presentation Manager running on 16-bit OS/2)"));
					break;
				case 0x00030003:
					sb->AppendC(UTF8STRC(" (32-bit Presentation Manager running on 32-bit OS/2)"));
					break;
				case 0x00010000:
					sb->AppendC(UTF8STRC(" (MS-DOS)"));
					break;
				case 0x00040000:
					sb->AppendC(UTF8STRC(" (Windows NT)"));
					break;
				case 0x00000001:
					sb->AppendC(UTF8STRC(" (16-bit Windows)"));
					break;
				case 0x00000004:
					sb->AppendC(UTF8STRC(" (32-bit Windows)"));
					break;
				case 0x00020000:
					sb->AppendC(UTF8STRC(" (16-bit OS/2)"));
					break;
				case 0x00030000:
					sb->AppendC(UTF8STRC(" (32-bit OS/2)"));
					break;
				case 0x00000002:
					sb->AppendC(UTF8STRC(" (16-bit Presentation Manager)"));
					break;
				case 0x00000003:
					sb->AppendC(UTF8STRC(" (32-bit Presentation Manager)"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\nFile Type = 0x"));
				sb->AppendHex32(v = ReadUInt32(&res->data[56]));
				switch (v)
				{
				case 0x00000001:
					sb->AppendC(UTF8STRC(" (application)"));
					break;
				case 0x00000002:
					sb->AppendC(UTF8STRC(" (DLL)"));
					break;
				case 0x00000003:
					sb->AppendC(UTF8STRC(" (device driver)"));
					break;
				case 0x00000004:
					sb->AppendC(UTF8STRC(" (font)"));
					break;
				case 0x00000007:
					sb->AppendC(UTF8STRC(" (static-link library)"));
					break;
				case 0x00000005:
					sb->AppendC(UTF8STRC(" (virtual device)"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\nFile Sub-Type = 0x"));
				sb->AppendHex32(v2 = ReadUInt32(&res->data[60]));
				if (v == 0x00000003) //VFT_DRV
				{
					switch (v2)
					{
					case 0x0000000A:
						sb->AppendC(UTF8STRC(" (communications driver)"));
						break;
					case 0x00000004:
						sb->AppendC(UTF8STRC(" (display driver)"));
						break;
					case 0x00000008:
						sb->AppendC(UTF8STRC(" (installable driver)"));
						break;
					case 0x00000002:
						sb->AppendC(UTF8STRC(" (keyboard driver)"));
						break;
					case 0x00000003:
						sb->AppendC(UTF8STRC(" (language driver)"));
						break;
					case 0x00000005:
						sb->AppendC(UTF8STRC(" (mouse driver)"));
						break;
					case 0x00000006:
						sb->AppendC(UTF8STRC(" (network driver)"));
						break;
					case 0x00000001:
						sb->AppendC(UTF8STRC(" (printer driver)"));
						break;
					case 0x00000009:
						sb->AppendC(UTF8STRC(" (sound driver)"));
						break;
					case 0x00000007:
						sb->AppendC(UTF8STRC(" (system driver)"));
						break;
					case 0x0000000C:
						sb->AppendC(UTF8STRC(" (versioned printer driver)"));
						break;
					}
				}
				else if (v == 0x00000004) //VFT_FONT
				{
					switch (v2)
					{
					case 0x00000001:
						sb->AppendC(UTF8STRC(" (raster font)"));
						break;
					case 0x00000003:
						sb->AppendC(UTF8STRC(" (TrueType font)"));
						break;
					case 0x00000002:
						sb->AppendC(UTF8STRC(" (vector font.)"));
						break;
					}
				}
				sb->AppendC(UTF8STRC("\r\nFile Date = 0x"));
				sb->AppendHex32(ReadUInt32(&res->data[64]));
				sb->AppendHex32(ReadUInt32(&res->data[68]));
				sb->AppendC(UTF8STRC("\r\nString File Length = "));
				sb->AppendU16(ReadUInt16(&res->data[72]));
				sb->AppendC(UTF8STRC("\r\nString File Value Length = "));
				sb->AppendU16(ReadUInt16(&res->data[74]));
				sb->AppendC(UTF8STRC("\r\nString File Key = "));
				sb->AppendSlow((UTF8Char*)&res->data[76]);
				UOSInt strLen = ReadUInt16(&res->data[92]);
				UOSInt i;
				sb->AppendC(UTF8STRC("\r\nString Table Length = "));
				sb->AppendUOSInt(strLen);
				sb->AppendC(UTF8STRC("\r\nString Table Value Length = "));
				sb->AppendU16(ReadUInt16(&res->data[94]));
				sb->AppendC(UTF8STRC("\r\nString Table Key = "));
				sb->AppendSlow((UTF8Char*)&res->data[96]);
				v = (UInt32)Text::StrHex2Int32C((Char*)&res->data[96]);
				sb->AppendC(UTF8STRC("\r\nLanguage = "));
				sb->AppendU32((v >> 16) & 0xffff);
				Text::Locale::LocaleEntry *locale = Text::Locale::GetLocaleEntry((v >> 16) & 0xffff);
				if (locale)
				{
					sb->AppendC(UTF8STRC(" ("));
					sb->AppendSlow(locale->desc);
					sb->AppendC(UTF8STRC(")"));
				}
				sb->AppendC(UTF8STRC("\r\nCodePage = "));
				sb->AppendU32(v & 0xffff);
				if ((sptr = Text::EncodingFactory::GetName(u8buff, v & 0xffff)) != 0)
				{
					sb->AppendC(UTF8STRC(" ("));
					sb->AppendC(u8buff, (UOSInt)(sptr - u8buff));
					sb->AppendC(UTF8STRC(")"));
				}
				Text::Encoding enc(v & 0xffff);
				i = 108;
				strLen -= 16;
				while (strLen >= 4)
				{
					v = ReadUInt16(&res->data[i]);
					v2 = ReadUInt16(&res->data[i + 2]);
					if (strLen < v || v <= 4)
						break;
					sb->AppendC(UTF8STRC("\r\n"));
					sb->AppendSlow((UTF8Char*)&res->data[i + 4]);
					sptr = enc.UTF8FromBytes(u8buff, &res->data[i + v - v2], v2, 0);
					sb->AppendC(UTF8STRC(" = "));
					sb->AppendC(u8buff, (UOSInt)(sptr - u8buff));
					v2 = (v + 3) & (UInt32)~3;
					i += v2;
					strLen -= v2;
				}
			}
		}
	}
}
