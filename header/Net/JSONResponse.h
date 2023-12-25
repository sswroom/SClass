#ifndef _SM_NET_JSONRESPONSE
#define _SM_NET_JSONRESPONSE
#include "Data/ArrayListNN.h"
#include "Net/HTTPJSONReader.h"
#include "Text/JSON.h"

#include <stdio.h>

namespace Net
{
	class JSONResponse
	{
	public:
		class Field
		{
		private:
			Text::CStringNN name;
			Text::JSONType fieldType;
			Bool optional;
			Bool allowNull;
		public:
			Field(Text::CStringNN name, Text::JSONType fieldType, Bool optional, Bool allowNull);
			virtual ~Field();

			Text::CStringNN GetName() const;
		};

		class ObjectField : public Field
		{
		private:
			Optional<JSONResponse> obj;
		public:
			ObjectField(Text::CStringNN name, Text::JSONType fieldType, Bool optional, Bool allowNull, Optional<JSONResponse> obj);
			virtual ~ObjectField();

			Optional<JSONResponse> GetValue() const;
		};
	protected:
		Bool valid;
		NotNullPtr<Text::JSONBase> json;
		Data::FastStringMap<Field*> fieldMap;
		Text::CStringNN clsName;

		void FindMissingFields();
		void AppendFuncName(NotNullPtr<Text::StringBuilderUTF8> sb, Bool boolFunc, NotNullPtr<Text::String> fieldName);
		void AddField(Text::CStringNN name, Text::JSONType fieldType, Bool optional, Bool allowNull);
	public:
		JSONResponse(NotNullPtr<Text::JSONBase> json, Text::CStringNN clsName);
		virtual ~JSONResponse();

		Bool IsValid() const;
	};
}

#define JSONRESP_BEGIN(clsName) class clsName : public Net::JSONResponse \
	{ \
	public: \
		clsName(NotNullPtr<Text::JSONBase> json) : Net::JSONResponse(json, CSTR(#clsName)) \
		{

#define JSONRESP_SEC_GET(clsName) this->FindMissingFields(); \
		} \
		virtual ~clsName() {} \

#define JSONRESP_END };

#define JSONRESP_BOOL(name, optional, allowNull) this->AddField(CSTR(name), Text::JSONType::BOOL, optional, allowNull);
#define JSONRESP_STR(name, optional, allowNull) this->AddField(CSTR(name), Text::JSONType::String, optional, allowNull);
#define JSONRESP_DOUBLE(name, optional, allowNull) this->AddField(CSTR(name), Text::JSONType::Number, optional, allowNull);
#define JSONRESP_ARRAY_DOUBLE(name, optional, allowNull) this->AddFieldArrDbl(CSTR(name), optional, allowNull);
#define JSONRESP_ARRAY_STR(name, optional, allowNull) this->AddFieldArrStr(CSTR(name), optional, allowNull);
#define JSONRESP_OBJ(name, optional, allowNull, className) { \
	NotNullPtr<Text::JSONBase> jobj; \
	NotNullPtr<className> cobj; \
	ObjectField *ofield; \
	Field *field; \
	if (jobj.Set(this->json->GetValue(CSTR(name))) && jobj->GetType() == Text::JSONType::Object) { \
		NEW_CLASSNN(cobj, className(jobj)); \
		NEW_CLASS(ofield, ObjectField(CSTR(name), Text::JSONType::Object, optional, allowNull, Optional<className>(cobj))); \
		field = this->fieldMap.PutC(CSTR(name), ofield); \
		if (field) { DEL_CLASS(field); } \
	} else { \
		NEW_CLASS(ofield, ObjectField(CSTR(name), Text::JSONType::Object, optional, allowNull, 0)); \
		field = this->fieldMap.PutC(CSTR(name), ofield); \
		if (field) { DEL_CLASS(field); } \
		if (jobj.Set(this->json->GetValue(CSTR(name)))) { \
			if (jobj->GetType() != Text::JSONType::Null) printf("JSONResponse: %s.%s is not object type, type is %s\r\n", this->clsName.v, name, Text::JSONTypeGetName(jobj->GetType()).v); \
			else if (!allowNull) printf("JSONResponse: %s.%s is null which is not allowed\r\n", this->clsName.v, name); \
		} else if (!optional) printf("JSONResponse: %s.%s is not found which is not optional\r\n", this->clsName.v, name); \
	} }

#define JSONRESP_GETSTR(name, funcName) NotNullPtr<Text::String> funcName() const { return Text::String::OrEmpty(this->json->GetValueString(CSTR(name))); }
#define JSONRESP_GETSTROPT(name, funcName) Optional<Text::String> funcName() const { return this->json->GetValueString(CSTR(name)); }
#define JSONRESP_GETDOUBLE(name, funcName, defVal) Double funcName() const { Double v; if (!this->json->GetValueAsDouble(CSTR(name), v)) return defVal; return v; }
#define JSONRESP_GETINT32(name, funcName, defVal) Int32 funcName() const { Int32 v; if (!this->json->GetValueAsInt32(CSTR(name), v)) return defVal; return v; }
#define JSONRESP_GETINT64(name, funcName, defVal) Int64 funcName() const { Int64 v; if (!this->json->GetValueAsInt64(CSTR(name), v)) return defVal; return v; }
#define JSONRESP_GETBOOL(name, funcName) Bool funcName() const { return this->json->GetValueAsBool(CSTR(name)); }
#define JSONRESP_GETOBJ(name, funcName, clsName) Optional<clsName> funcName() const { ObjectField *f = (ObjectField*)this->fieldMap.GetC(CSTR(name)); if (f) return Optional<clsName>::ConvertFrom(f->GetValue()); return 0; }
#define JSONRESP_GETARRAY_DOUBLE(name, funcName) //Bool funcName() const { return this->json->GetValueAsBool(CSTR(name)); }
#define JSONRESP_GETARRAY_STR(name, funcName) //Bool funcName() const { return this->json->GetValueAsBool(CSTR(name)); }

#define JSONREQ_RET(sockf, ssl, url, respType) \
	NotNullPtr<Text::JSONBase> json; \
	if (!json.Set(Net::HTTPJSONReader::Read(sockf, ssl, url))) return 0; \
	NotNullPtr<respType> ret; \
	NEW_CLASSNN(ret, respType(json)); \
	json->EndUse(); \
	if (ret->IsValid()) return ret; \
	ret.Delete(); \
	return 0;
#endif
