#include "Stdafx.h"
#include <math.h>
#include "Media/CCITTFaxDecoder.h"
#include "Media/JBIG2Decoder.h"
#include "Media/PDFDocument.h"
#include "Media/PDFImage.h"
#include "Media/PDFUtil.h"

struct PDFSeparationInstruction
{
	enum class Op
	{
		Number,
		Dup,
		Exch,
		Pop,
		Add,
		Sub,
		Mul,
		Div,
		Neg,
		Abs
	};

	Op op;
	Double value;
};

struct PDFSeparationInfo
{
	static const UIntOS MAX_FUNCTION_INSTRUCTIONS = 128;
	UIntOS components;
	Double domainMin;
	Double domainMax;
	Double exponent;
	Double decodeMin;
	Double decodeMax;
	Double c0[4];
	Double c1[4];
	Double rangeMin[4];
	Double rangeMax[4];
	Double encodeMin;
	Double encodeMax;
	Double decodeRangeMin[4];
	Double decodeRangeMax[4];
	UIntOS functionType;
	UIntOS sampleSize;
	UIntOS bitsPerSample;
	Data::ByteBuffer samples;
	PDFSeparationInstruction functionInstructions[MAX_FUNCTION_INSTRUCTIONS];
	UIntOS functionInstructionCount;
	Bool hasRange;
	Media::ColorProfile color;

	PDFSeparationInfo() : color(Media::ColorProfile::CPT_PUNKNOWN)
	{
		this->components = 0;
		this->domainMin = 0;
		this->domainMax = 1;
		this->exponent = 1;
		this->decodeMin = 0;
		this->decodeMax = 1;
		this->functionType = 0;
		this->sampleSize = 0;
		this->bitsPerSample = 0;
		this->functionInstructionCount = 0;
		this->hasRange = false;
	}
};

static Bool PDFImageReadNumberArray(UnsafeArray<const UTF8Char> str, UIntOS length, UIntOS &pos, UnsafeArray<Double> values, UIntOS count)
{
	Media::PDFUtil::SkipIndexedWhitespace(str, length, pos);
	if (pos >= length || str[pos++] != '[')
		return false;
	for (UIntOS i = 0; i < count; i++)
	{
		UIntOS start;
		UIntOS end;
		Double value;
		if (!Media::PDFUtil::ReadIndexedToken(str, length, pos, start, end))
			return false;
		NN<Text::String> number = Text::String::New(str + start, end - start);
		Bool validNumber = number->ToDouble(value) && isfinite(value);
		number->Release();
		if (!validNumber)
			return false;
		values[i] = value;
	}
	Media::PDFUtil::SkipIndexedWhitespace(str, length, pos);
	return pos < length && str[pos++] == ']';
}

static Bool PDFImageCompileType4Function(NN<IO::StreamData> streamData, PDFSeparationInfo &info)
{
	UInt64 streamSize = streamData->GetDataSize();
	if (streamSize == 0 || streamSize > 1048576)
		return false;
	Data::ByteBuffer program((UIntOS)streamSize);
	if (streamData->GetRealData(0, streamSize, program) != streamSize)
		return false;
	UnsafeArray<const UTF8Char> str = (UnsafeArray<const UTF8Char>)program.Arr();
	UIntOS length = (UIntOS)streamSize;
	UIntOS pos = 0;
	Media::PDFUtil::SkipIndexedWhitespace(str, length, pos);
	if (pos >= length || str[pos++] != '{')
		return false;

	while (pos < length)
	{
		Media::PDFUtil::SkipIndexedWhitespace(str, length, pos);
		if (pos >= length)
			return false;
		if (str[pos] == '}')
		{
			pos++;
			Media::PDFUtil::SkipIndexedWhitespace(str, length, pos);
			return pos == length && info.functionInstructionCount > 0;
		}
		if (str[pos] == '%')
		{
			while (pos < length && str[pos] != '\r' && str[pos] != '\n')
				pos++;
			continue;
		}

		UIntOS start = pos;
		while (pos < length && str[pos] != ' ' && str[pos] != '\t' && str[pos] != '\r' &&
			str[pos] != '\n' && str[pos] != '\f' && str[pos] != '{' && str[pos] != '}')
			pos++;
		UIntOS tokenLength = pos - start;
		if (tokenLength == 0 || info.functionInstructionCount >= PDFSeparationInfo::MAX_FUNCTION_INSTRUCTIONS)
			return false;

		PDFSeparationInstruction instruction;
		Text::CStringNN token(str + start, tokenLength);
		if (token.Equals(CSTR("dup")))
			instruction.op = PDFSeparationInstruction::Op::Dup;
		else if (token.Equals(CSTR("exch")))
			instruction.op = PDFSeparationInstruction::Op::Exch;
		else if (token.Equals(CSTR("pop")))
			instruction.op = PDFSeparationInstruction::Op::Pop;
		else if (token.Equals(CSTR("add")))
			instruction.op = PDFSeparationInstruction::Op::Add;
		else if (token.Equals(CSTR("sub")))
			instruction.op = PDFSeparationInstruction::Op::Sub;
		else if (token.Equals(CSTR("mul")))
			instruction.op = PDFSeparationInstruction::Op::Mul;
		else if (token.Equals(CSTR("div")))
			instruction.op = PDFSeparationInstruction::Op::Div;
		else if (token.Equals(CSTR("neg")))
			instruction.op = PDFSeparationInstruction::Op::Neg;
		else if (token.Equals(CSTR("abs")))
			instruction.op = PDFSeparationInstruction::Op::Abs;
		else
		{
			NN<Text::String> number = Text::String::New(str + start, tokenLength);
			Bool isNumber = number->ToDouble(instruction.value) && isfinite(instruction.value);
			number->Release();
			if (!isNumber)
				return false;
			instruction.op = PDFSeparationInstruction::Op::Number;
		}
		info.functionInstructions[info.functionInstructionCount++] = instruction;
	}
	return false;
}

static Bool PDFImageEvaluateType4Function(const PDFSeparationInfo &info, Double input, UnsafeArray<Double> values)
{
	Double stack[64];
	UIntOS stackCount = 1;
	stack[0] = input;
	for (UIntOS i = 0; i < info.functionInstructionCount; i++)
	{
		const PDFSeparationInstruction &instruction = info.functionInstructions[i];
		switch (instruction.op)
		{
		case PDFSeparationInstruction::Op::Number:
			if (stackCount >= 64)
				return false;
			stack[stackCount++] = instruction.value;
			break;
		case PDFSeparationInstruction::Op::Dup:
			if (stackCount == 0 || stackCount >= 64)
				return false;
			stack[stackCount] = stack[stackCount - 1];
			stackCount++;
			break;
		case PDFSeparationInstruction::Op::Exch:
			if (stackCount < 2)
				return false;
			{
				Double tmp = stack[stackCount - 1];
				stack[stackCount - 1] = stack[stackCount - 2];
				stack[stackCount - 2] = tmp;
			}
			break;
		case PDFSeparationInstruction::Op::Pop:
			if (stackCount == 0)
				return false;
			stackCount--;
			break;
		case PDFSeparationInstruction::Op::Neg:
			if (stackCount == 0)
				return false;
			stack[stackCount - 1] = -stack[stackCount - 1];
			break;
		case PDFSeparationInstruction::Op::Abs:
			if (stackCount == 0)
				return false;
			stack[stackCount - 1] = fabs(stack[stackCount - 1]);
			break;
		case PDFSeparationInstruction::Op::Add:
		case PDFSeparationInstruction::Op::Sub:
		case PDFSeparationInstruction::Op::Mul:
		case PDFSeparationInstruction::Op::Div:
			if (stackCount < 2)
				return false;
			{
				Double rhs = stack[--stackCount];
				Double &lhs = stack[stackCount - 1];
				switch (instruction.op)
				{
				case PDFSeparationInstruction::Op::Add: lhs += rhs; break;
				case PDFSeparationInstruction::Op::Sub: lhs -= rhs; break;
				case PDFSeparationInstruction::Op::Mul: lhs *= rhs; break;
				case PDFSeparationInstruction::Op::Div:
					if (rhs == 0)
						return false;
					lhs /= rhs;
					break;
				case PDFSeparationInstruction::Op::Number:
				case PDFSeparationInstruction::Op::Dup:
				case PDFSeparationInstruction::Op::Exch:
				case PDFSeparationInstruction::Op::Pop:
				case PDFSeparationInstruction::Op::Neg:
				case PDFSeparationInstruction::Op::Abs:
				default: return false;
				}
				if (!isfinite(lhs))
					return false;
			}
			break;
		default:
			return false;
		}
	}
	if (stackCount != info.components)
		return false;
	for (UIntOS i = 0; i < info.components; i++)
	{
		values[i] = stack[i];
		if (values[i] < info.rangeMin[i])
			values[i] = info.rangeMin[i];
		else if (values[i] > info.rangeMax[i])
			values[i] = info.rangeMax[i];
	}
	return true;
}

static Bool PDFImageParseFunction(NN<Media::PDFParameter> functionParams, Optional<Media::PDFObject> functionObj, PDFSeparationInfo &info)
{
	NN<Text::String> value;
	if (!functionParams->GetEntryValue(CSTR("FunctionType")).SetTo(value))
		return false;
	info.functionType = value->ToUIntOS();
	if (info.functionType == 0)
	{
		Double domain[2];
		UIntOS domainPos = 0;
		if (!functionParams->GetEntryValue(CSTR("Domain")).SetTo(value) ||
			!PDFImageReadNumberArray(value->v, value->leng, domainPos, domain, 2))
			return false;
		Media::PDFUtil::SkipIndexedWhitespace(value->v, value->leng, domainPos);
		if (domainPos != value->leng || domain[0] >= domain[1])
			return false;
		info.domainMin = domain[0];
		info.domainMax = domain[1];

		Double sizes[1];
		UIntOS sizePos = 0;
		if (!functionParams->GetEntryValue(CSTR("Size")).SetTo(value) ||
			!PDFImageReadNumberArray(value->v, value->leng, sizePos, sizes, 1))
			return false;
		Media::PDFUtil::SkipIndexedWhitespace(value->v, value->leng, sizePos);
		if (sizePos != value->leng || sizes[0] < 1 || sizes[0] > 1048576 || floor(sizes[0]) != sizes[0])
			return false;
		info.sampleSize = (UIntOS)sizes[0];
		if (!functionParams->GetEntryValue(CSTR("BitsPerSample")).SetTo(value) ||
			!value->ToUIntOS(info.bitsPerSample) || info.bitsPerSample == 0 || info.bitsPerSample > 16)
			return false;
		if (functionParams->GetEntryValue(CSTR("Order")).SetTo(value))
		{
			UIntOS order;
			if (!value->ToUIntOS(order) || order != 1)
				return false;
		}

		Double ranges[8];
		UIntOS rangePos = 0;
		if (!functionParams->GetEntryValue(CSTR("Range")).SetTo(value) ||
			!PDFImageReadNumberArray(value->v, value->leng, rangePos, ranges, info.components * 2))
			return false;
		Media::PDFUtil::SkipIndexedWhitespace(value->v, value->leng, rangePos);
		if (rangePos != value->leng)
			return false;
		for (UIntOS i = 0; i < info.components; i++)
		{
			if (ranges[i * 2] > ranges[i * 2 + 1])
				return false;
			info.rangeMin[i] = ranges[i * 2];
			info.rangeMax[i] = ranges[i * 2 + 1];
		}

		if (functionParams->GetEntryValue(CSTR("Encode")).SetTo(value))
		{
			Double encode[2];
			UIntOS encodePos = 0;
			if (!PDFImageReadNumberArray(value->v, value->leng, encodePos, encode, 2))
				return false;
			Media::PDFUtil::SkipIndexedWhitespace(value->v, value->leng, encodePos);
			if (encodePos != value->leng)
				return false;
			info.encodeMin = encode[0];
			info.encodeMax = encode[1];
		}
		else
		{
			info.encodeMin = 0;
			info.encodeMax = (Double)(info.sampleSize - 1);
		}
		Double decode[8];
		UIntOS decodePos = 0;
		if (functionParams->GetEntryValue(CSTR("Decode")).SetTo(value))
		{
			if (!PDFImageReadNumberArray(value->v, value->leng, decodePos, decode, info.components * 2))
				return false;
			Media::PDFUtil::SkipIndexedWhitespace(value->v, value->leng, decodePos);
			if (decodePos != value->leng)
				return false;
		}
		else
		{
			for (UIntOS i = 0; i < info.components * 2; i++)
				decode[i] = ranges[i];
		}
		for (UIntOS i = 0; i < info.components; i++)
		{
			info.decodeRangeMin[i] = decode[i * 2];
			info.decodeRangeMax[i] = decode[i * 2 + 1];
		}

		NN<Media::PDFObject> streamObj;
		NN<IO::StreamData> streamData;
		UInt64 totalBits = (UInt64)info.sampleSize * info.components * info.bitsPerSample;
		UIntOS requiredBytes = (UIntOS)((totalBits + 7) / 8);
		if (totalBits > (UInt64)INVALID_INDEX * 8 || requiredBytes > 16777216 ||
			!functionObj.SetTo(streamObj) || !streamObj->GetData().SetTo(streamData) ||
			streamData->GetDataSize() < requiredBytes)
			return false;
		info.samples.ChangeSizeAndIgnoreData(requiredBytes);
		return streamData->GetRealData(0, requiredBytes, info.samples) == requiredBytes;
	}
	if (info.functionType == 4)
	{
		Double domain[2];
		UIntOS domainPos = 0;
		if (!functionParams->GetEntryValue(CSTR("Domain")).SetTo(value) ||
			!PDFImageReadNumberArray(value->v, value->leng, domainPos, domain, 2))
			return false;
		Media::PDFUtil::SkipIndexedWhitespace(value->v, value->leng, domainPos);
		if (domainPos != value->leng || domain[0] >= domain[1])
			return false;
		info.domainMin = domain[0];
		info.domainMax = domain[1];
		Double ranges[8];
		UIntOS rangePos = 0;
		if (!functionParams->GetEntryValue(CSTR("Range")).SetTo(value) ||
			!PDFImageReadNumberArray(value->v, value->leng, rangePos, ranges, info.components * 2))
			return false;
		Media::PDFUtil::SkipIndexedWhitespace(value->v, value->leng, rangePos);
		if (rangePos != value->leng)
			return false;
		for (UIntOS i = 0; i < info.components; i++)
		{
			if (ranges[i * 2] > ranges[i * 2 + 1])
				return false;
			info.rangeMin[i] = ranges[i * 2];
			info.rangeMax[i] = ranges[i * 2 + 1];
		}
		NN<Media::PDFObject> streamObj;
		NN<IO::StreamData> streamData;
		return functionObj.SetTo(streamObj) && streamObj->GetData().SetTo(streamData) &&
			PDFImageCompileType4Function(streamData, info);
	}
	if (info.functionType != 2)
		return false;
	if (functionParams->GetEntryValue(CSTR("Domain")).SetTo(value))
	{
		Double domain[2];
		UIntOS domainPos = 0;
		if (!PDFImageReadNumberArray(value->v, value->leng, domainPos, domain, 2))
			return false;
		Media::PDFUtil::SkipIndexedWhitespace(value->v, value->leng, domainPos);
		if (domainPos != value->leng || domain[0] >= domain[1])
			return false;
		info.domainMin = domain[0];
		info.domainMax = domain[1];
	}
	if (!functionParams->GetEntryValue(CSTR("N")).SetTo(value) ||
		!value->ToDouble(info.exponent) || !isfinite(info.exponent) || info.exponent <= 0)
		return false;
	for (UIntOS i = 0; i < info.components; i++)
	{
		info.c0[i] = 0;
		info.c1[i] = 1;
		info.rangeMin[i] = 0;
		info.rangeMax[i] = 1;
	}
	if (functionParams->GetEntryValue(CSTR("C0")).SetTo(value))
	{
		UIntOS arrayPos = 0;
		if (!PDFImageReadNumberArray(value->v, value->leng, arrayPos, info.c0, info.components))
			return false;
		Media::PDFUtil::SkipIndexedWhitespace(value->v, value->leng, arrayPos);
		if (arrayPos != value->leng)
			return false;
	}
	if (functionParams->GetEntryValue(CSTR("C1")).SetTo(value))
	{
		UIntOS arrayPos = 0;
		if (!PDFImageReadNumberArray(value->v, value->leng, arrayPos, info.c1, info.components))
			return false;
		Media::PDFUtil::SkipIndexedWhitespace(value->v, value->leng, arrayPos);
		if (arrayPos != value->leng)
			return false;
	}
	if (functionParams->GetEntryValue(CSTR("Range")).SetTo(value))
	{
		Double range[8];
		UIntOS arrayPos = 0;
		if (!PDFImageReadNumberArray(value->v, value->leng, arrayPos, range, info.components * 2))
			return false;
		Media::PDFUtil::SkipIndexedWhitespace(value->v, value->leng, arrayPos);
		if (arrayPos != value->leng)
			return false;
		for (UIntOS i = 0; i < info.components; i++)
		{
			if (range[i * 2] > range[i * 2 + 1])
				return false;
			info.rangeMin[i] = range[i * 2];
			info.rangeMax[i] = range[i * 2 + 1];
		}
		info.hasRange = true;
	}
	return true;
}

static Bool PDFImageParseSeparation(NN<Media::PDFDocument> pdf, Text::CStringNN colorSpace, PDFSeparationInfo &info)
{
	UnsafeArray<const UTF8Char> str = colorSpace.v;
	UIntOS length = colorSpace.leng;
	UIntOS pos = 0;
	UIntOS start;
	UIntOS end;
	Media::PDFUtil::SkipIndexedWhitespace(str, length, pos);
	if (pos >= length || str[pos++] != '[' ||
		!Media::PDFUtil::ReadIndexedToken(str, length, pos, start, end) ||
		!Text::CStringNN(str + start, end - start).Equals(CSTR("/Separation")) ||
		!Media::PDFUtil::ReadIndexedToken(str, length, pos, start, end) || str[start] != '/')
		return false;

	Media::PDFUtil::SkipIndexedWhitespace(str, length, pos);
	UIntOS alternateStart = pos;
	UIntOS alternateEnd;
	if (pos < length && str[pos] == '[')
	{
		UIntOS depth = 0;
		do
		{
			if (str[pos] == '[')
				depth++;
			else if (str[pos] == ']')
				depth--;
			pos++;
		} while (pos < length && depth > 0);
		if (depth != 0)
			return false;
		alternateEnd = pos;
	}
	else
	{
		if (!Media::PDFUtil::ReadIndexedToken(str, length, pos, start, end))
			return false;
		if (str[start] >= '0' && str[start] <= '9')
		{
			pos = start;
			UInt32 alternateId;
			if (!Media::PDFUtil::ReadIndexedReference(str, length, pos, alternateId))
				return false;
		}
		alternateEnd = pos;
	}
	if (!Media::PDFUtil::ResolveIndexedBase(pdf, Text::CStringNN(str + alternateStart, alternateEnd - alternateStart), info.components, info.color, true) ||
		info.components == 0 || info.components > 4)
		return false;

	Media::PDFUtil::SkipIndexedWhitespace(str, length, pos);
	NN<Media::PDFParameter> functionParams;
	Optional<Media::PDFObject> functionObj = nullptr;
	Optional<Media::PDFParameter> ownedFunctionParams = nullptr;
	if (pos + 1 < length && str[pos] == '<' && str[pos + 1] == '<')
	{
		UIntOS functionStart = pos + 2;
		UIntOS depth = 1;
		pos += 2;
		while (pos + 1 < length && depth > 0)
		{
			if (str[pos] == '<' && str[pos + 1] == '<')
			{
				depth++;
				pos += 2;
			}
			else if (str[pos] == '>' && str[pos + 1] == '>')
			{
				depth--;
				if (depth == 0)
					break;
				pos += 2;
			}
			else
			{
				pos++;
			}
		}
		if (depth != 0)
			return false;
		if (!Media::PDFParameter::Parse(Text::CStringNN(str + functionStart, pos - functionStart)).SetTo(functionParams))
			return false;
		ownedFunctionParams = functionParams;
		pos += 2;
	}
	else
	{
		UInt32 functionId;
		if (!Media::PDFUtil::ReadIndexedReference(str, length, pos, functionId))
			return false;
		NN<Media::PDFObject> functionObject;
		if (!pdf->GetObjectById(functionId).SetTo(functionObject) || !functionObject->GetParameter().SetTo(functionParams))
			return false;
		functionObj = functionObject;
	}

	Media::PDFUtil::SkipIndexedWhitespace(str, length, pos);
	Bool valid = pos < length && str[pos++] == ']';
	Media::PDFUtil::SkipIndexedWhitespace(str, length, pos);
	valid = valid && pos == length;
	valid = valid && PDFImageParseFunction(functionParams, functionObj, info);
	NN<Media::PDFParameter> ownedParams;
	if (ownedFunctionParams.SetTo(ownedParams))
		ownedParams.Delete();
	return valid;
}

static void PDFImageEvaluateSampledFunction(const PDFSeparationInfo &info, Double input, UnsafeArray<Double> values)
{
	if (input < info.domainMin)
		input = info.domainMin;
	else if (input > info.domainMax)
		input = info.domainMax;
	Double samplePos = info.encodeMin + (input - info.domainMin) * (info.encodeMax - info.encodeMin) / (info.domainMax - info.domainMin);
	if (samplePos < 0)
		samplePos = 0;
	else if (samplePos > (Double)(info.sampleSize - 1))
		samplePos = (Double)(info.sampleSize - 1);
	UIntOS sample0 = (UIntOS)floor(samplePos);
	UIntOS sample1 = sample0 + 1 < info.sampleSize ? sample0 + 1 : sample0;
	Double fraction = samplePos - (Double)sample0;
	UnsafeArray<const UInt8> samples = info.samples.Arr();
	UInt32 maxSample = ((UInt32)1 << info.bitsPerSample) - 1;
	for (UIntOS c = 0; c < info.components; c++)
	{
		UIntOS bitPos0 = (sample0 * info.components + c) * info.bitsPerSample;
		UIntOS bitPos1 = (sample1 * info.components + c) * info.bitsPerSample;
		UInt32 raw0 = 0;
		UInt32 raw1 = 0;
		for (UIntOS bit = 0; bit < info.bitsPerSample; bit++)
		{
			raw0 = (raw0 << 1) | ((samples[(bitPos0 + bit) / 8] >> (7 - ((bitPos0 + bit) & 7))) & 1);
			raw1 = (raw1 << 1) | ((samples[(bitPos1 + bit) / 8] >> (7 - ((bitPos1 + bit) & 7))) & 1);
		}
		Double raw = (Double)raw0 + ((Double)raw1 - (Double)raw0) * fraction;
		values[c] = info.decodeRangeMin[c] + raw * (info.decodeRangeMax[c] - info.decodeRangeMin[c]) / maxSample;
		if (values[c] < info.rangeMin[c])
			values[c] = info.rangeMin[c];
		else if (values[c] > info.rangeMax[c])
			values[c] = info.rangeMax[c];
	}
}

UInt8 Media::PDFImage::IndexedHexValue(UTF8Char c)
{
	if (c >= '0' && c <= '9')
		return (UInt8)(c - '0');
	if (c >= 'A' && c <= 'F')
		return (UInt8)(c - 'A' + 10);
	if (c >= 'a' && c <= 'f')
		return (UInt8)(c - 'a' + 10);
	return 255;
}


Optional<Media::ImageList> Media::PDFImage::Parse(NN<Media::PDFDocument> pdf, UInt32 id, NN<Parser::ParserList> parsers)
{
	NN<Media::PDFObject> obj;
	if (!pdf->GetObjectById(id).SetTo(obj))
		return nullptr;
	if (!obj->IsImage())
		return nullptr;
	NN<IO::StreamData> fd;
	NN<Text::String> filter;
	if (obj->GetData().SetTo(fd))
	{
		Text::StringBuilderUTF8 sbFilter;
		Bool hasFilter = false;
		if (obj->GetFilter().SetTo(filter))
		{
			hasFilter = true;
			sbFilter.Append(filter);
			if (sbFilter.EndsWith(UTF8STRC(" R")))
			{
				
			}
		}
		if (hasFilter && filter->IndexOf(UTF8STRC("/DCTDecode")) != INVALID_INDEX)
		{
			return Optional<Media::ImageList>::ConvertFrom(pdf->SetPObjName(parsers->ParseFileType(fd, IO::ParserType::ImageList), id, CSTR(".jpg")));
		}
		else if (hasFilter && filter->IndexOf(UTF8STRC("/CCITTFaxDecode")) != INVALID_INDEX)
		{
			UIntOS width = obj->GetWidth();
			UIntOS height = obj->GetHeight();
			NN<Media::PDFParameter> params;
			NN<Text::String> paramStr;
			Int32 k = 0;
			Bool endOfLine = false;
			Bool encodedByteAlign = false;
			if (width == 0 || height == 0)
			{
				printf("PDFImage: Creating CCITT image failed, invalid dimensions\r\n");
				return nullptr;
			}
			if (obj->GetParameter().SetTo(params) && params->GetEntryValue(CSTR("DecodeParms")).SetTo(paramStr))
			{
				Optional<Media::PDFParameter> decodeParams = nullptr;
				if (!Media::PDFParameter::ParseDecodeParams(paramStr, decodeParams))
				{
					printf("PDFImage: Creating CCITT image failed, invalid DecodeParms\r\n");
					return nullptr;
				}
				NN<Media::PDFParameter> nnDecodeParams;
				if (decodeParams.SetTo(nnDecodeParams))
				{
					if (nnDecodeParams->GetEntryValue(CSTR("K")).SetTo(paramStr))
						k = paramStr->ToInt32();
					if (nnDecodeParams->GetEntryValue(CSTR("EndOfLine")).SetTo(paramStr))
						endOfLine = paramStr->Equals(UTF8STRC("true"));
					if (nnDecodeParams->GetEntryValue(CSTR("EncodedByteAlign")).SetTo(paramStr))
						encodedByteAlign = paramStr->Equals(UTF8STRC("true"));
					if (nnDecodeParams->GetEntryValue(CSTR("Columns")).SetTo(paramStr) && paramStr->ToUIntOS() != width)
					{
						printf("PDFImage: Creating CCITT image failed, Columns does not match image width\r\n");
						decodeParams.Delete();
						return nullptr;
					}
					if (nnDecodeParams->GetEntryValue(CSTR("Rows")).SetTo(paramStr) && paramStr->ToUIntOS() != 0 && paramStr->ToUIntOS() != height)
					{
						printf("PDFImage: Creating CCITT image failed, Rows does not match image height\r\n");
						decodeParams.Delete();
						return nullptr;
					}
				}
				decodeParams.Delete();
			}
			UInt64 dataSize = fd->GetDataSize();
			if (dataSize > (UInt64)INVALID_INDEX)
			{
				printf("PDFImage: Creating CCITT image failed, image data is too large\r\n");
				return nullptr;
			}
			Data::ByteBuffer dataBuff((UIntOS)dataSize);
			if (fd->GetRealData(0, dataSize, dataBuff) != dataSize)
			{
				printf("PDFImage: Creating CCITT image failed, unable to read image data: id=%d\r\n", id);
				return nullptr;
			}
			Media::CCITTFaxDecoder decoder;
			NN<Media::StaticImage> simg;
			if (!decoder.DecodeImage(dataBuff, width, height, k, endOfLine, encodedByteAlign).SetTo(simg))
			{
				printf("PDFImage: Creating CCITT image failed, invalid encoded data: id=%d\r\n", id);
				return nullptr;
			}
			NN<Media::ImageList> imgList;
			NEW_CLASSNN(imgList, Media::ImageList(CSTR("Image.ccitt")));
			imgList->AddImage(simg, 0);
			return imgList;
		}
		else if (hasFilter && filter->IndexOf(UTF8STRC("/JBIG2Decode")) != INVALID_INDEX)
		{
			UIntOS width = obj->GetWidth();
			UIntOS height = obj->GetHeight();
			if (width == 0 || height == 0 || width > INVALID_INDEX / height)
			{
				printf("PDFImage: Creating JBIG2 image failed, invalid dimensions\r\n");
				return nullptr;
			}

			Data::ByteBuffer globalData;
			Bool hasGlobalData = false;
			NN<Media::PDFParameter> params;
			NN<Text::String> paramStr;
			if (obj->GetParameter().SetTo(params) && params->GetEntryValue(CSTR("DecodeParms")).SetTo(paramStr))
			{
				Optional<Media::PDFParameter> decodeParams = nullptr;
				if (!Media::PDFParameter::ParseDecodeParams(paramStr, decodeParams))
				{
					printf("PDFImage: Creating JBIG2 image failed, invalid DecodeParms\r\n");
					return nullptr;
				}
				NN<Media::PDFParameter> nnDecodeParams;
				if (decodeParams.SetTo(nnDecodeParams))
				{
					NN<Text::String> globalRef;
					NN<Media::PDFObject> globalObj;
					NN<IO::StreamData> globalStream;
					UInt32 globalObjId;
					if (nnDecodeParams->GetEntryValue(CSTR("JBIG2Globals")).SetTo(globalRef))
					{
						if (!globalRef->EndsWith(CSTR(" 0 R")) ||
							!Text::CStringNN(globalRef->v, globalRef->leng - 4).ToUInt32(globalObjId) ||
							!pdf->GetObjectById(globalObjId).SetTo(globalObj) ||
							!globalObj->GetData().SetTo(globalStream))
						{
							printf("PDFImage: Creating JBIG2 image failed, invalid JBIG2Globals reference\r\n");
							decodeParams.Delete();
							return nullptr;
						}
						UInt64 globalSize = globalStream->GetDataSize();
						if (globalSize == 0 || globalSize > (UInt64)INVALID_INDEX)
						{
							printf("PDFImage: Creating JBIG2 image failed, invalid JBIG2Globals stream size\r\n");
							decodeParams.Delete();
							return nullptr;
						}
						globalData.ChangeSizeAndIgnoreData((UIntOS)globalSize);
						if (globalStream->GetRealData(0, globalSize, globalData) != globalSize)
						{
							printf("PDFImage: Creating JBIG2 image failed, unable to read JBIG2Globals stream\r\n");
							decodeParams.Delete();
							return nullptr;
						}
						hasGlobalData = true;
					}
				}
				decodeParams.Delete();
			}

			UInt64 dataSize = fd->GetDataSize();
			if (dataSize == 0 || dataSize > (UInt64)INVALID_INDEX)
			{
				printf("PDFImage: Creating JBIG2 image failed, invalid image data size\r\n");
				return nullptr;
			}
			Data::ByteBuffer dataBuff((UIntOS)dataSize);
			if (fd->GetRealData(0, dataSize, dataBuff) != dataSize)
			{
				printf("PDFImage: Creating JBIG2 image failed, unable to read image data: id=%d\r\n", id);
				return nullptr;
			}

			Media::JBIG2Decoder decoder;
			NN<Media::StaticImage> simg;
			if (hasGlobalData)
			{
				if (!decoder.DecodeImage(dataBuff, width, height, globalData).SetTo(simg))
				{
					printf("PDFImage: Creating JBIG2 image failed, invalid encoded data or decoder unavailable: id=%d\r\n", id);
					return nullptr;
				}
			}
			else
			{
				if (!decoder.DecodeImage(dataBuff, width, height).SetTo(simg))
				{
					printf("PDFImage: Creating JBIG2 image failed, invalid encoded data or decoder unavailable: id=%d\r\n", id);
					return nullptr;
				}
			}
			NN<Media::PDFParameter> imageParams;
			NN<Text::String> decodeArray;
			Bool reverseDecode = false;
			if (obj->GetParameter().SetTo(imageParams) && imageParams->GetEntryValue(CSTR("Decode")).SetTo(decodeArray))
			{
				reverseDecode = Media::PDFUtil::IsReverseDecodeArray(Text::CStringNN(decodeArray->v, decodeArray->leng));
			}
			if (reverseDecode)
			{
				UIntOS bpl = simg->GetDataBpl();
				UIntOS y = 0;
				while (y < height)
				{
					UIntOS x = 0;
					while (x < width)
					{
						simg->data[y * bpl + x] = (UInt8)(255 - simg->data[y * bpl + x]);
						x++;
					}
					y++;
				}
			}
			NN<Media::ImageList> imgList;
			NEW_CLASSNN(imgList, Media::ImageList(CSTR("Image.jbig2")));
			imgList->AddImage(simg, 0);
			return imgList;
		}
		else
		{
			UIntOS bpc = obj->GetBitsPerComponent();
			UIntOS width = obj->GetWidth();
			UIntOS height = obj->GetHeight();
			NN<Text::String> colorSpace;
			NN<Media::StaticImage> simg;
			NN<Media::ImageList> imgList;
			if (bpc == 0 || width == 0 || height == 0)
			{
				printf("PDFImage: Creating image failed, invalid image parameters: bpc=%d, width=%d, height=%d\r\n", (Int32)bpc, (Int32)width, (Int32)height);
				return nullptr;
			}
			if ((bpc == 1 || bpc == 2 || bpc == 4 || bpc == 8 || bpc == 16) && obj->GetColorSpace().SetTo(colorSpace))
			{
				Text::StringBuilderUTF8 sbColorSpace;
				sbColorSpace.Append(colorSpace);
				PDFUtil::ResolveReference(pdf, sbColorSpace);

				UnsafeArray<const UTF8Char> csStr = sbColorSpace.v;
				UIntOS csLen = sbColorSpace.leng;
				UIntOS csPos = 0;
				PDFUtil::SkipIndexedWhitespace(csStr, csLen, csPos);
				if (csPos < csLen && csStr[csPos] == '[')
				{
					csPos++;
					UIntOS kindStart;
					UIntOS kindEnd;
					Bool hasKind = PDFUtil::ReadIndexedToken(csStr, csLen, csPos, kindStart, kindEnd);
					if (hasKind && Text::CStringNN(csStr + kindStart, kindEnd - kindStart).Equals(CSTR("/Separation")))
					{
						PDFSeparationInfo separation;
						NN<Media::PDFParameter> imageParams;
						NN<Text::String> decodeArray;
						if (!PDFImageParseSeparation(pdf, sbColorSpace.ToCString(), separation))
						{
							printf("PDFImage: Creating Separation image failed, unsupported or invalid color space or tint function\r\n");
							return nullptr;
						}
						if (obj->GetParameter().SetTo(imageParams) && imageParams->GetEntryValue(CSTR("Decode")).SetTo(decodeArray))
						{
							Double decode[2];
							UIntOS decodePos = 0;
							if (!PDFImageReadNumberArray(decodeArray->v, decodeArray->leng, decodePos, decode, 2))
							{
								printf("PDFImage: Creating Separation image failed, invalid Decode array\r\n");
								return nullptr;
							}
							PDFUtil::SkipIndexedWhitespace(decodeArray->v, decodeArray->leng, decodePos);
							if (decodePos != decodeArray->leng)
							{
								printf("PDFImage: Creating Separation image failed, invalid Decode array\r\n");
								return nullptr;
							}
							separation.decodeMin = decode[0];
							separation.decodeMax = decode[1];
						}
						if (width > (INVALID_INDEX - 7) / bpc || width > INVALID_INDEX / 3 ||
							height > INVALID_INDEX / width || width * height > INVALID_INDEX / 3)
						{
							printf("PDFImage: Separation image dimensions are too large\r\n");
							return nullptr;
						}
						UIntOS rowSize = (width * bpc + 7) / 8;
						if (height > INVALID_INDEX / rowSize || fd->GetDataSize() != rowSize * height)
						{
							printf("PDFImage: Separation image has unexpected data size\r\n");
							return nullptr;
						}
						NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UIntOS>(width, height), 0, 24, Media::PixelFormat::PF_B8G8R8, width * height * 3, separation.color, Media::ColorProfile::YUVT_UNKNOWN, Media::AlphaType::AT_IGNORE_ALPHA, Media::YCOFST_C_CENTER_LEFT));
						Data::ByteBuffer rowData(rowSize);
						UInt32 maxSample = ((UInt32)1 << bpc) - 1;
						for (UIntOS y = 0; y < height; y++)
						{
							if (fd->GetRealData(y * rowSize, rowSize, rowData) != rowSize)
							{
								printf("PDFImage: Unable to read Separation image data\r\n");
								simg.Delete();
								return nullptr;
							}
							UnsafeArray<UInt8> dest = simg->data + y * simg->GetDataBpl();
							for (UIntOS x = 0; x < width; x++)
							{
								UIntOS bitPos = x * bpc;
								UInt32 sample = bpc == 16 ? ((UInt32)rowData[bitPos / 8] << 8) | rowData[bitPos / 8 + 1] :
									(rowData[bitPos / 8] >> (8 - bpc - (bitPos & 7))) & maxSample;
								Double values[4];
								if (separation.functionType == 0)
								{
									Double tint = separation.decodeMin + (Double)sample * (separation.decodeMax - separation.decodeMin) / maxSample;
									PDFImageEvaluateSampledFunction(separation, tint, values);
								}
								else if (separation.functionType == 4)
								{
									Double tint = separation.decodeMin + (Double)sample * (separation.decodeMax - separation.decodeMin) / maxSample;
									if (tint < separation.domainMin)
										tint = separation.domainMin;
									else if (tint > separation.domainMax)
										tint = separation.domainMax;
									if (!PDFImageEvaluateType4Function(separation, tint, values))
									{
										printf("PDFImage: Invalid Separation calculator function output\r\n");
										simg.Delete();
										return nullptr;
									}
								}
								else
								{
									Double tint = separation.decodeMin + (Double)sample * (separation.decodeMax - separation.decodeMin) / maxSample;
									if (tint < separation.domainMin)
										tint = separation.domainMin;
									else if (tint > separation.domainMax)
										tint = separation.domainMax;
									Double t = pow((tint - separation.domainMin) / (separation.domainMax - separation.domainMin), separation.exponent);
									for (UIntOS c = 0; c < separation.components; c++)
									{
										values[c] = separation.c0[c] + (separation.c1[c] - separation.c0[c]) * t;
										if (separation.hasRange)
										{
											if (values[c] < separation.rangeMin[c])
												values[c] = separation.rangeMin[c];
											else if (values[c] > separation.rangeMax[c])
												values[c] = separation.rangeMax[c];
										}
										if (values[c] < 0)
											values[c] = 0;
										else if (values[c] > 1)
											values[c] = 1;
									}
								}
								if (separation.functionType == 0)
								{
									for (UIntOS c = 0; c < separation.components; c++)
									{
										if (values[c] < 0)
											values[c] = 0;
										else if (values[c] > 1)
											values[c] = 1;
									}
								}
								Double r;
								Double g;
								Double b;
								if (separation.components == 1)
								{
									r = g = b = values[0];
								}
								else if (separation.components == 3)
								{
									r = values[0];
									g = values[1];
									b = values[2];
								}
								else
								{
									r = (1 - values[0]) * (1 - values[3]);
									g = (1 - values[1]) * (1 - values[3]);
									b = (1 - values[2]) * (1 - values[3]);
								}
								dest[x * 3] = (UInt8)(b * 255 + 0.5);
								dest[x * 3 + 1] = (UInt8)(g * 255 + 0.5);
								dest[x * 3 + 2] = (UInt8)(r * 255 + 0.5);
							}
						}
						NEW_CLASSNN(imgList, Media::ImageList(CSTR("Image.separation")));
						imgList->AddImage(simg, 0);
						return imgList;
					}
					if (hasKind && Text::CStringNN(csStr + kindStart, kindEnd - kindStart).Equals(CSTR("/ICCBased")))
					{
						Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
						UIntOS components = 0;
						NN<Media::PDFParameter> params;
						NN<Text::String> decode;
						if (!PDFUtil::ResolveIndexedBase(pdf, sbColorSpace.ToCString(), components, color) || components != 3 ||
							(obj->GetParameter().SetTo(params) && params->GetEntryValue(CSTR("Decode")).SetTo(decode) &&
								!PDFUtil::IsDefaultDecodeArray(decode->ToCString(), components)))
						{
							printf("PDFImage: Unsupported or invalid ICCBased color space or Decode array\r\n");
							return nullptr;
						}
						if (width > (INVALID_INDEX - 7) / (3 * bpc) || width > INVALID_INDEX / 3 ||
							height > (INVALID_INDEX - 4) / (width * 3))
						{
							printf("PDFImage: ICCBased image dimensions are too large\r\n");
							return nullptr;
						}
						UIntOS rowSize = (width * 3 * bpc + 7) / 8;
						if (height > INVALID_INDEX / rowSize || fd->GetDataSize() < rowSize * height)
						{
							printf("PDFImage: ICCBased image has unexpected data size\r\n");
							return nullptr;
						}
						NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UIntOS>(width, height), 0, 24, Media::PixelFormat::PF_B8G8R8, width * height * 3, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AlphaType::AT_IGNORE_ALPHA, Media::YCOFST_C_CENTER_LEFT));
						Data::ByteBuffer rowData(rowSize);
						UInt32 maxSample = ((UInt32)1 << bpc) - 1;
						for (UIntOS y = 0; y < height; y++)
						{
							if (fd->GetRealData(y * rowSize, rowSize, rowData) != rowSize)
							{
								printf("PDFImage: Unable to read ICCBased image data\r\n");
								simg.Delete();
								return nullptr;
							}
							UnsafeArray<UInt8> dest = simg->data + y * simg->GetDataBpl();
							for (UIntOS x = 0; x < width; x++)
							{
								for (UIntOS c = 0; c < 3; c++)
								{
									UIntOS bitPos = (x * 3 + c) * bpc;
									UInt32 sample = bpc == 16 ? ((UInt32)rowData[bitPos / 8] << 8) | rowData[bitPos / 8 + 1] :
										(rowData[bitPos / 8] >> (8 - bpc - (bitPos & 7))) & maxSample;
									dest[x * 3 + 2 - c] = (UInt8)((sample * 255 + maxSample / 2) / maxSample);
								}
							}
						}
						NEW_CLASSNN(imgList, Media::ImageList(CSTR("Image.icc")));
						imgList->AddImage(simg, 0);
						return imgList;
					}
					if (hasKind && bpc != 16 &&
						(Text::CStringNN(csStr + kindStart, kindEnd - kindStart).Equals(CSTR("/Indexed")) ||
							Text::CStringNN(csStr + kindStart, kindEnd - kindStart).Equals(CSTR("/I"))))
					{
						UIntOS baseStart;
						UIntOS baseEnd;
						UIntOS highStart;
						UIntOS highEnd;
						UIntOS lookupStart;
						UIntOS lookupEnd;
						UIntOS lookupType = 0;
						UIntOS baseComponents = 0;
						UIntOS high = 0;
						Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
						PDFUtil::SkipIndexedWhitespace(csStr, csLen, csPos);
						baseStart = csPos;
						Bool valid = false;
						if (csPos < csLen && csStr[csPos] == '[')
						{
							csPos++;
							while (csPos < csLen && csStr[csPos] != ']')
								csPos++;
							if (csPos < csLen)
							{
								csPos++;
								valid = true;
							}
						}
						else if (PDFUtil::ReadIndexedToken(csStr, csLen, csPos, baseStart, baseEnd))
						{
							if (csStr[baseStart] == '/')
								valid = true;
							else
							{
								UInt32 baseId;
								csPos = baseStart;
								valid = PDFUtil::ReadIndexedReference(csStr, csLen, csPos, baseId);
							}
						}
						if (valid)
							valid = PDFUtil::ResolveIndexedBase(pdf, Text::CStringNN(csStr + baseStart, csPos - baseStart), baseComponents, color);
						if (!valid)
							printf("PDFImage: Creating Indexed image failed, unsupported or invalid base color space\r\n");
						if (valid && (!PDFUtil::ReadIndexedToken(csStr, csLen, csPos, highStart, highEnd) ||
							!PDFUtil::ParseIndexedUInt(csStr, highStart, highEnd, high) || high > 255))
							valid = false;
						UIntOS paletteSize = (high + 1) * baseComponents;
						Data::ByteBuffer paletteData(paletteSize);
						PDFUtil::SkipIndexedWhitespace(csStr, csLen, csPos);
						lookupStart = csPos;
						lookupEnd = csPos;
						if (valid && csPos < csLen && csStr[csPos] == '<')
						{
							lookupType = 1;
							csPos++;
							while (csPos < csLen && csStr[csPos] != '>')
								csPos++;
							if (csPos >= csLen)
								valid = false;
							else
								lookupEnd = ++csPos;
						}
						else if (valid && csPos < csLen && csStr[csPos] == '(')
						{
							lookupType = 2;
							UIntOS depth = 1;
							csPos++;
							while (csPos < csLen && depth > 0)
							{
								if (csStr[csPos] == '\\')
								{
									csPos += csPos + 1 < csLen ? 2 : 1;
								}
								else if (csStr[csPos] == '(')
								{
									depth++;
									csPos++;
								}
								else if (csStr[csPos] == ')')
								{
									depth--;
									csPos++;
								}
								else
								{
									csPos++;
								}
							}
							if (depth != 0)
								valid = false;
							else
								lookupEnd = csPos;
						}
						else if (valid && PDFUtil::ReadIndexedToken(csStr, csLen, csPos, lookupStart, lookupEnd))
						{
							lookupType = 3;
						}
						else if (valid)
						{
							valid = false;
						}
						NN<IO::StreamData> lookupStream;
						if (valid && lookupType == 1)
						{
							UIntOS outPos = 0;
							Bool hasHighNibble = false;
							UInt8 highNibble = 0;
							UIntOS i = lookupStart + 1;
							while (i + 1 < lookupEnd)
							{
								UTF8Char c = csStr[i++];
								if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f')
									continue;
								UInt8 nibble = IndexedHexValue(c);
								if (nibble == 255)
								{
									valid = false;
									break;
								}
								if (!hasHighNibble)
								{
									highNibble = nibble;
									hasHighNibble = true;
								}
								else
								{
									if (outPos >= paletteSize)
									{
										valid = false;
										break;
									}
									paletteData[outPos++] = (UInt8)((highNibble << 4) | nibble);
									hasHighNibble = false;
								}
							}
							if (valid && hasHighNibble)
							{
								if (outPos >= paletteSize)
									valid = false;
								else
									paletteData[outPos++] = (UInt8)(highNibble << 4);
							}
							if (outPos != paletteSize)
								valid = false;
						}
						else if (valid && lookupType == 2)
						{
							UIntOS outPos = 0;
							UIntOS i = lookupStart + 1;
							while (i + 1 < lookupEnd)
							{
								UInt8 c = csStr[i++];
								if (c == '\\' && i + 1 < lookupEnd)
								{
									c = csStr[i++];
									if (c >= '0' && c <= '7')
									{
										UInt16 value = (UInt16)(c - '0');
										UIntOS octalCount = 1;
										while (octalCount < 3 && i + 1 < lookupEnd && csStr[i] >= '0' && csStr[i] <= '7')
										{
											value = (UInt16)(value * 8 + csStr[i++] - '0');
											octalCount++;
										}
										c = (UInt8)value;
									}
									else if (c == 'n') c = '\n';
									else if (c == 'r') c = '\r';
									else if (c == 't') c = '\t';
									else if (c == 'b') c = '\b';
									else if (c == 'f') c = '\f';
									else if (c == '\r' || c == '\n')
									{
										if (c == '\r' && i + 1 < lookupEnd && csStr[i] == '\n')
											i++;
										continue;
									}
								}
								if (outPos >= paletteSize)
								{
									valid = false;
									break;
								}
								paletteData[outPos++] = c;
							}
							if (outPos != paletteSize)
								valid = false;
						}
						else if (valid)
						{
							UIntOS lookupId;
							if (!PDFUtil::ParseIndexedUInt(csStr, lookupStart, lookupEnd, lookupId) || lookupId > 0xffffffffU)
								valid = false;
							UIntOS genStart;
							UIntOS genEnd;
							UIntOS rStart;
							UIntOS rEnd;
							UIntOS generation;
							if (valid && (!PDFUtil::ReadIndexedToken(csStr, csLen, csPos, genStart, genEnd) ||
								!PDFUtil::ParseIndexedUInt(csStr, genStart, genEnd, generation) ||
								!PDFUtil::ReadIndexedToken(csStr, csLen, csPos, rStart, rEnd) ||
								!Text::CStringNN(csStr + rStart, rEnd - rStart).Equals(CSTR("R"))))
								valid = false;
							NN<Media::PDFObject> lookupObj;
							if (valid && (!pdf->GetObjectById((UInt32)lookupId).SetTo(lookupObj) || !lookupObj->GetData().SetTo(lookupStream) ||
								lookupStream->GetDataSize() < paletteSize ||
								lookupStream->GetRealData(0, paletteSize, paletteData) != paletteSize))
								valid = false;
							(void)generation;
						}
						PDFUtil::SkipIndexedWhitespace(csStr, csLen, csPos);
						if (valid && (csPos >= csLen || csStr[csPos++] != ']'))
							valid = false;
						PDFUtil::SkipIndexedWhitespace(csStr, csLen, csPos);
						if (valid && csPos != csLen)
							valid = false;
						if (!valid)
						{
							printf("PDFImage: Creating Indexed image failed, invalid color space or lookup table: %s\r\n", sbColorSpace.v.Ptr());
							return nullptr;
						}
						UIntOS maxSample = ((UIntOS)1 << bpc) - 1;
						if (high > maxSample)
						{
							printf("PDFImage: Creating Indexed image failed, hival exceeds sample range\r\n");
							return nullptr;
						}
						if (width > (INVALID_INDEX - 7) / bpc)
						{
							printf("PDFImage: Creating Indexed image failed, width exceeds sample range\r\n");
							return nullptr;
						}
						UIntOS rowSize = (width * bpc + 7) / 8;
						if (height > INVALID_INDEX / rowSize || width > INVALID_INDEX / height ||
							fd->GetDataSize() != rowSize * height)
						{
							printf("PDFImage: Creating Indexed image failed, unexpected data size: width=%llu, height=%llu, expected=%llu, actual=%llu\r\n", (UInt64)width, (UInt64)height, (UInt64)(rowSize * height), (UInt64)fd->GetDataSize());
							return nullptr;
						}
						NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UIntOS>(width, height), 0, 8, Media::PixelFormat::PF_PAL_W8, width * height, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AlphaType::AT_IGNORE_ALPHA, Media::YCOFST_C_CENTER_LEFT));
						UnsafeArray<UInt8> pal;
						Data::ByteBuffer imageData(rowSize * height);
						if (!simg->pal.SetTo(pal) || fd->GetRealData(0, rowSize * height, imageData) != rowSize * height)
						{
							simg.Delete();
							printf("PDFImage: Creating Indexed image failed, unable to set palette or read image data\r\n");
							return nullptr;
						}
						UIntOS p = 0;
						while (p < 256)
						{
							pal[p * 4] = 0;
							pal[p * 4 + 1] = 0;
							pal[p * 4 + 2] = 0;
							pal[p * 4 + 3] = 255;
							p++;
						}
						p = 0;
						while (p <= high)
						{
							UInt8 r;
							UInt8 g;
							UInt8 b;
							if (baseComponents == 1)
							{
								r = g = b = paletteData[p];
							}
							else if (baseComponents == 3)
							{
								r = paletteData[p * 3];
								g = paletteData[p * 3 + 1];
								b = paletteData[p * 3 + 2];
							}
							else
							{
								UInt8 c = paletteData[p * 4];
								UInt8 m = paletteData[p * 4 + 1];
								UInt8 y = paletteData[p * 4 + 2];
								UInt8 k = paletteData[p * 4 + 3];
								r = (UInt8)(((255 - c) * (255 - k)) / 255);
								g = (UInt8)(((255 - m) * (255 - k)) / 255);
								b = (UInt8)(((255 - y) * (255 - k)) / 255);
							}
							pal[p * 4] = b;
							pal[p * 4 + 1] = g;
							pal[p * 4 + 2] = r;
							p++;
						}
						UIntOS y = 0;
						Bool validSamples = true;
						while (y < height)
						{
							UIntOS x = 0;
							while (x < width)
							{
								UIntOS bitPos = x * bpc;
								UIntOS shift = 8 - bpc - (bitPos & 7);
								UIntOS sample = (imageData[y * rowSize + bitPos / 8] >> shift) & maxSample;
								if (sample > high)
								{
									validSamples = false;
									break;
								}
								simg->data[y * width + x] = (UInt8)sample;
								x++;
							}
							if (!validSamples)
								break;
							y++;
						}
						if (!validSamples)
						{
							printf("PDFImage: Creating Indexed image failed, sample exceeds hival\r\n");
							simg.Delete();
							return nullptr;
						}
						NEW_CLASSNN(imgList, Media::ImageList(CSTR("Image.indexed")));
						imgList->AddImage(simg, 0);
						return imgList;
					}
				}

				if (bpc == 1 && (sbColorSpace.Equals(UTF8STRC("/DeviceGray")) || sbColorSpace.Equals(UTF8STRC("/G"))))
				{
					UIntOS bpl = (width + 7) / 8;
					if (fd->GetDataSize() == bpl * height)
					{
						Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
						NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UIntOS>(width, height), 0, 1, Media::PixelFormat::PF_PAL_1, bpl * height, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AlphaType::AT_IGNORE_ALPHA, Media::YCOFST_C_CENTER_LEFT));
						if (fd->GetRealData(0, bpl * height, simg->GetDataArray()) != bpl * height)
						{
							simg.Delete();
							printf("PDFImage: Creating Indexed image failed, unable to read image data\r\n");
							return nullptr;
						}
						UnsafeArray<UInt8> pal;
						if (simg->pal.SetTo(pal))
						{
							pal[0] = 0;
							pal[1] = 0;
							pal[2] = 0;
							pal[3] = 255;
							pal[4] = 255;
							pal[5] = 255;
							pal[6] = 255;
							pal[7] = 255;
						}
						NEW_CLASSNN(imgList, Media::ImageList(CSTR("Image.raw")));
						imgList->AddImage(simg, 0);
						return imgList;
					}
				}
				else if (bpc == 8 && sbColorSpace.Equals(UTF8STRC("/DeviceGray")))
				{
					if (fd->GetDataSize() == (width * height))
					{
						Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
						NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UIntOS>(width, height), 0, 8, Media::PixelFormat::PF_PAL_W8, width * height, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AlphaType::AT_IGNORE_ALPHA, Media::YCOFST_C_CENTER_LEFT));
						fd->GetRealData(0, width * height, simg->GetDataArray());
						UnsafeArray<UInt8> pal;
						if (simg->pal.SetTo(pal))
						{
							UIntOS i = 0;
							UIntOS j = 256;
							while (i < j)
							{
								pal[i * 4] = (UInt8)i;
								pal[i * 4 + 1] = (UInt8)i;
								pal[i * 4 + 2] = (UInt8)i;
								pal[i * 4 + 3] = 255;
								i++;
							}
						}
						NEW_CLASSNN(imgList, Media::ImageList(CSTR("Image.raw")));
						imgList->AddImage(simg, 0);
						return imgList;
					}
					else
					{
						printf("PDFImage: Creating Gray image failed, unexpected data size, width=%d, height=%d, size=%llu\r\n", (UInt32)width, (UInt32)height, fd->GetDataSize());
					}
				}
				else if (bpc == 8 && sbColorSpace.Equals(UTF8STRC("/DeviceCMYK")))
				{
					if (fd->GetDataSize() == (width * height * 4))
					{
						Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
						NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UIntOS>(width, height), 0, 24, Media::PixelFormat::PF_B8G8R8, width * height, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AlphaType::AT_IGNORE_ALPHA, Media::YCOFST_C_CENTER_LEFT));
						UnsafeArray<UInt8> cmyk = MemAllocArr(UInt8, width * 4);
						UnsafeArray<UInt8> bytePtr = simg->data;
						UIntOS ofst = 0;
						UIntOS currH = 0;
						while (currH < height)
						{
							fd->GetRealData(ofst, width * 4, Data::ByteArray(cmyk, width * 4));
							UIntOS i = 0;
							while (i < width)
							{
								UInt8 c = cmyk[i * 4];
								UInt8 m = cmyk[i * 4 + 1];
								UInt8 y = cmyk[i * 4 + 2];
								UInt8 k = cmyk[i * 4 + 3];
								bytePtr[i * 3] = (UInt8)(((255 - y) * (255 - k)) / 255);
								bytePtr[i * 3 + 1] = (UInt8)(((255 - m) * (255 - k)) / 255);
								bytePtr[i * 3 + 2] = (UInt8)(((255 - c) * (255 - k)) / 255);
								i++;
							}
							bytePtr += width * 3;
							ofst += width * 4;
							currH++;
						}
						MemFreeArr(cmyk);
						NEW_CLASSNN(imgList, Media::ImageList(CSTR("Image.raw")));
						imgList->AddImage(simg, 0);
						return imgList;
					}
					else
					{
						printf("PDFImage: Creating CMYK image failed, unexpected data size, width=%d, height=%d, size=%llu\r\n", (UInt32)width, (UInt32)height, fd->GetDataSize());
					}
				}
				else if (bpc == 8 && sbColorSpace.Equals(UTF8STRC("/DeviceRGB")))
				{
					if (fd->GetDataSize() == (width * height * 3))
					{
						Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
						NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UIntOS>(width, height), 0, 24, Media::PixelFormat::PF_B8G8R8, width * height, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AlphaType::AT_IGNORE_ALPHA, Media::YCOFST_C_CENTER_LEFT));
						Data::ByteBuffer rgb(width * 3);
						UIntOS row = 0;
						while (row < height)
						{
							if (fd->GetRealData(row * width * 3, width * 3, rgb) != width * 3)
							{
								printf("PDFImage: Unable to read RGB image data\r\n");
								simg.Delete();
								return nullptr;
							}
							UnsafeArray<UInt8> bytePtr = simg->data + row * simg->GetDataBpl();
							UIntOS x = 0;
							while (x < width)
							{
								bytePtr[x * 3] = rgb[x * 3 + 2];
								bytePtr[x * 3 + 1] = rgb[x * 3 + 1];
								bytePtr[x * 3 + 2] = rgb[x * 3];
								x++;
							}
							row++;
						}
						NEW_CLASSNN(imgList, Media::ImageList(CSTR("Image.raw")));
						imgList->AddImage(simg, 0);
						return imgList;
					}
					else
					{
						printf("PDFImage: Creating RGB image failed, unexpected data size, width=%d, height=%d, size=%llu\r\n", (UInt32)width, (UInt32)height, fd->GetDataSize());
					}
				}
				else
				{
					printf("PDFImage: Creating image failed, unsupported color space: %s\r\n", sbColorSpace.v.Ptr());
				}
			}
			else if (bpc == 1)
			{
				UIntOS bpl = (width + 7) / 8;
				if (fd->GetDataSize() == (bpl * height))
				{
					Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
					NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UIntOS>(width, height), 0, 1, Media::PixelFormat::PF_PAL_1, bpl * height, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AlphaType::AT_IGNORE_ALPHA, Media::YCOFST_C_CENTER_LEFT));
					fd->GetRealData(0, bpl * height, simg->GetDataArray());
					UnsafeArray<UInt8> pal;
					if (simg->pal.SetTo(pal))
					{
						pal[0] = 0;
						pal[1] = 0;
						pal[2] = 0;
						pal[3] = 255;
						pal[4] = 255;
						pal[5] = 255;
						pal[6] = 255;
						pal[7] = 255;
					}
					NEW_CLASSNN(imgList, Media::ImageList(CSTR("Image.raw")));
					imgList->AddImage(simg, 0);
					return imgList;
				}
				else
				{
					printf("PDFImage: Creating Gray image failed, unexpected data size\r\n");
				}
			}
			else
			{
				printf("PDFImage: Creating image failed, unsupported bit per component: bpc=%d, width=%d, height=%d, size=%llu\r\n", (UInt32)bpc, (UInt32)width, (UInt32)height, fd->GetDataSize());
			}
		}
	}
	return nullptr;
}
