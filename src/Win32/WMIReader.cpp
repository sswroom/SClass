#include "Stdafx.h"
#include "DB/ColDef.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Win32/WMIReader.h"
#include <wbemidl.h>

Win32::WMIReader::WMIReader(void *pEnum)
{
	this->pEnum = pEnum;
	this->pObject = 0;
	this->fObject = 0;
	this->isFirst = false;
	NEW_CLASS(this->columns, Data::ArrayList<WMIColumn*>());

	IWbemClassObject *pObject;
	ULONG returned;
	HRESULT hr = ((IEnumWbemClassObject*)this->pEnum)->Next(WBEM_INFINITE, 1, &pObject, &returned);
	if (SUCCEEDED(hr))
	{
		if (returned)
		{
			this->fObject = pObject;
			this->isFirst = true;
		}
	}
	else if (hr == (HRESULT)WBEM_E_ACCESS_DENIED)
	{
		this->isFirst = false;
	}

	if (this->fObject)
	{
		hr = pObject->BeginEnumeration(0);
		if (SUCCEEDED(hr))
		{
			WMIColumn *col;
			BSTR bs;
			CIMTYPE t;
			while (pObject->Next(0, &bs, 0, &t, 0) == S_OK)
			{
				if (bs[0] != '_')
				{
					col = MemAlloc(WMIColumn, 1);
					col->name = Text::StrCopyNew(bs);
					col->colType = t;
					this->columns->Add(col);
				}
				SysFreeString(bs);
			}
			pObject->EndEnumeration();
		}
	}
}

Win32::WMIReader::~WMIReader()
{
	UOSInt i;
	WMIColumn *col;

	if (this->pObject)
	{
		((IWbemClassObject *)this->pObject)->Release();
		this->pObject = 0;
	}
	if (this->fObject)
	{
		((IWbemClassObject *)this->fObject)->Release();
		this->fObject = 0;
	}
	i = this->columns->GetCount();
	while (i-- > 0)
	{
		col = this->columns->GetItem(i);
		Text::StrDelNew(col->name);
		MemFree(col);
	}
	DEL_CLASS(this->columns);
	((IEnumWbemClassObject*)this->pEnum)->Release();
	this->pEnum = 0;
}

Bool Win32::WMIReader::ReadNext()
{
	if (this->pObject)
	{
		((IWbemClassObject *)this->pObject)->Release();
		this->pObject = 0;
	}
	if (this->isFirst)
	{
		this->isFirst = false;
		this->pObject = this->fObject;
		this->fObject = 0;
		if (this->pObject)
		{
			return true;
		}
	}

	IWbemClassObject *pObject;
	ULONG returned;
	HRESULT hr = ((IEnumWbemClassObject*)this->pEnum)->Next(WBEM_INFINITE, 1, &pObject, &returned);
	if (SUCCEEDED(hr))
	{
		if (returned)
		{
			this->pObject = pObject;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

UOSInt Win32::WMIReader::ColCount()
{
	return this->columns->GetCount();
}

OSInt Win32::WMIReader::GetRowChanged()
{
	return -1;
}

Int32 Win32::WMIReader::GetInt32(UOSInt colIndex)
{
	WMIColumn *col = this->columns->GetItem(colIndex);
	if (col == 0 || this->pObject == 0)
		return 0;

	Int32 ret = 0;
	HRESULT hr;
	VARIANT v;
	CIMTYPE t;
	VariantInit(&v);
	hr = ((IWbemClassObject*)this->pObject)->Get(col->name, 0, &v, &t, NULL);
	if (SUCCEEDED(hr))
	{
		if (V_VT(&v) == VT_NULL)
		{
		}
		else
		{
			switch (t)
			{
			case CIM_SINT8:
				ret = V_I1(&v);
				break;
			case CIM_UINT8:
				ret = V_UI1(&v);
				break;
			case CIM_SINT16:
				ret = V_I2(&v);
				break;
			case CIM_UINT16:
				ret = V_UI2(&v);
				break;
			case CIM_SINT32:
				ret = V_I4(&v);
				break;
			case CIM_UINT32:
				ret = V_I4(&v);
				break;
			case CIM_SINT64:
				ret = (Int32)(V_I8(&v) & 0xffffffff);
				break;
			case CIM_UINT64:
				ret = (Int32)(V_UI8(&v) & 0xffffffff);
				break;
			case CIM_REAL32:
				ret = Double2Int32(V_R4(&v));
				break;
			case CIM_REAL64:
				ret = Double2Int32(V_R8(&v));
				break;
			case CIM_BOOLEAN:
				ret = V_BOOL(&v);
				break;
			case CIM_STRING:
				{
					BSTR bs = V_BSTR(&v);
					ret = Text::StrToInt32(bs);
				}
				break;
			case CIM_DATETIME:
				break;
			}
		}
	}
	VariantClear(&v);
	return ret;
}

Int64 Win32::WMIReader::GetInt64(UOSInt colIndex)
{
	WMIColumn *col = this->columns->GetItem(colIndex);
	if (col == 0 || this->pObject == 0)
		return 0;

	Int64 ret = 0;
	HRESULT hr;
	VARIANT v;
	CIMTYPE t;
	VariantInit(&v);
	hr = ((IWbemClassObject*)this->pObject)->Get(col->name, 0, &v, &t, NULL);
	if (SUCCEEDED(hr))
	{
		if (V_VT(&v) == VT_NULL)
		{
		}
		else
		{
			switch (t)
			{
			case CIM_SINT8:
				ret = V_I1(&v);
				break;
			case CIM_UINT8:
				ret = V_UI1(&v);
				break;
			case CIM_SINT16:
				ret = V_I2(&v);
				break;
			case CIM_UINT16:
				ret = V_UI2(&v);
				break;
			case CIM_SINT32:
				ret = V_I4(&v);
				break;
			case CIM_UINT32:
				ret = V_UI4(&v);
				break;
			case CIM_SINT64:
				ret = V_I8(&v);
				break;
			case CIM_UINT64:
				ret = (Int64)V_UI8(&v);
				break;
			case CIM_REAL32:
				ret = Double2Int32(V_R4(&v));
				break;
			case CIM_REAL64:
				ret = Double2Int32(V_R8(&v));
				break;
			case CIM_BOOLEAN:
				ret = V_BOOL(&v);
				break;
			case CIM_STRING:
				{
					BSTR bs = V_BSTR(&v);
					ret = Text::StrToInt64(bs);
				}
				break;
			case CIM_DATETIME:
				break;
			}
		}
	}
	VariantClear(&v);
	return ret;
}

WChar *Win32::WMIReader::GetStr(UOSInt colIndex, WChar *buff)
{
	WMIColumn *col = this->columns->GetItem(colIndex);
	if (col == 0 || this->pObject == 0)
		return 0;

	WChar *ret = 0;
	HRESULT hr;
	VARIANT v;
	CIMTYPE t;
	VariantInit(&v);
	hr = ((IWbemClassObject*)this->pObject)->Get(col->name, 0, &v, &t, NULL);
	if (SUCCEEDED(hr))
	{
		if (V_VT(&v) == VT_NULL)
		{
		}
		else
		{
			switch (t)
			{
			case CIM_SINT8:
				ret = Text::StrInt32(buff, V_I1(&v));
				break;
			case CIM_UINT8:
				ret = Text::StrInt32(buff, V_UI1(&v));
				break;
			case CIM_UINT8 | CIM_FLAG_ARRAY:
				{
					SAFEARRAY * arr = V_ARRAY(&v);
					*buff++ = '0';
					*buff++ = 'x';
					ret = Text::StrHexBytes(buff, (UInt8*)arr->pvData, arr->rgsabound[0].cElements, 0);
				}
				break;
			case CIM_SINT16:
				ret = Text::StrInt32(buff, V_I2(&v));
				break;
			case CIM_UINT16:
				ret = Text::StrInt32(buff, V_UI2(&v));
				break;
			case CIM_SINT32:
				ret = Text::StrInt32(buff, V_I4(&v));
				break;
			case CIM_UINT32:
				ret = Text::StrUInt32(buff, V_UI4(&v));
				break;
			case CIM_SINT64:
				ret = Text::StrInt64(buff, V_I8(&v));
				break;
			case CIM_UINT64:
				ret = Text::StrUInt64(buff, V_UI8(&v));
				break;
			case CIM_REAL32:
				ret = Text::StrDouble(buff, V_R4(&v));
				break;
			case CIM_REAL64:
				ret = Text::StrDouble(buff, V_R8(&v));
				break;
			case CIM_BOOLEAN:
				ret = Text::StrConcat(buff, (V_BOOL(&v))?L"True":L"False");
				break;
			case CIM_STRING:
				{
					BSTR bs = V_BSTR(&v);
					ret = Text::StrConcat(buff, bs);
				}
				break;
			case CIM_DATETIME:
				{
					Data::DateTime dt;
					UTF8Char sbuff[32];
					dt.SetValueVariTime(V_DATE(&v));
					dt.ToString(sbuff);
					ret = Text::StrUTF8_WChar(buff, sbuff, 0);
				}
				break;
			}
		}
	}
	VariantClear(&v);
	return ret;
}

Bool Win32::WMIReader::GetStr(UOSInt colIndex, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	WMIColumn *col = this->columns->GetItem(colIndex);
	if (col == 0 || this->pObject == 0)
		return 0;

	Bool ret = false;
	HRESULT hr;
	VARIANT v;
	CIMTYPE t;
	VariantInit(&v);
	hr = ((IWbemClassObject*)this->pObject)->Get(col->name, 0, &v, &t, NULL);
	if (SUCCEEDED(hr))
	{
		if (V_VT(&v) == VT_NULL)
		{
		}
		else
		{
			switch (t)
			{
			case CIM_SINT8:
				sb->AppendI16(V_I1(&v));
				ret = true;
				break;
			case CIM_UINT8:
				sb->AppendU16(V_UI1(&v));
				ret = true;
				break;
			case CIM_UINT8 | CIM_FLAG_ARRAY:
				{
					SAFEARRAY * arr = V_ARRAY(&v);
					sb->AppendC(UTF8STRC("0x"));
					sb->AppendHexBuff((UInt8*)arr->pvData, arr->rgsabound[0].cElements, 0, Text::LineBreakType::None);
					ret = true;
				}
				break;
			case CIM_SINT16:
				sb->AppendI16(V_I2(&v));
				ret = true;
				break;
			case CIM_UINT16:
				sb->AppendU16(V_UI2(&v));
				ret = true;
				break;
			case CIM_SINT32:
				sb->AppendI32((Int32)V_I4(&v));
				ret = true;
				break;
			case CIM_UINT32:
				sb->AppendU32((UInt32)V_UI4(&v));
				ret = true;
				break;
			case CIM_SINT64:
				sb->AppendI64(V_I8(&v));
				ret = true;
				break;
			case CIM_UINT64:
				sb->AppendU64(V_UI8(&v));
				ret = true;
				break;
			case CIM_REAL32:
				Text::SBAppendF32(sb, V_R4(&v));
				ret = true;
				break;
			case CIM_REAL64:
				Text::SBAppendF64(sb, V_R8(&v));
				ret = true;
				break;
			case CIM_BOOLEAN:
				if (V_BOOL(&v))
				{
					sb->AppendC(UTF8STRC("True"));
				}
				else
				{
					sb->AppendC(UTF8STRC("False"));
				}
				break;
			case CIM_STRING:
				{
					BSTR bs = V_BSTR(&v);
					sb->AppendW(bs);
					ret = true;
				}
				break;
			case CIM_DATETIME:
				{
					sb->AppendTSNoZone(Data::Timestamp::FromVariTime(V_DATE(&v)));
					ret = true;
				}
				break;
			}
		}
	}
	VariantClear(&v);
	return ret;
}

Optional<Text::String> Win32::WMIReader::GetNewStr(UOSInt colIndex)
{
	WMIColumn *col = this->columns->GetItem(colIndex);
	if (col == 0 || this->pObject == 0)
		return 0;

	UTF8Char sbuff[64];
	UTF8Char* sptr;
	Optional<Text::String> ret = 0;
	HRESULT hr;
	VARIANT v;
	CIMTYPE t;
	VariantInit(&v);
	hr = ((IWbemClassObject*)this->pObject)->Get(col->name, 0, &v, &t, NULL);
	if (SUCCEEDED(hr))
	{
		if (V_VT(&v) == VT_NULL)
		{
		}
		else
		{
			switch (t)
			{
			case CIM_SINT8:
				sptr = Text::StrInt32(sbuff, V_I1(&v));
				ret = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				break;
			case CIM_UINT8:
				sptr = Text::StrInt32(sbuff, V_UI1(&v));
				ret = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				break;
			case CIM_UINT8 | CIM_FLAG_ARRAY:
				{
					SAFEARRAY * arr = V_ARRAY(&v);
					NotNullPtr<Text::String> tmpV = Text::String::New(2 + (arr->cbElements << 1));
					ret = tmpV;
					Text::StrHexBytes(Text::StrConcatC(tmpV->v, UTF8STRC("0x")), (UInt8*)arr->pvData, arr->rgsabound[0].cElements, 0);
				}
				break;
			case CIM_SINT16:
				sptr = Text::StrInt32(sbuff, V_I2(&v));
				ret = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				break;
			case CIM_UINT16:
				sptr = Text::StrInt32(sbuff, V_UI2(&v));
				ret = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				break;
			case CIM_SINT32:
				sptr = Text::StrInt32(sbuff, V_I4(&v));
				ret = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				break;
			case CIM_UINT32:
				sptr = Text::StrUInt32(sbuff, V_UI4(&v));
				ret = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				break;
			case CIM_SINT64:
				sptr = Text::StrInt64(sbuff, V_I8(&v));
				ret = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				break;
			case CIM_UINT64:
				sptr = Text::StrUInt64(sbuff, V_UI8(&v));
				ret = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				break;
			case CIM_REAL32:
				sptr = Text::StrDouble(sbuff, V_R4(&v));
				ret = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				break;
			case CIM_REAL64:
				sptr = Text::StrDouble(sbuff, V_R8(&v));
				ret = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				break;
			case CIM_BOOLEAN:
				ret = Text::String::New((V_BOOL(&v))?CSTR("True"):CSTR("False"));
				break;
			case CIM_STRING:
				{
					BSTR bs = V_BSTR(&v);
					ret = Text::String::NewNotNull(bs);
				}
				break;
			case CIM_DATETIME:
				{
					Data::DateTime dt;
					dt.SetValueVariTime(V_DATE(&v));
					sptr = dt.ToString(sbuff);
					ret = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				}
				break;
			}
		}
	}
	VariantClear(&v);
	return ret;
}

UTF8Char *Win32::WMIReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	WMIColumn *col = this->columns->GetItem(colIndex);
	if (col == 0 || this->pObject == 0)
		return 0;

	*buff = 0;
	HRESULT hr;
	VARIANT v;
	CIMTYPE t;
	VariantInit(&v);
	hr = ((IWbemClassObject*)this->pObject)->Get(col->name, 0, &v, &t, NULL);
	if (SUCCEEDED(hr))
	{
		if (V_VT(&v) == VT_NULL)
		{
		}
		else
		{
			switch (t)
			{
			case CIM_SINT8:
				buff = Text::StrInt32(buff, V_I1(&v));
				break;
			case CIM_UINT8:
				buff = Text::StrUInt16(buff, V_UI1(&v));
				break;
			case CIM_UINT8 | CIM_FLAG_ARRAY:
				{
					SAFEARRAY * arr = V_ARRAY(&v);
					if (arr->cbElements * 2 + 3 >= buffSize)
					{
						buff = Text::StrHexBytes(Text::StrConcatC(buff, UTF8STRC("0x")), (UInt8*)arr->pvData, (buffSize - 3) >> 1, 0);
					}
					else
					{
						buff = Text::StrHexBytes(Text::StrConcatC(buff, UTF8STRC("0x")), (UInt8*)arr->pvData, arr->rgsabound[0].cElements, 0);
					}
				}
				break;
			case CIM_SINT16:
				buff = Text::StrInt32(buff, V_I2(&v));
				break;
			case CIM_UINT16:
				buff = Text::StrUInt32(buff, V_UI2(&v));
				break;
			case CIM_SINT32:
				buff = Text::StrInt32(buff, V_I4(&v));
				break;
			case CIM_UINT32:
				buff = Text::StrUInt32(buff, V_UI4(&v));
				break;
			case CIM_SINT64:
				buff = Text::StrInt64(buff, V_I8(&v));
				break;
			case CIM_UINT64:
				buff = Text::StrUInt64(buff, V_UI8(&v));
				break;
			case CIM_REAL32:
				buff = Text::StrDouble(buff, V_R4(&v));
				break;
			case CIM_REAL64:
				buff = Text::StrDouble(buff, V_R8(&v));
				break;
			case CIM_BOOLEAN:
				buff = Text::StrConcatS(buff, (V_BOOL(&v))?(const UTF8Char*)"True":(const UTF8Char*)"False", buffSize);
				break;
			case CIM_STRING:
				{
					BSTR bs = V_BSTR(&v);
					buff = Text::StrWChar_UTF8(buff, bs);
				}
				break;
			case CIM_DATETIME:
				{
					Data::DateTime dt;
					dt.SetValueVariTime(V_DATE(&v));
					buff = dt.ToString(buff);
				}
				break;
			}
		}
	}
	VariantClear(&v);
	return buff;
}

Data::Timestamp Win32::WMIReader::GetTimestamp(UOSInt colIndex)
{
	WMIColumn *col = this->columns->GetItem(colIndex);
	if (col == 0 || this->pObject == 0)
		return Data::Timestamp(0);

	Data::Timestamp ret = Data::Timestamp(0);
	HRESULT hr;
	VARIANT v;
	CIMTYPE t;
	VariantInit(&v);
	hr = ((IWbemClassObject*)this->pObject)->Get(col->name, 0, &v, &t, NULL);
	if (SUCCEEDED(hr))
	{
		if (V_VT(&v) == VT_NULL)
		{
		}
		else
		{
			switch (t)
			{
			case CIM_EMPTY:
				break;
			case CIM_DATETIME:
				ret = Data::Timestamp::FromVariTime(V_DATE(&v));
				break;
			}
		}
	}
	VariantClear(&v);
	return ret;
}

Double Win32::WMIReader::GetDbl(UOSInt colIndex)
{
	WMIColumn *col = this->columns->GetItem(colIndex);
	if (col == 0 || this->pObject == 0)
		return 0;

	Double ret = 0;
	HRESULT hr;
	VARIANT v;
	CIMTYPE t;
	VariantInit(&v);
	hr = ((IWbemClassObject*)this->pObject)->Get(col->name, 0, &v, &t, NULL);
	if (SUCCEEDED(hr))
	{
		if (V_VT(&v) == VT_NULL)
		{
		}
		else
		{
			switch (t)
			{
			case CIM_SINT8:
				ret = V_I1(&v);
				break;
			case CIM_UINT8:
				ret = V_UI1(&v);
				break;
			case CIM_SINT16:
				ret = V_I2(&v);
				break;
			case CIM_UINT16:
				ret = V_UI2(&v);
				break;
			case CIM_SINT32:
				ret = V_I4(&v);
				break;
			case CIM_UINT32:
				ret = V_I4(&v);
				break;
			case CIM_SINT64:
				ret = (Double)V_I8(&v);
				break;
			case CIM_UINT64:
				ret = (Double)V_UI8(&v);
				break;
			case CIM_REAL32:
				ret = V_R4(&v);
				break;
			case CIM_REAL64:
				ret = V_R8(&v);
				break;
			case CIM_BOOLEAN:
				ret = V_BOOL(&v);
				break;
			case CIM_STRING:
				{
					BSTR bs = V_BSTR(&v);
					ret = Text::StrToDouble(bs);
				}
				break;
			case CIM_DATETIME:
				ret = V_DATE(&v);
				break;
			}
		}
	}
	VariantClear(&v);
	return ret;
}

Bool Win32::WMIReader::GetBool(UOSInt colIndex)
{
	return GetInt32(colIndex) != 0;
}

UOSInt Win32::WMIReader::GetBinarySize(UOSInt colIndex)
{
	WMIColumn *col = this->columns->GetItem(colIndex);
	if (col == 0 || this->pObject == 0)
		return 0;

	UOSInt ret = 0;
	HRESULT hr;
	VARIANT v;
	CIMTYPE t;
	VariantInit(&v);
	hr = ((IWbemClassObject*)this->pObject)->Get(col->name, 0, &v, &t, NULL);
	if (SUCCEEDED(hr))
	{
		if (V_VT(&v) == VT_NULL)
		{
		}
		else
		{
			switch (t)
			{
			case CIM_UINT8 | CIM_FLAG_ARRAY:
				{
					SAFEARRAY * arr = V_ARRAY(&v);
					ret = arr->rgsabound[0].cElements;
				}
				break;
			}
		}
	}
	VariantClear(&v);
	return ret;
}

UOSInt Win32::WMIReader::GetBinary(UOSInt colIndex, UInt8 *buff)
{
	WMIColumn *col = this->columns->GetItem(colIndex);
	if (col == 0 || this->pObject == 0)
		return 0;

	UOSInt ret = 0;
	HRESULT hr;
	VARIANT v;
	CIMTYPE t;
	VariantInit(&v);
	hr = ((IWbemClassObject*)this->pObject)->Get(col->name, 0, &v, &t, NULL);
	if (SUCCEEDED(hr))
	{
		if (V_VT(&v) == VT_NULL)
		{
		}
		else
		{
			switch (t)
			{
			case CIM_UINT8 | CIM_FLAG_ARRAY:
				{
					SAFEARRAY * arr = V_ARRAY(&v);
					MemCopyNO(buff, arr->pvData, arr->rgsabound[0].cElements);
					ret = arr->rgsabound[0].cElements;
				}
				break;
			}
		}
	}
	VariantClear(&v);
	return ret;
}

Optional<Math::Geometry::Vector2D> Win32::WMIReader::GetVector(UOSInt colIndex)
{
	return 0;
}

Bool Win32::WMIReader::GetUUID(UOSInt colIndex, NotNullPtr<Data::UUID> uuid)
{
	return false;
}

Bool Win32::WMIReader::IsNull(UOSInt colIndex)
{
	WMIColumn *col = this->columns->GetItem(colIndex);
	if (col == 0 || this->pObject == 0)
		return 0;

	Bool ret = false;
	HRESULT hr;
	VARIANT v;
	CIMTYPE t;
	VariantInit(&v);
	hr = ((IWbemClassObject*)this->pObject)->Get(col->name, 0, &v, &t, NULL);
	if (SUCCEEDED(hr))
	{
		switch (t)
		{
		case CIM_EMPTY:
			ret = true;
			break;
		}
	}
	else
	{
		ret = true;
	}
	VariantClear(&v);
	return ret;
}

UTF8Char *Win32::WMIReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	WMIColumn *col = this->columns->GetItem(colIndex);
	if (col == 0)
		return 0;
	return Text::StrWChar_UTF8(buff, col->name);
}

DB::DBUtil::ColType Win32::WMIReader::GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	WMIColumn *col = this->columns->GetItem(colIndex);
	if (col == 0)
		return DB::DBUtil::CT_Unknown;

	colSize.Set(0);
	switch (col->colType)
	{
	case CIM_SINT8:
		return DB::DBUtil::CT_Byte;
	case CIM_UINT8:
		return DB::DBUtil::CT_Byte;
	case CIM_SINT16:
		return DB::DBUtil::CT_Int16;
	case CIM_UINT16:
		return DB::DBUtil::CT_Int16;
	case CIM_SINT32:
		return DB::DBUtil::CT_Int32;
	case CIM_UINT32:
		return DB::DBUtil::CT_UInt32;
	case CIM_SINT64:
		return DB::DBUtil::CT_Int64;
	case CIM_UINT64:
		return DB::DBUtil::CT_UInt64;
	case CIM_REAL32:
		return DB::DBUtil::CT_Float;
	case CIM_REAL64:
		return DB::DBUtil::CT_Double;
	case CIM_BOOLEAN:
		return DB::DBUtil::CT_Bool;
	case CIM_STRING:
		colSize.Set(1024);
		return DB::DBUtil::CT_VarUTF16Char;
	case CIM_DATETIME:
		return DB::DBUtil::CT_DateTime;
	}
	return DB::DBUtil::CT_Unknown;
}

Bool Win32::WMIReader::GetColDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
{
	WMIColumn *col = this->columns->GetItem(colIndex);
	if (col == 0)
		return false;

	NotNullPtr<Text::String> s = Text::String::NewNotNull(col->name);
	colDef->SetColName(s);
	s->Release();
	switch (col->colType)
	{
	case CIM_SINT8:
		colDef->SetColType(DB::DBUtil::CT_Byte);
		colDef->SetColSize(4);
		return true;
	case CIM_UINT8:
		colDef->SetColType(DB::DBUtil::CT_Byte);
		colDef->SetColSize(3);
		return true;
	case CIM_UINT8 | CIM_FLAG_ARRAY:
		colDef->SetColType(DB::DBUtil::CT_Binary);
		colDef->SetColSize(65536);
		return true;
	case CIM_SINT16:
		colDef->SetColType(DB::DBUtil::CT_Int16);
		colDef->SetColSize(6);
		return true;
	case CIM_UINT16:
		colDef->SetColType(DB::DBUtil::CT_Int16);
		colDef->SetColSize(5);
		return true;
	case CIM_SINT32:
		colDef->SetColType(DB::DBUtil::CT_Int32);
		colDef->SetColSize(11);
		return true;
	case CIM_UINT32:
		colDef->SetColType(DB::DBUtil::CT_UInt32);
		colDef->SetColSize(10);
		return true;
	case CIM_SINT64:
		colDef->SetColType(DB::DBUtil::CT_Int64);
		colDef->SetColSize(21);
		return true;
	case CIM_UINT64:
		colDef->SetColType(DB::DBUtil::CT_UInt64);
		colDef->SetColSize(20);
		return true;
	case CIM_REAL32:
		colDef->SetColType(DB::DBUtil::CT_Float);
		colDef->SetColSize(30);
		return true;
	case CIM_REAL64:
		colDef->SetColType(DB::DBUtil::CT_Double);
		colDef->SetColSize(6);
		return true;
	case CIM_BOOLEAN:
		colDef->SetColType(DB::DBUtil::CT_Bool);
		colDef->SetColSize(1);
		return true;
	case CIM_STRING:
		colDef->SetColType(DB::DBUtil::CT_VarUTF16Char);
		colDef->SetColSize(1024);
		return true;
	case CIM_DATETIME:
		colDef->SetColType(DB::DBUtil::CT_DateTime);
		colDef->SetColSize(30);
		return true;
	}
	return false;
}

Int32 Win32::WMIReader::GetInt32(const WChar *colName)
{
	if (this->pObject == 0)
	{
		return 0;
	}
	Int32 ret = 0;

	BSTR temp = SysAllocString(colName);
	HRESULT hr;
	VARIANT v;
	VariantInit(&v);
	hr = ((IWbemClassObject*)this->pObject)->Get(temp, 0, &v, NULL, NULL);
	SysFreeString(temp);
	if (SUCCEEDED(hr))
	{
		ret = V_I4(&v);
	}
	VariantClear(&v);
	return ret;
}

WChar *Win32::WMIReader::GetStr(const WChar *colName, WChar *buff)
{
	if (this->pObject == 0)
	{
		return 0;
	}
	WChar *ret = 0;

	BSTR temp = SysAllocString(colName);
	HRESULT hr;
	VARIANT v;
	VariantInit(&v);
	hr = ((IWbemClassObject*)this->pObject)->Get(temp, 0, &v, NULL, NULL);
	SysFreeString(temp);
	if (SUCCEEDED(hr))
	{
		BSTR bs = V_BSTR(&v);
		ret = Text::StrConcat(buff, bs);
	}
	VariantClear(&v);
	return ret;
}
