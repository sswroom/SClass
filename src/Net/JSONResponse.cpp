#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Net/JSONResponse.h"
#include "Text/CharUtil.h"
#include "Text/JSText.h"
#include "Text/StringTool.h"

Net::JSONResponse::Field::Field(Text::CStringNN name, Text::JSONType fieldType, Bool optional, Bool allowNull)
{
	this->name = name;
	this->fieldType = fieldType;
	this->optional = optional;
	this->allowNull = allowNull;
}

Net::JSONResponse::Field::~Field()
{
}

Text::CStringNN Net::JSONResponse::Field::GetName() const
{
	return this->name;
}

Text::JSONType Net::JSONResponse::Field::GetFieldType() const
{
	return this->fieldType;
}

Bool Net::JSONResponse::Field::IsOptional() const
{
	return this->optional;
}

Bool Net::JSONResponse::Field::IsAllowNull() const
{
	return this->allowNull;
}

Net::JSONResponse::ObjectField::ObjectField(Text::CStringNN name, Text::JSONType fieldType, Bool optional, Bool allowNull, Optional<JSONResponse> obj) : Field(name, fieldType, optional, allowNull)
{
	this->obj = obj;
}

Net::JSONResponse::ObjectField::~ObjectField()
{
	this->obj.Delete();
}

Optional<Net::JSONResponse> Net::JSONResponse::ObjectField::GetValue() const
{
	return this->obj;
}

void Net::JSONResponse::FindMissingFields()
{
	if (this->json->GetType() == Text::JSONType::Object)
	{
		NN<Text::JSONObject> obj = NN<Text::JSONObject>::ConvertFrom(this->json);

		Text::StringBuilderUTF8 sb;
		Text::StringBuilderUTF8 sbSuggest;
		Text::StringBuilderUTF8 sbGet;
		Data::ArrayListStringNN nameList;
		obj->GetObjectNames(nameList);
		Data::Sort::ArtificialQuickSort::Sort<NN<Text::String>>(nameList, nameList);
		NN<Text::String> name;
		NN<Text::String> s;
		Bool hasObj = false;
		NN<Field> field;
		Data::ArrayIterator<NN<Text::String>> itName = nameList.Iterator();
		while (itName.HasNext())
		{
			name = itName.Next();
			if (this->fieldMap.GetNN(name).IsNull() && !this->allowAll)
			{
				NN<Text::JSONBase> val;
				NN<Text::JSONArray> arr;
				printf("JSONResponse: %s.%s is not supported, type is %s\r\n", this->clsName.v.Ptr(), name->v.Ptr(), Text::JSONTypeGetName(obj->GetObjectValue(name->ToCString()).SetTo(val)?val->GetType():Text::JSONType::Null).v.Ptr());
				if (obj->GetObjectValue(name->ToCString()).SetTo(val))
				{
					switch (val->GetType())
					{
					case Text::JSONType::Array:
						arr = NN<Text::JSONArray>::ConvertFrom(val);
						if (arr->GetArrayLength() == 0)
						{
							printf("JSONResponse: %s.%s Array is empty\r\n", this->clsName.v.Ptr(), name->v.Ptr());
						}
						else
						{
							Text::JSONType type = arr->GetArrayType(0);
							Bool allSameType = true;
							UIntOS i = arr->GetArrayLength();
							while (i-- > 1)
							{
								if (type != arr->GetArrayType(i))
								{
									allSameType = false;
									printf("JSONResponse: %s.%s Array type is mixed of %s and %s\r\n", this->clsName.v.Ptr(), name->v.Ptr(), Text::JSONTypeGetName(type).v.Ptr(), Text::JSONTypeGetName(arr->GetArrayType(i)).v.Ptr());
									break;
								}
							}

							if (allSameType)
							{
								if (type == Text::JSONType::Object)
								{
									NN<Text::JSONBase> jbase;
									sb.ClearStr();
									if (arr->GetArrayValue(0).SetTo(jbase))
										jbase->ToJSONString(sb);
									else
										sb.Append(CSTR("null"));
									printf("JSONResponse: %s.%s Array type is Object: %s\r\n", this->clsName.v.Ptr(), name->v.Ptr(), sb.ToPtr());
									hasObj = true;

									sbSuggest.Append(CSTR("JSONRESP_ARRAY_OBJ("));
									s = Text::JSText::ToNewJSTextDQuote(name->v);
									sbSuggest.Append(s);
									sbSuggest.Append(CSTR(",false,false,ClassName)\r\n"));
									sbGet.Append(CSTR("JSONRESP_GETARRAY_OBJ("));
									sbGet.Append(s);
									s->Release();
									sbGet.AppendUTF8Char(',');
									AppendFuncName(sbGet, false, name);
									sbGet.Append(CSTR(",ClassName)\r\n"));
								}
								else
								{
									printf("JSONResponse: %s.%s Array type is %s\r\n", this->clsName.v.Ptr(), name->v.Ptr(), Text::JSONTypeGetName(type).v.Ptr());

									if (type == Text::JSONType::String)
									{
										sbSuggest.Append(CSTR("JSONRESP_ARRAY_STR("));
										s = Text::JSText::ToNewJSTextDQuote(name->v);
										sbSuggest.Append(s);
										sbSuggest.Append(CSTR(",false,false)\r\n"));
										sbGet.Append(CSTR("JSONRESP_GETARRAY_STR("));
										sbGet.Append(s);
										s->Release();
										sbGet.AppendUTF8Char(',');
										AppendFuncName(sbGet, false, name);
										sbGet.Append(CSTR(")\r\n"));
									}
									else if (type == Text::JSONType::Number)
									{
										sbSuggest.Append(CSTR("JSONRESP_ARRAY_DOUBLE("));
										s = Text::JSText::ToNewJSTextDQuote(name->v);
										sbSuggest.Append(s);
										sbSuggest.Append(CSTR(",false,false)\r\n"));
										sbGet.Append(CSTR("JSONRESP_GETARRAY_DOUBLE("));
										sbGet.Append(s);
										s->Release();
										sbGet.AppendUTF8Char(',');
										AppendFuncName(sbGet, false, name);
										sbGet.Append(CSTR(")\r\n"));
									}
								}
							}
						}
						break;
					case Text::JSONType::BOOL:
						sbSuggest.Append(CSTR("JSONRESP_BOOL("));
						s = Text::JSText::ToNewJSTextDQuote(name->v);
						sbSuggest.Append(s);
						sbSuggest.Append(CSTR(",false,false)\r\n"));
						sbGet.Append(CSTR("JSONRESP_GETBOOL("));
						sbGet.Append(s);
						s->Release();
						sbGet.AppendUTF8Char(',');
						AppendFuncName(sbGet, true, name);
						sbGet.Append(CSTR(")\r\n"));
						break;
					case Text::JSONType::INT32:
						sbSuggest.Append(CSTR("JSONRESP_INT32("));
						s = Text::JSText::ToNewJSTextDQuote(name->v);
						sbSuggest.Append(s);
						sbSuggest.Append(CSTR(",false,false)\r\n"));
						sbGet.Append(CSTR("JSONRESP_GETINT32("));
						sbGet.Append(s);
						s->Release();
						sbGet.AppendUTF8Char(',');
						AppendFuncName(sbGet, false, name);
						sbGet.Append(CSTR(",0)\r\n"));
						break;
					case Text::JSONType::INT64:
						sbSuggest.Append(CSTR("JSONRESP_INT64("));
						s = Text::JSText::ToNewJSTextDQuote(name->v);
						sbSuggest.Append(s);
						sbSuggest.Append(CSTR(",false,false)\r\n"));
						sbGet.Append(CSTR("JSONRESP_GETINT64("));
						sbGet.Append(s);
						s->Release();
						sbGet.AppendUTF8Char(',');
						AppendFuncName(sbGet, false, name);
						sbGet.Append(CSTR(",0)\r\n"));
						break;
					case Text::JSONType::Null:
						sbSuggest.Append(CSTR("JSONRESP_STR("));
						s = Text::JSText::ToNewJSTextDQuote(name->v);
						sbSuggest.Append(s);
						sbSuggest.Append(CSTR(",false,true)\r\n"));
						sbGet.Append(CSTR("JSONRESP_GETSTROPT("));
						sbGet.Append(s);
						s->Release();
						sbGet.AppendUTF8Char(',');
						AppendFuncName(sbGet, false, name);
						sbGet.Append(CSTR(")\r\n"));
						break;
					case Text::JSONType::Number:
						sbSuggest.Append(CSTR("JSONRESP_DOUBLE("));
						s = Text::JSText::ToNewJSTextDQuote(name->v);
						sbSuggest.Append(s);
						sbSuggest.Append(CSTR(",false,false)\r\n"));
						sbGet.Append(CSTR("JSONRESP_GETDOUBLE("));
						sbGet.Append(s);
						s->Release();
						sbGet.AppendUTF8Char(',');
						AppendFuncName(sbGet, false, name);
						sbGet.Append(CSTR(",0.0)\r\n"));
						break;
					case Text::JSONType::Object:
						sb.ClearStr();
						val->ToJSONString(sb);
						printf("JSONResponse: Object Content: %s\r\n", sb.v.Ptr());
						hasObj = true;

						sbSuggest.Append(CSTR("JSONRESP_OBJ("));
						s = Text::JSText::ToNewJSTextDQuote(name->v);
						sbSuggest.Append(s);
						sbSuggest.Append(CSTR(",false,false,ClassName)\r\n"));
						sbGet.Append(CSTR("JSONRESP_GETOBJ("));
						sbGet.Append(s);
						s->Release();
						sbGet.AppendUTF8Char(',');
						AppendFuncName(sbGet, false, name);
						sbGet.Append(CSTR(",ClassName)\r\n"));
						break;
					case Text::JSONType::String:
						printf("JSONResponse: %s.%s = %s\r\n", this->clsName.v.Ptr(), name->v.Ptr(), NN<Text::JSONString>::ConvertFrom(val)->GetValue()->v.Ptr());
						sbSuggest.Append(CSTR("JSONRESP_STR("));
						s = Text::JSText::ToNewJSTextDQuote(name->v);
						sbSuggest.Append(s);
						sbSuggest.Append(CSTR(",false,false)\r\n"));
						sbGet.Append(CSTR("JSONRESP_GETSTR("));
						sbGet.Append(s);
						s->Release();
						sbGet.AppendUTF8Char(',');
						AppendFuncName(sbGet, false, name);
						sbGet.Append(CSTR(")\r\n"));
						break;
					}
				}
				this->valid = false;
			}
		}
		UIntOS i = 0;
		UIntOS j = this->fieldMap.GetCount();
		while (i < j)
		{
			field = this->fieldMap.GetItemNoCheck(i);
			if (field->IsOptional() || this->json->GetValue(field->GetName()).NotNull())
			{
			}
			else
			{
				printf("JSONResponse: %s.%s not found\r\n", this->clsName.v.Ptr(), field->GetName().v.Ptr());
				this->valid = false;
			}
			i++;
		}
		if (sbSuggest.leng > 0)
		{
			printf("Suggested content for %s:\r\n%s", this->clsName.v.Ptr(), sbSuggest.v.Ptr());
		}
		if (sbGet.leng > 0)
		{
			printf("Suggested get section for %s:\r\n%s\r\n", this->clsName.v.Ptr(), sbGet.v.Ptr());
		}
		if (hasObj)
		{
			printf("Use the following code to define Object:\r\n");
			printf("JSONRESP_BEGIN(ClassName)\r\n");
			printf("JSONRESP_SEC_GET(ClassName)\r\n");
			printf("JSONRESP_END\r\n\r\n");
		}
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		this->json->ToJSONString(sb);
		printf("JSONResponse: data is not an object: %s\r\n", sb.ToPtr());
	}
}

void Net::JSONResponse::AppendFuncName(NN<Text::StringBuilderUTF8> sb, Bool boolFunc, NN<Text::String> fieldName)
{
	if (boolFunc)
	{
		if (fieldName->StartsWith(UTF8STRC("is")) && (fieldName->v[2] == '_' || Text::CharUtil::IsUpperCase(fieldName->v[2])))
		{
		}
		else if (fieldName->StartsWith(UTF8STRC("has")) && (fieldName->v[3] == '_' || Text::CharUtil::IsUpperCase(fieldName->v[3])))
		{
		}
		else if (fieldName->StartsWith(UTF8STRC("can")) && (fieldName->v[3] == '_' || Text::CharUtil::IsUpperCase(fieldName->v[3])))
		{
		}
		else if (fieldName->StartsWith(UTF8STRC("allow")) && (fieldName->v[5] == '_' || Text::CharUtil::IsUpperCase(fieldName->v[5])))
		{
		}
		else if (fieldName->StartsWith(UTF8STRC("use")) && (fieldName->v[3] == '_' || Text::CharUtil::IsUpperCase(fieldName->v[3])))
		{
		}
		else
		{
			sb->Append(CSTR("Is"));
		}
	}
	else
	{
		sb->Append(CSTR("Get"));
	}
	if (fieldName->IndexOf('_') != INVALID_INDEX)
	{
		UIntOS i = 0;
		UIntOS j;
		while (i < fieldName->leng && fieldName->v[i] == '_')
		{
			i++;
		}
		while (i < fieldName->leng)
		{
			j = fieldName->IndexOf('_', i);
			sb->AppendUTF8Char(Text::CharUtil::ToUpper(fieldName->v[i]));
			if (j == INVALID_INDEX)
			{
				sb->Append(fieldName->ToCString().Substring(i + 1));
				break;
			}
			sb->Append(Text::CStringNN(&fieldName->v[i + 1], j - i - 1));
			i = j + 1;
			while (i < fieldName->leng && fieldName->v[i] == '_')
			{
				i++;
			}
		}
	}
	else
	{
		sb->AppendUTF8Char(Text::CharUtil::ToUpper(fieldName->v[0]));
		sb->Append(fieldName->ToCString().Substring(1));
	}
}

void Net::JSONResponse::AddField(Text::CStringNN name, Text::JSONType fieldType, Bool optional, Bool allowNull)
{
	NN<Field> field;
	NEW_CLASSNN(field, Field(name, fieldType, optional, allowNull));
	if (this->fieldMap.PutC(name, field).SetTo(field))
	{
		field.Delete();
	}

}

void Net::JSONResponse::AddFieldArrDbl(Text::CStringNN name, Bool optional, Bool allowNull)
{
	Bool hasError = false;
	NN<ArrayNativeField<Double>> field;
	NEW_CLASSNN(field, ArrayNativeField<Double>(name, optional, allowNull, Text::JSONType::Number));
	NN<Text::JSONArray> arr;
	if (!this->json->GetValueArray(name).SetTo(arr))
	{
		if (!optional)
		{
			this->valid = false;
			printf("JSONResponse: %s.%s is not array\r\n", clsName.v.Ptr(), name.v.Ptr());
		}
		return;
	}
	NN<Text::JSONBase> val;
	UIntOS i = 0;
	UIntOS j = arr->GetArrayLength();
	while (i < j)
	{
		if (!arr->GetArrayValue(i).SetTo(val) || val->GetType() != Text::JSONType::Number)
		{
			if (!hasError)
			{
				hasError = true;
				this->valid = false;
				printf("JSONResponse: %s.%s[%d] is not number, type = %s\r\n", clsName.v.Ptr(), name.v.Ptr(), (UInt32)i, Text::JSONTypeGetName(arr->GetArrayValue(i).SetTo(val)?val->GetType():Text::JSONType::Null).v.Ptr());
			}
		}
		else
		{
			field->AddValue(val->GetAsDoubleOrNAN());
		}
		i++;
	}
	NN<Field> orifield;
	if (this->fieldMap.PutC(name, field).SetTo(orifield))
	{
		orifield.Delete();
	}
}

void Net::JSONResponse::AddFieldArrStr(Text::CStringNN name, Bool optional, Bool allowNull)
{
	Bool hasError = false;
	NN<ArrayStrField> field;
	NN<Text::JSONArray> arr;
	if (!this->json->GetValueArray(name).SetTo(arr))
	{
		if (this->json->GetValue(name).NotNull())
		{
			this->valid = false;
			printf("JSONResponse: %s.%s is not array\r\n", clsName.v.Ptr(), name.v.Ptr());
		}
		else if (!optional)
		{
			this->valid = false;
			printf("JSONResponse: %s.%s is not found and not optional\r\n", clsName.v.Ptr(), name.v.Ptr());
		}
		return;
	}
	NEW_CLASSNN(field, ArrayStrField(name, optional, allowNull));
	NN<Text::JSONBase> val;
	UIntOS i = 0;
	UIntOS j = arr->GetArrayLength();
	while (i < j)
	{
		if (!arr->GetArrayValue(i).SetTo(val) || val->GetType() != Text::JSONType::String)
		{
			if (!hasError)
			{
				hasError = true;
				this->valid = false;
				printf("JSONResponse: %s.%s[%d] is not string, type = %s\r\n", clsName.v.Ptr(), name.v.Ptr(), (UInt32)i, Text::JSONTypeGetName(arr->GetArrayValue(i).SetTo(val)?val->GetType():Text::JSONType::Null).v.Ptr());
			}
		}
		else
		{
			field->AddValue(NN<Text::JSONString>::ConvertFrom(val)->GetValue()->Clone());
		}
		i++;
	}
	NN<Field> orifield;
	if (this->fieldMap.PutC(name, field).SetTo(orifield))
	{
		orifield.Delete();
	}
}

Net::JSONResponse::JSONResponse(NN<Text::JSONBase> json, Text::CStringNN clsName)
{
	this->clsName = clsName;
	this->json = json;
	this->allowAll = false;
	this->valid = this->json->GetType() == Text::JSONType::Object;
	if (!this->valid)
	{
		printf("JSONResponse: %s is not object\r\n", clsName.v.Ptr());
	}
	this->json->BeginUse();
}

Net::JSONResponse::~JSONResponse()
{
	this->json->EndUse();
	UIntOS i = this->fieldMap.GetCount();
	while (i-- > 0)
	{
		NN<Field> field = this->fieldMap.GetItemNoCheck(i);
		field.Delete();
	}
}

Bool Net::JSONResponse::IsValid() const
{
	return this->valid;
}

void Net::JSONResponse::ToString(NN<Text::StringBuilderUTF8> sb, Text::CStringNN linePrefix) const
{
	Text::StringBuilderUTF8 sbTmp;
	NN<Text::String> s;
	Data::ArrayListStringNN keys;
	Data::FastStringNNKeyIterator<Field> it = this->fieldMap.KeyIterator();
	while (it.HasNext())
		keys.Add(it.Next());
	Data::Sort::ArtificialQuickSort::Sort<NN<Text::String>>(keys, keys);
	NN<Field> field;
	NN<Text::String> key;
	Data::ArrayIterator<NN<Text::String>> itKey = keys.Iterator();
	while (itKey.HasNext())
	{
		key = itKey.Next();
		if (this->fieldMap.GetNN(key).SetTo(field))
		{
			switch (field->GetFieldType())
			{
			case Text::JSONType::Object:
				{
					NN<ObjectField> ofield = NN<ObjectField>::ConvertFrom(field);
					NN<Net::JSONResponse> subObj;
					if (ofield->GetValue().SetTo(subObj))
					{
						sbTmp.ClearStr();
						sbTmp.Append(linePrefix);
						sbTmp.Append(field->GetName());
						sbTmp.AppendUTF8Char('.');
						subObj->ToString(sb, sbTmp.ToCString());
					}
					else
					{
						sb->Append(linePrefix);
						sb->Append(field->GetName());
						sb->Append(CSTR(" = null\r\n"));
					}
				}
				break;
			case Text::JSONType::Array:
				{
					NN<ArrayField> afield = NN<ArrayField>::ConvertFrom(field);
					if (afield->GetArrType() == Text::JSONType::Object)
					{
						NN<ArrayNNField<Net::JSONResponse>> anfield = NN<ArrayNNField<Net::JSONResponse>>::ConvertFrom(afield);
						NN<const Data::ArrayListNN<Net::JSONResponse>> arr = anfield->GetValue();
						NN<Net::JSONResponse> subObj;
						UIntOS i = 0;
						UIntOS j = arr->GetCount();
						while (i < j)
						{
							if (arr->GetItem(i).SetTo(subObj))
							{
								sbTmp.ClearStr();
								sbTmp.Append(linePrefix);
								sbTmp.Append(field->GetName());
								sbTmp.AppendUTF8Char('[');
								sbTmp.AppendUIntOS(i);
								sbTmp.AppendUTF8Char(']');
								sbTmp.AppendUTF8Char('.');
								subObj->ToString(sb, sbTmp.ToCString());
							}
							i++;
						}
					}
					else if (afield->GetArrType() == Text::JSONType::String)
					{
						NN<ArrayStrField> anfield = NN<ArrayStrField>::ConvertFrom(afield);
						NN<const Data::ArrayListStringNN> arr = anfield->GetValue();
						if (arr->GetCount() == 0)
						{
							sb->Append(linePrefix);
							sb->Append(field->GetName());
							sb->Append(CSTR(" = []\r\n"));
						}
						else
						{
							sb->Append(linePrefix);
							sb->Append(field->GetName());
							sb->Append(CSTR(" = [\r\n"));
							Data::ArrayIterator<NN<Text::String>> it = arr->Iterator();
							while (it.HasNext())
							{
								sb->AppendUTF8Char('\t');
								s = Text::JSText::ToNewJSTextDQuote(it.Next()->v);
								sb->Append(s);
								s->Release();
							}
							sb->Append(CSTR("]\r\n"));
						}
					}
					else if (afield->GetArrType() == Text::JSONType::Number)
					{
						NN<ArrayNativeField<Double>> anfield = NN<ArrayNativeField<Double>>::ConvertFrom(afield);
						NN<const Data::ArrayListNative<Double>> arr = anfield->GetValue();
						UIntOS i = 0;
						UIntOS j = arr->GetCount();
						if (j < 10)
						{
							sb->Append(linePrefix);
							sb->Append(field->GetName());
							sb->Append(CSTR(" = ["));
							while (i < j)
							{
								if (i > 0) sb->Append(CSTR(", "));
								sb->AppendDouble(arr->GetItem(i));
								i++;
							}
							sb->Append(CSTR("]\r\n"));
						}
						else
						{
							sb->Append(linePrefix);
							sb->Append(field->GetName());
							sb->Append(CSTR(" = ["));
							while (i < j)
							{
								if (i > 0) sb->AppendUTF8Char(',');
								sb->Append(CSTR("\r\n\t"));
								sb->AppendDouble(arr->GetItem(i));
								i++;
							}
							sb->Append(CSTR("\r\n]\r\n"));
						}
					}
				}
				break;
			case Text::JSONType::Number:
				sb->Append(linePrefix);
				sb->Append(field->GetName());
				sb->Append(CSTR(" = "));
				sb->AppendDouble(this->json->GetValueAsDoubleOrNAN(field->GetName()));
				sb->Append(CSTR("\r\n"));
				break;
			case Text::JSONType::String:
				sb->Append(linePrefix);
				sb->Append(field->GetName());
				sb->Append(CSTR(" = "));
				s = Text::JSText::ToNewJSTextDQuote(OPTSTR_CSTR(this->json->GetValueString(field->GetName())).v);
				sb->Append(s);
				s->Release();
				sb->Append(CSTR("\r\n"));
				break;
			case Text::JSONType::BOOL:
				sb->Append(linePrefix);
				sb->Append(field->GetName());
				sb->Append(CSTR(" = "));
				sb->Append(this->json->GetValueAsBool(field->GetName())?CSTR("true"):CSTR("false"));
				sb->Append(CSTR("\r\n"));
				break;
			case Text::JSONType::Null:
				sb->Append(linePrefix);
				sb->Append(field->GetName());
				sb->Append(CSTR(" = null\r\n"));
				break;
			case Text::JSONType::INT32:
				sb->Append(linePrefix);
				sb->Append(field->GetName());
				sb->Append(CSTR(" = "));
				sb->AppendI32(this->json->GetValueAsInt32(field->GetName()));
				sb->Append(CSTR("\r\n"));
				break;
			case Text::JSONType::INT64:
				sb->Append(linePrefix);
				sb->Append(field->GetName());
				sb->Append(CSTR(" = "));
				sb->AppendI64(this->json->GetValueAsInt64(field->GetName()));
				sb->Append(CSTR("\r\n"));
				break;
			default:
				break;
			}
		}
	}
}

void Net::JSONResponse::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	this->ToString(sb, CSTR(""));
}

void Net::JSONResponse::ToJSONWF(NN<Text::StringBuilderUTF8> sb) const
{
	Text::StringBuilderUTF8 sbTmp;
	this->json->ToJSONString(sbTmp);
	Text::JSText::JSONWellFormat(sbTmp.v, sbTmp.leng, 0, sb);
}
