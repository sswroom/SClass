#include "Stdafx.h"
#include "Media/ICCProfile.h"
#include "Media/PDFDocument.h"
#include "Media/PDFUtil.h"

void Media::PDFUtil::SkipPDFWhitespace(UnsafeArray<const UTF8Char> str, UIntOS length, UIntOS &pos)
{
	while (pos < length && (str[pos] == ' ' || str[pos] == '\t' || str[pos] == '\r' || str[pos] == '\n' || str[pos] == '\f'))
		pos++;
}

Bool Media::PDFUtil::ReadPDFNumberToken(UnsafeArray<const UTF8Char> str, UIntOS length, UIntOS &pos, UIntOS &start, UIntOS &end)
{
	SkipPDFWhitespace(str, length, pos);
	start = pos;
	while (pos < length && str[pos] != ' ' && str[pos] != '\t' && str[pos] != '\r' && str[pos] != '\n' && str[pos] != '\f' && str[pos] != ']')
		pos++;
	end = pos;
	return start != end;
}

Bool Media::PDFUtil::IsPDFDecodeEndpoint(UnsafeArray<const UTF8Char> str, UIntOS start, UIntOS end, UInt8 endpoint)
{
	Bool negative = false;
	if (start < end && (str[start] == '+' || str[start] == '-'))
	{
		negative = str[start] == '-';
		start++;
	}
	Bool hasDigit = false;
	UInt8 integer = 0;
	while (start < end && str[start] >= '0' && str[start] <= '9')
	{
		hasDigit = true;
		if (integer > 1)
			return false;
		integer = (UInt8)(integer * 10 + str[start] - '0');
		start++;
	}
	if (start < end && str[start] == '.')
	{
		start++;
		while (start < end && str[start] >= '0' && str[start] <= '9')
		{
			hasDigit = true;
			if (str[start] != '0')
				return false;
			start++;
		}
	}
	return start == end && hasDigit && integer == endpoint && (!negative || endpoint == 0);
}

Bool Media::PDFUtil::IsReverseDecodeArray(Text::CStringNN value)
{
	UnsafeArray<const UTF8Char> str = value.v;
	UIntOS pos = 0;
	SkipPDFWhitespace(str, value.leng, pos);
	if (pos >= value.leng || str[pos++] != '[')
		return false;
	UIntOS firstStart;
	UIntOS firstEnd;
	UIntOS secondStart;
	UIntOS secondEnd;
	if (!ReadPDFNumberToken(str, value.leng, pos, firstStart, firstEnd) ||
		!ReadPDFNumberToken(str, value.leng, pos, secondStart, secondEnd))
		return false;
	SkipPDFWhitespace(str, value.leng, pos);
	if (pos >= value.leng || str[pos++] != ']')
		return false;
	SkipPDFWhitespace(str, value.leng, pos);
	return pos == value.leng &&
		IsPDFDecodeEndpoint(str, firstStart, firstEnd, 1) &&
		IsPDFDecodeEndpoint(str, secondStart, secondEnd, 0);
}

Bool Media::PDFUtil::IsDefaultDecodeArray(Text::CStringNN value, UIntOS components)
{
	UnsafeArray<const UTF8Char> str = value.v;
	UIntOS pos = 0;
	SkipPDFWhitespace(str, value.leng, pos);
	if (pos >= value.leng || str[pos++] != '[')
		return false;
	for (UIntOS i = 0; i < components * 2; i++)
	{
		UIntOS start;
		UIntOS end;
		if (!ReadPDFNumberToken(str, value.leng, pos, start, end) || !IsPDFDecodeEndpoint(str, start, end, (UInt8)(i & 1)))
			return false;
	}
	SkipPDFWhitespace(str, value.leng, pos);
	if (pos >= value.leng || str[pos++] != ']')
		return false;
	SkipPDFWhitespace(str, value.leng, pos);
	return pos == value.leng;
}

void Media::PDFUtil::SkipIndexedWhitespace(UnsafeArray<const UTF8Char> str, UIntOS length, UIntOS &pos)
{
	while (pos < length && (str[pos] == ' ' || str[pos] == '\t' || str[pos] == '\r' || str[pos] == '\n' || str[pos] == '\f'))
		pos++;
}

Bool Media::PDFUtil::ReadIndexedToken(UnsafeArray<const UTF8Char> str, UIntOS length, UIntOS &pos, UIntOS &start, UIntOS &end)
{
	SkipIndexedWhitespace(str, length, pos);
	start = pos;
	while (pos < length)
	{
		UTF8Char c = str[pos];
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f' ||
			c == '(' || c == ')' || c == '<' || c == '>' || c == '[' || c == ']' || c == '{' || c == '}' || c == '%')
			break;
		if (c == '/' && pos != start)
			break;
		pos++;
	}
	end = pos;
	return start != end;
}

Bool Media::PDFUtil::ParseIndexedUInt(UnsafeArray<const UTF8Char> str, UIntOS start, UIntOS end, UIntOS &value)
{
	if (start == end)
		return false;
	value = 0;
	while (start < end)
	{
		if (str[start] < '0' || str[start] > '9')
			return false;
		UIntOS digit = (UIntOS)(str[start++] - '0');
		if (value > (INVALID_INDEX - digit) / 10)
			return false;
		value = value * 10 + digit;
	}
	return true;
}

Bool Media::PDFUtil::ReadIndexedReference(UnsafeArray<const UTF8Char> str, UIntOS length, UIntOS &pos, UInt32 &id)
{
	UIntOS start;
	UIntOS end;
	UIntOS objectId;
	UIntOS generation;
	if (!ReadIndexedToken(str, length, pos, start, end) || !ParseIndexedUInt(str, start, end, objectId) || objectId > 0xffffffffU ||
		!ReadIndexedToken(str, length, pos, start, end) || !ParseIndexedUInt(str, start, end, generation) ||
		!ReadIndexedToken(str, length, pos, start, end) || !Text::CStringNN(str + start, end - start).Equals(CSTR("R")))
		return false;
	id = (UInt32)objectId;
	return true;
}

Bool Media::PDFUtil::ValidateIndexedICC(Data::ByteArrayR bytes)
{
	UIntOS size = bytes.GetSize();
	if (size < 132 || bytes.ReadMU32(0) != size || bytes.ReadMU32(36) != 0x61637370 ||
		bytes.ReadMU32(16) != 0x52474220)
		return false;
	UIntOS count = bytes.ReadMU32(128);
	if (count > (size - 132) / 12)
		return false;
	for (UIntOS i = 0; i < count; i++)
	{
		UIntOS entry = 132 + i * 12;
		UInt32 tag = bytes.ReadMU32(entry);
		UIntOS offset = bytes.ReadMU32(entry + 4);
		UIntOS length = bytes.ReadMU32(entry + 8);
		if (offset > size || length > size - offset)
			return false;
		if (tag == 0x72545243 || tag == 0x67545243 || tag == 0x62545243 || tag == 0x6b545243)
		{
			if (length < 12)
				return false;
			UInt32 type = bytes.ReadMU32(offset);
			if (type == 0x63757276)
			{
				UIntOS values = bytes.ReadMU32(offset + 8);
				if (values > (length - 12) / 2)
					return false;
			}
			else if (type == 0x70617261)
			{
				UInt16 function = bytes.ReadMU16(offset + 8);
				if ((function == 0 && length < 16) || (function == 3 && length < 32) ||
					(function == 4 && length < 40))
					return false;
			}
		}
		else if ((tag == 0x7258595a || tag == 0x6758595a || tag == 0x6258595a || tag == 0x77747074) && length < 20)
			return false;
	}
	return true;
}

Bool Media::PDFUtil::ResolveIndexedBase(NN<Media::PDFDocument> doc, Text::CStringNN base, UIntOS &components, NN<Media::ColorProfile> color, Bool allowICCDeviceAlternate)
{
	Optional<Text::String> owned = nullptr;
	UIntOS depth = 0;
	while (depth++ < 32)
	{
		UnsafeArray<const UTF8Char> str = base.v;
		UIntOS pos = 0;
		SkipIndexedWhitespace(str, base.leng, pos);
		Bool array = pos < base.leng && str[pos] == '[';
		if (array)
			pos++;
		UIntOS start;
		UIntOS end;
		if (!ReadIndexedToken(str, base.leng, pos, start, end))
			break;
		Text::CStringNN token(str + start, end - start);
		if (!array && (token.Equals(CSTR("/DeviceGray")) || token.Equals(CSTR("/G")) ||
			token.Equals(CSTR("/DeviceRGB")) || token.Equals(CSTR("/RGB")) ||
			token.Equals(CSTR("/DeviceCMYK")) || token.Equals(CSTR("/CMYK"))))
		{
			SkipIndexedWhitespace(str, base.leng, pos);
			if (pos != base.leng)
				break;
			components = token.Equals(CSTR("/DeviceGray")) || token.Equals(CSTR("/G")) ? 1 :
				(token.Equals(CSTR("/DeviceRGB")) || token.Equals(CSTR("/RGB")) ? 3 : 4);
			OPTSTR_DEL(owned);
			return true;
		}
		if (array && token.Equals(CSTR("/ICCBased")))
		{
			UInt32 profileId;
			NN<Media::PDFObject> profileObj;
			NN<Media::PDFParameter> params;
			NN<Text::String> n;
			NN<Text::String> range;
			NN<IO::StreamData> data;
			NN<Media::ICCProfile> profile;
			if (!ReadIndexedReference(str, base.leng, pos, profileId))
				break;
			SkipIndexedWhitespace(str, base.leng, pos);
			if (pos >= base.leng || str[pos++] != ']')
				break;
			SkipIndexedWhitespace(str, base.leng, pos);
			if (pos != base.leng || !doc->GetObjectById(profileId).SetTo(profileObj) ||
				!profileObj->GetParameter().SetTo(params) || !params->GetEntryValue(CSTR("N")).SetTo(n) ||
				!n->ToUIntOS(components) || (components != 3 && !(allowICCDeviceAlternate && components == 4)) || !profileObj->GetData().SetTo(data) ||
				data->GetDataSize() > 16777216)
				break;
			if (params->GetEntryValue(CSTR("Range")).SetTo(range) && !IsDefaultDecodeArray(range->ToCString(), components))
				break;
			Data::ByteBuffer bytes((UIntOS)data->GetDataSize());
			if (data->GetRealData(0, bytes.GetSize(), bytes) != bytes.GetSize())
				break;
			if (components == 4)
			{
				NN<Text::String> alternate;
				if ((params->GetEntryValue(CSTR("Alternate")).SetTo(alternate) && !alternate->Equals(UTF8STRC("/DeviceCMYK"))) ||
					bytes.GetSize() < 132 || bytes.ReadMU32(0) != bytes.GetSize() ||
					bytes.ReadMU32(16) != 0x434d594b || bytes.ReadMU32(36) != 0x61637370)
					break;
				UIntOS tagCount = bytes.ReadMU32(128);
				if (tagCount > (bytes.GetSize() - 132) / 12)
					break;
				Bool validTags = true;
				for (UIntOS i = 0; i < tagCount; i++)
				{
					UIntOS offset = bytes.ReadMU32(132 + i * 12 + 4);
					UIntOS length = bytes.ReadMU32(132 + i * 12 + 8);
					if (offset > bytes.GetSize() || length > bytes.GetSize() - offset)
					{
						validTags = false;
						break;
					}
				}
				if (!validTags)
					break;
				printf("PDFUtil: CMYK ICC color conversion unavailable; using DeviceCMYK alternate approximation\r\n");
				OPTSTR_DEL(owned);
				return true;
			}
			if (!ValidateIndexedICC(bytes) || !Media::ICCProfile::Parse(bytes).SetTo(profile))
				break;
			Bool ok = profile->SetToColorProfile(color);
			profile.Delete();
			OPTSTR_DEL(owned);
			return ok;
		}
		if (array)
			break;
		pos = start;
		UInt32 objectId;
		NN<Media::PDFObject> refObj;
		NN<IO::StreamData> data;
		if (!ReadIndexedReference(str, base.leng, pos, objectId))
			break;
		SkipIndexedWhitespace(str, base.leng, pos);
		if (pos != base.leng || !doc->GetObjectById(objectId).SetTo(refObj) ||
			!refObj->GetData().SetTo(data) || data->GetDataSize() > 1048576)
			break;
		Data::ByteBuffer bytes((UIntOS)data->GetDataSize());
		if (data->GetRealData(0, bytes.GetSize(), bytes) != bytes.GetSize())
			break;
		NN<Text::String> resolved = Text::String::New(bytes.Arr(), bytes.GetSize());
		OPTSTR_DEL(owned);
		owned = resolved;
		base = resolved->ToCString();
	}
	OPTSTR_DEL(owned);
	return false;
}

Bool Media::PDFUtil::ResolveReference(NN<Media::PDFDocument> doc, NN<Text::StringBuilderUTF8> sbRef)
{
	UIntOS refDepth = 0;
	while (sbRef->EndsWith(CSTR(" R")))
	{
		if (refDepth++ >= 32)
		{
			printf("PDFUtil: Reference chain too deep\r\n");
			return false;
		}
		UTF8Char sbuff[32];
		UnsafeArray<UTF8Char> sptr;
		if (sbRef->GetLength() > 20)
		{
			printf("PDFUtil: Reference too long: %s\r\n", sbRef->v.Ptr());
			return false;
		}
		sptr = sbRef->ConcatTo(sbuff);
		sptr -= 2;
		sptr[0] = 0;
		UIntOS i = Text::StrLastIndexOfC(sbuff, (UIntOS)(sptr - sbuff), ' ');
		if (i == INVALID_INDEX)
		{
			printf("PDFUtil: Invalid reference: %s\r\n", sbRef->v.Ptr());
			return false;
		}
		sbuff[i] = 0;
		UInt32 id;
		if (!Text::StrToUInt32(sbuff, id))
		{	
			printf("PDFUtil: Invalid reference ID: %s\r\n", sbuff);
			break;
		}
		NN<Media::PDFObject> refObj;
		if (!doc->GetObjectById(id).SetTo(refObj))
		{
			printf("PDFUtil: Failed to get reference object: %s\r\n", sbuff);
			return false;
		}
		NN<IO::StreamData> refData;
		if (!refObj->GetData().SetTo(refData))
		{
			printf("PDFUtil: Failed to get reference data: %s\r\n", sbuff);
			return false;
		}
		if (refData->GetDataSize() > 1048576)
		{
			printf("PDFUtil: Reference too large\r\n");
			return false;
		}
		UnsafeArray<UInt8> refArr = MemAllocArr(UInt8, (UIntOS)refData->GetDataSize() + 1);
		if (refData->GetRealData(0, (UIntOS)refData->GetDataSize(), Data::ByteArray(refArr, (UIntOS)refData->GetDataSize())) != refData->GetDataSize())
		{
			printf("PDFUtil: Unable to read reference\r\n");
			MemFreeArr(refArr);
			return false;
		}
		refArr[(UIntOS)refData->GetDataSize()] = 0;
		Text::StrReplace(refArr, '\r', ' ');
		Text::StrReplace(refArr, '\n', ' ');
		sbRef->ClearStr();
		sbRef->AppendC(refArr, (UIntOS)refData->GetDataSize());
		MemFreeArr(refArr);
	}
	return true;
}