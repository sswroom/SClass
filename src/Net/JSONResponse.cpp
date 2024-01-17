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
		NotNullPtr<Text::JSONObject> obj = NotNullPtr<Text::JSONObject>::ConvertFrom(this->json);

		Text::StringBuilderUTF8 sb;
		Text::StringBuilderUTF8 sbSuggest;
		Text::StringBuilderUTF8 sbGet;
		Data::ArrayListStringNN nameList;
		obj->GetObjectNames(nameList);
		Data::Sort::ArtificialQuickSort::Sort(&nameList, &nameList);
		NotNullPtr<Text::String> name;
		NotNullPtr<Text::String> s;
		Bool hasObj = false;
		Field *field;
		Data::ArrayIterator<NotNullPtr<Text::String>> itName = nameList.Iterator();
		while (itName.HasNext())
		{
			name = itName.Next();
			if (this->fieldMap.GetNN(name) == 0 && !this->allowAll)
			{
				Text::JSONBase *val = obj->GetObjectValue(name->ToCString());
				Text::JSONArray *arr;
				printf("JSONResponse: %s.%s is not supported, type is %s\r\n", this->clsName.v, name->v, Text::JSONTypeGetName(val?val->GetType():Text::JSONType::Null).v);
				if (val)
				{
					switch (val->GetType())
					{
					case Text::JSONType::Array:
						arr = (Text::JSONArray *)val;
						if (arr->GetArrayLength() == 0)
						{
							printf("JSONResponse: %s.%s Array is empty\r\n", this->clsName.v, name->v);
						}
						else
						{
							Text::JSONType type = arr->GetArrayValue(0)->GetType();
							Bool allSameType = true;
							UOSInt i = arr->GetArrayLength();
							while (i-- > 1)
							{
								if (type != arr->GetArrayValue(i)->GetType())
								{
									allSameType = false;
									printf("JSONResponse: %s.%s Array type is mixed of %s and %s\r\n", this->clsName.v, name->v, Text::JSONTypeGetName(type).v, Text::JSONTypeGetName(arr->GetArrayValue(i)->GetType()).v);
									break;
								}
							}

							if (allSameType)
							{
								if (type == Text::JSONType::Object)
								{
									sb.ClearStr();
									arr->GetArrayValue(0)->ToJSONString(sb);
									printf("JSONResponse: %s.%s Array type is Object: %s\r\n", this->clsName.v, name->v, sb.v);
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
									printf("JSONResponse: %s.%s Array type is %s\r\n", this->clsName.v, name->v, Text::JSONTypeGetName(type).v);

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
						printf("JSONResponse: Object Content: %s\r\n", sb.v);
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
						printf("JSONResponse: %s.%s = %s\r\n", this->clsName.v, name->v, ((Text::JSONString*)val)->GetValue()->v);
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
		UOSInt i = 0;
		UOSInt j = this->fieldMap.GetCount();
		while (i < j)
		{
			field = this->fieldMap.GetItem(i);
			if (field->IsOptional() || this->json->GetValue(field->GetName()) != 0)
			{
			}
			else
			{
				printf("JSONResponse: %s.%s not found\r\n", this->clsName.v, field->GetName().v);
				this->valid = false;
			}
			i++;
		}
		if (sbSuggest.leng > 0)
		{
			printf("Suggested content for %s:\r\n%s", this->clsName.v, sbSuggest.v);
		}
		if (sbGet.leng > 0)
		{
			printf("Suggested get section for %s:\r\n%s\r\n", this->clsName.v, sbGet.v);
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
		printf("JSONResponse: data is not an object: %s\r\n", sb.ToString());
	}
}

void Net::JSONResponse::AppendFuncName(NotNullPtr<Text::StringBuilderUTF8> sb, Bool boolFunc, NotNullPtr<Text::String> fieldName)
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
		UOSInt i = 0;
		UOSInt j;
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
			sb->Append(Text::CString(&fieldName->v[i + 1], j - i - 1));
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
	Field *field;
	NEW_CLASS(field, Field(name, fieldType, optional, allowNull));
	field = this->fieldMap.PutC(name, field);
	if (field)
	{
		DEL_CLASS(field);
	}

}

void Net::JSONResponse::AddFieldArrDbl(Text::CStringNN name, Bool optional, Bool allowNull)
{
	Bool hasError = false;
	ArrayNativeField<Double> *field;
	NEW_CLASS(field, ArrayNativeField<Double>(name, optional, allowNull, Text::JSONType::Number));
	Text::JSONArray *arr = this->json->GetValueArray(name);
	if (arr == 0)
	{
		if (!optional)
		{
			this->valid = false;
			printf("JSONResponse: %s.%s is not array\r\n", clsName.v, name.v);
		}
		return;
	}
	Text::JSONBase *val;
	UOSInt i = 0;
	UOSInt j = arr->GetArrayLength();
	while (i < j)
	{
		val = arr->GetArrayValue(i);
		if (val == 0 || val->GetType() != Text::JSONType::Number)
		{
			if (!hasError)
			{
				hasError = true;
				this->valid = false;
				printf("JSONResponse: %s.%s[%d] is not number, type = %s\r\n", clsName.v, name.v, (UInt32)i, Text::JSONTypeGetName(val?val->GetType():Text::JSONType::Null).v);
			}
		}
		else
		{
			field->AddValue(val->GetAsDouble());
		}
		i++;
	}
	Field *orifield = this->fieldMap.PutC(name, field);
	if (orifield)
	{
		DEL_CLASS(orifield);
	}
}

void Net::JSONResponse::AddFieldArrStr(Text::CStringNN name, Bool optional, Bool allowNull)
{
	Bool hasError = false;
	ArrayStrField *field;
	NEW_CLASS(field, ArrayStrField(name, optional, allowNull));
	Text::JSONArray *arr = this->json->GetValueArray(name);
	if (arr == 0)
	{
		if (!optional)
		{
			this->valid = false;
			printf("JSONResponse: %s.%s is not array\r\n", clsName.v, name.v);
		}
		return;
	}
	Text::JSONBase *val;
	UOSInt i = 0;
	UOSInt j = arr->GetArrayLength();
	while (i < j)
	{
		val = arr->GetArrayValue(i);
		if (val == 0 || val->GetType() != Text::JSONType::String)
		{
			if (!hasError)
			{
				hasError = true;
				this->valid = false;
				printf("JSONResponse: %s.%s[%d] is not string, type = %s\r\n", clsName.v, name.v, (UInt32)i, Text::JSONTypeGetName(val?val->GetType():Text::JSONType::Null).v);
			}
		}
		else
		{
			field->AddValue(((Text::JSONString*)val)->GetValue());
		}
		i++;
	}
	Field *orifield = this->fieldMap.PutC(name, field);
	if (orifield)
	{
		DEL_CLASS(orifield);
	}
}

Net::JSONResponse::JSONResponse(NotNullPtr<Text::JSONBase> json, Text::CStringNN clsName)
{
	this->clsName = clsName;
	this->json = json;
	this->allowAll = false;
	this->valid = this->json->GetType() == Text::JSONType::Object;
	if (!this->valid)
	{
		printf("JSONResponse: %s is not object\r\n", clsName.v);
	}
	this->json->BeginUse();
}

Net::JSONResponse::~JSONResponse()
{
	this->json->EndUse();
	UOSInt i = this->fieldMap.GetCount();
	while (i-- > 0)
	{
		Field *field = this->fieldMap.GetItem(i);
		DEL_CLASS(field);
	}
}

Bool Net::JSONResponse::IsValid() const
{
	return this->valid;
}

void Net::JSONResponse::ToString(NotNullPtr<Text::StringBuilderUTF8> sb, Text::CStringNN linePrefix) const
{
	Text::StringBuilderUTF8 sbTmp;
	NotNullPtr<Text::String> s;
	Data::ArrayListStringNN keys;
	Data::FastStringKeyIterator<Field*> it = this->fieldMap.KeyIterator();
	while (it.HasNext())
		keys.Add(it.Next());
	Data::Sort::ArtificialQuickSort::Sort(&keys, &keys);
	Field *field;
	Data::ArrayIterator<NotNullPtr<Text::String>> itKey = keys.Iterator();
	while (itKey.HasNext())
	{
		field = this->fieldMap.GetNN(itKey.Next());
		if (field)
		{
			switch (field->GetFieldType())
			{
			case Text::JSONType::Object:
				{
					ObjectField *ofield = (ObjectField*)field;
					NotNullPtr<Net::JSONResponse> subObj;
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
					ArrayField *afield = (ArrayField*)field;
					if (afield->GetArrType() == Text::JSONType::Object)
					{
						ArrayNNField<Net::JSONResponse> *anfield = (ArrayNNField<Net::JSONResponse>*)afield;
						NotNullPtr<const Data::ArrayListNN<Net::JSONResponse>> arr = anfield->GetValue();
						NotNullPtr<Net::JSONResponse> subObj;
						UOSInt i = 0;
						UOSInt j = arr->GetCount();
						while (i < j)
						{
							if (arr->GetItem(i).SetTo(subObj))
							{
								sbTmp.ClearStr();
								sbTmp.Append(linePrefix);
								sbTmp.Append(field->GetName());
								sbTmp.AppendUTF8Char('[');
								sbTmp.AppendUOSInt(i);
								sbTmp.AppendUTF8Char(']');
								sbTmp.AppendUTF8Char('.');
								subObj->ToString(sb, sbTmp.ToCString());
							}
							i++;
						}
					}
					else if (afield->GetArrType() == Text::JSONType::String)
					{
						ArrayStrField *anfield = (ArrayStrField*)afield;
						NotNullPtr<const Data::ArrayListStringNN> arr = anfield->GetValue();
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
							Data::ArrayIterator<NotNullPtr<Text::String>> it = arr->Iterator();
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
						ArrayNativeField<Double> *anfield = (ArrayNativeField<Double>*)afield;
						NotNullPtr<const Data::ArrayList<Double>> arr = anfield->GetValue();
						UOSInt i = 0;
						UOSInt j = arr->GetCount();
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
				sb->AppendDouble(this->json->GetValueAsDouble(field->GetName()));
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

void Net::JSONResponse::ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	this->ToString(sb, CSTR(""));
}
