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

IO::EXEFile::EXEFile(const UTF8Char *fileName) : IO::ParsedObject(fileName)
{
	NEW_CLASS(this->propNames, Data::ArrayList<const UTF8Char *>());
	NEW_CLASS(this->propValues, Data::ArrayList<const UTF8Char *>());
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
		Text::StrDelNew(this->propNames->GetItem(i));
		Text::StrDelNew(this->propValues->GetItem(i));
	}
	DEL_CLASS(this->propNames);
	DEL_CLASS(this->propValues);

	i = this->importList->GetCount();
	while (i-- > 0)
	{
		ImportInfo *imp = this->importList->GetItem(i);
		Text::StrDelNew(imp->moduleName);
		j = imp->funcs->GetCount();
		while (j-- > 0)
		{
			Text::StrDelNew(imp->funcs->GetItem(j));
		}
		DEL_CLASS(imp->funcs);
		MemFree(imp);
	}
	DEL_CLASS(this->importList);

	i = this->exportList->GetCount();
	while (i-- > 0)
	{
		ExportInfo *exp = this->exportList->GetItem(i);
		Text::StrDelNew(exp->funcName);
		MemFree(exp);
	}
	DEL_CLASS(this->exportList);

	i = this->resList->GetCount();
	while (i-- > 0)
	{
		ResourceInfo *res = this->resList->GetItem(i);
		Text::StrDelNew(res->name);
		MemFree((void*)res->data);
		MemFree(res);
	}
	DEL_CLASS(this->resList);
}

IO::ParsedObject::ParserType IO::EXEFile::GetParserType()
{
	return IO::ParsedObject::PT_EXE_PARSER;
}

void IO::EXEFile::AddProp(const UTF8Char *name, const UTF8Char *value)
{
	if (name != 0 && value != 0)
	{
		this->propNames->Add(Text::StrCopyNew(name));
		this->propValues->Add(Text::StrCopyNew(value));
	}
}

UOSInt IO::EXEFile::GetPropCount()
{
	return this->propNames->GetCount();
}

const UTF8Char *IO::EXEFile::GetPropName(UOSInt index)
{
	return this->propNames->GetItem(index);
}

const UTF8Char *IO::EXEFile::GetPropValue(UOSInt index)
{
	return this->propValues->GetItem(index);
}

UOSInt IO::EXEFile::AddImportModule(const UTF8Char *moduleName)
{
	ImportInfo *imp;
	imp = MemAlloc(ImportInfo, 1);
	NEW_CLASS(imp->funcs, Data::ArrayList<const UTF8Char*>());
	imp->moduleName = Text::StrCopyNew(moduleName);
	return this->importList->Add(imp);
}

void IO::EXEFile::AddImportFunc(UOSInt modIndex, const UTF8Char *funcName)
{
	ImportInfo *imp;
	imp = this->importList->GetItem(modIndex);
	if (imp)
	{
		imp->funcs->Add(Text::StrCopyNew(funcName));
	}
}

UOSInt IO::EXEFile::GetImportCount()
{
	return this->importList->GetCount();
}

const UTF8Char *IO::EXEFile::GetImportName(UOSInt modIndex)
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

const UTF8Char *IO::EXEFile::GetImportFunc(UOSInt modIndex, UOSInt funcIndex)
{
	ImportInfo *imp = this->importList->GetItem(modIndex);
	if (imp)
	{
		return imp->funcs->GetItem(funcIndex);
	}
	return 0;
}

void IO::EXEFile::AddExportFunc(const UTF8Char *funcName)
{
	ExportInfo *exp = MemAlloc(ExportInfo, 1);
	exp->funcName = Text::StrCopyNew(funcName);
	this->exportList->Add(exp);
}

UOSInt IO::EXEFile::GetExportCount()
{
	return this->exportList->GetCount();
}

const UTF8Char *IO::EXEFile::GetExportName(UOSInt index)
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

void IO::EXEFile::AddResource(const UTF8Char *name, const UInt8 *data, UOSInt dataSize, UInt32 codePage, ResourceType rt)
{
	ResourceInfo *res = MemAlloc(ResourceInfo, 1);
	res->name = Text::StrCopyNew(name);
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

Bool IO::EXEFile::GetFileTime(const UTF8Char *fileName, Data::DateTime *fileTimeOut)
{
	IO::FileStream *fs;
	UInt8 buff[64];

	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
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

const UTF8Char *IO::EXEFile::GetResourceTypeName(ResourceType rt)
{
	switch (rt)
	{
	case RT_VERSIONINFO:
		return (const UTF8Char*)"VersionInfo";
	case RT_BITMAP:
		return (const UTF8Char*)"Bitmap";
	case RT_MENU:
		return (const UTF8Char*)"Menu";
	case RT_DIALOG:
		return (const UTF8Char*)"Dialog";
	case RT_STRINGTABLE:
		return (const UTF8Char*)"StringTable";
	case RT_ACCELERATOR:
		return (const UTF8Char*)"Accelerator";
	case RT_CURSOR:
		return (const UTF8Char*)"Cursor";
	case RT_ICON:
		return (const UTF8Char*)"Icon";
	case RT_FONT:
		return (const UTF8Char*)"Font";
	case RT_FONTDIR:
		return (const UTF8Char*)"FontDir";
	case RT_RAW_DATA:
		return (const UTF8Char*)"RawData";
	case RT_MESSAGETABLE:
		return (const UTF8Char*)"MessageTable";
	case RT_DLGINCLUDE:
		return (const UTF8Char*)"DlgInclude";
	case RT_PLUGPLAY:
		return (const UTF8Char*)"PlugAndPlay";
	case RT_VXD:
		return (const UTF8Char*)"VXD";
	case RT_ANICURSOR:
		return (const UTF8Char*)"AniCursor";
	case RT_ANIICON:
		return (const UTF8Char*)"AniIcon";
	case RT_HTML:
		return (const UTF8Char*)"HTML";
	case RT_MANIFEST:
		return (const UTF8Char*)"ManiFest";
	case RT_UNKNOWN:
	default:
		return (const UTF8Char*)"Unknown";
	}
}

void IO::EXEFile::GetResourceDesc(const ResourceInfo *res, Text::StringBuilderUTF *sb)
{
	UTF8Char u8buff[256];
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
				sb->Append((const UTF8Char*)"Length = ");
				sb->AppendU32(verSize);
				sb->Append((const UTF8Char*)"\r\nValue Length = ");
				sb->AppendU16(ReadUInt16(&res->data[2]));
				sb->Append((const UTF8Char*)"\r\nKey = ");
				sb->Append((UTF8Char*)&res->data[4]);
				sb->Append((const UTF8Char*)"\r\nSignature = 0x");
				sb->AppendHex32(ReadUInt32(&res->data[20]));
				sb->Append((const UTF8Char*)"\r\nStruct Version = ");
				sb->AppendU16(ReadUInt16(&res->data[26]));
				sb->Append((const UTF8Char*)".");
				sb->AppendU16(ReadUInt16(&res->data[24]));
				sb->Append((const UTF8Char*)"\r\nFile Version = ");
				sb->AppendU16(ReadUInt16(&res->data[30]));
				sb->Append((const UTF8Char*)".");
				sb->AppendU16(ReadUInt16(&res->data[28]));
				sb->Append((const UTF8Char*)".");
				sb->AppendU16(ReadUInt16(&res->data[34]));
				sb->Append((const UTF8Char*)".");
				sb->AppendU16(ReadUInt16(&res->data[32]));
				sb->Append((const UTF8Char*)"\r\nProduct Version = ");
				sb->AppendU16(ReadUInt16(&res->data[38]));
				sb->Append((const UTF8Char*)".");
				sb->AppendU16(ReadUInt16(&res->data[36]));
				sb->Append((const UTF8Char*)".");
				sb->AppendU16(ReadUInt16(&res->data[42]));
				sb->Append((const UTF8Char*)".");
				sb->AppendU16(ReadUInt16(&res->data[40]));
				sb->Append((const UTF8Char*)"\r\nFile Flags Mask = 0x");
				sb->AppendHex32(ReadUInt32(&res->data[44]));
				sb->Append((const UTF8Char*)"\r\nFile Flags = 0x");
				sb->AppendHex32(ReadUInt32(&res->data[48]));
				sb->Append((const UTF8Char*)"\r\nFile OS = 0x");
				sb->AppendHex32(v = ReadUInt32(&res->data[52]));
				switch (v)
				{
				case 0x00010001:
					sb->Append((const UTF8Char*)" (16-bit Windows running on MS-DOS)");
					break;
				case 0x00010004:
					sb->Append((const UTF8Char*)" (32-bit Windows running on MS-DOS)");
					break;
				case 0x00040004:
					sb->Append((const UTF8Char*)" (Windows NT)");
					break;
				case 0x00020002:
					sb->Append((const UTF8Char*)" (16-bit Presentation Manager running on 16-bit OS/2)");
					break;
				case 0x00030003:
					sb->Append((const UTF8Char*)" (32-bit Presentation Manager running on 32-bit OS/2)");
					break;
				case 0x00010000:
					sb->Append((const UTF8Char*)" (MS-DOS)");
					break;
				case 0x00040000:
					sb->Append((const UTF8Char*)" (Windows NT)");
					break;
				case 0x00000001:
					sb->Append((const UTF8Char*)" (16-bit Windows)");
					break;
				case 0x00000004:
					sb->Append((const UTF8Char*)" (32-bit Windows)");
					break;
				case 0x00020000:
					sb->Append((const UTF8Char*)" (16-bit OS/2)");
					break;
				case 0x00030000:
					sb->Append((const UTF8Char*)" (32-bit OS/2)");
					break;
				case 0x00000002:
					sb->Append((const UTF8Char*)" (16-bit Presentation Manager)");
					break;
				case 0x00000003:
					sb->Append((const UTF8Char*)" (32-bit Presentation Manager)");
					break;
				}
				sb->Append((const UTF8Char*)"\r\nFile Type = 0x");
				sb->AppendHex32(v = ReadUInt32(&res->data[56]));
				switch (v)
				{
				case 0x00000001:
					sb->Append((const UTF8Char*)" (application)");
					break;
				case 0x00000002:
					sb->Append((const UTF8Char*)" (DLL)");
					break;
				case 0x00000003:
					sb->Append((const UTF8Char*)" (device driver)");
					break;
				case 0x00000004:
					sb->Append((const UTF8Char*)" (font)");
					break;
				case 0x00000007:
					sb->Append((const UTF8Char*)" (static-link library)");
					break;
				case 0x00000005:
					sb->Append((const UTF8Char*)" (virtual device)");
					break;
				}
				sb->Append((const UTF8Char*)"\r\nFile Sub-Type = 0x");
				sb->AppendHex32(v2 = ReadUInt32(&res->data[60]));
				if (v == 0x00000003) //VFT_DRV
				{
					switch (v2)
					{
					case 0x0000000A:
						sb->Append((const UTF8Char*)" (communications driver)");
						break;
					case 0x00000004:
						sb->Append((const UTF8Char*)" (display driver)");
						break;
					case 0x00000008:
						sb->Append((const UTF8Char*)" (installable driver)");
						break;
					case 0x00000002:
						sb->Append((const UTF8Char*)" (keyboard driver)");
						break;
					case 0x00000003:
						sb->Append((const UTF8Char*)" (language driver)");
						break;
					case 0x00000005:
						sb->Append((const UTF8Char*)" (mouse driver)");
						break;
					case 0x00000006:
						sb->Append((const UTF8Char*)" (network driver)");
						break;
					case 0x00000001:
						sb->Append((const UTF8Char*)" (printer driver)");
						break;
					case 0x00000009:
						sb->Append((const UTF8Char*)" (sound driver)");
						break;
					case 0x00000007:
						sb->Append((const UTF8Char*)" (system driver)");
						break;
					case 0x0000000C:
						sb->Append((const UTF8Char*)" (versioned printer driver)");
						break;
					}
				}
				else if (v == 0x00000004) //VFT_FONT
				{
					switch (v2)
					{
					case 0x00000001:
						sb->Append((const UTF8Char*)" (raster font)");
						break;
					case 0x00000003:
						sb->Append((const UTF8Char*)" (TrueType font)");
						break;
					case 0x00000002:
						sb->Append((const UTF8Char*)" (vector font.)");
						break;
					}
				}
				sb->Append((const UTF8Char*)"\r\nFile Date = 0x");
				sb->AppendHex32(ReadUInt32(&res->data[64]));
				sb->AppendHex32(ReadUInt32(&res->data[68]));
				sb->Append((const UTF8Char*)"\r\nString File Length = ");
				sb->AppendU16(ReadUInt16(&res->data[72]));
				sb->Append((const UTF8Char*)"\r\nString File Value Length = ");
				sb->AppendU16(ReadUInt16(&res->data[74]));
				sb->Append((const UTF8Char*)"\r\nString File Key = ");
				sb->Append((UTF8Char*)&res->data[76]);
				OSInt strLen = ReadUInt16(&res->data[92]);
				OSInt i;
				sb->Append((const UTF8Char*)"\r\nString Table Length = ");
				sb->AppendOSInt(strLen);
				sb->Append((const UTF8Char*)"\r\nString Table Value Length = ");
				sb->AppendU16(ReadUInt16(&res->data[94]));
				sb->Append((const UTF8Char*)"\r\nString Table Key = ");
				sb->Append((UTF8Char*)&res->data[96]);
				v = (UInt32)Text::StrHex2Int32C((Char*)&res->data[96]);
				sb->Append((const UTF8Char*)"\r\nLanguage = ");
				sb->AppendU32((v >> 16) & 0xffff);
				Text::Locale::LocaleEntry *locale = Text::Locale::GetLocaleEntry((v >> 16) & 0xffff);
				if (locale)
				{
					sb->Append((const UTF8Char*)" (");
					sb->Append(locale->desc);
					sb->Append((const UTF8Char*)")");
				}
				sb->Append((const UTF8Char*)"\r\nCodePage = ");
				sb->AppendU32(v & 0xffff);
				if (Text::EncodingFactory::GetName(u8buff, v & 0xffff))
				{
					sb->Append((const UTF8Char*)" (");
					sb->Append(u8buff);
					sb->Append((const UTF8Char*)")");
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
					sb->Append((const UTF8Char*)"\r\n");
					sb->Append((UTF8Char*)&res->data[i + 4]);
					enc.UTF8FromBytes(u8buff, &res->data[i + v - v2], v2, 0);
					sb->Append((const UTF8Char*)" = ");
					sb->Append(u8buff);
					v2 = (v + 3) & (UInt32)~3;
					i += v2;
					strLen -= v2;
				}
			}
		}
	}
}
