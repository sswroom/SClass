#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListNative.hpp"
#include "Data/ByteBuffer.h"
#include "Data/Compress/Inflater.h"
#include "Core/ByteTool_C.h"
#include "Crypto/Encrypt/RC4Cipher.h"
#include "Crypto/Hash/MD5.h"
#include "IO/FileStream.h"
#include "IO/StreamData.h"
#include "Media/PDFDocument.h"
#include "Parser/FileParser/PDFParser.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

struct Parser::FileParser::PDFParser::PDFParseEnv
{
	Data::ByteArray buff;
	UIntOS buffSize;
	UIntOS currOfst;
	UIntOS dataSize;
	UInt64 dataOfst;
	UInt64 fileSize;
	NN<IO::StreamData> fd;
	UInt64 lineBegin;
	Bool normalEnd;
	Bool succ;
};

UInt8 Parser::FileParser::PDFParser::HexValue(UTF8Char c)
{
	if (c >= '0' && c <= '9')
		return (UInt8)(c - '0');
	if (c >= 'A' && c <= 'F')
		return (UInt8)(c - 'A' + 10);
	if (c >= 'a' && c <= 'f')
		return (UInt8)(c - 'a' + 10);
	return 255;
}

Bool Parser::FileParser::PDFParser::ParseHex(Text::CStringNN value, UnsafeArray<UInt8> output, UIntOS outputSize)
{
	UIntOS pos = 0;
	while (pos < value.leng && value.v[pos] != '<')
		pos++;
	if (pos == value.leng)
		return false;
	pos++;
	UIntOS outPos = 0;
	Bool hasHigh = false;
	UInt8 high = 0;
	while (pos < value.leng && value.v[pos] != '>')
	{
		UTF8Char c = value.v[pos++];
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			continue;
		UInt8 v = HexValue(c);
		if (v == 255)
			return false;
		if (!hasHigh)
		{
			high = v;
			hasHigh = true;
		}
		else
		{
			if (outPos >= outputSize)
				return false;
			output[outPos++] = (UInt8)((high << 4) | v);
			hasHigh = false;
		}
	}
	if (pos == value.leng || hasHigh || outPos != outputSize)
		return false;
	return true;
}

Bool Parser::FileParser::PDFParser::ParsePDFString(Text::CStringNN value, UnsafeArray<UInt8> output, UIntOS outputSize)
{
	UIntOS pos = 0;
	while (pos < value.leng && (value.v[pos] == ' ' || value.v[pos] == '\t' || value.v[pos] == '\r' || value.v[pos] == '\n'))
		pos++;
	if (pos < value.leng && value.v[pos] == '<')
		return ParseHex(Text::CStringNN(&value.v[pos], value.leng - pos), output, outputSize);
	if (pos >= value.leng || value.v[pos] != '(')
		return false;
	pos++;
	UIntOS outPos = 0;
	UIntOS depth = 1;
	while (pos < value.leng && depth > 0)
	{
		UTF8Char c = value.v[pos++];
		if (c == '\\')
		{
			if (pos >= value.leng)
				return false;
			c = value.v[pos++];
			if (c >= '0' && c <= '7')
			{
				UInt32 octal = (UInt32)(c - '0');
				UIntOS count = 1;
				while (count < 3 && pos < value.leng && value.v[pos] >= '0' && value.v[pos] <= '7')
				{
					octal = (octal << 3) | (UInt32)(value.v[pos++] - '0');
					count++;
				}
				c = (UTF8Char)octal;
			}
			else if (c == 'n')
				c = '\n';
			else if (c == 'r')
				c = '\r';
			else if (c == 't')
				c = '\t';
			else if (c == 'b')
				c = '\b';
			else if (c == 'f')
				c = '\f';
			else if (c == '\r')
			{
				if (pos < value.leng && value.v[pos] == '\n')
					pos++;
				continue;
			}
			else if (c == '\n')
				continue;
		}
		else if (c == '(')
		{
			depth++;
		}
		else if (c == ')')
		{
			depth--;
			if (depth == 0)
				break;
		}
		if (outPos >= outputSize)
			return false;
		output[outPos++] = (UInt8)c;
	}
	return depth == 0 && outPos == outputSize;
}

Bool Parser::FileParser::PDFParser::DeriveStandardRC4Key(NN<Media::PDFParameter> encryption, Text::CStringNN fileID, UnsafeArray<UInt8> fileKey, UIntOS &keyLen)
{
	static const UInt8 passwordPadding[] =
	{
		0x28,0xBF,0x4E,0x5E,0x4E,0x75,0x8A,0x41,0x64,0x00,0x4E,0x56,0xFF,0xFA,0x01,0x08,
		0x2E,0x2E,0x00,0xB6,0xD0,0x68,0x3E,0x80,0x2F,0x0C,0xA9,0xFE,0x64,0x53,0x69,0x7A
	};
	NN<Text::String> s;
	if (!encryption->GetEntryValue(CSTR("Filter")).SetTo(s) || !s->Equals(UTF8STRC("/Standard")))
		return false;
	if (!encryption->GetEntryValue(CSTR("R")).SetTo(s))
		return false;
	Int32 revision = s->ToInt32();
	if (!encryption->GetEntryValue(CSTR("V")).SetTo(s))
		return false;
	Int32 version = s->ToInt32();
	if (revision != 2 && revision != 3)
		return false;
	if (version != 1 && version != 2)
		return false;
	UIntOS bits = 40;
	if (encryption->GetEntryValue(CSTR("Length")).SetTo(s))
		bits = s->ToUIntOS();
	if (bits < 40 || bits > 128 || (bits & 7) != 0)
		return false;
	keyLen = bits >> 3;

	UInt8 ownerKey[32];
	UInt8 userValue[32];
	if (!encryption->GetEntryValue(CSTR("O")).SetTo(s) || !ParsePDFString(s->ToCString(), UnsafeArray<UInt8>(ownerKey), sizeof(ownerKey)))
		return false;
	if (!encryption->GetEntryValue(CSTR("U")).SetTo(s) || !ParsePDFString(s->ToCString(), UnsafeArray<UInt8>(userValue), sizeof(userValue)))
		return false;
	if (!encryption->GetEntryValue(CSTR("P")).SetTo(s))
		return false;
	UInt32 permissions = (UInt32)s->ToInt32();

	UInt8 keyInput[32 + 32 + 4 + 128];
	MemCopyNO(keyInput, passwordPadding, sizeof(passwordPadding));
	MemCopyNO(&keyInput[32], ownerKey, sizeof(ownerKey));
	WriteLUInt32(&keyInput[64], permissions);
	UIntOS fileIDLen = 0;
	UIntOS i = 0;
	Bool hasHigh = false;
	UInt8 high = 0;
	while (i < fileID.leng && fileID.v[i] != '<')
		i++;
	if (i == fileID.leng)
		return false;
	i++;
	while (i < fileID.leng && fileID.v[i] != '>')
	{
		UTF8Char c = fileID.v[i++];
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			continue;
		UInt8 v = HexValue(c);
		if (v == 255)
			return false;
		if (!hasHigh)
		{
			high = v;
			hasHigh = true;
		}
		else
		{
			if (fileIDLen >= 128)
				return false;
			keyInput[68 + fileIDLen++] = (UInt8)((high << 4) | v);
			hasHigh = false;
		}
	}
	if (i == fileID.leng || hasHigh)
		return false;
	UIntOS keyInputSize = 68 + fileIDLen;
	UInt8 digest[16];
	Crypto::Hash::MD5 md5;
	md5.Calc(keyInput, keyInputSize);
	md5.GetValue(digest);
	if (revision >= 3)
	{
		UIntOS round = 0;
		while (round < 50)
		{
			md5.Clear();
			md5.Calc(digest, keyLen);
			md5.GetValue(digest);
			round++;
		}
	}
	MemCopyNO(fileKey.Ptr(), digest, keyLen);

	UInt8 validationInput[32 + 128];
	UInt8 validation[32];
	UIntOS validationSize;
	if (revision == 2)
	{
		MemCopyNO(validation, passwordPadding, sizeof(passwordPadding));
		validationSize = sizeof(passwordPadding);
	}
	else
	{
		MemCopyNO(validationInput, passwordPadding, sizeof(passwordPadding));
		MemCopyNO(&validationInput[32], &keyInput[68], fileIDLen);
		md5.Clear();
		md5.Calc(validationInput, sizeof(passwordPadding) + fileIDLen);
		md5.GetValue(digest);
		MemCopyNO(validation, digest, sizeof(digest));
		validationSize = sizeof(digest);
	}
	Crypto::Encrypt::RC4Cipher cipher(UnsafeArray<const UInt8>(fileKey), keyLen);
	cipher.Encrypt(UnsafeArray<const UInt8>(validation), validationSize, UnsafeArray<UInt8>(validation));
	if (revision >= 3)
	{
		UInt8 roundKey[16];
		UIntOS round = 1;
		while (round <= 19)
		{
			i = 0;
			while (i < keyLen)
			{
				roundKey[i] = (UInt8)(fileKey[i] ^ (UInt8)round);
				i++;
			}
			Crypto::Encrypt::RC4Cipher roundCipher(UnsafeArray<const UInt8>(roundKey), keyLen);
			roundCipher.Encrypt(UnsafeArray<const UInt8>(validation), validationSize, UnsafeArray<UInt8>(validation));
			round++;
		}
	}
	i = 0;
	UIntOS compareSize = revision >= 3 ? 16 : 32;
	while (i < compareSize)
	{
		if (validation[i] != userValue[i])
			return false;
		i++;
	}
	return true;
}

void Parser::FileParser::PDFParser::SetupEncryption(NN<Media::PDFDocument> doc, NN<PDFXRef> xref)
{
	NN<Media::PDFParameter> trailer;
	NN<Text::String> encryptRef;
	if (!xref->trailer.SetTo(trailer) || !trailer->GetEntryValue(CSTR("Encrypt")).SetTo(encryptRef))
		return;
	doc->SetEncryptionRequired();
	UInt32 encryptId = 0;
	UIntOS pos = 0;
	while (pos < encryptRef->leng && (encryptRef->v[pos] == ' ' || encryptRef->v[pos] == '\t'))
		pos++;
	while (pos < encryptRef->leng && encryptRef->v[pos] >= '0' && encryptRef->v[pos] <= '9')
	{
		UInt32 digit = (UInt32)(encryptRef->v[pos++] - '0');
		if (encryptId > (0xffffffffU - digit) / 10)
			return;
		encryptId = encryptId * 10 + digit;
	}
	if (encryptId == 0)
		return;
	NN<Media::PDFObject> encryptObj;
	NN<Media::PDFParameter> encryptParams;
	NN<Text::String> fileID;
	NN<Text::String> idValue;
	if (!doc->GetObjectById(encryptId).SetTo(encryptObj) ||
		!encryptObj->GetParameter().SetTo(encryptParams) ||
		!trailer->GetEntryValue(CSTR("ID")).SetTo(idValue))
		return;
	fileID = idValue;
	UInt8 key[16];
	UIntOS keyLen;
	if (DeriveStandardRC4Key(encryptParams, fileID->ToCString(), UnsafeArray<UInt8>(key), keyLen))
		doc->SetEncryptionKey(UnsafeArray<const UInt8>(key), keyLen);
}

Bool Parser::FileParser::PDFParser::IsComment(UnsafeArray<const UTF8Char> buff, UIntOS size)
{
	if (buff[0] == '%')
	{
		if (Text::StrStartsWithC(buff, size, UTF8STRC("%%EOF")))
			return false;
		if (Text::StrStartsWithC(buff, size, UTF8STRC("%PDF-")))
			return false;
		return true;
	}
	return false;
}

Bool Parser::FileParser::PDFParser::NextLine(NN<PDFParseEnv> env, NN<Text::StringBuilderUTF8> sb, Bool skipComment)
{
	UIntOS i = env->currOfst;
	while (i < env->dataSize)
	{
		if (env->buff[i] == '\n' || env->buff[i] == '\r')
		{
			if (skipComment && IsComment(&env->buff[env->currOfst], i - env->currOfst))
			{
				env->lineBegin = env->dataOfst + env->currOfst;
				env->currOfst = i + 1;
				if (env->buff[env->currOfst] == '\n' && env->buff[env->currOfst - 1] == '\r')
				{
					env->currOfst++;
				}
				return NextLine(env, sb, true);
			}
			sb->AppendC(&env->buff[env->currOfst], i - env->currOfst);
			sb->RTrim();
			env->lineBegin = env->dataOfst + env->currOfst;
			env->currOfst = i + 1;
			if (env->buff[env->currOfst] == '\n' && env->buff[env->currOfst - 1] == '\r')
			{
				env->currOfst++;
			}
			if (sb->EndsWith('\\'))
			{
				sb->RemoveChars(1);
				return NextLine(env, sb, true);
			}
			return true;
		}
		i++;
	}
	if (env->dataSize - env->currOfst > 1024)
	{
		return false;
	}
	env->dataOfst += env->currOfst;
	env->buff.CopyInner(0, env->currOfst, env->dataSize - env->currOfst);
	env->dataSize -= env->currOfst;
	env->currOfst = 0;
	i = env->dataSize;
	env->dataSize += env->fd->GetRealData(env->dataOfst + env->dataSize, env->buffSize - env->dataSize, env->buff.SubArray(env->dataSize));
	if (i == env->dataSize)
	{
		if (env->dataSize > 0 && env->fileSize == env->dataOfst + env->dataSize)
		{
			env->currOfst = env->dataSize;
			env->lineBegin = env->dataOfst;
			sb->AppendC(env->buff.Arr(), env->dataSize);
			sb->RTrim();
			return true;
		}
		return false;
	}
	while (i < env->dataSize)
	{
		if (env->buff[i] == '\n' || env->buff[i] == '\r')
		{
			if (skipComment && IsComment(&env->buff[env->currOfst], i - env->currOfst))
			{
				env->lineBegin = env->dataOfst + env->currOfst;
				env->currOfst = i + 1;
				if (env->buff[env->currOfst] == '\n' && env->buff[env->currOfst - 1] == '\r')
				{
					env->currOfst++;
				}
				return NextLine(env, sb, true);
			}
			sb->AppendC(&env->buff[env->currOfst], i - env->currOfst);
			sb->RTrim();
			env->lineBegin = env->dataOfst + env->currOfst;
			env->currOfst = i + 1;
			if (env->buff[env->currOfst] == '\n' && env->buff[env->currOfst - 1] == '\r')
			{
				env->currOfst++;
			}
			if (sb->EndsWith('\\'))
			{
				sb->RemoveChars(1);
				return NextLine(env, sb, true);
			}
			return true;
		}
		i++;
	}
	return false;
}

Bool Parser::FileParser::PDFParser::NextLineFixed(NN<PDFParseEnv> env, UIntOS size)
{
	if (env->currOfst + size < env->dataSize)
	{
		if (env->buff[env->currOfst + size] == '\n')
		{
			env->currOfst = env->currOfst + size + 1;
			return true;
		}
		else if (env->buff[env->currOfst + size] == '\r')
		{
			if (env->buff[env->currOfst + size + 1] == '\n')
				env->currOfst = env->currOfst + size + 2;
			else
				env->currOfst = env->currOfst + size + 1;
			return true;
		}
		else if (Text::StrStartsWithC(&env->buff[env->currOfst + size], env->dataSize - env->currOfst + size, UTF8STRC("end")))
		{
			env->currOfst = env->currOfst + size;
			return true;
		}
		return false;
	}
	else
	{
		env->dataOfst += env->currOfst + size;
		env->dataSize = env->fd->GetRealData(env->dataOfst, env->dataSize, env->buff);
		env->currOfst = 0;
		if (env->buff[0] == '\n')
		{
			env->currOfst++;
			return true;
		}
		else if (env->buff[0] == '\r')
		{
			if (env->buff[1] == '\n')
				env->currOfst = 2;
			else
				env->currOfst = 1;
			return true;
		}
		else if (Text::StrStartsWithC(&env->buff[0], env->dataSize, UTF8STRC("end")))
		{
			env->currOfst = 0;
			return true;
		}
		return false;
	}
}

void Parser::FileParser::PDFParser::ParseStartxref(NN<PDFParseEnv> env, NN<Text::StringBuilderUTF8> sb)
{
	if (!sb->Equals(UTF8STRC("startxref")) && !sb->StartsWith(UTF8STRC("startxref ")))
	{
#if defined(VERBOSE)
		printf("PDFParser: startxref not present after trailer: %s\r\n", sb->v.Ptr());
#endif
		env->normalEnd = true;
		return;
	}
	UInt64 ofst;
	if (sb->Equals(UTF8STRC("startxref")))
	{
		sb->ClearStr();
		if (!NextLine(env, sb, true))
		{
#if defined(VERBOSE)
			printf("PDFParser: startxref offset not found\r\n");
#endif
			env->normalEnd = true;
			return;
		}
	}
	else
	{
		if (!sb->StartsWith(UTF8STRC("startxref")))
		{
#if defined(VERBOSE)
			printf("PDFParser: startxref not present after trailer: %s\r\n", sb->v.Ptr());
#endif
			env->normalEnd = true;
			return;
		}
		sb->SetSubstr(9);
		sb->Trim();
	}
	if (sb->ToUInt64(ofst))
	{
#if defined(VERBOSE)
		printf("PDFParser: startxref value = %lld\r\n", ofst);
#endif
	}
	else
	{
#if defined(VERBOSE)
		printf("PDFParser: startxref offset not valid: %s\r\n", sb->v.Ptr());
#endif
		env->normalEnd = true;
		return;
	}
	sb->ClearStr();
	if (!NextLine(env, sb, true))
	{
#if defined(VERBOSE)
		printf("PDFParser: unexpected end of file after startxref\r\n");
#endif
		env->normalEnd = true;
		return;
	}
	if (sb->Equals(UTF8STRC("%%EOF")))
	{
#if defined(VERBOSE)
		printf("PDFParser: EOF found, ofst = %lld\r\n", env->lineBegin);
#endif
		env->succ = true;
		UInt64 ofst = env->dataOfst + env->currOfst;
		if (ofst <= env->fileSize)
		{
			UInt8 buff[1024];
			Bool onlyWhitespace = true;
			while (ofst < env->fileSize && onlyWhitespace)
			{
				UIntOS readSize = (UIntOS)((env->fileSize - ofst > sizeof(buff)) ? sizeof(buff) : env->fileSize - ofst);
				if (env->fd->GetRealData(ofst, readSize, BYTEARR(buff)) != readSize)
					break;
				UIntOS i = 0;
				while (i < readSize)
				{
					if (buff[i] != 0 && buff[i] != 9 && buff[i] != 10 && buff[i] != 12 && buff[i] != 13 && buff[i] != 32)
					{
						onlyWhitespace = false;
						break;
					}
					i++;
				}
				ofst += readSize;
			}
			if (onlyWhitespace && ofst == env->fileSize)
			{
				env->normalEnd = true;
			}
		}
	}
	else if (sb->StartsWith(UTF8STRC("%%EOF")))
	{
#if defined(VERBOSE)
		printf("PDFParser: EOF found no EOL, ofst = %lld\r\n", env->lineBegin);
#endif
		env->currOfst = (UIntOS)(env->lineBegin + 5 - env->dataOfst);
	}
	else
	{
#if defined(VERBOSE)
		printf("PDFParser: Unknown entry after startxref: %s\r\n", sb->v.Ptr());
#endif
		env->normalEnd = true;
	}
}

Bool Parser::FileParser::PDFParser::ParseObject(NN<PDFParseEnv> env, NN<Text::StringBuilderUTF8> sb, NN<Media::PDFDocument> doc, Optional<PDFXRef> xref)
{
	UInt32 id;
	sb->RemoveChars(6);
	if (!sb->ToUInt32(id))
	{
		if (sb->Equals(UTF8STRC("WebFastLoad2")))
		{
			sb->ClearStr();
			if (NextLine(env, sb, true))
			{
				sb->ClearStr();
				if (NextLine(env, sb, true) && sb->Equals(UTF8STRC("endobj")))
					return true;
			}
		}
#if defined(VERBOSE)
		printf("PDFParser: Error in parsing object id: %s\r\n", sb->v.Ptr());
#endif
		env->normalEnd = true;
		return false;
	}
#if defined(VERBOSE)
	printf("PDFParser: Object found, id = %d, ofst = %lld\r\n", id, env->lineBegin);
#endif
	if (env->lineBegin == 651863)
	{
#if defined(VERBOSE)
		printf("PDFParser: debug start\r\n");
#endif

	}
	sb->ClearStr();
	NN<Media::PDFParameter> param;
	UInt64 dataOfst = 0;
	NN<Media::PDFObject> obj = doc->AddObject(id);
	while (NextLine(env, sb, true))
	{
		if (sb->Equals(UTF8STRC("endobj")))
		{
			if (dataOfst != 0)
			{
				obj->SetData(env->fd, dataOfst, (UIntOS)(env->lineBegin - dataOfst));
			}
			return true;
		}
		else if (sb->EndsWith(UTF8STRC("endobj")) && !sb->EndsWith(UTF8STRC(">>endobj")))
		{
			if (dataOfst != 0)
			{
				obj->SetData(env->fd, dataOfst, (UIntOS)(env->lineBegin + sb->leng - 6 - dataOfst));
			}
			return true;
		}
		else if (sb->Equals(UTF8STRC("stream")))
		{
			if (!ParseObjectStream(env, sb, obj, xref))
				break;
			if (sb->EndsWith(UTF8STRC("endstream endobj")))
				return true;
		}
		else if (sb->StartsWith(UTF8STRC("<<")))
		{
			Bool notBreak = false;
			while (true)
			{
				if (sb->EndsWith(UTF8STRC(">>stream")))
				{
					if (!Media::PDFParameter::Parse(Text::CStringNN(&sb->v[2], sb->leng - 10)).SetTo(param))
					{
#if defined(VERBOSE)
						printf("PDFParser: Error in parsing object stream parameter: %s\r\n", sb->v.Ptr());
#endif
						env->normalEnd = true;
						break;
					}
					obj->SetParameter(param);
					notBreak = ParseObjectStream(env, sb, obj, xref);
					if (notBreak && sb->EndsWith(UTF8STRC("endstream endobj")))
						return true;
					break;
				}
				else if (sb->EndsWith(UTF8STRC(">>endobj")))
				{
					if (!Media::PDFParameter::Parse(Text::CStringNN(&sb->v[2], sb->leng - 8)).SetTo(param))
					{
	#if defined(VERBOSE)
						printf("PDFParser: Error in parsing object parameter: %s\r\n", sb->v.Ptr());
	#endif
						env->normalEnd = true;
						break;
					}
					obj->SetParameter(param);
					return true;
				}
				else
				{
					sb->RTrim();
					UIntOS startCnt = sb->CountStr(CSTR("<<"));
					UIntOS endCnt = sb->CountStr(UTF8STRC(">>"));
					if (startCnt > endCnt)
					{
						sb->AppendUTF8Char(' ');
						if (!NextLine(env, sb, true))
						{
							break;
						}
					}
 					else if (sb->EndsWith(UTF8STRC(">>")))
					{
						if (!Media::PDFParameter::Parse(Text::CStringNN(&sb->v[2], sb->leng - 4)).SetTo(param))
						{
#if defined(VERBOSE)
							printf("PDFParser: Error in parsing object parameter: %s\r\n", sb->v.Ptr());
#endif
							env->normalEnd = true;
							break;
						}
						obj->SetParameter(param);
						notBreak = true;
						break;
					}
					else
					{
#if defined(VERBOSE)
						printf("PDFParser: Unknown object content: %s\r\n", sb->v.Ptr());
#endif
						env->normalEnd = true;
						break;
					}
				}
			}
			if (!notBreak)
				break;
		}
		else
		{
			if (dataOfst == 0)
				dataOfst = env->lineBegin;
		}
		sb->ClearStr();
	}
	return false;
}

Bool Parser::FileParser::PDFParser::ParseObjectStream(NN<PDFParseEnv> env, NN<Text::StringBuilderUTF8> sb, NN<Media::PDFObject> obj, Optional<PDFXRef> xref)
{
	NN<PDFXRef> nnxref;
	NN<Media::PDFParameter> param;
	if (!obj->GetParameter().SetTo(param))
	{
#if defined(VERBOSE)
		printf("PDFParser: Object parameter not found for stream data\r\n");
#endif
		env->normalEnd = true;
		return false;
	}
	NN<Text::String> leng;
	if (!param->GetEntryValue(CSTR("Length")).SetTo(leng))
	{
#if defined(VERBOSE)
		printf("PDFParser: Length not found in stream filters: %s\r\n", sb->v.Ptr());
#endif
		env->normalEnd = true;
		return false;
	}
	UIntOS iLeng;
	if (leng->EndsWith(UTF8STRC(" 0 R")))
	{
		UInt8 buff[128];
		if (leng->leng > 35)
		{
#if defined(VERBOSE)
			printf("PDFParser: Stream Length Filter is too long: %s\r\n", leng->v.Ptr());
#endif
			env->normalEnd = true;
			return false;
		}
		UInt32 id;
		Text::StrConcatC(buff, leng->v, leng->leng - 4);
		if (!Text::StrToUInt32(buff, id))
		{
#if defined(VERBOSE)
			printf("PDFParser: Stream Length Filter cannot parse id: %s\r\n", leng->v.Ptr());
#endif
			env->normalEnd = true;
			return false;
		}
		UInt64 dataOfst = 0;
		while (true)
		{
			if (!xref.SetTo(nnxref))
			{
#if defined(VERBOSE)
				printf("PDFParser: Stream Length Filter xref entry not found: %d\r\n", id);
#endif
				env->normalEnd = true;
				return false;
			}
			else if (id >= nnxref->startId && id < nnxref->startId + nnxref->count)
			{
				if (nnxref->items[id - nnxref->startId].type != 'n')
				{
#if defined(VERBOSE)
					printf("PDFParser: Stream Length Filter xref item type not valid: %c\r\n", nnxref->items[id - nnxref->startId].type);
#endif
					env->normalEnd = true;
					return false;
				}
				dataOfst = nnxref->items[id - nnxref->startId].ofst;
				break;
			}
			else
			{
				xref = nnxref->nextRef;
			}
		}

		PDFParseEnv innerEnv;
		innerEnv.buff = BYTEARR(buff);
		innerEnv.buffSize = sizeof(buff);
		innerEnv.dataOfst = dataOfst;
		innerEnv.currOfst = 0;
		innerEnv.dataSize = 0;
		innerEnv.fd = env->fd;
		innerEnv.fileSize = env->fileSize;
		innerEnv.lineBegin = 0;
		innerEnv.normalEnd = false;
		innerEnv.succ = false;
		sb->ClearStr();
		if (!NextLine(innerEnv, sb, true))
		{
#if defined(VERBOSE)
			printf("PDFParser: Stream Length reference object cannot read\r\n");
#endif
			env->normalEnd = true;
			return false;
		}
		if (!sb->EndsWith(UTF8STRC(" 0 obj")))
		{
#if defined(VERBOSE)
			printf("PDFParser: Stream Length reference object not valid: %s\r\n", sb->v.Ptr());
#endif
			env->normalEnd = true;
			return false;
		}
		sb->ClearStr();
		if (!NextLine(innerEnv, sb, true))
		{
#if defined(VERBOSE)
			printf("PDFParser: Stream Length reference object cannot read content\r\n");
#endif
			env->normalEnd = true;
			return false;
		}
		if (!sb->ToUIntOS(iLeng))
		{
#if defined(VERBOSE)
			printf("PDFParser: Stream Length reference object content not valid: %s\r\n", sb->v.Ptr());
#endif
			env->normalEnd = true;
			return false;
		}
		sb->ClearStr();
		if (!NextLine(innerEnv, sb, true))
		{
#if defined(VERBOSE)
			printf("PDFParser: Stream Length reference object cannot read content2\r\n");
#endif
			env->normalEnd = true;
			return false;
		}
		if (!sb->Equals(UTF8STRC("endobj")))
		{
#if defined(VERBOSE)
			printf("PDFParser: Stream Length reference object endobj not found: %s\r\n", sb->v.Ptr());
#endif
			env->normalEnd = true;
			return false;
		}
	}
	else
	{
		if (!leng->ToUIntOS(iLeng))
		{
#if defined(VERBOSE)
			printf("PDFParser: Stream Length Filter is not valid: %s\r\n", leng->v.Ptr());
#endif
			env->normalEnd = true;
			return false;
		}
	}
	obj->SetStream(env->fd, env->dataOfst + env->currOfst, iLeng);
	if (!NextLineFixed(env, iLeng))
	{
#if defined(VERBOSE)
		printf("PDFParser: Stream is not ended by LF\r\n");
#endif
		env->normalEnd = true;
		return false;
	}
	sb->ClearStr();
	if (!NextLine(env, sb, true))
	{
		return false;
	}
	if (!sb->Equals(UTF8STRC("endstream")))
	{
		if (sb->EndsWith(UTF8STRC("endstream endobj")))
			return true;
#if defined(VERBOSE)
		printf("PDFParser: End of stream not found: %s\r\n", sb->v.Ptr());
#endif
		env->normalEnd = true;
		return false;
	}
	return true;
}

static Bool PDFParseUIntArray(NN<Text::String> value, Data::ArrayListNative<UIntOS> &values)
{
	if (value->leng < 2 || value->v[0] != '[' || value->v[value->leng - 1] != ']')
		return false;
	UIntOS i = 1;
	while (i + 1 < value->leng)
	{
		while (i + 1 < value->leng && (value->v[i] == ' ' || value->v[i] == '\t' || value->v[i] == '\r' || value->v[i] == '\n'))
			i++;
		if (i + 1 == value->leng)
			break;
		if (value->v[i] < '0' || value->v[i] > '9')
			return false;
		UIntOS number = 0;
		while (i + 1 < value->leng && value->v[i] >= '0' && value->v[i] <= '9')
		{
			UIntOS digit = (UIntOS)(value->v[i] - '0');
			if (number > (INVALID_INDEX - digit) / 10)
				return false;
			number = number * 10 + digit;
			i++;
		}
		values.Add(number);
		if (i + 1 < value->leng && value->v[i] != ' ' && value->v[i] != '\t' && value->v[i] != '\r' && value->v[i] != '\n' && value->v[i] != ']')
			return false;
	}
	return true;
}

Optional<Parser::FileParser::PDFParser::PDFXRef> Parser::FileParser::PDFParser::ParseXRef(NN<PDFParseEnv> env, NN<Text::StringBuilderUTF8> sb)
{
	if (!NextLine(env, sb, true))
	{
#if defined(VERBOSE)
		printf("PDFParser: Error in reading xref\r\n");
#endif
		return nullptr;
	}
	if (!sb->Equals(UTF8STRC("xref")))
	{
		if (!sb->EndsWith(UTF8STRC(" obj")))
		{
#if defined(VERBOSE)
			printf("PDFParser: xref stream object not found, ofst = %lld\r\n", env->lineBegin);
#endif
			env->normalEnd = true;
			return nullptr;
		}
		sb->ClearStr();
		if (!NextLine(env, sb, true) || !sb->StartsWith(UTF8STRC("<<")))
		{
			env->normalEnd = true;
			return nullptr;
		}
		Bool inlineStream = false;
		while (true)
		{
			if (sb->EndsWith(UTF8STRC(">>stream")))
			{
				sb->TrimToLength(sb->leng - 6);
				inlineStream = true;
			}
			UIntOS startCnt = sb->CountStr(UTF8STRC("<<"));
			UIntOS endCnt = sb->CountStr(UTF8STRC(">>"));
			if (startCnt <= endCnt)
				break;
			sb->AppendUTF8Char(' ');
			if (!NextLine(env, sb, true))
			{
				env->normalEnd = true;
				return nullptr;
			}
		}
		if (!sb->EndsWith(UTF8STRC(">>")))
		{
			env->normalEnd = true;
			return nullptr;
		}
		sb->RemoveChars(2);
		sb->SetSubstr(2);
		NN<Media::PDFParameter> trailer;
		Optional<Media::PDFParameter> optTrailer = Media::PDFParameter::Parse(sb->ToCString());
		if (!optTrailer.SetTo(trailer))
		{
			env->normalEnd = true;
			return nullptr;
		}
		Optional<Media::PDFParameter> decodeParameters = nullptr;
#define PARSE_ERROR \
			decodeParameters.Delete(); \
			trailer.Delete(); \
			env->normalEnd = true; \
			return nullptr;

		NN<Text::String> widthValue;
		NN<Text::String> sizeValue;
		Data::ArrayListNative<UIntOS> widths;
		if (!trailer->GetEntryValue(CSTR("W")).SetTo(widthValue) || !PDFParseUIntArray(widthValue, widths) || widths.GetCount() != 3 ||
			!trailer->GetEntryValue(CSTR("Size")).SetTo(sizeValue))
		{
			PARSE_ERROR
		}
		UIntOS size = sizeValue->ToUIntOS();
		if (size == 0 || widths.GetItem(0) > 8 || widths.GetItem(1) > 8 || widths.GetItem(2) > 8 ||
			widths.GetItem(0) + widths.GetItem(1) + widths.GetItem(2) == 0)
		{
			PARSE_ERROR
		}
		UIntOS rowSize = widths.GetItem(0) + widths.GetItem(1) + widths.GetItem(2);
		UIntOS predictor = 1;
		UIntOS colors = 1;
		UIntOS columns = rowSize;
		UIntOS bitsPerComponent = 8;
		NN<Text::String> decodeParmsValue;
		NN<Media::PDFParameter> nnDecodeParameters;
		if (trailer->GetEntryValue(CSTR("DecodeParms")).SetTo(decodeParmsValue))
		{
			if (decodeParmsValue->leng < 4 || !decodeParmsValue->StartsWith(UTF8STRC("<<")) || !decodeParmsValue->EndsWith(UTF8STRC(">>")) ||
				!Media::PDFParameter::Parse(Text::CStringNN(&decodeParmsValue->v[2], decodeParmsValue->leng - 4)).SetTo(nnDecodeParameters))
			{
				PARSE_ERROR
			}
			decodeParameters = nnDecodeParameters;
			NN<Text::String> parameterValue;
			if (nnDecodeParameters->GetEntryValue(CSTR("Predictor")).SetTo(parameterValue))
				predictor = parameterValue->ToUIntOS();
			if (nnDecodeParameters->GetEntryValue(CSTR("Colors")).SetTo(parameterValue))
				colors = parameterValue->ToUIntOS();
			if (nnDecodeParameters->GetEntryValue(CSTR("Columns")).SetTo(parameterValue))
				columns = parameterValue->ToUIntOS();
			if (nnDecodeParameters->GetEntryValue(CSTR("BitsPerComponent")).SetTo(parameterValue))
				bitsPerComponent = parameterValue->ToUIntOS();
		}
		if (colors == 0 || columns == 0 || bitsPerComponent == 0 || bitsPerComponent > 16 ||
			(UInt64)colors * columns * bitsPerComponent > (UInt64)INVALID_INDEX * 8 ||
			((UIntOS)(((UInt64)colors * columns * bitsPerComponent + 7) / 8) != rowSize))
		{
			PARSE_ERROR
		}
		Bool hasPNGFilter = predictor >= 10 && predictor <= 15;
		if (predictor != 1 && predictor != 2 && !hasPNGFilter)
		{
			PARSE_ERROR
		}
		if (predictor == 2 && bitsPerComponent != 8)
		{
			PARSE_ERROR
		}
		UIntOS bytesPerPixel = (colors * bitsPerComponent + 7) / 8;
		if (bytesPerPixel == 0)
			bytesPerPixel = 1;
		Data::ArrayListNative<UIntOS> indexes;
		NN<Text::String> indexValue;
		if (trailer->GetEntryValue(CSTR("Index")).SetTo(indexValue))
		{
			if (!PDFParseUIntArray(indexValue, indexes) || indexes.GetCount() == 0 || (indexes.GetCount() & 1) != 0)
			{
				PARSE_ERROR
			}
		}
		else
		{
			indexes.Add(0);
			indexes.Add(size);
		}
		UIntOS rowCount = 0;
		UIntOS rangeIndex = 0;
		while (rangeIndex < indexes.GetCount())
		{
			UIntOS startId = indexes.GetItem(rangeIndex++);
			UIntOS count = indexes.GetItem(rangeIndex++);
			if (startId > size || count > size - startId || count > INVALID_INDEX - rowCount)
			{
				PARSE_ERROR
			}
			rowCount += count;
		}
		if (rowCount > INVALID_INDEX / rowSize)
		{
			PARSE_ERROR
		}
		UIntOS decodedSize = rowCount * rowSize;
		if (hasPNGFilter && rowCount > INVALID_INDEX / (rowSize + 1))
		{
			PARSE_ERROR
		}
		UIntOS encodedSize = rowCount * (rowSize + (hasPNGFilter ? 1 : 0));

		NN<Text::String> lengthValue;
		UIntOS streamLength;
		sb->ClearStr();
		if (!trailer->GetEntryValue(CSTR("Length")).SetTo(lengthValue) || !lengthValue->ToUIntOS(streamLength))
		{
			PARSE_ERROR
		}
		if (!inlineStream && (!NextLine(env, sb, true) || !sb->Equals(UTF8STRC("stream"))))
		{
			PARSE_ERROR
		}
		UInt64 streamOfst = env->dataOfst + env->currOfst;
		if (streamOfst > env->fileSize || streamLength > env->fileSize - streamOfst)
		{
			PARSE_ERROR
		}
		Data::ByteBuffer streamData(streamLength);
		if (env->fd->GetRealData(streamOfst, streamLength, streamData) != streamLength)
		{
			PARSE_ERROR
		}
		NN<Text::String> filterValue;
		Bool hasFilter = trailer->GetEntryValue(CSTR("Filter")).SetTo(filterValue);
		if (hasFilter && !filterValue->Equals(UTF8STRC("/FlateDecode")))
		{
			PARSE_ERROR
		}
		Data::ByteBuffer encodedData(encodedSize);
		UIntOS actualDecodedSize = encodedSize;
		if (hasFilter)
		{
			if (!Data::Compress::Inflater::DecompressDirect(encodedData, actualDecodedSize, streamData.AsByteArray(), true))
			{
				PARSE_ERROR
			}
		}
		else
		{
			if (streamLength != encodedSize)
			{
				PARSE_ERROR
			}
			encodedData.CopyFrom(streamData);
		}
		if (actualDecodedSize != encodedSize)
		{
			PARSE_ERROR
		}
		Data::ByteBuffer decodedData(decodedSize);
		if (hasPNGFilter)
		{
			UIntOS encodedIndex = 0;
			UIntOS decodedIndex = 0;
			UIntOS row = 0;
			while (row < rowCount)
			{
				UInt8 filter = encodedData[encodedIndex++];
				if (filter > 4)
				{
					PARSE_ERROR
				}
				UIntOS column = 0;
				while (column < rowSize)
				{
					UInt8 value = encodedData[encodedIndex++];
					UInt8 left = column >= bytesPerPixel ? decodedData[decodedIndex - bytesPerPixel] : 0;
					UInt8 up = row > 0 ? decodedData[decodedIndex - rowSize] : 0;
					UInt8 upperLeft = row > 0 && column >= bytesPerPixel ? decodedData[decodedIndex - rowSize - bytesPerPixel] : 0;
					if (filter == 1)
					{
						value = (UInt8)(value + left);
					}
					else if (filter == 2)
					{
						value = (UInt8)(value + up);
					}
					else if (filter == 3)
					{
						value = (UInt8)(value + ((UInt16)left + up) / 2);
					}
					else if (filter == 4)
					{
						IntOS p = (IntOS)left + up - upperLeft;
						IntOS pa = p - left;
						if (pa < 0)
							pa = -pa;
						IntOS pb = p - up;
						if (pb < 0)
							pb = -pb;
						IntOS pc = p - upperLeft;
						if (pc < 0)
							pc = -pc;
						if (pa <= pb && pa <= pc)
							value = (UInt8)(value + left);
						else if (pb <= pc)
							value = (UInt8)(value + up);
						else
							value = (UInt8)(value + upperLeft);
					}
					decodedData[decodedIndex++] = value;
					column++;
				}
				row++;
			}
		}
		else
		{
			decodedData.CopyFrom(encodedData);
			if (predictor == 2)
			{
				UIntOS row = 0;
				while (row < rowCount)
				{
					UIntOS column = bytesPerPixel;
					while (column < rowSize)
					{
						decodedData[row * rowSize + column] = (UInt8)(decodedData[row * rowSize + column] + decodedData[row * rowSize + column - bytesPerPixel]);
						column++;
					}
					row++;
				}
			}
		}
		decodeParameters.Delete();

		PDFXRef *thisRef = MemAlloc(PDFXRef, 1);
		thisRef->startId = 0;
		thisRef->count = size;
		thisRef->items = MemAlloc(PDFXRefItem, size);
		thisRef->nextRef = nullptr;
		thisRef->trailer = trailer;
		for (UIntOS i = 0; i < size; i++)
		{
			thisRef->items[i].ofst = 0;
			thisRef->items[i].generation = 0;
			thisRef->items[i].type = 'f';
		}
		UIntOS dataIndex = 0;
		rangeIndex = 0;
		while (rangeIndex < indexes.GetCount())
		{
			UIntOS startId = indexes.GetItem(rangeIndex++);
			UIntOS count = indexes.GetItem(rangeIndex++);
			for (UIntOS i = 0; i < count; i++)
			{
				UInt64 fields[3] = {0, 0, 0};
				for (UIntOS field = 0; field < 3; field++)
				{
					UIntOS fieldWidth = widths.GetItem(field);
					for (UIntOS b = 0; b < fieldWidth; b++)
					{
						fields[field] = (fields[field] << 8) | decodedData[dataIndex++];
					}
				}
				if (widths.GetItem(0) == 0)
					fields[0] = 1;
				if (fields[0] > 2 || fields[2] > INVALID_INDEX)
				{
					MemFree(thisRef->items);
					thisRef->trailer.Delete();
					MemFree(thisRef);
					env->normalEnd = true;
					return nullptr;
				}
				thisRef->items[startId + i].type = fields[0] == 1 ? 'n' : (fields[0] == 2 ? 'c' : 'f');
				thisRef->items[startId + i].ofst = fields[1];
				thisRef->items[startId + i].generation = (UIntOS)fields[2];
			}
		}

		NN<Text::String> prevOfst;
		if (!trailer->GetEntryValue(CSTR("Prev")).SetTo(prevOfst))
			return thisRef;
		if (!prevOfst->ToUInt64(env->dataOfst))
		{
			MemFree(thisRef->items);
			thisRef->trailer.Delete();
			MemFree(thisRef);
			env->normalEnd = true;
			return nullptr;
		}
		env->currOfst = 0;
		env->dataSize = 0;
		sb->ClearStr();
		thisRef->nextRef = ParseXRef(env, sb);
		if (thisRef->nextRef.IsNull())
		{
			MemFree(thisRef->items);
			thisRef->trailer.Delete();
			MemFree(thisRef);
			env->normalEnd = true;
			return nullptr;
		}
		return thisRef;
	}
	PDFXRef *thisRef = 0;
	Optional<PDFXRef> firstRef = nullptr;
	PDFXRef *lastRef;
	NN<PDFXRef> nnxref;
	while (true)
	{
		sb->ClearStr();
		if (!NextLine(env, sb, true))
		{
#if defined(VERBOSE)
			printf("PDFParser: xref count not found\r\n");
#endif
			env->normalEnd = true;
			break;
		}
		if (sb->Equals(UTF8STRC("trailer")))
		{
#if defined(VERBOSE)
			printf("PDFParser: trailer found, ofst = %lld\r\n", env->lineBegin);
#endif
			if (thisRef == 0)
			{
#if defined(VERBOSE)
				printf("PDFParser: table not found before trailer\r\n");
#endif
				env->normalEnd = true;
				return nullptr;
			}
			sb->ClearStr();
			if (!NextLine(env, sb, true))
			{
#if defined(VERBOSE)
				printf("PDFParser: trailer values not found\r\n");
#endif
				env->normalEnd = true;
				break;
			}
			if (!sb->StartsWith(UTF8STRC("<<")))
			{
#if defined(VERBOSE)
				printf("PDFParser: trailer values is not valid format: %s\r\n", sb->v.Ptr());
#endif
				env->normalEnd = true;
				break;
			}
			while (true)
			{
				UIntOS startCnt = sb->CountStr(UTF8STRC("<<"));
				UIntOS endCnt = sb->CountStr(UTF8STRC(">>"));
				if (startCnt > endCnt)
				{
					sb->AppendUTF8Char(' ');
					if (!NextLine(env, sb, true))
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
			if (!sb->EndsWith(UTF8STRC(">>")))
			{
#if defined(VERBOSE)
				printf("PDFParser: trailer values is not valid format2: %s\r\n", sb->v.Ptr());
#endif
				env->normalEnd = true;
				break;
			}
			sb->RemoveChars(2);
			sb->SetSubstr(2);
			NN<Media::PDFParameter> trailer;
			thisRef->trailer = Media::PDFParameter::Parse(sb->ToCString());
			if (!thisRef->trailer.SetTo(trailer))
			{
#if defined(VERBOSE)
				printf("PDFParser: Error in parsing trailer: %s\r\n", sb->v.Ptr());
#endif
				env->normalEnd = true;
				break;
			}
			NN<Text::String> prevOfst;
			Bool hasPrev = trailer->GetEntryValue(CSTR("Prev")).SetTo(prevOfst);
			sb->ClearStr();
			if (!NextLine(env, sb, true))
			{
#if defined(VERBOSE)
				printf("PDFParser: startxref not found\r\n");
#endif
				env->normalEnd = true;
				break;
			}
			Bool isNextXRef = sb->Equals(UTF8STRC("xref"));
			Bool isStartXRef = sb->Equals(UTF8STRC("startxref")) || sb->StartsWith(UTF8STRC("startxref "));
			if (!isStartXRef && (!isNextXRef || !hasPrev))
			{
#if defined(VERBOSE)
				printf("PDFParser: startxref not present after trailer: %s\r\n", sb->v.Ptr());
#endif
				env->normalEnd = true;
				break;
			}
			if (!isNextXRef)
			{
				if (sb->Equals(UTF8STRC("startxref")))
				{
					sb->ClearStr();
					if (!NextLine(env, sb, true))
					{
#if defined(VERBOSE)
						printf("PDFParser: startxref offset not found\r\n");
#endif
						env->normalEnd = true;
						break;
					}
				}
				else
				{
					sb->SetSubstr(9);
					sb->Trim();
				}
				UInt64 ofst;
				if (!sb->ToUInt64(ofst))
				{
#if defined(VERBOSE)
					printf("PDFParser: startxref offset not valid: %s\r\n", sb->v.Ptr());
#endif
					env->normalEnd = true;
					break;
				}
				sb->ClearStr();
				if (!NextLine(env, sb, true))
				{
#if defined(VERBOSE)
					printf("PDFParser: unexpected end of file after startxref\r\n");
#endif
					env->normalEnd = true;
					break;
				}
				if (!sb->StartsWith(UTF8STRC("%%EOF")))
				{
#if defined(VERBOSE)
					printf("PDFParser: Unknown entry after startxref: %s\r\n", sb->v.Ptr());
#endif
					env->normalEnd = true;
					break;
				}
#if defined(VERBOSE)
				printf("PDFParser: EOF found, ofst = %lld\r\n", env->lineBegin);
#endif
			}
			if (!hasPrev)
			{
				return firstRef;
			}
			NN<Text::String> nnPrevOfst;
			if (!trailer->GetEntryValue(CSTR("Prev")).SetTo(nnPrevOfst))
			{
				return firstRef;
			}
			env->currOfst = 0;
			env->dataSize = 0;
			if (nnPrevOfst->ToUInt64(env->dataOfst))
			{
				sb->ClearStr();
				thisRef->nextRef = ParseXRef(env, sb);
				if (thisRef->nextRef.IsNull())
				{
					env->normalEnd = true;
					break;
				}
				return firstRef;
			}
			else
			{
#if defined(VERBOSE)
				printf("PDFParser: Error in parsing previous Xref offset: %s\r\n", prevOfst->v.Ptr());
#endif
				env->normalEnd = true;
				break;
			}
		}
		else
		{
			UIntOS i;
			UIntOS j;
			i = sb->IndexOf(' ');
			if (i == INVALID_INDEX)
			{
#if defined(VERBOSE)
				printf("PDFParser: xref count format is not valid: %s\r\n", sb->v.Ptr());
#endif
				env->normalEnd = true;
				break;
			}
			sb->v[i] = 0;
			if (!Text::StrToUIntOS(&sb->v[i + 1], j) || !Text::StrToUIntOS(sb->v, i))
			{
				sb->v[i] = ' ';
#if defined(VERBOSE)
				printf("PDFParser: xref count format is not valid 2: %s\r\n", sb->v.Ptr());
#endif
				env->normalEnd = true;
				break;
			}
			lastRef = thisRef;
			thisRef = MemAlloc(PDFXRef, 1);
			thisRef->startId = i;
			thisRef->count = j;
			thisRef->nextRef = nullptr;
			if (lastRef)
				lastRef->nextRef = thisRef;
			thisRef->trailer = nullptr;
			thisRef->items = MemAlloc(PDFXRefItem, j);
			if (firstRef.IsNull())
				firstRef = thisRef;
			i = 0;
			while (i < j)
			{
				sb->ClearStr();
				if (!NextLine(env, sb, true))
				{
#if defined(VERBOSE)
					printf("PDFParser: Error in reading xref value\r\n");
#endif
					env->normalEnd = true;
					break;
				}
				if (sb->leng != 18 || sb->v[10] != ' ' || sb->v[16] != ' ')
				{
#if defined(VERBOSE)
					printf("PDFParser: xref value format invalid: %d, %d, %d, %s\r\n", (UInt32)sb->leng, sb->v[10], sb->v[16], sb->v.Ptr());
#endif
					env->normalEnd = true;
					break;
				}
				sb->v[10] = 0;
				sb->v[16] = 0;
				if (!Text::StrToUInt64(&sb->v[0], thisRef->items[i].ofst) || !Text::StrToUIntOS(&sb->v[11], thisRef->items[i].generation))
				{
#if defined(VERBOSE)
					printf("PDFParser: xref value format invalid2: %d, %d, %d, %s\r\n", (UInt32)sb->leng, sb->v[10], sb->v[16], sb->v.Ptr());
#endif
					env->normalEnd = true;
					break;
				}
				thisRef->items[i].type = sb->v[17];
				i++;
			}
			if (env->normalEnd)
				break;
		}
	}
	if (firstRef.SetTo(nnxref))
	{
		FreeXRef(nnxref);
	}
	return nullptr;
}

void Parser::FileParser::PDFParser::FreeXRef(NN<PDFXRef> xref)
{
	NN<PDFXRef> nextRef;
	if (xref->nextRef.SetTo(nextRef))
		FreeXRef(nextRef);
	xref->trailer.Delete();
	MemFree(xref->items);
	MemFreeNN(xref);
}

Parser::FileParser::PDFParser::PDFParser()
{
}

Parser::FileParser::PDFParser::~PDFParser()
{
}

Int32 Parser::FileParser::PDFParser::GetName()
{
	return *(Int32*)"PDFP";
}

void Parser::FileParser::PDFParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PDFDocument)
	{
		selector->AddFilter(CSTR("*.pdf"), CSTR("Portable Document Format"));
	}
}

IO::ParserType Parser::FileParser::PDFParser::GetParserType()
{
	return IO::ParserType::PDFDocument;
}

Optional<IO::ParsedObject> Parser::FileParser::PDFParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (ReadNInt32(&hdr[0]) != ReadNInt32((const UInt8*)"%PDF") || hdr[4] != '-' || hdr[6] != '.')
	{
		return nullptr;
	}

	NN<Media::PDFDocument> doc;
	Data::ByteBuffer buffTemp(1048576);
	PDFParseEnv env;
	env.buff = buffTemp;
	env.buffSize = 1048576;
	env.currOfst = 0;
	env.dataOfst = 0;
	env.dataSize = 0;
	env.fd = fd;
	env.fileSize = fd->GetDataSize();
	env.normalEnd = false;
	env.succ = false;
	Text::StringBuilderUTF8 sb;
	Optional<PDFXRef> xref = nullptr;
	NN<PDFXRef> nnxref;

	UInt8 tmpBuff[32];
	env.fd->GetRealData(env.fileSize - 32, 32, BYTEARR(tmpBuff));
	UIntOS i = Text::StrIndexOfC(tmpBuff, 32, UTF8STRC("startxref"));
	UIntOS j;
	if (i != INVALID_INDEX)
	{
		i += 9;
		if (tmpBuff[i] == 13 || tmpBuff[i] == 10)
		{
			if (i < 32)
				i++;
			if (i < 32 && tmpBuff[i] == 10)
				i++;
		}
		else if (tmpBuff[i] == ' ' || tmpBuff[i] == '\t')
		{
			while (tmpBuff[i] == ' ' || tmpBuff[i] == '\t')
				i++;
		}
		else
		{
			i = 32;
		}
		j = i;
		while (j < 32)
		{
			if (tmpBuff[j] == 13 || tmpBuff[j] == 10)
			{
				tmpBuff[j] = 0;
				if (Text::StrToUInt64(&tmpBuff[i], env.dataOfst))
					xref = ParseXRef(env, sb);
				break;
			}
			j++;
		}
	}

	env.currOfst = 0;
	env.dataOfst = 0;
	env.dataSize = 0;

	sb.ClearStr();
	if (!NextLine(env, sb, true))
	{
		if (xref.SetTo(nnxref))
			FreeXRef(nnxref);
		return nullptr;
	}
	NEW_CLASSNN(doc, Media::PDFDocument(fd->GetFullFileName(), sb.ToCString().Substring(5)));
	sb.ClearStr();
	while (NextLine(env, sb, true))
	{
		sb.Trim();
		if (sb.EndsWith(UTF8STRC(" 0 obj")))
		{
			if (!ParseObject(env, sb, doc, xref))
				break;
		}
		else if (sb.Equals(UTF8STRC("xref")))
		{
#if defined(VERBOSE)
			printf("PDFParser: xref found, ofst = %lld\r\n", env.lineBegin);
#endif
			while (true)
			{
				sb.ClearStr();
				if (!NextLine(env, sb, true))
				{
#if defined(VERBOSE)
					printf("PDFParser: xref count not found\r\n");
#endif
					env.normalEnd = true;
					break;
				}
				if (sb.Equals(UTF8STRC("trailer")))
				{
#if defined(VERBOSE)
					printf("PDFParser: trailer found, ofst = %lld\r\n", env.lineBegin);
#endif
					sb.ClearStr();
					if (!NextLine(env, sb, true))
					{
#if defined(VERBOSE)
						printf("PDFParser: trailer values not found\r\n");
#endif
						env.normalEnd = true;
						break;
					}
					if (!sb.StartsWith(UTF8STRC("<<")))
					{
#if defined(VERBOSE)
						printf("PDFParser: trailer values is not valid format: %s\r\n", sb.v.Ptr());
#endif
						env.normalEnd = true;
						break;
					}
					while (true)
					{
						UIntOS startCnt = sb.CountStr(UTF8STRC("<<"));
						UIntOS endCnt = sb.CountStr(UTF8STRC(">>"));
						if (startCnt > endCnt)
						{
							sb.AppendUTF8Char(' ');
							if (!NextLine(env, sb, true))
							{
								break;
							}
						}
						else
						{
							break;
						}
					}
					if (!sb.EndsWith(UTF8STRC(">>")))
					{
#if defined(VERBOSE)
						printf("PDFParser: trailer values is not valid format2: %s\r\n", sb.v.Ptr());
#endif
						env.normalEnd = true;
						break;
					}
					sb.ClearStr();
					if (!NextLine(env, sb, true))
					{
#if defined(VERBOSE)
						printf("PDFParser: startxref not found\r\n");
#endif
						env.normalEnd = true;
						break;
					}
					if (sb.Equals(UTF8STRC("xref")))
						env.currOfst = env.lineBegin - env.dataOfst;
					else
						ParseStartxref(env, sb);
					break;
				}
				else
				{
					UIntOS i;
					UIntOS j;
					i = sb.IndexOf(' ');
					if (i == INVALID_INDEX)
					{
#if defined(VERBOSE)
						printf("PDFParser: xref count format is not valid: %s\r\n", sb.v.Ptr());
#endif
						env.normalEnd = true;
						break;
					}
					sb.v[i] = 0;
					if (!Text::StrToUIntOS(&sb.v[i + 1], j) || !Text::StrToUIntOS(sb.v, i))
					{
						sb.v[i] = ' ';
#if defined(VERBOSE)
						printf("PDFParser: xref count format is not valid 2: %s\r\n", sb.v.Ptr());
#endif
						env.normalEnd = true;
						break;
					}
					j += i;
					while (i < j)
					{
						sb.ClearStr();
						if (!NextLine(env, sb, true))
						{
#if defined(VERBOSE)
							printf("PDFParser: Error in reading xref value\r\n");
#endif
							env.normalEnd = true;
							break;
						}
						if ((sb.leng != 18 && sb.leng != 19) || sb.v[10] != ' ' || sb.v[16] != ' ')
						{
#if defined(VERBOSE)
							printf("PDFParser: xref value format invalid: %d, %d, %d, %s\r\n", (UInt32)sb.leng, sb.v[10], sb.v[16], sb.v.Ptr());
#endif
							env.normalEnd = true;
							break;
						}
						i++;
					}
					if (env.normalEnd)
						break;
				}
			}
			if (env.normalEnd)
				break;
		}
		else if (sb.Equals(UTF8STRC("startxref")) || sb.StartsWith(UTF8STRC("startxref ")))
		{
			ParseStartxref(env, sb);
			if (env.normalEnd)
				break;
		}
		else if (sb.IndexOf(UTF8STRC(" 0 obj ")) != INVALID_INDEX)
		{
			UIntOS i = sb.IndexOf(UTF8STRC(" 0 obj "));
			sb.TrimToLength(i + 6);
			env.currOfst = env.lineBegin + i + 7 - env.dataOfst;
			if (!ParseObject(env, sb, doc, xref))
				break;
		}
		else if (sb.IndexOf(UTF8STRC(" 0 obj<<")) != INVALID_INDEX)
		{
			UIntOS i = sb.IndexOf(UTF8STRC(" 0 obj"));
			sb.TrimToLength(i + 6);
			env.currOfst = env.lineBegin + i + 6 - env.dataOfst;
			if (!ParseObject(env, sb, doc, xref))
				break;
		}
		else if (sb.IndexOf(UTF8STRC(" 0 obj[")) != INVALID_INDEX)
		{
			UIntOS i = sb.IndexOf(UTF8STRC(" 0 obj"));
			sb.TrimToLength(i + 6);
			env.currOfst = env.lineBegin + i + 6 - env.dataOfst;
			if (!ParseObject(env, sb, doc, xref))
				break;
		}
		else if (sb.IndexOf(UTF8STRC(" 0 obj/")) != INVALID_INDEX)
		{
			UIntOS i = sb.IndexOf(UTF8STRC(" 0 obj"));
			sb.TrimToLength(i + 6);
			env.currOfst = env.lineBegin + i + 6 - env.dataOfst;
			if (!ParseObject(env, sb, doc, xref))
				break;
		}
		else
		{
			if (sb.GetLength() > 0)
			{
#if defined(VERBOSE)
				printf("PDFParser: Unsupported line: %s, ofst = %lld\r\n", sb.v.Ptr(), env.lineBegin);
#endif
				if (env.lineBegin >= 16)
				{
					env.normalEnd = true;
					break;
				}
			}
		}
		sb.ClearStr();
	}
#if defined(VERBOSE)
	if (!env.normalEnd)
	{
		printf("PDFParser: Error in parsing next line\r\n");
	}
#endif
	if (env.succ && xref.SetTo(nnxref))
		this->SetupEncryption(doc, nnxref);
	if (xref.SetTo(nnxref))
		FreeXRef(nnxref);
	if (env.succ)
		return doc;
	doc.Delete();
	return nullptr;
}
