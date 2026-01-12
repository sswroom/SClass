#include "Stdafx.h"
#include "IO/ProtocolBuffersMessage.h"

void __stdcall IO::ProtocolBuffersMessage::FreeFieldInfo(NN<FieldInfo> fieldInfo)
{
	NN<Text::String> str;
	NN<Data::ByteBuffer> bytes;
	fieldInfo->name->Release();
	if (fieldInfo->fieldType == ProtocolBuffersUtil::FieldType::String && fieldInfo->val.strVal.SetTo(str))
	{
		str->Release();
	}
	else if (fieldInfo->fieldType == ProtocolBuffersUtil::FieldType::Bytes && fieldInfo->val.bytesVal.SetTo(bytes))
	{
		bytes.Delete();
	}
	else if (fieldInfo->fieldType == ProtocolBuffersUtil::FieldType::SubMsg)
	{
		fieldInfo->val.subMsg.Delete();
	}
	fieldInfo.Delete();
}

IO::ProtocolBuffersMessage::ProtocolBuffersMessage(Text::CStringNN name)
{
	this->name = Text::String::New(name);
}

IO::ProtocolBuffersMessage::~ProtocolBuffersMessage()
{
	this->name->Release();
	this->fieldMap.FreeAll(FreeFieldInfo);
}

void IO::ProtocolBuffersMessage::AddInt64(Bool required, Text::CStringNN name, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::Int64;
	fieldInfo->required = required;
	fieldInfo->name = Text::String::New(name);
	fieldInfo->valCount = 0;
	fieldInfo->val.strVal = nullptr;
	this->fieldMap.Put((UInt32)id, fieldInfo);
}

void IO::ProtocolBuffersMessage::AddSInt64(Bool required, Text::CStringNN name, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::SInt64;
	fieldInfo->required = required;
	fieldInfo->name = Text::String::New(name);
	fieldInfo->valCount = 0;
	fieldInfo->val.strVal = nullptr;
	this->fieldMap.Put((UInt32)id, fieldInfo);
}

void IO::ProtocolBuffersMessage::AddUInt64(Bool required, Text::CStringNN name, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::UInt64;
	fieldInfo->required = required;
	fieldInfo->name = Text::String::New(name);
	fieldInfo->valCount = 0;
	fieldInfo->val.strVal = nullptr;
	this->fieldMap.Put((UInt32)id, fieldInfo);
}

void IO::ProtocolBuffersMessage::AddInt32(Bool required, Text::CStringNN name, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::Int32;
	fieldInfo->required = required;
	fieldInfo->name = Text::String::New(name);
	fieldInfo->valCount = 0;
	fieldInfo->val.strVal = nullptr;
	this->fieldMap.Put((UInt32)id, fieldInfo);
}

void IO::ProtocolBuffersMessage::AddSInt32(Bool required, Text::CStringNN name, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::SInt32;
	fieldInfo->required = required;
	fieldInfo->name = Text::String::New(name);
	fieldInfo->valCount = 0;
	fieldInfo->val.strVal = nullptr;
	this->fieldMap.Put((UInt32)id, fieldInfo);
}

void IO::ProtocolBuffersMessage::AddUInt32(Bool required, Text::CStringNN name, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::UInt32;
	fieldInfo->required = required;
	fieldInfo->name = Text::String::New(name);
	fieldInfo->valCount = 0;
	fieldInfo->val.strVal = nullptr;
	this->fieldMap.Put((UInt32)id, fieldInfo);
}

void IO::ProtocolBuffersMessage::AddBool(Bool required, Text::CStringNN name, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::Bool;
	fieldInfo->required = required;
	fieldInfo->name = Text::String::New(name);
	fieldInfo->valCount = 0;
	fieldInfo->val.strVal = nullptr;
	this->fieldMap.Put((UInt32)id, fieldInfo);
}

void IO::ProtocolBuffersMessage::AddEnum(Bool required, Text::CStringNN name, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::Enum;
	fieldInfo->required = required;
	fieldInfo->name = Text::String::New(name);
	fieldInfo->valCount = 0;
	fieldInfo->val.strVal = nullptr;
	this->fieldMap.Put((UInt32)id, fieldInfo);
}

void IO::ProtocolBuffersMessage::AddFixed64(Bool required, Text::CStringNN name, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::Fixed64;
	fieldInfo->required = required;
	fieldInfo->name = Text::String::New(name);
	fieldInfo->valCount = 0;
	fieldInfo->val.strVal = nullptr;
	this->fieldMap.Put((UInt32)id, fieldInfo);
}

void IO::ProtocolBuffersMessage::AddSFixed64(Bool required, Text::CStringNN name, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::SFixed64;
	fieldInfo->required = required;
	fieldInfo->name = Text::String::New(name);
	fieldInfo->valCount = 0;
	fieldInfo->val.strVal = nullptr;
	this->fieldMap.Put((UInt32)id, fieldInfo);
}

void IO::ProtocolBuffersMessage::AddFixed32(Bool required, Text::CStringNN name, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::Fixed32;
	fieldInfo->required = required;
	fieldInfo->name = Text::String::New(name);
	fieldInfo->valCount = 0;
	fieldInfo->val.strVal = nullptr;
	this->fieldMap.Put((UInt32)id, fieldInfo);
}

void IO::ProtocolBuffersMessage::AddSFixed32(Bool required, Text::CStringNN name, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::SFixed32;
	fieldInfo->required = required;
	fieldInfo->name = Text::String::New(name);
	fieldInfo->valCount = 0;
	fieldInfo->val.strVal = nullptr;
	this->fieldMap.Put((UInt32)id, fieldInfo);
}

void IO::ProtocolBuffersMessage::AddFloat(Bool required, Text::CStringNN name, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::Float;
	fieldInfo->required = required;
	fieldInfo->name = Text::String::New(name);
	fieldInfo->valCount = 0;
	fieldInfo->val.strVal = nullptr;
	this->fieldMap.Put((UInt32)id, fieldInfo);
}

void IO::ProtocolBuffersMessage::AddDouble(Bool required, Text::CStringNN name, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::Double;
	fieldInfo->required = required;
	fieldInfo->name = Text::String::New(name);
	fieldInfo->valCount = 0;
	fieldInfo->val.strVal = nullptr;
	this->fieldMap.Put((UInt32)id, fieldInfo);
}

void IO::ProtocolBuffersMessage::AddString(Bool required, Text::CStringNN name, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::String;
	fieldInfo->required = required;
	fieldInfo->name = Text::String::New(name);
	fieldInfo->valCount = 0;
	fieldInfo->val.strVal = nullptr;
	this->fieldMap.Put((UInt32)id, fieldInfo);
}

void IO::ProtocolBuffersMessage::AddBytes(Bool required, Text::CStringNN name, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::Bytes;
	fieldInfo->required = required;
	fieldInfo->name = Text::String::New(name);
	fieldInfo->valCount = 0;
	fieldInfo->val.strVal = nullptr;
	this->fieldMap.Put((UInt32)id, fieldInfo);
}

void IO::ProtocolBuffersMessage::AddSubMessage(Bool required, Text::CStringNN name, UInt8 id, NN<ProtocolBuffersMessage> subMsg)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::SubMsg;
	fieldInfo->required = required;
	fieldInfo->name = Text::String::New(name);
	fieldInfo->valCount = 0;
	fieldInfo->val.subMsg = subMsg;
	this->fieldMap.Put((UInt32)id, fieldInfo);
}

void IO::ProtocolBuffersMessage::ClearValues()
{
	UOSInt i = this->fieldMap.GetCount();
	while (i-- > 0)
	{
		NN<FieldInfo> fieldInfo = this->fieldMap.GetItemNoCheck(i);
		if (fieldInfo->fieldType == ProtocolBuffersUtil::FieldType::String)
		{
			OPTSTR_DEL(fieldInfo->val.strVal);
		}
		else if (fieldInfo->fieldType == ProtocolBuffersUtil::FieldType::Bytes)
		{
			fieldInfo->val.bytesVal.Delete();
		}
		else if (fieldInfo->fieldType == ProtocolBuffersUtil::FieldType::SubMsg)
		{
			NN<ProtocolBuffersMessage> subMsg;
			if (fieldInfo->val.subMsg.SetTo(subMsg))
			{
				subMsg->ClearValues();
			}
		}
		fieldInfo->valCount = 0;
	}
}

Bool IO::ProtocolBuffersMessage::GetInt32(UInt8 id, OutParam<Int32> val) const
{
	NN<FieldInfo> fieldInfo;
	if (!this->fieldMap.Get((UInt32)id).SetTo(fieldInfo))
	{
		return false;
	}
	if (fieldInfo->fieldType != ProtocolBuffersUtil::FieldType::Int32)
	{
		return false;
	}
	if (fieldInfo->valCount == 0)
	{
		return false;
	}
	val.Set(fieldInfo->val.int32Val);
	return true;
}

Bool IO::ProtocolBuffersMessage::GetInt64(UInt8 id, OutParam<Int64> val) const
{
	NN<FieldInfo> fieldInfo;
	if (!this->fieldMap.Get((UInt32)id).SetTo(fieldInfo))
	{
		return false;
	}
	if (fieldInfo->fieldType != ProtocolBuffersUtil::FieldType::Int64)
	{
		return false;
	}
	if (fieldInfo->valCount == 0)
	{
		return false;
	}
	val.Set(fieldInfo->val.int64Val);
	return true;
}

Bool IO::ProtocolBuffersMessage::GetUInt32(UInt8 id, OutParam<UInt32> val) const
{
	NN<FieldInfo> fieldInfo;
	if (!this->fieldMap.Get((UInt32)id).SetTo(fieldInfo))
	{
		return false;
	}
	if (fieldInfo->fieldType != ProtocolBuffersUtil::FieldType::UInt32)
	{
		return false;
	}
	if (fieldInfo->valCount == 0)
	{
		return false;
	}
	val.Set(fieldInfo->val.uint32Val);
	return true;
}

Bool IO::ProtocolBuffersMessage::GetUInt64(UInt8 id, OutParam<UInt64> val) const
{
	NN<FieldInfo> fieldInfo;
	if (!this->fieldMap.Get((UInt32)id).SetTo(fieldInfo))
	{
		return false;
	}
	if (fieldInfo->fieldType != ProtocolBuffersUtil::FieldType::UInt64)
	{
		return false;
	}
	if (fieldInfo->valCount == 0)
	{
		return false;
	}
	val.Set(fieldInfo->val.uint64Val);
	return true;
}

Bool IO::ProtocolBuffersMessage::GetString(UInt8 id, OutParam<NN<Text::String>> val) const
{
	NN<FieldInfo> fieldInfo;
	if (!this->fieldMap.Get((UInt32)id).SetTo(fieldInfo))
	{
		return false;
	}
	if (fieldInfo->fieldType != ProtocolBuffersUtil::FieldType::String)
	{
		return false;
	}
	if (fieldInfo->valCount == 0)
	{
		return false;
	}
	NN<Text::String> str;
	if (!fieldInfo->val.strVal.SetTo(str))
	{
		return false;
	}
	val.Set(str);
	return true;
}

Bool IO::ProtocolBuffersMessage::GetBytes(UInt8 id, OutParam<NN<Data::ByteBuffer>> val) const
{
	NN<FieldInfo> fieldInfo;
	if (!this->fieldMap.Get((UInt32)id).SetTo(fieldInfo))
	{
		return false;
	}
	if (fieldInfo->fieldType != ProtocolBuffersUtil::FieldType::Bytes)
	{
		return false;
	}
	if (fieldInfo->valCount == 0)
	{
		return false;
	}
	NN<Data::ByteBuffer> bytes;
	if (!fieldInfo->val.bytesVal.SetTo(bytes))
	{
		return false;
	}
	val.Set(bytes);
	return true;
}

Bool IO::ProtocolBuffersMessage::GetFixed32(UInt8 id, OutParam<UInt32> val) const
{
	NN<FieldInfo> fieldInfo;
	if (!this->fieldMap.Get((UInt32)id).SetTo(fieldInfo))
	{
		return false;
	}
	if (fieldInfo->fieldType != ProtocolBuffersUtil::FieldType::Fixed32)
	{
		return false;
	}
	if (fieldInfo->valCount == 0)
	{
		return false;
	}
	val.Set(fieldInfo->val.uint32Val);
	return true;
}

Bool IO::ProtocolBuffersMessage::GetFixed64(UInt8 id, OutParam<UInt64> val) const
{
	NN<FieldInfo> fieldInfo;
	if (!this->fieldMap.Get((UInt32)id).SetTo(fieldInfo))
	{
		return false;
	}
	if (fieldInfo->fieldType != ProtocolBuffersUtil::FieldType::Fixed64)
	{
		return false;
	}
	if (fieldInfo->valCount == 0)
	{
		return false;
	}
	val.Set(fieldInfo->val.uint64Val);
	return true;
}

Bool IO::ProtocolBuffersMessage::GetSFixed32(UInt8 id, OutParam<Int32> val) const
{
	NN<FieldInfo> fieldInfo;
	if (!this->fieldMap.Get((UInt32)id).SetTo(fieldInfo))
	{
		return false;
	}
	if (fieldInfo->fieldType != ProtocolBuffersUtil::FieldType::SFixed32)
	{
		return false;
	}
	if (fieldInfo->valCount == 0)
	{
		return false;
	}
	val.Set(fieldInfo->val.int32Val);
	return true;
}

Bool IO::ProtocolBuffersMessage::GetSFixed64(UInt8 id, OutParam<Int64> val) const
{
	NN<FieldInfo> fieldInfo;
	if (!this->fieldMap.Get((UInt32)id).SetTo(fieldInfo))
	{
		return false;
	}
	if (fieldInfo->fieldType != ProtocolBuffersUtil::FieldType::SFixed64)
	{
		return false;
	}
	if (fieldInfo->valCount == 0)
	{
		return false;
	}
	val.Set(fieldInfo->val.int64Val);
	return true;
}

Bool IO::ProtocolBuffersMessage::GetFloat(UInt8 id, OutParam<Single> val) const
{
	NN<FieldInfo> fieldInfo;
	if (!this->fieldMap.Get((UInt32)id).SetTo(fieldInfo))
	{
		return false;
	}
	if (fieldInfo->fieldType != ProtocolBuffersUtil::FieldType::Float)
	{
		return false;
	}
	if (fieldInfo->valCount == 0)
	{
		return false;
	}
	val.Set(fieldInfo->val.floatVal);
	return true;
}

Bool IO::ProtocolBuffersMessage::GetDouble(UInt8 id, OutParam<Double> val) const
{
	NN<FieldInfo> fieldInfo;
	if (!this->fieldMap.Get((UInt32)id).SetTo(fieldInfo))
	{
		return false;
	}
	if (fieldInfo->fieldType != ProtocolBuffersUtil::FieldType::Double)
	{
		return false;
	}
	if (fieldInfo->valCount == 0)
	{
		return false;
	}
	val.Set(fieldInfo->val.doubleVal);
	return true;
}

Bool IO::ProtocolBuffersMessage::GetBool(UInt8 id, OutParam<Bool> val) const
{
	NN<FieldInfo> fieldInfo;
	if (!this->fieldMap.Get((UInt32)id).SetTo(fieldInfo))
	{
		return false;
	}
	if (fieldInfo->fieldType != ProtocolBuffersUtil::FieldType::Bool)
	{
		return false;
	}
	if (fieldInfo->valCount == 0)
	{
		return false;
	}
	val.Set(fieldInfo->val.boolVal);
	return true;
}

Bool IO::ProtocolBuffersMessage::GetEnum(UInt8 id, OutParam<Int32> val) const
{
	NN<FieldInfo> fieldInfo;
	if (!this->fieldMap.Get((UInt32)id).SetTo(fieldInfo))
	{
		return false;
	}
	if (fieldInfo->fieldType != ProtocolBuffersUtil::FieldType::Enum)
	{
		return false;
	}
	if (fieldInfo->valCount == 0)
	{
		return false;
	}
	val.Set(fieldInfo->val.int32Val);
	return true;
}

NN<IO::ProtocolBuffersMessage> IO::ProtocolBuffersMessage::Clone() const
{
	NN<ProtocolBuffersMessage> newMsg;
	NN<ProtocolBuffersMessage> subMsg;
	NEW_CLASSNN(newMsg, ProtocolBuffersMessage(this->name->ToCString()));
	UOSInt i = 0;
	UOSInt j = this->fieldMap.GetCount();
	while (i < j)
	{
		NN<FieldInfo> fieldInfo = this->fieldMap.GetItemNoCheck(i);
		switch (fieldInfo->fieldType)
		{
		case ProtocolBuffersUtil::FieldType::Int64:
			newMsg->AddInt64(fieldInfo->required, fieldInfo->name->ToCString(), fieldInfo->id);
			break;
		case ProtocolBuffersUtil::FieldType::SInt64:
			newMsg->AddSInt64(fieldInfo->required, fieldInfo->name->ToCString(), fieldInfo->id);
			break;
		case ProtocolBuffersUtil::FieldType::UInt64:
			newMsg->AddUInt64(fieldInfo->required, fieldInfo->name->ToCString(), fieldInfo->id);
			break;
		case ProtocolBuffersUtil::FieldType::Int32:
			newMsg->AddInt32(fieldInfo->required, fieldInfo->name->ToCString(), fieldInfo->id);
			break;
		case ProtocolBuffersUtil::FieldType::SInt32:
			newMsg->AddSInt32(fieldInfo->required, fieldInfo->name->ToCString(), fieldInfo->id);
			break;
		case ProtocolBuffersUtil::FieldType::UInt32:
			newMsg->AddUInt32(fieldInfo->required, fieldInfo->name->ToCString(), fieldInfo->id);
			break;
		case ProtocolBuffersUtil::FieldType::Bool:
			newMsg->AddBool(fieldInfo->required, fieldInfo->name->ToCString(), fieldInfo->id);
			break;
		case ProtocolBuffersUtil::FieldType::Enum:
			newMsg->AddEnum(fieldInfo->required, fieldInfo->name->ToCString(), fieldInfo->id);
			break;
		case ProtocolBuffersUtil::FieldType::Fixed64:
			newMsg->AddFixed64(fieldInfo->required, fieldInfo->name->ToCString(), fieldInfo->id);
			break;
		case ProtocolBuffersUtil::FieldType::SFixed64:
			newMsg->AddSFixed64(fieldInfo->required, fieldInfo->name->ToCString(), fieldInfo->id);
			break;
		case ProtocolBuffersUtil::FieldType::Fixed32:
			newMsg->AddFixed32(fieldInfo->required, fieldInfo->name->ToCString(), fieldInfo->id);
			break;
		case ProtocolBuffersUtil::FieldType::SFixed32:
			newMsg->AddSFixed32(fieldInfo->required, fieldInfo->name->ToCString(), fieldInfo->id);
			break;
		case ProtocolBuffersUtil::FieldType::Float:
			newMsg->AddFloat(fieldInfo->required, fieldInfo->name->ToCString(), fieldInfo->id);
			break;
		case ProtocolBuffersUtil::FieldType::Double:
			newMsg->AddDouble(fieldInfo->required, fieldInfo->name->ToCString(), fieldInfo->id);
			break;
		case ProtocolBuffersUtil::FieldType::String:
			newMsg->AddString(fieldInfo->required, fieldInfo->name->ToCString(), fieldInfo->id);
			break;
		case ProtocolBuffersUtil::FieldType::Bytes:
			newMsg->AddBytes(fieldInfo->required, fieldInfo->name->ToCString(), fieldInfo->id);
			break;
		case ProtocolBuffersUtil::FieldType::SubMsg:
			if (fieldInfo->val.subMsg.SetTo(subMsg))
				newMsg->AddSubMessage(fieldInfo->required, fieldInfo->name->ToCString(), fieldInfo->id, subMsg->Clone());
			break;
		}
		i++;
	}
	return newMsg;
}

Bool IO::ProtocolBuffersMessage::ParseMsssage(NN<IO::FileAnalyse::FrameDetail> frame, UnsafeArray<const UInt8> buff, UOSInt buffOfst, UOSInt buffSize)
{
	NN<ProtocolBuffersMessage> subMsg;
	UOSInt buffEnd = buffOfst + buffSize;
	UOSInt nextBuffOfst;
	while (buffOfst < buffEnd)
	{
		UInt8 tag = buff[buffOfst];
		UInt8 fieldNum = tag >> 3;
		UInt8 wireType = tag & 0x07;
		UInt64 v;
		Int64 iv;
		NN<FieldInfo> fieldInfo;

		frame->AddUIntName(buffOfst, 1, CSTR("Wire Type"), wireType, ProtocolBuffersUtil::WireTypeGetName(wireType));
		if (!this->fieldMap.Get((UInt32)fieldNum).SetTo(fieldInfo))
		{
			frame->AddUInt(buffOfst, 1, CSTR("Field Number"), fieldNum);
			return false;
		}
		else
		{
			frame->AddUIntName(buffOfst, 1, CSTR("Field Number"), fieldNum, fieldInfo->name->ToCString());
		}
		
		buffOfst++;
		switch (fieldInfo->fieldType)
		{
		case ProtocolBuffersUtil::FieldType::Int64:
		case ProtocolBuffersUtil::FieldType::Int32:
		case ProtocolBuffersUtil::FieldType::Enum:
			if (wireType != 0)
			{
				return false;
			}
			nextBuffOfst = ProtocolBuffersUtil::ReadVarInt(buff, buffOfst, iv);
			if (nextBuffOfst > buffEnd)
			{
				return false;
			}
			if (fieldInfo->valCount > 1)
			{
				return false;
			}
			fieldInfo->valCount++;
			frame->AddInt(buffOfst, nextBuffOfst - buffOfst, fieldInfo->name->ToCString(), iv);
			fieldInfo->val.int64Val = iv;
			buffOfst = nextBuffOfst;
			break;
		case ProtocolBuffersUtil::FieldType::SInt64:
		case ProtocolBuffersUtil::FieldType::SInt32:
			if (wireType != 0)
			{
				return false;
			}
			nextBuffOfst = ProtocolBuffersUtil::ReadVarSInt(buff, buffOfst, iv);
			if (nextBuffOfst > buffEnd)
			{
				return false;
			}
			if (fieldInfo->valCount > 1)
			{
				return false;
			}
			fieldInfo->valCount++;
			frame->AddInt(buffOfst, nextBuffOfst - buffOfst, fieldInfo->name->ToCString(), iv);
			fieldInfo->val.int64Val = iv;
			buffOfst = nextBuffOfst;
			break;
		case ProtocolBuffersUtil::FieldType::UInt64:
		case ProtocolBuffersUtil::FieldType::UInt32:
			if (wireType != 0)
			{
				return false;
			}
			nextBuffOfst = ProtocolBuffersUtil::ReadVarUInt(buff, buffOfst, v);
			if (nextBuffOfst > buffEnd)
			{
				return false;
			}
			if (fieldInfo->valCount > 1)
			{
				return false;
			}
			fieldInfo->valCount++;
			frame->AddUInt(buffOfst, nextBuffOfst - buffOfst, fieldInfo->name->ToCString(), v);
			fieldInfo->val.uint64Val = v;
			buffOfst = nextBuffOfst;
			break;
		case ProtocolBuffersUtil::FieldType::Bool:
			if (wireType != 0)
			{
				return false;
			}
			nextBuffOfst = ProtocolBuffersUtil::ReadVarUInt(buff, buffOfst, v);
			if (nextBuffOfst > buffEnd)
			{
				return false;
			}
			if (fieldInfo->valCount > 1)
			{
				return false;
			}
			fieldInfo->valCount++;
			frame->AddBool(buffOfst, fieldInfo->name->ToCString(), (UInt8)v);
			fieldInfo->val.boolVal = (Bool)v;
			buffOfst = nextBuffOfst;
			break;
		case ProtocolBuffersUtil::FieldType::Fixed64:
			if (wireType != 1)
			{
				return false;
			}
			if (buffOfst + 8 > buffEnd)
			{
				return false;
			}
			if (fieldInfo->valCount > 1)
			{
				return false;
			}
			fieldInfo->valCount++;
			frame->AddUInt64(buffOfst, fieldInfo->name->ToCString(), ReadMUInt64(&buff[buffOfst]));
			fieldInfo->val.uint64Val = ReadMUInt64(&buff[buffOfst]);
			buffOfst += 8;
			break;
		case ProtocolBuffersUtil::FieldType::SFixed64:
			if (wireType != 1)
			{
				return false;
			}
			if (buffOfst + 8 > buffEnd)
			{
				return false;
			}
			if (fieldInfo->valCount > 1)
			{
				return false;
			}
			fieldInfo->valCount++;
			frame->AddInt64(buffOfst, fieldInfo->name->ToCString(), ReadMInt64(&buff[buffOfst]));
			fieldInfo->val.int64Val = ReadMInt64(&buff[buffOfst]);
			buffOfst += 8;
			break;
		case ProtocolBuffersUtil::FieldType::Double:
			if (wireType != 1)
			{
				return false;
			}
			if (buffOfst + 8 > buffEnd)
			{
				return false;
			}
			if (fieldInfo->valCount > 1)
			{
				return false;
			}
			fieldInfo->valCount++;
			frame->AddFloat(buffOfst, 8, fieldInfo->name->ToCString(), ReadMDouble(&buff[buffOfst]));
			fieldInfo->val.doubleVal = ReadMDouble(&buff[buffOfst]);
			buffOfst += 8;
			break;
		case ProtocolBuffersUtil::FieldType::Fixed32:
			if (wireType != 5)
			{
				return false;
			}
			if (buffOfst + 4 > buffEnd)
			{
				return false;
			}
			if (fieldInfo->valCount > 1)
			{
				return false;
			}
			fieldInfo->valCount++;
			frame->AddUInt(buffOfst, 4, fieldInfo->name->ToCString(), ReadMUInt32(&buff[buffOfst]));
			fieldInfo->val.uint64Val = ReadMUInt32(&buff[buffOfst]);
			buffOfst += 4;
			break;
		case ProtocolBuffersUtil::FieldType::SFixed32:
			if (wireType != 5)
			{
				return false;
			}
			if (buffOfst + 4 > buffEnd)
			{
				return false;
			}
			if (fieldInfo->valCount > 1)
			{
				return false;
			}
			fieldInfo->valCount++;
			frame->AddInt(buffOfst, 4, fieldInfo->name->ToCString(), ReadMInt32(&buff[buffOfst]));
			fieldInfo->val.int32Val = ReadMInt32(&buff[buffOfst]);
			buffOfst += 4;
			break;
		case ProtocolBuffersUtil::FieldType::Float:
			if (wireType != 5)
			{
				return false;
			}
			if (fieldInfo->valCount > 1)
			{
				return false;
			}
			fieldInfo->valCount++;
			frame->AddFloat(buffOfst, 4, fieldInfo->name->ToCString(), ReadMFloat(&buff[buffOfst]));
			fieldInfo->val.floatVal = ReadMFloat(&buff[buffOfst]);
			buffOfst += 4;
			break;
		case ProtocolBuffersUtil::FieldType::String:
			if (wireType != 2)
			{
				return false;
			}
			nextBuffOfst = ProtocolBuffersUtil::ReadVarUInt(buff, buffOfst, v);
			if (nextBuffOfst + v > buffEnd)
			{
				return false;
			}
			if (fieldInfo->valCount > 1)
			{
				return false;
			}
			fieldInfo->valCount++;
			frame->AddUInt(buffOfst, nextBuffOfst - buffOfst, CSTR("String Length"), (UOSInt)v);
			frame->AddStrC(nextBuffOfst, (UOSInt)v, fieldInfo->name->ToCString(), &buff[nextBuffOfst]);
			OPTSTR_DEL(fieldInfo->val.strVal);
			fieldInfo->val.strVal = Text::String::New(&buff[nextBuffOfst], (UOSInt)v);
			buffOfst = nextBuffOfst + (UOSInt)v;
			break;
		case ProtocolBuffersUtil::FieldType::Bytes:
			if (wireType != 2)
			{
				return false;
			}
			nextBuffOfst = ProtocolBuffersUtil::ReadVarUInt(buff, buffOfst, v);
			if (nextBuffOfst + v > buffEnd)
			{
				return false;
			}
			if (fieldInfo->valCount > 1)
			{
				return false;
			}
			fieldInfo->valCount++;
			frame->AddUInt(buffOfst, nextBuffOfst - buffOfst, CSTR("Bytes Length"), (UOSInt)v);
			frame->AddHexBuff(nextBuffOfst, fieldInfo->name->ToCString(), Data::ByteArrayR(&buff[nextBuffOfst], (UOSInt)v), true);
			fieldInfo->val.bytesVal.Delete();
			NEW_CLASSOPT(fieldInfo->val.bytesVal, Data::ByteBuffer(Data::ByteArrayR(&buff[nextBuffOfst], (UOSInt)v)));
			buffOfst = nextBuffOfst + (UOSInt)v;
			break;
		case ProtocolBuffersUtil::FieldType::SubMsg:
			if (wireType != 2)
			{
				return false;
			}
			nextBuffOfst = ProtocolBuffersUtil::ReadVarUInt(buff, buffOfst, v);
			if (nextBuffOfst + v > buffEnd)
			{
				return false;
			}
			if (fieldInfo->valCount > 1)
			{
				return false;
			}
			fieldInfo->valCount++;
			frame->AddUInt(buffOfst, nextBuffOfst - buffOfst, CSTR("Sub Message Length"), (UOSInt)v);
			if (!fieldInfo->val.subMsg.SetTo(subMsg))
			{
				return false;
			}
			else
			{
				if (!subMsg->ParseMsssage(frame, buff, nextBuffOfst, (UOSInt)v))
				{
					return false;
				}
			}
			buffOfst = nextBuffOfst + (UOSInt)v;
			break;
		default:
			return false;
		}
	}
	return true;
}

void IO::ProtocolBuffersMessage::ToString(NN<Text::StringBuilderUTF8> sb, UOSInt level)
{
	NN<ProtocolBuffersMessage> subMsg;
	sb->AppendChar('\t', level);
	sb->Append(CSTR("message "));
	sb->Append(this->name);
	sb->Append(CSTR(" {\r\n"));
	UOSInt i = 0;
	UOSInt j = this->fieldMap.GetCount();
	while (i < j)
	{
		NN<FieldInfo> fieldInfo = this->fieldMap.GetItemNoCheck(i);
		sb->AppendChar('\t', level + 1);
		if (fieldInfo->required)
		{
			sb->Append(CSTR("required "));
		}
		else
		{
			sb->Append(CSTR("optional "));
		}
		switch (fieldInfo->fieldType)
		{
		case ProtocolBuffersUtil::FieldType::Int64:
			sb->Append(CSTR("int64 "));
			break;
		case ProtocolBuffersUtil::FieldType::SInt64:
			sb->Append(CSTR("sint64 "));
			break;
		case ProtocolBuffersUtil::FieldType::UInt64:
			sb->Append(CSTR("uint64 "));
			break;
		case ProtocolBuffersUtil::FieldType::Int32:
			sb->Append(CSTR("int32 "));
			break;
		case ProtocolBuffersUtil::FieldType::SInt32:
			sb->Append(CSTR("sint32 "));
			break;
		case ProtocolBuffersUtil::FieldType::UInt32:
			sb->Append(CSTR("uint32 "));
			break;
		case ProtocolBuffersUtil::FieldType::Bool:
			sb->Append(CSTR("bool "));
			break;
		case ProtocolBuffersUtil::FieldType::Enum:
			sb->Append(CSTR("enum "));
			break;
		case ProtocolBuffersUtil::FieldType::Fixed64:
			sb->Append(CSTR("fixed64 "));
			break;
		case ProtocolBuffersUtil::FieldType::SFixed64:
			sb->Append(CSTR("sfixed64 "));
			break;
		case ProtocolBuffersUtil::FieldType::Fixed32:
			sb->Append(CSTR("fixed32 "));
			break;
		case ProtocolBuffersUtil::FieldType::SFixed32:
			sb->Append(CSTR("sfixed32 "));
			break;
		case ProtocolBuffersUtil::FieldType::Float:
			sb->Append(CSTR("float "));
			break;
		case ProtocolBuffersUtil::FieldType::Double:
			sb->Append(CSTR("double "));
			break;
		case ProtocolBuffersUtil::FieldType::String:
			sb->Append(CSTR("string "));
			break;
		case ProtocolBuffersUtil::FieldType::Bytes:
			sb->Append(CSTR("bytes "));
			break;
		case ProtocolBuffersUtil::FieldType::SubMsg:
			if (fieldInfo->val.subMsg.SetTo(subMsg))
			{
				sb->Append(subMsg->name);
			}
			sb->AppendUTF8Char(' ');
			break;
		}
		sb->Append(fieldInfo->name);
		sb->Append(CSTR(" = "));
		sb->AppendU32(fieldInfo->id);
		sb->Append(CSTR(";\r\n"));
		i++;
	}
	sb->AppendChar('\t', level);
	sb->Append(CSTR("}\r\n"));
}
