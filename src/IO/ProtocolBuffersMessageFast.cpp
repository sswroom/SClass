#include "Stdafx.h"
#include "IO/ProtocolBuffersMessageFast.h"
#include "IO/ProtocolBuffersUtil.h"

// https://wiki.openstreetmap.org/wiki/PBF_Format
// https://protobuf.dev/programming-guides/encoding/

void IO::ProtocolBuffersMessageFast::FreeFieldInfo(NN<IO::ProtocolBuffersMessageFast::FieldInfo> fieldInfo)
{
	NN<Text::String> str;
	NN<Data::ByteBuffer> bytes;
	if (fieldInfo->fieldType == ProtocolBuffersUtil::FieldType::String && fieldInfo->val.strVal.SetTo(str))
	{
		str->Release();
	}
	else if (fieldInfo->fieldType == ProtocolBuffersUtil::FieldType::Bytes && fieldInfo->val.bytesVal.SetTo(bytes))
	{
		bytes.Delete();
	}
	fieldInfo.Delete();
}

IO::ProtocolBuffersMessageFast::ProtocolBuffersMessageFast()
{
}

IO::ProtocolBuffersMessageFast::~ProtocolBuffersMessageFast()
{
	this->fieldMap.FreeAll(FreeFieldInfo);
}

void IO::ProtocolBuffersMessageFast::AddInt32(Bool required, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::Int32;
	fieldInfo->required = required;
	fieldInfo->valExist = false;
	fieldInfo->val.strVal = nullptr;
	if (this->fieldMap.Put((UInt32)id, fieldInfo).SetTo(fieldInfo))
	{
		FreeFieldInfo(fieldInfo);
	}
}

void IO::ProtocolBuffersMessageFast::AddInt64(Bool required, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::Int64;
	fieldInfo->required = required;
	fieldInfo->valExist = false;
	fieldInfo->val.strVal = nullptr;
	if (this->fieldMap.Put((UInt32)id, fieldInfo).SetTo(fieldInfo))
	{
		FreeFieldInfo(fieldInfo);
	}
}

void IO::ProtocolBuffersMessageFast::AddSInt32(Bool required, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::SInt32;
	fieldInfo->required = required;
	fieldInfo->valExist = false;
	fieldInfo->val.strVal = nullptr;
	if (this->fieldMap.Put((UInt32)id, fieldInfo).SetTo(fieldInfo))
	{
		FreeFieldInfo(fieldInfo);
	}
}

void IO::ProtocolBuffersMessageFast::AddSInt64(Bool required, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::SInt64;
	fieldInfo->required = required;
	fieldInfo->valExist = false;
	fieldInfo->val.strVal = nullptr;
	if (this->fieldMap.Put((UInt32)id, fieldInfo).SetTo(fieldInfo))
	{
		FreeFieldInfo(fieldInfo);
	}
}

void IO::ProtocolBuffersMessageFast::AddUInt32(Bool required, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::UInt32;
	fieldInfo->required = required;
	fieldInfo->valExist = false;
	fieldInfo->val.strVal = nullptr;
	if (this->fieldMap.Put((UInt32)id, fieldInfo).SetTo(fieldInfo))
	{
		FreeFieldInfo(fieldInfo);
	}
}

void IO::ProtocolBuffersMessageFast::AddUInt64(Bool required, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::UInt64;
	fieldInfo->required = required;
	fieldInfo->valExist = false;
	fieldInfo->val.strVal = nullptr;
	if (this->fieldMap.Put((UInt32)id, fieldInfo).SetTo(fieldInfo))
	{
		FreeFieldInfo(fieldInfo);
	}
}

void IO::ProtocolBuffersMessageFast::AddString(Bool required, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::String;
	fieldInfo->required = required;
	fieldInfo->valExist = false;
	fieldInfo->val.strVal = nullptr;
	if (this->fieldMap.Put((UInt32)id, fieldInfo).SetTo(fieldInfo))
	{
		FreeFieldInfo(fieldInfo);
	}
}

void IO::ProtocolBuffersMessageFast::AddBytes(Bool required, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::Bytes;
	fieldInfo->required = required;
	fieldInfo->valExist = false;
	fieldInfo->val.strVal = nullptr;
	if (this->fieldMap.Put((UInt32)id, fieldInfo).SetTo(fieldInfo))
	{
		FreeFieldInfo(fieldInfo);
	}
}

void IO::ProtocolBuffersMessageFast::AddFixed32(Bool required, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::Fixed32;
	fieldInfo->required = required;
	fieldInfo->valExist = false;
	fieldInfo->val.strVal = nullptr;
	if (this->fieldMap.Put((UInt32)id, fieldInfo).SetTo(fieldInfo))
	{
		FreeFieldInfo(fieldInfo);
	}
}

void IO::ProtocolBuffersMessageFast::AddFixed64(Bool required, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::Fixed64;
	fieldInfo->required = required;
	fieldInfo->valExist = false;
	fieldInfo->val.strVal = nullptr;
	if (this->fieldMap.Put((UInt32)id, fieldInfo).SetTo(fieldInfo))
	{
		FreeFieldInfo(fieldInfo);
	}
}

void IO::ProtocolBuffersMessageFast::AddSFixed32(Bool required, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::SFixed32;
	fieldInfo->required = required;
	fieldInfo->valExist = false;
	fieldInfo->val.strVal = nullptr;
	if (this->fieldMap.Put((UInt32)id, fieldInfo).SetTo(fieldInfo))
	{
		FreeFieldInfo(fieldInfo);
	}
}

void IO::ProtocolBuffersMessageFast::AddSFixed64(Bool required, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::SFixed64;
	fieldInfo->required = required;
	fieldInfo->valExist = false;
	fieldInfo->val.strVal = nullptr;
	if (this->fieldMap.Put((UInt32)id, fieldInfo).SetTo(fieldInfo))
	{
		FreeFieldInfo(fieldInfo);
	}
}

void IO::ProtocolBuffersMessageFast::AddFloat(Bool required, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::Float;
	fieldInfo->required = required;
	fieldInfo->valExist = false;
	fieldInfo->val.strVal = nullptr;
	if (this->fieldMap.Put((UInt32)id, fieldInfo).SetTo(fieldInfo))
	{
		FreeFieldInfo(fieldInfo);
	}
}

void IO::ProtocolBuffersMessageFast::AddDouble(Bool required, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::Double;
	fieldInfo->required = required;
	fieldInfo->valExist = false;
	fieldInfo->val.strVal = nullptr;
	if (this->fieldMap.Put((UInt32)id, fieldInfo).SetTo(fieldInfo))
	{
		FreeFieldInfo(fieldInfo);
	}
}

void IO::ProtocolBuffersMessageFast::AddBool(Bool required, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::Bool;
	fieldInfo->required = required;
	fieldInfo->valExist = false;
	fieldInfo->val.strVal = nullptr;
	if (this->fieldMap.Put((UInt32)id, fieldInfo).SetTo(fieldInfo))
	{
		FreeFieldInfo(fieldInfo);
	}
}

void IO::ProtocolBuffersMessageFast::AddEnum(Bool required, UInt8 id)
{
	NN<FieldInfo> fieldInfo;
	NEW_CLASSNN(fieldInfo, FieldInfo());
	fieldInfo->id = id;
	fieldInfo->fieldType = ProtocolBuffersUtil::FieldType::Enum;
	fieldInfo->required = required;
	fieldInfo->valExist = false;
	fieldInfo->val.strVal = nullptr;
	if (this->fieldMap.Put((UInt32)id, fieldInfo).SetTo(fieldInfo))
	{
		FreeFieldInfo(fieldInfo);
	}
}

void IO::ProtocolBuffersMessageFast::ClearValues()
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
			NN<IO::ProtocolBuffersMessageFast> subMsgVal;
			if (fieldInfo->val.subMsgVal.SetTo(subMsgVal))
			{
				subMsgVal->ClearValues();
			}
		}
		fieldInfo->valExist = false;
	}
}

Bool IO::ProtocolBuffersMessageFast::GetInt32(UInt8 id, OutParam<Int32> val) const
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
	if (!fieldInfo->valExist)
	{
		return false;
	}
	val.Set(fieldInfo->val.int32Val);
	return true;
}

Bool IO::ProtocolBuffersMessageFast::GetInt64(UInt8 id, OutParam<Int64> val) const
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
	if (!fieldInfo->valExist)
	{
		return false;
	}
	val.Set(fieldInfo->val.int64Val);
	return true;
}

Bool IO::ProtocolBuffersMessageFast::GetUInt32(UInt8 id, OutParam<UInt32> val) const
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
	if (!fieldInfo->valExist)
	{
		return false;
	}
	val.Set(fieldInfo->val.uint32Val);
	return true;
}

Bool IO::ProtocolBuffersMessageFast::GetUInt64(UInt8 id, OutParam<UInt64> val) const
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
	if (!fieldInfo->valExist)
	{
		return false;
	}
	val.Set(fieldInfo->val.uint64Val);
	return true;
}

Bool IO::ProtocolBuffersMessageFast::GetString(UInt8 id, OutParam<NN<Text::String>> val) const
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
	if (!fieldInfo->valExist)
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

Bool IO::ProtocolBuffersMessageFast::GetBytes(UInt8 id, OutParam<NN<Data::ByteBuffer>> val) const
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
	if (!fieldInfo->valExist)
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

Bool IO::ProtocolBuffersMessageFast::GetFixed32(UInt8 id, OutParam<UInt32> val) const
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
	if (!fieldInfo->valExist)
	{
		return false;
	}
	val.Set(fieldInfo->val.uint32Val);
	return true;
}

Bool IO::ProtocolBuffersMessageFast::GetFixed64(UInt8 id, OutParam<UInt64> val) const
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
	if (!fieldInfo->valExist)
	{
		return false;
	}
	val.Set(fieldInfo->val.uint64Val);
	return true;
}

Bool IO::ProtocolBuffersMessageFast::GetSFixed32(UInt8 id, OutParam<Int32> val) const
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
	if (!fieldInfo->valExist)
	{
		return false;
	}
	val.Set(fieldInfo->val.int32Val);
	return true;
}

Bool IO::ProtocolBuffersMessageFast::GetSFixed64(UInt8 id, OutParam<Int64> val) const
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
	if (!fieldInfo->valExist)
	{
		return false;
	}
	val.Set(fieldInfo->val.int64Val);
	return true;
}

Bool IO::ProtocolBuffersMessageFast::GetFloat(UInt8 id, OutParam<Single> val) const
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
	if (!fieldInfo->valExist)
	{
		return false;
	}
	val.Set(fieldInfo->val.floatVal);
	return true;
}

Bool IO::ProtocolBuffersMessageFast::GetDouble(UInt8 id, OutParam<Double> val) const
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
	if (!fieldInfo->valExist)
	{
		return false;
	}
	val.Set(fieldInfo->val.doubleVal);
	return true;
}

Bool IO::ProtocolBuffersMessageFast::GetBool(UInt8 id, OutParam<Bool> val) const
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
	if (!fieldInfo->valExist)
	{
		return false;
	}
	val.Set(fieldInfo->val.boolVal);
	return true;
}

Bool IO::ProtocolBuffersMessageFast::GetEnum(UInt8 id, OutParam<Int32> val) const
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
	if (!fieldInfo->valExist)
	{
		return false;
	}
	val.Set(fieldInfo->val.int32Val);
	return true;
}

Bool IO::ProtocolBuffersMessageFast::ParseMsssage(UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
	NN<IO::ProtocolBuffersMessageFast> subMsgVal;
	NN<FieldInfo> fieldInfo;
	UInt64 v;
	Int64 iv;
	UOSInt ofst = 0;
	while (ofst < buffSize)
	{
		UInt8 key = buff[ofst++];
		UInt8 fieldNum = key >> 3;
		UInt8 wireType = key & 0x07;
		if (!this->fieldMap.Get((UInt32)fieldNum).SetTo(fieldInfo))
		{
			return false;
		}
		switch (fieldInfo->fieldType)
		{
		case ProtocolBuffersUtil::FieldType::Int32:
			if (wireType != 0)
				return false;
			if (fieldInfo->valExist)
			{
				return false;
			}
			ofst = ProtocolBuffersUtil::ReadVarInt(buff, ofst, iv);
			fieldInfo->val.int32Val = (Int32)iv;
			fieldInfo->valExist = true;
			break;
		case ProtocolBuffersUtil::FieldType::Int64:
			if (wireType != 0)
				return false;
			if (fieldInfo->valExist)
			{
				return false;
			}
			ofst = ProtocolBuffersUtil::ReadVarInt(buff, ofst, iv);
			fieldInfo->val.int64Val = iv;
			fieldInfo->valExist = true;
			break;
		case ProtocolBuffersUtil::FieldType::SInt32:
			if (wireType != 0)
				return false;
			if (fieldInfo->valExist)
			{
				return false;
			}
			ofst = ProtocolBuffersUtil::ReadVarSInt(buff, ofst, iv);
			fieldInfo->val.int32Val = (Int32)iv;
			fieldInfo->valExist = true;
			break;
		case ProtocolBuffersUtil::FieldType::SInt64:
			if (wireType != 0)
				return false;
			if (fieldInfo->valExist)
			{
				return false;
			}
			ofst = ProtocolBuffersUtil::ReadVarSInt(buff, ofst, iv);
			fieldInfo->val.int64Val = iv;
			fieldInfo->valExist = true;
			break;
		case ProtocolBuffersUtil::FieldType::UInt32:
			if (wireType != 0)
				return false;
			if (fieldInfo->valExist)
			{
				return false;
			}
			ofst = ProtocolBuffersUtil::ReadVarUInt(buff, ofst, v);
			fieldInfo->val.uint32Val = (UInt32)v;
			fieldInfo->valExist = true;
			break;
		case ProtocolBuffersUtil::FieldType::UInt64:
			if (wireType != 0)
				return false;
			if (fieldInfo->valExist)
			{
				return false;
			}
			ofst = ProtocolBuffersUtil::ReadVarUInt(buff, ofst, v);
			fieldInfo->val.uint64Val = v;
			fieldInfo->valExist = true;
			break;
		case ProtocolBuffersUtil::FieldType::Bool:
			if (wireType != 0)
				return false;
			if (fieldInfo->valExist)
			{
				return false;
			}
			ofst = ProtocolBuffersUtil::ReadVarUInt(buff, ofst, v);
			fieldInfo->val.boolVal = v != 0;
			fieldInfo->valExist = true;
			break;
		case ProtocolBuffersUtil::FieldType::Enum:
			if (wireType != 0)
				return false;
			if (fieldInfo->valExist)
			{
				return false;
			}
			ofst = ProtocolBuffersUtil::ReadVarUInt(buff, ofst, v);
			fieldInfo->val.int32Val = (Int32)v;
			fieldInfo->valExist = true;
			break;
		case ProtocolBuffersUtil::FieldType::Fixed32:
			if (wireType != 5)
				return false;
			if (fieldInfo->valExist)
			{
				return false;
			}
			if (ofst + 4 > buffSize)
				return false;
			fieldInfo->val.uint32Val = ReadMUInt32(&buff[ofst]);
			ofst += 4;
			fieldInfo->valExist = true;
			break;
		case ProtocolBuffersUtil::FieldType::Fixed64:
			if (wireType != 1)
				return false;
			if (fieldInfo->valExist)
			{
				return false;
			}
			if (ofst + 8 > buffSize)
				return false;
			fieldInfo->val.uint64Val = ReadMUInt64(&buff[ofst]);
			ofst += 8;
			fieldInfo->valExist = true;
			break;
		case ProtocolBuffersUtil::FieldType::SFixed32:
			if (wireType != 5)
				return false;
			if (fieldInfo->valExist)
			{
				return false;
			}
			if (ofst + 4 > buffSize)
				return false;
			fieldInfo->val.int32Val = ReadMInt32(&buff[ofst]);
			ofst += 4;
			fieldInfo->valExist = true;
			break;
		case ProtocolBuffersUtil::FieldType::SFixed64:
			if (wireType != 1)
				return false;
			if (fieldInfo->valExist)
			{
				return false;
			}
			if (ofst + 8 > buffSize)
				return false;
			fieldInfo->val.int64Val = ReadMInt64(&buff[ofst]);
			ofst += 8;
			fieldInfo->valExist = true;
			break;
		case ProtocolBuffersUtil::FieldType::Float:
			if (wireType != 5)
				return false;
			if (fieldInfo->valExist)
			{
				return false;
			}
			if (ofst + 4 > buffSize)
				return false;
			fieldInfo->val.floatVal = ReadMFloat(&buff[ofst]);
			ofst += 4;
			fieldInfo->valExist = true;
			break;
		case ProtocolBuffersUtil::FieldType::Double:
			if (wireType != 1)
				return false;
			if (fieldInfo->valExist)
			{
				return false;
			}
			if (ofst + 8 > buffSize)
				return false;
			fieldInfo->val.doubleVal = ReadMDouble(&buff[ofst]);
			ofst += 8;
			fieldInfo->valExist = true;
			break;
		case ProtocolBuffersUtil::FieldType::String:
			if (wireType != 2)
				return false;
			if (fieldInfo->valExist)
			{
				return false;
			}
			ofst = ProtocolBuffersUtil::ReadVarUInt(buff, ofst, v);
			if (ofst + v > buffSize)
				return false;
			fieldInfo->val.strVal = Text::String::New(&buff[ofst], (UOSInt)v);
			ofst += (UOSInt)v;
			fieldInfo->valExist = true;
			break;
		case ProtocolBuffersUtil::FieldType::Bytes:
			if (wireType != 2)
				return false;
			if (fieldInfo->valExist)
			{
				return false;
			}
			ofst = ProtocolBuffersUtil::ReadVarUInt(buff, ofst, v);
			if (ofst + v > buffSize)
				return false;
			NEW_CLASSOPT(fieldInfo->val.bytesVal, Data::ByteBuffer(Data::ByteArrayR(&buff[ofst], (UOSInt)v)));
			ofst += (UOSInt)v;
			fieldInfo->valExist = true;
			break;
		case ProtocolBuffersUtil::FieldType::SubMsg:
			if (wireType != 2)
				return false;
			if (fieldInfo->valExist)
			{
				return false;
			}
			ofst = ProtocolBuffersUtil::ReadVarUInt(buff, ofst, v);
			if (ofst + v > buffSize || !fieldInfo->val.subMsgVal.SetTo(subMsgVal))
			{
				return false;
			}
			if (!subMsgVal->ParseMsssage(&buff[ofst], (UOSInt)v))
			{
				return false;
			}
			ofst += (UOSInt)v;
			fieldInfo->valExist = true;
			break;
		default:
			return false;
		}
	}
	return true;
}
