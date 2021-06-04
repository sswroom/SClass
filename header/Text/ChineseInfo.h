#ifndef _SM_TEXT_CHINESEINFO
#define _SM_TEXT_CHINESEINFO
#include "Data/ArrayList.h"
#include "IO/FileStream.h"

namespace Text
{
	class ChineseInfo
	{
	public:
		typedef enum
		{
			CPF_NONE,
			CPF_B,
			CPF_C,
			CPF_D,
			CPF_F,
			CPF_G,
			CPF_GW,
			CPF_H,
			CPF_J,
			CPF_K,
			CPF_KW,
			CPF_L,
			CPF_M,
			CPF_N,
			CPF_NG,
			CPF_P,
			CPF_S,
			CPF_T,
			CPF_W,
			CPF_Z
		} CantonesePronunFront;

		typedef enum
		{
			CPT_NONE,
			CPT_AA,
			CPT_AAI,
			CPT_AAK,
			CPT_AAM,
			CPT_AAN,
			CPT_AANG,
			CPT_AAP,
			CPT_AAT,
			CPT_AAU,
			CPT_AI,
			CPT_AK,
			CPT_AM,
			CPT_AN,
			CPT_ANG,
			CPT_AP,
			CPT_AT,
			CPT_AU,
			CPT_E,
			CPT_EI,
			CPT_EK,
			CPT_ENG,
			CPT_EOI,
			CPT_EON,
			CPT_EONG,
			CPT_EOT,
			CPT_I,
			CPT_IK,
			CPT_IM,
			CPT_IN,
			CPT_ING,
			CPT_IP,
			CPT_IT,
			CPT_IU,
			CPT_O,
			CPT_OE,
			CPT_OEI,
			CPT_OEK,
			CPT_OENG,
			CPT_OI,
			CPT_OK,
			CPT_ON,
			CPT_ONG,
			CPT_OT,
			CPT_OU,
			CPT_U,
			CPT_UI,
			CPT_UK,
			CPT_UN,
			CPT_UNG,
			CPT_UT,
			CPT_YU,
			CPT_YUK,
			CPT_YUN,
			CPT_YUNG,
			CPT_YUT
		} CantonesePronunTail;

		typedef enum
		{
			CT_UNKNOWN,
			CT_CHINESET,
			CT_CHINESES,
			CT_CHINESETS,
			CT_ENGLISH,
			CT_JAPANESE,
			CT_JAPANESE_KANJI
		} CharType;

		typedef struct
		{
			UInt16 cantonPronun[4];
			UInt32 radical;
			UInt8 strokeCount;
			CharType charType;
			Bool mainChar;
		} CharacterInfo;
	private:
		IO::FileStream *fs;
		UInt8 *currCharBuff;
		UInt32 currCharCode;
		UInt64 fileSize;
	public:
		ChineseInfo();
		~ChineseInfo();

		Bool GetCharInfo(UInt32 charCode, CharacterInfo *chInfo);
		Bool SetCharInfo(UInt32 charCode, CharacterInfo *chInfo);
		Bool GetRelatedChars(UInt32 charCode, Data::ArrayList<UInt32> *relatedChars);
		Bool AddRelation(UInt32 charCode, UInt32 relatedCharCode);

		UTF8Char *AppendCharCode(UTF8Char *buff, UInt32 charCode);
		UInt32 GetCharCode(const UTF8Char *s);
		UInt16 Cantonese2Int(const UTF8Char *s);
		UTF8Char *Int2Cantonese(UTF8Char *buff, UInt16 iVal);
	};
}
#endif
