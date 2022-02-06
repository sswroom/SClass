#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "Text/ChineseInfo.h"
#include "Text/MyString.h"

Text::ChineseInfo::ChineseInfo()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPathC(sbuff, sptr, UTF8STRC("Chinese.dat"));
	NEW_CLASS(this->fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	this->currCharBuff = MemAlloc(UInt8, 256);
	this->currCharCode = 0;
	this->fileSize = this->fs->GetLength();
}

Text::ChineseInfo::~ChineseInfo()
{
	MemFree(this->currCharBuff);
	DEL_CLASS(this->fs);
}

Bool Text::ChineseInfo::GetCharInfo(UInt32 charCode, CharacterInfo *chInfo)
{
	UInt64 startOfst = ((UInt64)charCode) << 8;
	Bool fromFile = false;
	if (this->currCharCode != charCode && this->fileSize >= startOfst + 256)
	{
		if (fs->SeekFromBeginning(startOfst) == startOfst)
		{
			if (fs->Read(this->currCharBuff, 256) == 256)
			{
				this->currCharCode = charCode;
			}
			else
			{
				this->currCharCode = 0;
			}
		}
	}
	if (this->currCharCode == charCode)
	{
		fromFile = true;
		if (this->currCharBuff[224])
		{
			chInfo->cantonPronun[0] = Cantonese2Int((UTF8Char*)&this->currCharBuff[224]);
		}
		else
		{
			chInfo->cantonPronun[0] = 0;
		}
		if (this->currCharBuff[232])
		{
			chInfo->cantonPronun[1] = Cantonese2Int((UTF8Char*)&this->currCharBuff[232]);
		}
		else
		{
			chInfo->cantonPronun[1] = 0;
		}
		if (this->currCharBuff[240])
		{
			chInfo->cantonPronun[2] = Cantonese2Int((UTF8Char*)&this->currCharBuff[240]);
		}
		else
		{
			chInfo->cantonPronun[2] = 0;
		}
		if (this->currCharBuff[248])
		{
			chInfo->cantonPronun[3] = Cantonese2Int((UTF8Char*)&this->currCharBuff[248]);
		}
		else
		{
			chInfo->cantonPronun[3] = 0;
		}
		chInfo->radical = ReadUInt32(&this->currCharBuff[8]);
		chInfo->strokeCount = this->currCharBuff[12];
		chInfo->charType = (CharType)(this->currCharBuff[13] & 0xf);
		chInfo->mainChar = ((this->currCharBuff[13] & 0x80) != 0);
		if (chInfo->cantonPronun[0] != ReadUInt16(&this->currCharBuff[0]) || chInfo->cantonPronun[1] != ReadUInt16(&this->currCharBuff[2]) || chInfo->cantonPronun[2] != ReadUInt16(&this->currCharBuff[4]) || chInfo->cantonPronun[3] != ReadUInt16(&this->currCharBuff[6]))
		{
			SetCharInfo(charCode, chInfo);
		}
	}
	if (!fromFile)
	{
		chInfo->cantonPronun[0] = 0;
		chInfo->cantonPronun[1] = 0;
		chInfo->cantonPronun[2] = 0;
		chInfo->cantonPronun[3] = 0;
		chInfo->radical = 0;
		chInfo->strokeCount = 0;
		MemClear(this->currCharBuff, 256);
		this->currCharCode = charCode;
	}
	return true;
}

Bool Text::ChineseInfo::SetCharInfo(UInt32 charCode, CharacterInfo *chInfo)
{
	UInt8 buff[256];
	UInt64 startOfst = ((UInt64)charCode) << 8;
	UInt64 writeSize;
	if (this->currCharCode != charCode)
	{
		return false;
	}
	if (this->fileSize < startOfst)
	{
		UInt8 *tmpBuff;
		if (this->fs->SeekFromBeginning(this->fileSize) != this->fileSize)
		{
			return false;
		}
		writeSize = startOfst - this->fileSize;

		tmpBuff = MemAlloc(UInt8, 65536);
		MemClear(tmpBuff, 65536);
		while (writeSize > 0)
		{
			if (writeSize >= 65536)
			{
				this->fs->Write(tmpBuff, 65536);
				writeSize -= 65536;
			}
			else
			{
				this->fs->Write(tmpBuff, (UOSInt)writeSize);
				writeSize = 0;
			}
		}
		MemFree(tmpBuff);
		this->fileSize = startOfst;
	}
	if (this->fs->SeekFromBeginning(startOfst) != startOfst)
	{
		return false;
	}

	MemClear(buff, 256);
	WriteInt16(&buff[0], chInfo->cantonPronun[0]);
	WriteInt16(&buff[2], chInfo->cantonPronun[1]);
	WriteInt16(&buff[4], chInfo->cantonPronun[2]);
	WriteInt16(&buff[6], chInfo->cantonPronun[3]);
	WriteUInt32(&buff[8], chInfo->radical);
	buff[12] = chInfo->strokeCount;
	buff[13] = (UInt8)chInfo->charType;
	if (chInfo->mainChar)
	{
		buff[13] |= 0x80;
	}
	WriteInt32(&buff[16], ReadInt32(&this->currCharBuff[16]));
	if (chInfo->cantonPronun[0])
	{
		Int2Cantonese((UTF8Char*)&buff[224], chInfo->cantonPronun[0]);
	}
	if (chInfo->cantonPronun[1])
	{
		Int2Cantonese((UTF8Char*)&buff[232], chInfo->cantonPronun[1]);
	}
	if (chInfo->cantonPronun[2])
	{
		Int2Cantonese((UTF8Char*)&buff[240], chInfo->cantonPronun[2]);
	}
	if (chInfo->cantonPronun[3])
	{
		Int2Cantonese((UTF8Char*)&buff[248], chInfo->cantonPronun[3]);
	}

	Bool diff = false;
	UOSInt i = 0;
	while (i < 256)
	{
		if (buff[i] != this->currCharBuff[i])
		{
			diff = true;
			break;
		}
		i++;
	}
	if (!diff)
		return true;

	if (this->fs->Write(buff, 256) != 256)
	{
		return false;
	}
	if (this->fileSize == startOfst)
		this->fileSize += 256;
	if (this->currCharCode == charCode)
	{
		MemCopyNO(this->currCharBuff, buff, 256);
	}
	return true;
}

Bool Text::ChineseInfo::GetRelatedChars(UInt32 charCode, Data::ArrayList<UInt32> *relatedChars)
{
	UInt8 buff[256];
	UInt64 startOfst = ((UInt64)charCode) << 8;
	UInt32 code;
	if (this->fileSize < startOfst + 256)
	{
		return true;
	}
	if (this->fs->SeekFromBeginning(startOfst) != startOfst)
		return false;
	if (this->fs->Read(buff, 256) != 256)
		return false;

	code = ReadUInt32(&buff[16]);
	if (code == 0)
		return true;
	while (true)
	{
		if (code == 0)
			return false;
		if (code == charCode)
		{
			break;
		}
		relatedChars->Add(code);
		startOfst = ((UInt64)code) << 8;
		if (this->fileSize < startOfst + 256)
		{
			return false;
		}
		if (this->fs->SeekFromBeginning(startOfst) != startOfst)
			return false;
		if (this->fs->Read(buff, 256) != 256)
			return false;
		code = ReadUInt32(&buff[16]);
	}

	return true;
}

Bool Text::ChineseInfo::AddRelation(UInt32 charCode, UInt32 relatedCharCode)
{
	UInt8 buff[256];
	UInt64 startOfst = ((UInt64)relatedCharCode) << 8;
	if (this->fileSize < startOfst + 256)
	{
		return false;
	}
	if (this->fs->SeekFromBeginning(startOfst) != startOfst)
		return false;
	if (this->fs->Read(buff, 256) != 256)
		return false;

	if (ReadInt32(&buff[16]) != 0)
	{
		return false;
	}
	WriteUInt32(&buff[16], charCode);
	if (this->fs->SeekFromBeginning(startOfst) != startOfst)
		return false;

	this->fs->Write(buff, 256);
	if (relatedCharCode == this->currCharCode)
	{
		MemCopyNO(this->currCharBuff, buff, 256);
	}
	UInt32 currCode = charCode;
	UInt32 nextCode;
	while (true)
	{
		startOfst = ((UInt64)currCode) << 8;
		if (this->fileSize < startOfst + 256)
		{
			return false;
		}
		if (this->fs->SeekFromBeginning(startOfst) != startOfst)
			return false;
		if (this->fs->Read(buff, 256) != 256)
			return false;

		nextCode = ReadUInt32(&buff[16]);
		if (nextCode == 0 || nextCode == charCode)
		{
			WriteUInt32(&buff[16], relatedCharCode);
			if (this->fs->SeekFromBeginning(startOfst) != startOfst)
				return false;
			if (this->fs->Write(buff, 256) != 256)
				return false;

			if (currCode == this->currCharCode)
			{
				MemCopyNO(this->currCharBuff, buff, 256);
			}
			return true;
		}
		currCode = nextCode;
	}
	return false;
}

UInt16 Text::ChineseInfo::Cantonese2Int(const UTF8Char *s)
{
	if (*s == 0)
		return 0;
	UTF8Char sbuff[8];
	UOSInt len = Text::StrCharCnt(s);
	if (len > 7)
		return 0;
	UTF8Char c;
	UTF8Char *sptr;
	Bool allowEmpty = false;
	CantonesePronunFront cpf = CPF_NONE;
	CantonesePronunTail cpt = CPT_NONE;
	UInt32 cpn = 0;
	c = *s;
	switch (c)
	{
	case 'a':
		break;
	case 'b':
		cpf = CPF_B;
		s++;
		break;
	case 'c':
		cpf = CPF_C;
		s++;
		break;
	case 'd':
		cpf = CPF_D;
		s++;
		break;
	case 'e':
		break;
	case 'f':
		cpf = CPF_F;
		s++;
		break;
	case 'g':
		if (s[1] == 'w')
		{
			cpf = CPF_GW;
			s += 2;
		}
		else
		{
			cpf = CPF_G;
			s++;
		}
		break;
	case 'h':
		cpf = CPF_H;
		s++;
		break;
	case 'i':
		break;
	case 'j':
		cpf = CPF_J;
		s++;
		break;
	case 'k':
		if (s[1] == 'w')
		{
			cpf = CPF_KW;
			s += 2;
		}
		else
		{
			cpf = CPF_K;
			s++;
		}
		break;
	case 'l':
		cpf = CPF_L;
		s++;
		break;
	case 'm':
		cpf = CPF_M;
		s++;
		allowEmpty = true;
		break;
	case 'n':
		if (s[1] == 'g')
		{
			cpf = CPF_NG;
			allowEmpty = true;
			s += 2;
		}
		else
		{
			cpf = CPF_N;
			s++;
		}
		break;
	case 'o':
		break;
	case 'p':
		cpf = CPF_P;
		s++;
		break;
	case 's':
		cpf = CPF_S;
		s++;
		break;
	case 't':
		cpf = CPF_T;
		s++;
		break;
	case 'u':
		break;
	case 'w':
		cpf = CPF_W;
		s++;
		break;
	case 'z':
		cpf = CPF_Z;
		s++;
		break;
	default:
		return 0;
	}
	sptr = Text::StrConcat(sbuff, s);
	if (sptr[-1] <= '0' || sptr[-1] > '9')
		return 0;
	cpn = (UInt32)sptr[-1] - '0';
	*--sptr = 0;

	if (sbuff[0] == 'a')
	{
		if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("aa")))
		{
			cpt = CPT_AA;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("aai")))
		{
			cpt = CPT_AAI;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("aak")))
		{
			cpt = CPT_AAK;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("aam")))
		{
			cpt = CPT_AAM;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("aan")))
		{
			cpt = CPT_AAN;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("aang")))
		{
			cpt = CPT_AANG;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("aap")))
		{
			cpt = CPT_AAP;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("aat")))
		{
			cpt = CPT_AAT;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("aau")))
		{
			cpt = CPT_AAU;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ai")))
		{
			cpt = CPT_AI;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ak")))
		{
			cpt = CPT_AI;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("am")))
		{
			cpt = CPT_AM;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("an")))
		{
			cpt = CPT_AN;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ang")))
		{
			cpt = CPT_ANG;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ap")))
		{
			cpt = CPT_AP;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("at")))
		{
			cpt = CPT_AT;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("au")))
		{
			cpt = CPT_AU;
		}
		else
		{
			return 0;
		}
	}
	else if (sbuff[0] == 'e')
	{
		if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("e")))
		{
			cpt = CPT_E;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ei")))
		{
			cpt = CPT_EI;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ek")))
		{
			cpt = CPT_EK;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("eng")))
		{
			cpt = CPT_ENG;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("eoi")))
		{
			cpt = CPT_EOI;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("eon")))
		{
			cpt = CPT_EON;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("eong")))
		{
			cpt = CPT_EONG;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("eot")))
		{
			cpt = CPT_EOT;
		}
		else
		{
			return 0;
		}
	}
	else if (sbuff[0] == 'i')
	{
		if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("i")))
		{
			cpt = CPT_I;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ik")))
		{
			cpt = CPT_IK;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("im")))
		{
			cpt = CPT_IM;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("in")))
		{
			cpt = CPT_IN;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ing")))
		{
			cpt = CPT_ING;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ip")))
		{
			cpt = CPT_IP;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("it")))
		{
			cpt = CPT_IT;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("iu")))
		{
			cpt = CPT_IU;
		}
		else
		{
			return 0;
		}
	}
	else if (sbuff[0] == 'o')
	{
		if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("o")))
		{
			cpt = CPT_O;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("oe")))
		{
			cpt = CPT_OE;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("oei")))
		{
			cpt = CPT_OEI;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("oek")))
		{
			cpt = CPT_OEK;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("oeng")))
		{
			cpt = CPT_OENG;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("oi")))
		{
			cpt = CPT_OI;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ok")))
		{
			cpt = CPT_OK;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("on")))
		{
			cpt = CPT_ON;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ong")))
		{
			cpt = CPT_ONG;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ot")))
		{
			cpt = CPT_OT;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ou")))
		{
			cpt = CPT_OU;
		}
		else
		{
			return 0;
		}
	}
	else if (sbuff[0] == 'u')
	{
		if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("u")))
		{
			cpt = CPT_U;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ui")))
		{
			cpt = CPT_UI;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("uk")))
		{
			cpt = CPT_UK;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("un")))
		{
			cpt = CPT_UN;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ung")))
		{
			cpt = CPT_UNG;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ut")))
		{
			cpt = CPT_UT;
		}
		else
		{
			return 0;
		}
	}
	else if (sbuff[0] == 'y')
	{
		if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("yu")))
		{
			cpt = CPT_YU;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("yuk")))
		{
			cpt = CPT_YUK;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("yun")))
		{
			cpt = CPT_YUN;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("yung")))
		{
			cpt = CPT_YUNG;
		}
		else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("yut")))
		{
			cpt = CPT_YUT;
		}
		else
		{
			return 0;
		}
	}
	else if (sbuff[0] == 0 && allowEmpty)
	{
		cpt = CPT_NONE;
	}
	else
	{
		return 0;
	}
	return (UInt16)((UInt32)(cpf << 11) | (UInt32)(cpt << 4) | cpn);
}

UTF8Char *Text::ChineseInfo::Int2Cantonese(UTF8Char *buff, UInt16 iVal)
{
	switch (iVal >> 11)
	{
	case CPF_B:
		*buff++ = 'b';
		break;
	case CPF_C:
		*buff++ = 'c';
		break;
	case CPF_D:
		*buff++ = 'd';
		break;
	case CPF_F:
		*buff++ = 'f';
		break;
	case CPF_G:
		*buff++ = 'g';
		break;
	case CPF_GW:
		*buff++ = 'g';
		*buff++ = 'w';
		break;
	case CPF_H:
		*buff++ = 'h';
		break;
	case CPF_J:
		*buff++ = 'j';
		break;
	case CPF_K:
		*buff++ = 'k';
		break;
	case CPF_KW:
		*buff++ = 'k';
		*buff++ = 'w';
		break;
	case CPF_L:
		*buff++ = 'l';
		break;
	case CPF_M:
		*buff++ = 'm';
		break;
	case CPF_N:
		*buff++ = 'n';
		break;
	case CPF_NG:
		*buff++ = 'n';
		*buff++ = 'g';
		break;
	case CPF_P:
		*buff++ = 'p';
		break;
	case CPF_S:
		*buff++ = 's';
		break;
	case CPF_T:
		*buff++ = 't';
		break;
	case CPF_W:
		*buff++ = 'w';
		break;
	case CPF_Z:
		*buff++ = 'z';
		break;
	}
	switch ((iVal & 0x7f0) >> 4)
	{
	case CPT_AA:
		*buff++ = 'a';
		*buff++ = 'a';
		break;
	case CPT_AAI:
		*buff++ = 'a';
		*buff++ = 'a';
		*buff++ = 'i';
		break;
	case CPT_AAK:
		*buff++ = 'a';
		*buff++ = 'a';
		*buff++ = 'k';
		break;
	case CPT_AAM:
		*buff++ = 'a';
		*buff++ = 'a';
		*buff++ = 'm';
		break;
	case CPT_AAN:
		*buff++ = 'a';
		*buff++ = 'a';
		*buff++ = 'n';
		break;
	case CPT_AANG:
		*buff++ = 'a';
		*buff++ = 'a';
		*buff++ = 'n';
		*buff++ = 'g';
		break;
	case CPT_AAP:
		*buff++ = 'a';
		*buff++ = 'a';
		*buff++ = 'p';
		break;
	case CPT_AAT:
		*buff++ = 'a';
		*buff++ = 'a';
		*buff++ = 't';
		break;
	case CPT_AAU:
		*buff++ = 'a';
		*buff++ = 'a';
		*buff++ = 'u';
		break;
	case CPT_AI:
		*buff++ = 'a';
		*buff++ = 'i';
		break;
	case CPT_AK:
		*buff++ = 'a';
		*buff++ = 'k';
		break;
	case CPT_AM:
		*buff++ = 'a';
		*buff++ = 'm';
		break;
	case CPT_AN:
		*buff++ = 'a';
		*buff++ = 'n';
		break;
	case CPT_ANG:
		*buff++ = 'a';
		*buff++ = 'n';
		*buff++ = 'g';
		break;
	case CPT_AP:
		*buff++ = 'a';
		*buff++ = 'p';
		break;
	case CPT_AT:
		*buff++ = 'a';
		*buff++ = 't';
		break;
	case CPT_AU:
		*buff++ = 'a';
		*buff++ = 'u';
		break;
	case CPT_E:
		*buff++ = 'e';
		break;
	case CPT_EI:
		*buff++ = 'e';
		*buff++ = 'i';
		break;
	case CPT_EK:
		*buff++ = 'e';
		*buff++ = 'k';
		break;
	case CPT_ENG:
		*buff++ = 'e';
		*buff++ = 'n';
		*buff++ = 'g';
		break;
	case CPT_EOI:
		*buff++ = 'e';
		*buff++ = 'o';
		*buff++ = 'i';
		break;
	case CPT_EON:
		*buff++ = 'e';
		*buff++ = 'o';
		*buff++ = 'n';
		break;
	case CPT_EONG:
		*buff++ = 'e';
		*buff++ = 'o';
		*buff++ = 'n';
		*buff++ = 'g';
		break;
	case CPT_EOT:
		*buff++ = 'e';
		*buff++ = 'o';
		*buff++ = 't';
		break;
	case CPT_I:
		*buff++ = 'i';
		break;
	case CPT_IK:
		*buff++ = 'i';
		*buff++ = 'k';
		break;
	case CPT_IM:
		*buff++ = 'i';
		*buff++ = 'm';
		break;
	case CPT_IN:
		*buff++ = 'i';
		*buff++ = 'n';
		break;
	case CPT_ING:
		*buff++ = 'i';
		*buff++ = 'n';
		*buff++ = 'g';
		break;
	case CPT_IP:
		*buff++ = 'i';
		*buff++ = 'p';
		break;
	case CPT_IT:
		*buff++ = 'i';
		*buff++ = 't';
		break;
	case CPT_IU:
		*buff++ = 'i';
		*buff++ = 'u';
		break;
	case CPT_O:
		*buff++ = 'o';
		break;
	case CPT_OE:
		*buff++ = 'o';
		*buff++ = 'e';
		break;
	case CPT_OEI:
		*buff++ = 'o';
		*buff++ = 'e';
		*buff++ = 'i';
		break;
	case CPT_OEK:
		*buff++ = 'o';
		*buff++ = 'e';
		*buff++ = 'k';
		break;
	case CPT_OENG:
		*buff++ = 'o';
		*buff++ = 'e';
		*buff++ = 'n';
		*buff++ = 'g';
		break;
	case CPT_OI:
		*buff++ = 'o';
		*buff++ = 'i';
		break;
	case CPT_OK:
		*buff++ = 'o';
		*buff++ = 'k';
		break;
	case CPT_ON:
		*buff++ = 'o';
		*buff++ = 'n';
		break;
	case CPT_ONG:
		*buff++ = 'o';
		*buff++ = 'n';
		*buff++ = 'g';
		break;
	case CPT_OT:
		*buff++ = 'o';
		*buff++ = 't';
		break;
	case CPT_OU:
		*buff++ = 'o';
		*buff++ = 'u';
		break;
	case CPT_U:
		*buff++ = 'u';
		break;
	case CPT_UI:
		*buff++ = 'u';
		*buff++ = 'i';
		break;
	case CPT_UK:
		*buff++ = 'u';
		*buff++ = 'k';
		break;
	case CPT_UN:
		*buff++ = 'u';
		*buff++ = 'n';
		break;
	case CPT_UNG:
		*buff++ = 'u';
		*buff++ = 'n';
		*buff++ = 'g';
		break;
	case CPT_UT:
		*buff++ = 'u';
		*buff++ = 't';
		break;
	case CPT_YU:
		*buff++ = 'y';
		*buff++ = 'u';
		break;
	case CPT_YUK:
		*buff++ = 'y';
		*buff++ = 'u';
		*buff++ = 'k';
		break;
	case CPT_YUN:
		*buff++ = 'y';
		*buff++ = 'u';
		*buff++ = 'n';
		break;
	case CPT_YUNG:
		*buff++ = 'y';
		*buff++ = 'u';
		*buff++ = 'n';
		*buff++ = 'g';
		break;
	case CPT_YUT:
		*buff++ = 'y';
		*buff++ = 'u';
		*buff++ = 't';
		break;
	}
	if (iVal & 15)
	{
		*buff++ = (UInt8)('0' + (iVal & 15));
	}
	*buff = 0;
	return buff;
}
