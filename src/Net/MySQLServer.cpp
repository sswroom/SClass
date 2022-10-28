#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/StringUTF8Map.h"
#include "DB/ColDef.h"
#include "Net/MySQLServer.h"
#include "Net/MySQLUtil.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

#define DEFAULT_BUFF_SIZE 2048
#define MYSQLVERSION "5.5.5-"

typedef struct
{
	UInt8 *buff;
	UOSInt buffSize;
	Int32 mode;
	Int32 connId;
	UInt32 capability;
	UInt32 clientCap;
	UInt16 clientCS;
	DB::DBMS::SessionParam param;
	UInt8 authPluginData[20];
	UTF8Char userName[64];
	UOSInt userNameLen;
	UTF8Char database[64];
	Data::StringUTF8Map<const UTF8Char*> *attrMap;
} ClientData;

Net::MySQLServer::CharsetInfo Net::MySQLServer::charsets[] = {
{1,   "big5_chinese_ci",              "big5"},
{2,   "latin2_czech_cs",              "latin2"},
{3,   "dec8_swedish_ci",              "dec8"},
{4,   "cp850_general_ci",             "cp850"},
{5,   "latin1_german1_ci",            "latin1"},
{6,   "hp8_english_ci",               "hp8"},
{7,   "koi8r_general_ci",             "koi8r"},
{8,   "latin1_swedish_ci",            "latin1"},
{9,   "latin2_general_ci",            "latin2"},
{10,  "swe7_swedish_ci",              "swe7"},
{11,  "ascii_general_ci",             "ascii"},
{12,  "ujis_japanese_ci",             "ujis"},
{13,  "sjis_japanese_ci",             "sjis"},
{14,  "cp1251_bulgarian_ci",          "cp1251"},
{15,  "latin1_danish_ci",             "latin1"},
{16,  "hebrew_general_ci",            "hebrew"},
{18,  "tis620_thai_ci",               "tis620"},
{19,  "euckr_korean_ci",              "euckr"},
{20,  "latin7_estonian_cs",           "latin7"},
{21,  "latin2_hungarian_ci",          "latin2"},
{22,  "koi8u_general_ci",             "koi8u"},
{23,  "cp1251_ukrainian_ci",          "cp1251"},
{24,  "gb2312_chinese_ci",            "gb2312"},
{25,  "greek_general_ci",             "greek"},
{26,  "cp1250_general_ci",            "cp1250"},
{27,  "latin2_croatian_ci",           "latin2"},
{28,  "gbk_chinese_ci",               "gbk"},
{29,  "cp1257_lithuanian_ci",         "cp1257"},
{30,  "latin5_turkish_ci",            "latin5"},
{31,  "latin1_german2_ci",            "latin1"},
{32,  "armscii8_general_ci",          "armscii8"},
{33,  "utf8_general_ci",              "utf8"},
{34,  "cp1250_czech_cs",              "cp1250"},
{35,  "ucs2_general_ci",              "ucs2"},
{36,  "cp866_general_ci",             "cp866"},
{37,  "keybcs2_general_ci",           "keybcs2"},
{38,  "macce_general_ci",             "macce"},
{39,  "macroman_general_ci",          "macroman"},
{40,  "cp852_general_ci",             "cp852"},
{41,  "latin7_general_ci",            "latin7"},
{42,  "latin7_general_cs",            "latin7"},
{43,  "macce_bin",                    "macce"},
{44,  "cp1250_croatian_ci",           "cp1250"},
{45,  "utf8mb4_general_ci",           "utf8mb4"},
{46,  "utf8mb4_bin",                  "utf8mb4"},
{47,  "latin1_bin",                   "latin1"},
{48,  "latin1_general_ci",            "latin1"},
{49,  "latin1_general_cs",            "latin1"},
{50,  "cp1251_bin",                   "cp1251"},
{51,  "cp1251_general_ci",            "cp1251"},
{52,  "cp1251_general_cs",            "cp1251"},
{53,  "macroman_bin",                 "macroman"},
{54,  "utf16_general_ci",             "utf16"},
{55,  "utf16_bin",                    "utf16"},
{56,  "utf16le_general_ci",           "utf16le"},
{57,  "cp1256_general_ci",            "cp1256"},
{58,  "cp1257_bin",                   "cp1257"},
{59,  "cp1257_general_ci",            "cp1257"},
{60,  "utf32_general_ci",             "utf32"},
{61,  "utf32_bin",                    "utf32"},
{62,  "utf16le_bin",                  "utf16le"},
{63,  "binary",                       "binary"},
{64,  "armscii8_bin",                 "armscii8"},
{65,  "ascii_bin",                    "ascii"},
{66,  "cp1250_bin",                   "cp1250"},
{67,  "cp1256_bin",                   "cp1256"},
{68,  "cp866_bin",                    "cp866"},
{69,  "dec8_bin",                     "dec8"},
{70,  "greek_bin",                    "greek"},
{71,  "hebrew_bin",                   "hebrew"},
{72,  "hp8_bin",                      "hp8"},
{73,  "keybcs2_bin",                  "keybcs2"},
{74,  "koi8r_bin",                    "koi8r"},
{75,  "koi8u_bin",                    "koi8u"},
{77,  "latin2_bin",                   "latin2"},
{78,  "latin5_bin",                   "latin5"},
{79,  "latin7_bin",                   "latin7"},
{80,  "cp850_bin",                    "cp850"},
{81,  "cp852_bin",                    "cp852"},
{82,  "swe7_bin",                     "swe7"},
{83,  "utf8_bin",                     "utf8"},
{84,  "big5_bin",                     "big5"},
{85,  "euckr_bin",                    "euckr"},
{86,  "gb2312_bin",                   "gb2312"},
{87,  "gbk_bin",                      "gbk"},
{88,  "sjis_bin",                     "sjis"},
{89,  "tis620_bin",                   "tis620"},
{90,  "ucs2_bin",                     "ucs2"},
{91,  "ujis_bin",                     "ujis"},
{92,  "geostd8_general_ci",           "geostd8"},
{93,  "geostd8_bin",                  "geostd8"},
{94,  "latin1_spanish_ci",            "latin1"},
{95,  "cp932_japanese_ci",            "cp932"},
{96,  "cp932_bin",                    "cp932"},
{97,  "eucjpms_japanese_ci",          "eucjpms"},
{98,  "eucjpms_bin",                  "eucjpms"},
{99,  "cp1250_polish_ci",             "cp1250"},
{101, "utf16_unicode_ci",             "utf16"},
{102, "utf16_icelandic_ci",           "utf16"},
{103, "utf16_latvian_ci",             "utf16"},
{104, "utf16_romanian_ci",            "utf16"},
{105, "utf16_slovenian_ci",           "utf16"},
{106, "utf16_polish_ci",              "utf16"},
{107, "utf16_estonian_ci",            "utf16"},
{108, "utf16_spanish_ci",             "utf16"},
{109, "utf16_swedish_ci",             "utf16"},
{110, "utf16_turkish_ci",             "utf16"},
{111, "utf16_czech_ci",               "utf16"},
{112, "utf16_danish_ci",              "utf16"},
{113, "utf16_lithuanian_ci",          "utf16"},
{114, "utf16_slovak_ci",              "utf16"},
{115, "utf16_spanish2_ci",            "utf16"},
{116, "utf16_roman_ci",               "utf16"},
{117, "utf16_persian_ci",             "utf16"},
{118, "utf16_esperanto_ci",           "utf16"},
{119, "utf16_hungarian_ci",           "utf16"},
{120, "utf16_sinhala_ci",             "utf16"},
{121, "utf16_german2_ci",             "utf16"},
{122, "utf16_croatian_mysql561_ci",   "utf16"},
{123, "utf16_unicode_520_ci",         "utf16"},
{124, "utf16_vietnamese_ci",          "utf16"},
{128, "ucs2_unicode_ci",              "ucs2"},
{129, "ucs2_icelandic_ci",            "ucs2"},
{130, "ucs2_latvian_ci",              "ucs2"},
{131, "ucs2_romanian_ci",             "ucs2"},
{132, "ucs2_slovenian_ci",            "ucs2"},
{133, "ucs2_polish_ci",               "ucs2"},
{134, "ucs2_estonian_ci",             "ucs2"},
{135, "ucs2_spanish_ci",              "ucs2"},
{136, "ucs2_swedish_ci",              "ucs2"},
{137, "ucs2_turkish_ci",              "ucs2"},
{138, "ucs2_czech_ci",                "ucs2"},
{139, "ucs2_danish_ci",               "ucs2"},
{140, "ucs2_lithuanian_ci",           "ucs2"},
{141, "ucs2_slovak_ci",               "ucs2"},
{142, "ucs2_spanish2_ci",             "ucs2"},
{143, "ucs2_roman_ci",                "ucs2"},
{144, "ucs2_persian_ci",              "ucs2"},
{145, "ucs2_esperanto_ci",            "ucs2"},
{146, "ucs2_hungarian_ci",            "ucs2"},
{147, "ucs2_sinhala_ci",              "ucs2"},
{148, "ucs2_german2_ci",              "ucs2"},
{149, "ucs2_croatian_mysql561_ci",    "ucs2"},
{150, "ucs2_unicode_520_ci",          "ucs2"},
{151, "ucs2_vietnamese_ci",           "ucs2"},
{159, "ucs2_general_mysql500_ci",     "ucs2"},
{160, "utf32_unicode_ci",             "utf32"},
{161, "utf32_icelandic_ci",           "utf32"},
{162, "utf32_latvian_ci",             "utf32"},
{163, "utf32_romanian_ci",            "utf32"},
{164, "utf32_slovenian_ci",           "utf32"},
{165, "utf32_polish_ci",              "utf32"},
{166, "utf32_estonian_ci",            "utf32"},
{167, "utf32_spanish_ci",             "utf32"},
{168, "utf32_swedish_ci",             "utf32"},
{169, "utf32_turkish_ci",             "utf32"},
{170, "utf32_czech_ci",               "utf32"},
{171, "utf32_danish_ci",              "utf32"},
{172, "utf32_lithuanian_ci",          "utf32"},
{173, "utf32_slovak_ci",              "utf32"},
{174, "utf32_spanish2_ci",            "utf32"},
{175, "utf32_roman_ci",               "utf32"},
{176, "utf32_persian_ci",             "utf32"},
{177, "utf32_esperanto_ci",           "utf32"},
{178, "utf32_hungarian_ci",           "utf32"},
{179, "utf32_sinhala_ci",             "utf32"},
{180, "utf32_german2_ci",             "utf32"},
{181, "utf32_croatian_mysql561_ci",   "utf32"},
{182, "utf32_unicode_520_ci",         "utf32"},
{183, "utf32_vietnamese_ci",          "utf32"},
{192, "utf8_unicode_ci",              "utf8"},
{193, "utf8_icelandic_ci",            "utf8"},
{194, "utf8_latvian_ci",              "utf8"},
{195, "utf8_romanian_ci",             "utf8"},
{196, "utf8_slovenian_ci",            "utf8"},
{197, "utf8_polish_ci",               "utf8"},
{198, "utf8_estonian_ci",             "utf8"},
{199, "utf8_spanish_ci",              "utf8"},
{200, "utf8_swedish_ci",              "utf8"},
{201, "utf8_turkish_ci",              "utf8"},
{202, "utf8_czech_ci",                "utf8"},
{203, "utf8_danish_ci",               "utf8"},
{204, "utf8_lithuanian_ci",           "utf8"},
{205, "utf8_slovak_ci",               "utf8"},
{206, "utf8_spanish2_ci",             "utf8"},
{207, "utf8_roman_ci",                "utf8"},
{208, "utf8_persian_ci",              "utf8"},
{209, "utf8_esperanto_ci",            "utf8"},
{210, "utf8_hungarian_ci",            "utf8"},
{211, "utf8_sinhala_ci",              "utf8"},
{212, "utf8_german2_ci",              "utf8"},
{213, "utf8_croatian_mysql561_ci",    "utf8"},
{214, "utf8_unicode_520_ci",          "utf8"},
{215, "utf8_vietnamese_ci",           "utf8"},
{223, "utf8_general_mysql500_ci",     "utf8"},
{224, "utf8mb4_unicode_ci",           "utf8mb4"},
{225, "utf8mb4_icelandic_ci",         "utf8mb4"},
{226, "utf8mb4_latvian_ci",           "utf8mb4"},
{227, "utf8mb4_romanian_ci",          "utf8mb4"},
{228, "utf8mb4_slovenian_ci",         "utf8mb4"},
{229, "utf8mb4_polish_ci",            "utf8mb4"},
{230, "utf8mb4_estonian_ci",          "utf8mb4"},
{231, "utf8mb4_spanish_ci",           "utf8mb4"},
{232, "utf8mb4_swedish_ci",           "utf8mb4"},
{233, "utf8mb4_turkish_ci",           "utf8mb4"},
{234, "utf8mb4_czech_ci",             "utf8mb4"},
{235, "utf8mb4_danish_ci",            "utf8mb4"},
{236, "utf8mb4_lithuanian_ci",        "utf8mb4"},
{237, "utf8mb4_slovak_ci",            "utf8mb4"},
{238, "utf8mb4_spanish2_ci",          "utf8mb4"},
{239, "utf8mb4_roman_ci",             "utf8mb4"},
{240, "utf8mb4_persian_ci",           "utf8mb4"},
{241, "utf8mb4_esperanto_ci",         "utf8mb4"},
{242, "utf8mb4_hungarian_ci",         "utf8mb4"},
{243, "utf8mb4_sinhala_ci",           "utf8mb4"},
{244, "utf8mb4_german2_ci",           "utf8mb4"},
{245, "utf8mb4_croatian_mysql561_ci", "utf8mb4"},
{246, "utf8mb4_unicode_520_ci",       "utf8mb4"},
{247, "utf8mb4_vietnamese_ci",        "utf8mb4"},
{576, "utf8_croatian_ci",             "utf8"},
{577, "utf8_myanmar_ci",              "utf8"},
{578, "utf8_thai_520_w2",             "utf8"},
{608, "utf8mb4_croatian_ci",          "utf8mb4"},
{609, "utf8mb4_myanmar_ci",           "utf8mb4"},
{610, "utf8mb4_thai_520_w2",          "utf8mb4"},
{640, "ucs2_croatian_ci",             "ucs2"},
{641, "ucs2_myanmar_ci",              "ucs2"},
{642, "ucs2_thai_520_w2",             "ucs2"},
{672, "utf16_croatian_ci",            "utf16"},
{673, "utf16_myanmar_ci",             "utf16"},
{674, "utf16_thai_520_w2",            "utf16"},
{736, "utf32_croatian_ci",            "utf32"},
{737, "utf32_myanmar_ci",             "utf32"},
{738, "utf32_thai_520_w2",            "utf32"},
};

void __stdcall Net::MySQLServer::OnClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	Net::MySQLServer *me = (Net::MySQLServer*)userObj;
	ClientData *data = (ClientData*)cliData;
	UOSInt i;

	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		if (me->log)
		{
			UTF8Char sbuff[256];
			UTF8Char *sptr;
			sptr = Text::StrConcatC(sbuff, UTF8STRC("Client "));
			sptr = cli->GetRemoteName(sptr);
			sptr = Text::StrConcatC(sptr, UTF8STRC(" disconnect"));
			me->log->LogMessage(CSTRP(sbuff, sptr), IO::ILogHandler::LogLevel::Action);
		}
		me->dbms->SessEnd(data->connId);
		#if defined(VERBOSE)
		printf("Client disconnected\r\n");
		#endif
		const Data::ArrayList<const UTF8Char*> *attrList = data->attrMap->GetValues();
		i = attrList->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(attrList->GetItem(i));
		}
		DEL_CLASS(data->attrMap);
		MemFree(data->buff);
		MemFree(data);
		DEL_CLASS(cli);
	}
}

void __stdcall Net::MySQLServer::OnClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size)
{
	Net::MySQLServer *me = (Net::MySQLServer*)userObj;
	ClientData *data = (ClientData*)cliData;

	#if defined(VERBOSE)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendHexBuff(buff, size, ' ', Text::LineBreakType::CRLF);
		printf("Received:\r\n%s\r\n", sb.ToString());
	}
	#endif
	if (me->log)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Received "));
		sb.AppendUOSInt(size);
		sb.AppendC(UTF8STRC(" bytes"));
		me->log->LogMessage(sb.ToCString(), IO::ILogHandler::LogLevel::Action);
	}
	MemCopyNO(&data->buff[data->buffSize], buff, size);
	data->buffSize += size;
	if (data->mode == 0)
	{
		if (data->buffSize >= 4)
		{
			UInt32 packetSize;
			if (data->buff[3] != 1)
			{
				cli->Close();
				return;
			}
			packetSize = ReadUInt24(data->buff);
			if (packetSize > data->param.clientMaxPacketSize - 4)
			{
				cli->Close();
				return;
			}
			if (packetSize + 4 <= data->buffSize)
			{
				UTF8Char sbuff[256];
				const UInt8 *bptr;
				const UInt8 *bptrEnd;
				UInt64 iVal;
				UOSInt len;
				const UTF8Char *authResp = 0;
				UOSInt authLen = 0;
				Text::StringBuilderUTF8 sb;

				data->clientCap = ReadUInt32(&data->buff[4]);
				if (data->clientCap & Net::MySQLUtil::CLIENT_PROTOCOL_41)
				{
					if (data->param.clientMaxPacketSize != ReadUInt32(&data->buff[8]))
					{
						if (data->param.clientMaxPacketSize < ReadUInt32(&data->buff[8]))
						{
							data->param.clientMaxPacketSize = ReadUInt32(&data->buff[8]);
							UInt8 *newBuff = MemAlloc(UInt8, data->param.clientMaxPacketSize + 2048);
							MemCopyNO(newBuff, data->buff, data->buffSize);
							MemFree(data->buff);
							data->buff = newBuff;
						}
						else
						{
							data->param.clientMaxPacketSize = ReadUInt32(&data->buff[8]);
						}
					}
					data->clientCS = data->buff[12];
					sb.AppendC(UTF8STRC("Handshake Response 41"));
					sb.AppendC(UTF8STRC("\r\nCapability Flags = 0x"));
					sb.AppendHex32(data->clientCap);
					sb.AppendC(UTF8STRC("\r\nMax Packet Size = "));
					sb.AppendU32(data->param.clientMaxPacketSize);
					sb.AppendC(UTF8STRC("\r\nCharacter Set = "));
					sb.AppendU16(data->clientCS);
					len = (UOSInt)(Text::StrConcat(data->userName, &data->buff[36]) - data->userName);
					data->userNameLen = len;
					bptr = &data->buff[37] + len;
					sb.AppendC(UTF8STRC("\r\nUsername = "));
					sb.AppendC(data->userName, len);
					if (data->clientCap & Net::MySQLUtil::CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA)
					{
						bptr = Net::MySQLUtil::ReadLenencInt(bptr, &iVal);
						authResp = bptr;
						bptr += iVal;
						authLen = (UOSInt)iVal;
					}
					else if (data->clientCap & Net::MySQLUtil::CLIENT_SECURE_CONNECTION)
					{
						authLen = bptr[0];
						authResp = &bptr[1];
						bptr += authLen + 1;
					}
					else
					{
						authLen = Text::StrCharCnt(bptr);
						authResp = bptr;
						bptr += authLen + 1;
					}
					sb.AppendC(UTF8STRC("\r\nAuth Response = "));
					sb.AppendHexBuff(authResp, authLen, ' ', Text::LineBreakType::None);
					if (data->clientCap & Net::MySQLUtil::CLIENT_CONNECT_WITH_DB)
					{
						len = (UOSInt)(Text::StrConcat(data->database, bptr) - data->database);
						sb.AppendC(UTF8STRC("\r\nDatabase = "));
						sb.AppendC(data->database, len);
						bptr += len + 1;
					}
					if (data->clientCap & Net::MySQLUtil::CLIENT_PLUGIN_AUTH)
					{
						len = Text::StrCharCnt(bptr);
						sb.AppendC(UTF8STRC("\r\nAuth Plugin Name = "));
						sb.AppendC(bptr, len);
						bptr += len + 1;
					}
					if (data->clientCap & Net::MySQLUtil::CLIENT_CONNECT_ATTRS)
					{
						bptrEnd = &data->buff[packetSize + 4];
						sb.AppendC(UTF8STRC("\r\nAttr:"));
						bptr = Net::MySQLUtil::ReadLenencInt(bptr, &iVal);
						sb.AppendC(UTF8STRC("\r\n-Length of all key-values = "));
						sb.AppendU64(iVal);
						while (bptr < bptrEnd)
						{
							bptr = Net::MySQLUtil::ReadLenencInt(bptr, &iVal);
							if (iVal == 0 || bptr + iVal > bptrEnd)
								break;
							sb.AppendC(UTF8STRC("\r\n-"));
							sb.AppendC(bptr, (UOSInt)iVal);
							Text::StrConcatC(sbuff, bptr, (UOSInt)iVal);
							bptr += iVal;
							sb.AppendC(UTF8STRC(" = "));
							bptr = Net::MySQLUtil::ReadLenencInt(bptr, &iVal);
							if (bptr + iVal > bptrEnd)
								break;
							sb.AppendC(bptr, (UOSInt)iVal);
							data->attrMap->Put(sbuff, Text::StrCopyNewC(bptr, (UOSInt)iVal));
							bptr += iVal;
						}
					}
				}
				else
				{
					data->clientCap = data->clientCap & 0xffff;
					data->param.clientMaxPacketSize = ReadUInt24(&data->buff[6]);
					sb.AppendC(UTF8STRC("Handshake Response 320"));
					sb.AppendC(UTF8STRC("\r\nCapability Flags = 0x"));
					sb.AppendHex16((UInt16)data->clientCap);
					sb.AppendC(UTF8STRC("\r\nMax Packet Size = "));
					sb.AppendU32(data->param.clientMaxPacketSize);
					len = (UOSInt)(Text::StrConcat(data->userName, &data->buff[9]) - data->userName);
					data->userNameLen = len;
					bptr = &data->buff[10] + len;
					sb.AppendC(UTF8STRC("\r\nUsername = "));
					sb.AppendC(data->userName, len);
					if (data->clientCap & Net::MySQLUtil::CLIENT_CONNECT_WITH_DB)
					{
						authResp = bptr;
						authLen = Text::StrCharCnt(authResp);
						bptr += authLen + 1;
						len = (UOSInt)(Text::StrConcat(data->database, bptr) - data->database);
						sb.AppendC(UTF8STRC("\r\nDatabase = "));
						sb.AppendC(data->database, len);
					}
					else
					{
						authResp = bptr;
						authLen = packetSize - (UOSInt)(bptr - data->buff) + 4;
					}
					sb.AppendC(UTF8STRC("\r\nAuth Response = "));
					sb.AppendHexBuff(authResp, authLen, ' ', Text::LineBreakType::None);
				}
				#if defined(VERBOSE)
				printf("%s\r\n", sb.ToString());
				#endif

				Net::SocketUtil::AddressInfo addr;
				Bool valid = false;
				cli->GetRemoteAddr(&addr);
				if (authLen == 20)
				{
					valid = me->dbms->UserLoginMySQL(data->connId, {data->userName, data->userNameLen}, data->authPluginData, authResp, &addr, &data->param, data->database);
				}

				if (valid)
				{
					sbuff[0] = 7;
					sbuff[1] = 0;
					sbuff[2] = 0;
					sbuff[3] = 2;
					sbuff[4] = 0;
					sbuff[5] = 0;
					sbuff[6] = 0;
					WriteInt16(&sbuff[7], 2);
					sbuff[9] = 0;
					sbuff[10] = 0;
					cli->Write(sbuff, 11);
					data->mode = 1;
					data->buffSize = 0;
					#if defined(VERBOSE)
					printf("Sent login success\r\n");
					#endif
				}
				else
				{
					UTF8Char *sptr;
					sptr = Text::StrConcatC(&sbuff[7], UTF8STRC("#28000Access denied for user '"));
					sptr = Text::StrConcatC(sptr, data->userName, data->userNameLen);
					sptr = Text::StrConcatC(sptr, UTF8STRC("'@'"));
					sptr = Net::SocketUtil::GetAddrName(sptr, &addr);
					*sptr++ = '\'';
					WriteInt24(sbuff, (sptr - sbuff - 4));
					sbuff[3] = 2;
					sbuff[4] = 0xff;
					WriteInt16(&sbuff[5], 0x6A2);
					cli->Write(sbuff, (UOSInt)(sptr - sbuff));
					data->mode = -1;
					#if defined(VERBOSE)
					printf("Sent login failure\r\n");
					#endif
				}
			}
		}
	}
	else if (data->mode == -1)
	{
		cli->Close();
	}
	else if (data->mode == 1)
	{
		UOSInt i = 0;
		while (i + 5 <= data->buffSize)
		{
			UInt32 packetSize = ReadUInt32(&data->buff[i]);
			UInt8 packetType = data->buff[i + 4];
			if (i + packetSize + 4 <= data->buffSize)
			{
				#if defined(VERBOSE)
				printf("Received command %d, size = %d\r\n", packetType, packetSize);
				#endif
				switch (packetType)
				{
				case 3:
					{
						Text::String *sql = Text::String::New(&data->buff[i + 5], packetSize - 1);
					#if defined(VERBOSE)
						printf("COM_QUERY: query_text = %s\r\n", sql->v);
					#endif
						
						DB::DBReader *r = me->dbms->ExecuteReader(data->connId, sql->v, sql->leng);
						if (r)
						{
							if (r->GetRowChanged() != -1)
							{
								UTF8Char sbuff[2048];
								UTF8Char *sptr;
								sbuff[3] = 1;
								sbuff[4] = 0;
								sptr = Net::MySQLUtil::AppendLenencInt(&sbuff[5], (UOSInt)r->GetRowChanged());
								sptr = Net::MySQLUtil::AppendLenencInt(sptr, 0); //last insert-id
								WriteInt16(&sptr[0], 0x4002);
								WriteInt16(&sptr[2], 0);
								sptr += 4;
								WriteInt24(&sbuff[0], sptr - sbuff - 4);
								cli->Write(sbuff, (UOSInt)(sptr - sbuff));
								#if defined(VERBOSE)
								printf("COM_QUERY OK, row changed = %d\r\n", (int)r->GetRowChanged());
								#endif

							}
							else
							{
								UTF8Char sbuff[2048];
								UTF8Char *sptr;
								UInt8 seqId = 1;
								UOSInt j;
								UOSInt k;
								DB::ColDef col(CSTR("a"));
/*
01 00 00 01 01
*/
								sbuff[3] = seqId++;
								sptr = Net::MySQLUtil::AppendLenencInt(&sbuff[4], r->ColCount());
								WriteInt24(&sbuff[0], sptr - sbuff - 4);
								cli->Write(sbuff, (UOSInt)(sptr - sbuff));
								#if defined(VERBOSE)
								printf("COM_QUERY OK, column_count = %d\r\n", (int)r->ColCount());
								#endif

/*
22 00 00 02 03 64 65 66 00 00 00 0C 40 40 61 75 
74 6F 63 6F 6D 6D 69 74 00 0C 3F 00 01 00 00 00 
08 80 00 00 00 00 
*/
								j = 0;
								k = r->ColCount();
								while (j < k)
								{
									r->GetColDef(j, &col);
									sbuff[3] = seqId++;
									sptr = Net::MySQLUtil::AppendLenencStrC(&sbuff[4], UTF8STRC("def")); //catalog
									sptr = Net::MySQLUtil::AppendLenencStrC(sptr, 0, 0); //schema
									sptr = Net::MySQLUtil::AppendLenencStrC(sptr, 0, 0); //table
									sptr = Net::MySQLUtil::AppendLenencStrC(sptr, 0, 0); //org_table
									sptr = Net::MySQLUtil::AppendLenencStrC(sptr, col.GetColName()->v, Text::StrCharCnt(col.GetColName()->v)); //name
									sptr = Net::MySQLUtil::AppendLenencStrC(sptr, 0, 0); //org_name

									sptr = Net::MySQLUtil::AppendLenencInt(sptr, 12);
									WriteInt16(&sptr[0], data->clientCS);
									WriteUInt32(&sptr[2], (UInt32)col.GetColSize());

									DB::DBUtil::ColType colType = col.GetColType();
									sptr[6] = Net::MySQLUtil::ColType2MySQLType(colType);
									UInt16 flags = 0;
									UInt8 digits = 0;
									if (col.IsNotNull())
									{
										flags |= 1;
									}
									if (col.IsPK())
									{
										flags |= 2;
									}
									if (colType == DB::DBUtil::CT_Binary || colType == DB::DBUtil::CT_Vector)
									{
										flags |= 16;
									}
									if (colType == DB::DBUtil::CT_UInt16 || colType == DB::DBUtil::CT_UInt32 || colType == DB::DBUtil::CT_UInt64)
									{
										flags |= 32;
									}
									if (col.IsAutoInc())
									{
										flags |= 512;
									}
									if (colType == DB::DBUtil::CT_DateTime)
									{
										flags |= 1024;
									}
									WriteInt32(&sptr[7], flags);
									if (colType == DB::DBUtil::CT_VarUTF8Char || colType == DB::DBUtil::CT_VarUTF16Char || colType == DB::DBUtil::CT_VarUTF32Char || colType == DB::DBUtil::CT_Double || colType == DB::DBUtil::CT_Float)
									{
										digits = 31;
									}
									sptr[9] = digits;
									sptr[10] = 0;
									sptr[11] = 0;
									sptr += 12;

									WriteInt24(&sbuff[0], sptr - sbuff - 4);
									cli->Write(sbuff, (UOSInt)(sptr - sbuff));
									#if defined(VERBOSE)
									printf("COM_QUERY column: %s\r\n", col.GetColName()->v);
									#endif

									j++;
								}

/*
05 00 00 03 FE 00 00 02 00 
*/
								WriteInt24(&sbuff[0], 5);
								sbuff[3] = seqId++;
								sbuff[4] = 0xfe;
								WriteInt16(&sbuff[5], 0);
								WriteInt16(&sbuff[7], 2);
								cli->Write(sbuff, 9);
								#if defined(VERBOSE)
								printf("COM_QUERY EOF (Columns)\r\n");
								#endif

								Text::StringBuilderUTF8 sb;
								while (r->ReadNext())
								{
/*
02 00 00 04 01 31 
*/
									sbuff[3] = seqId++;
									sptr = &sbuff[4];
									j = 0;
									k = r->ColCount();
									while (j < k)
									{
										if (r->IsNull(j))
										{
											*sptr++ = 0xfb;
										}
										else
										{
											sb.ClearStr();
											r->GetStr(j, &sb);
											sptr = Net::MySQLUtil::AppendLenencStrC(sptr, sb.ToString(), sb.GetLength());
										}
										
										j++;
									}

									WriteInt24(&sbuff[0], sptr - sbuff - 4);
									cli->Write(sbuff, (UOSInt)(sptr - sbuff));
									#if defined(VERBOSE)
									printf("COM_QUERY return row\r\n");
									#endif
								}

/*
05 00 00 05 FE 00 00 02 00
*/								
								WriteInt24(&sbuff[0], 5);
								sbuff[3] = seqId++;
								sbuff[4] = 0xfe;
								WriteInt16(&sbuff[5], 0);
								WriteInt16(&sbuff[7], 2);
								cli->Write(sbuff, 9);
								#if defined(VERBOSE)
								printf("COM_QUERY EOF (Rows)\r\n");
								#endif
							}
							me->dbms->CloseReader(r);
						}
						else
						{
							UTF8Char sbuff[2048];
							UTF8Char *sptr;
							sptr = me->dbms->GetErrMessage(data->connId, &sbuff[7]);
							WriteInt24(sbuff, (sptr - sbuff - 4));
							sbuff[3] = 1;
							sbuff[4] = 0xff;
							WriteInt16(&sbuff[5], 0x416);
							cli->Write(sbuff, (UOSInt)(sptr - sbuff));
							#if defined(VERBOSE)
							printf("COM_QUERY failure\r\n");
							#endif
						}
						sql->Release();
					}
					break;
				}
				i += packetSize + 4;
			}
			else
			{
				break;
			}
		}
		if (i >= data->buffSize)
		{
			data->buffSize = 0;
		}
		else if (i > 0)
		{
			MemCopyO(data->buff, &data->buff[i], data->buffSize - i);
			data->buffSize -= i;
		}
	}
	else
	{
		data->buffSize = 0;
	}
}

void __stdcall Net::MySQLServer::OnClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData)
{
}

void __stdcall Net::MySQLServer::OnClientConn(Socket *s, void *userObj)
{
	Net::MySQLServer *me = (Net::MySQLServer*)userObj;
	UInt8 buff[128];
	UInt8 *bptr;
	OSInt i;
	Net::TCPClient *cli;
	ClientData *data;
	NEW_CLASS(cli, Net::TCPClient(me->sockf, s));
	data = MemAlloc(ClientData, 1);
	data->buff = MemAlloc(UInt8, DEFAULT_BUFF_SIZE + 2048);
	data->buffSize = 0;
	data->mode = 0;
	data->connId = Sync::Interlocked::Increment(&me->connId);
	data->capability = 0xa03ff7ff;
	data->clientCap = 0;
	data->clientCS = 0;
	data->param.clientMaxPacketSize = DEFAULT_BUFF_SIZE;
	data->userName[0] = 0;
	data->userNameLen = 0;
	data->database[0] = 0;
	NEW_CLASS(data->attrMap, Data::StringUTF8Map<const UTF8Char*>());
	Sync::MutexUsage mutUsage(&me->randMut);
	i = 0;
	while (i < 20)
	{
		data->authPluginData[i] = (UInt8)(((UInt32)(me->rand.NextInt32()) % 0x5f) + 0x21);
		i++;
	}
	mutUsage.EndUse();
	me->cliMgr->AddClient(cli, data);

	buff[4] = 10;
	bptr = me->dbms->GetVersion()->ConcatTo(Text::StrConcatC(&buff[5], UTF8STRC(MYSQLVERSION))) + 1;
	WriteInt32(bptr, data->connId);
	MemCopyNO(&bptr[4], data->authPluginData, 8);
	bptr += 12;
	bptr[0] = 0; //filter_1
	WriteInt16(&bptr[1], (data->capability & 0xffff));
	bptr[3] = 45; //character set = 45 (utf8mb4)
	WriteInt16(&bptr[4], 2); //status flags = AUTOCOMMIT
	WriteInt16(&bptr[6], (data->capability >> 16));
	bptr[8] = 21;
	MemClear(&bptr[9], 10);
	bptr += 19;
	MemCopyNO(bptr, &data->authPluginData[8], 12);
	bptr[12] = 0;
	bptr += 13;
	bptr = Text::StrConcatC(bptr, UTF8STRC("mysql_native_password")) + 1;
	WriteInt32(buff, (Int32)(bptr - buff - 4));
	cli->Write(buff, (UOSInt)(bptr - buff));
}


Net::MySQLServer::MySQLServer(Net::SocketFactory *sockf, UInt16 port, DB::DBMS *dbms) : rand((UInt32)(Data::DateTimeUtil::GetCurrTimeMillis() & 0xffffffff))
{
	this->sockf = sockf;
	this->dbms = dbms;
	this->log = dbms->GetLogTool();
	this->connId = 0;

	NEW_CLASS(this->cliMgr, Net::TCPClientMgr(240, OnClientEvent, OnClientData, this, Sync::Thread::GetThreadCnt(), OnClientTimeout));
	NEW_CLASS(this->svr, Net::TCPServer(this->sockf, port, this->log, OnClientConn, this, CSTR("MySQL: ")));
	if (this->svr->IsV4Error())
	{
		DEL_CLASS(this->svr);
		this->svr = 0;
		DEL_CLASS(this->cliMgr);
		this->cliMgr = 0;
	}
}

Net::MySQLServer::~MySQLServer()
{
	if (this->svr)
	{
		DEL_CLASS(this->svr);
		DEL_CLASS(this->cliMgr);
		this->svr = 0;
		this->cliMgr = 0;
	}
	DEL_CLASS(this->dbms);
}

Bool Net::MySQLServer::IsError()
{
	return this->svr == 0;
}
