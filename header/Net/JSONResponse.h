#ifndef _SM_NET_JSONRESPONSE
#define _SM_NET_JSONRESPONSE
#include "Data/ArrayListNative.hpp"
#include "Data/ArrayListNN.hpp"
#include "Data/FastStringMapNN.hpp"
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
			Text::JSONType GetFieldType() const;
			Bool IsOptional() const;
			Bool IsAllowNull() const;
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

		class ArrayField : public Field
		{
		private:
			Text::JSONType arrType;
		public:
			ArrayField(Text::CStringNN name, Bool optional, Bool allowNull, Text::JSONType arrType) : Field(name, Text::JSONType::Array, optional, allowNull) {this->arrType = arrType;};
			virtual ~ArrayField() {};

			Text::JSONType GetArrType() const { return this->arrType; }
		};

		template <typename T> class ArrayNativeField : public ArrayField
		{
		private:
			Data::ArrayListNative<T> vals;
		public:
			ArrayNativeField(Text::CStringNN name, Bool optional, Bool allowNull, Text::JSONType arrType) : ArrayField(name, optional, allowNull, arrType) {};
			virtual ~ArrayNativeField() {};

			void AddValue(T val) { this->vals.Add(val);}
			NN<const Data::ArrayListNative<T>> GetValue() const { return this->vals; }
		};

		template <typename T> class ArrayNNField : public ArrayField
		{
		private:
			Data::ArrayListNN<T> vals;
		public:
			ArrayNNField(Text::CStringNN name, Bool optional, Bool allowNull) : ArrayField(name, optional, allowNull, Text::JSONType::Object) {};
			virtual ~ArrayNNField() { this->vals.DeleteAll(); };

			void AddValue(NN<T> val) { this->vals.Add(val);}
			NN<const Data::ArrayListNN<T>> GetValue() const { return this->vals; }
		};

		class ArrayStrField : public ArrayField
		{
		private:
			Data::ArrayListStringNN vals;
		public:
			ArrayStrField(Text::CStringNN name, Bool optional, Bool allowNull) : ArrayField(name, optional, allowNull, Text::JSONType::String) {};
			virtual ~ArrayStrField() { this->vals.FreeAll(); };

			void AddValue(NN<Text::String> val) { this->vals.Add(val);}
			NN<const Data::ArrayListStringNN> GetValue() const { return this->vals; }
		};

	protected:
		Bool valid;
		Bool allowAll;
		NN<Text::JSONBase> json;
		Data::FastStringMapNN<Field> fieldMap;
		Text::CStringNN clsName;

		void FindMissingFields();
		void AppendFuncName(NN<Text::StringBuilderUTF8> sb, Bool boolFunc, NN<Text::String> fieldName);
		void AddField(Text::CStringNN name, Text::JSONType fieldType, Bool optional, Bool allowNull);
		void AddFieldArrDbl(Text::CStringNN name, Bool optional, Bool allowNull);
		void AddFieldArrStr(Text::CStringNN name, Bool optional, Bool allowNull);
	public:
		JSONResponse(NN<Text::JSONBase> json, Text::CStringNN clsName);
		virtual ~JSONResponse();

		Bool IsValid() const;
		void ToString(NN<Text::StringBuilderUTF8> sb, Text::CStringNN linePrefix) const;
		void ToString(NN<Text::StringBuilderUTF8> sb) const;
		void ToJSONWF(NN<Text::StringBuilderUTF8> sb) const;
	};

	template <class T> class JSONResponseArr
	{
	protected:
		Bool valid;
		NN<Text::JSONBase> json;
		Text::CStringNN clsName;

	public:
		JSONResponseArr(NN<Text::JSONBase> json, Text::CStringNN clsName)
		{
			this->clsName = clsName;
			this->json = json;
			this->valid = this->json->GetType() == Text::JSONType::Array;
			if (!this->valid)
			{
				printf("JSONResponseArr: %s is not array\r\n", clsName.v.Ptr());
			}
			this->json->BeginUse();
		}

		virtual ~JSONResponseArr()
		{
			this->json->EndUse();
		}

		Optional<T> GetNewItem(UOSInt index)
		{
			NN<Text::JSONBase> json;
			NN<T> t;
			if (this->json->GetType() == Text::JSONType::Array && NN<Text::JSONArray>::ConvertFrom(this->json)->GetArrayValue(index).SetTo(json))
			{
				NEW_CLASSNN(t, T(json));
				if (!t->IsValid())
				{
					t.Delete();
					return nullptr;
				}
				return t;
			}
			return nullptr;
		}

		UOSInt GetCount() const
		{
			if (this->json->GetType() == Text::JSONType::Array)
			{
				return NN<Text::JSONArray>::ConvertFrom(this->json)->GetArrayLength();
			}
			return 0;
		}

		Bool IsValid() const
		{
			if (this->json->GetType() != Text::JSONType::Array)
			{
				return false;
			}
			NN<Text::JSONArray> arr = NN<Text::JSONArray>::ConvertFrom(this->json);
			NN<Text::JSONBase> o;
			UOSInt i = 0;
			UOSInt j = arr->GetArrayLength();
			while (i < j)
			{
				if (!arr->GetArrayValue(i).SetTo(o))
				{
					return false;
				}
				T t(o);
				if (!t.IsValid())
					return false;
				i++;
			}
			return true;			
		}

		void ToString(NN<Text::StringBuilderUTF8> sb, Text::CStringNN linePrefix) const
		{
			if (this->json->GetType() == Text::JSONType::Array)
			{
				NN<Text::JSONArray> arr = NN<Text::JSONArray>::ConvertFrom(this->json);
				Text::StringBuilderUTF8 sbPrefix;
				NN<Text::JSONBase> obj;
				UOSInt i = 0;
				UOSInt j = arr->GetArrayLength();
				while (i < j)
				{
					if (arr->GetArrayValue(i).SetTo(obj))
					{
						sbPrefix.ClearStr();
						sbPrefix.Append(linePrefix);
						sbPrefix.AppendUTF8Char('[');
						sbPrefix.AppendUOSInt(i);
						sbPrefix.AppendUTF8Char(']');
						T t(obj);
						t.ToString(sb, sbPrefix.ToCString());
					}
					i++;
				}
			}
		}

		void ToString(NN<Text::StringBuilderUTF8> sb) const
		{
			ToString(sb, CSTR(""));
		}
	};
}

#define JSONRESP_BEGIN(clsName) class clsName : public Net::JSONResponse \
	{ \
	public: \
		clsName(NN<Text::JSONBase> json) : Net::JSONResponse(json, CSTR(#clsName)) \
		{

#define JSONRESP_SEC_GET(clsName) this->FindMissingFields(); \
		} \
		virtual ~clsName() {} \

#define JSONRESP_END };
#define JSONRESP_BEGINARRAY(clsName, innerClass) class clsName : public Net::JSONResponseArr<innerClass> \
	{ \
	public: \
		clsName(NN<Text::JSONBase> json) : Net::JSONResponseArr<innerClass>(json, CSTR(#clsName)) \
		{ \
		} \
		virtual ~clsName() {} \
	};
#define JSONRESP_ENDARRAY

		
#define JSONRESP_ALLOW_ALL this->allowAll = true;

#define JSONRESP_BOOL(name, optional, allowNull) this->AddField(CSTR(name), Text::JSONType::BOOL, optional, allowNull);
#define JSONRESP_STR(name, optional, allowNull) this->AddField(CSTR(name), Text::JSONType::String, optional, allowNull);
#define JSONRESP_DOUBLE(name, optional, allowNull) this->AddField(CSTR(name), Text::JSONType::Number, optional, allowNull);
#define JSONRESP_ARRAY_DOUBLE(name, optional, allowNull) this->AddFieldArrDbl(CSTR(name), optional, allowNull);
#define JSONRESP_ARRAY_STR(name, optional, allowNull) this->AddFieldArrStr(CSTR(name), optional, allowNull);
#define JSONRESP_ARRAY_OBJ(name, optional, allowNull, className) { \
	Bool hasError = false; \
	NN<ArrayNNField<className>> field; \
	NN<Text::JSONArray> arr; \
	if (!this->json->GetValueArray(CSTR(name)).SetTo(arr)) \
	{ \
		if (this->json->GetValue(CSTR(name)).NotNull()) \
		{ \
			this->valid = false; \
			printf("JSONResponse: %s.%s is not array\r\n", clsName.v.Ptr(), name); \
		} \
		else if (!optional) \
		{ \
			this->valid = false; \
			printf("JSONResponse: %s.%s is not found and is not optional\r\n", clsName.v.Ptr(), name); \
		} \
	} \
	else \
	{ \
		NEW_CLASSNN(field, ArrayNNField<className>(CSTR(name), optional, allowNull)); \
		NN<Text::JSONObject> val; \
		UOSInt i = 0; \
		UOSInt j = arr->GetArrayLength(); \
		while (i < j) \
		{ \
			if (!arr->GetArrayObject(i).SetTo(val)) \
			{ \
				if (!hasError) \
				{ \
					hasError = true; \
					this->valid = false; \
					NN<Text::JSONBase> jobj; \
					printf("JSONResponse: %s.%s[%d] is not object, type = %s\r\n", clsName.v.Ptr(), name, (UInt32)i, Text::JSONTypeGetName(arr->GetArrayValue(i).SetTo(jobj)?jobj->GetType():Text::JSONType::Null).v.Ptr()); \
				} \
			} \
			else \
			{ \
				NN<className> v; \
				NEW_CLASSNN(v, className(val)); \
				if (!v->IsValid() && !hasError) \
				{ \
					hasError = true; \
					this->valid = false; \
					printf("JSONResponse: %s.%s[%d] is not valid %s type\r\n", clsName.v.Ptr(), name, (UInt32)i, #className); \
				} \
				field->AddValue(v); \
			} \
			i++; \
		} \
		NN<Field> orifield; \
		if (this->fieldMap.PutC(CSTR(name), field).SetTo(orifield)) \
		{ \
			orifield.Delete(); \
		} \
	} \
}
#define JSONRESP_OBJ(name, optional, allowNull, className) { \
	NN<Text::JSONBase> jobj; \
	NN<className> cobj; \
	NN<ObjectField> ofield; \
	NN<Field> field; \
	if (this->json->GetValue(CSTR(name)).SetTo(jobj) && jobj->GetType() == Text::JSONType::Object) { \
		NEW_CLASSNN(cobj, className(jobj)); \
		NEW_CLASSNN(ofield, ObjectField(CSTR(name), Text::JSONType::Object, optional, allowNull, Optional<className>(cobj))); \
		if (this->fieldMap.PutC(CSTR(name), ofield).SetTo(field)) { field.Delete(); } \
	} else { \
		NEW_CLASSNN(ofield, ObjectField(CSTR(name), Text::JSONType::Object, optional, allowNull, nullptr)); \
		if (this->fieldMap.PutC(CSTR(name), ofield).SetTo(field)) { field.Delete(); } \
		if (this->json->GetValue(CSTR(name)).SetTo(jobj)) { \
			if (jobj->GetType() != Text::JSONType::Null) printf("JSONResponse: %s.%s is not object type, type is %s\r\n", this->clsName.v.Ptr(), name, Text::JSONTypeGetName(jobj->GetType()).v.Ptr()); \
			else if (!allowNull) printf("JSONResponse: %s.%s is null which is not allowed\r\n", this->clsName.v.Ptr(), name); \
		} else if (!optional) printf("JSONResponse: %s.%s is not found which is not optional\r\n", this->clsName.v.Ptr(), name); \
	} }

#define JSONRESP_GETSTR(name, funcName) NN<Text::String> funcName() const { return Text::String::OrEmpty(this->json->GetValueString(CSTR(name))); }
#define JSONRESP_GETSTROPT(name, funcName) Optional<Text::String> funcName() const { return this->json->GetValueString(CSTR(name)); }
#define JSONRESP_GETDOUBLE(name, funcName, defVal) Double funcName() const { Double v; if (!this->json->GetValueAsDouble(CSTR(name), v)) return defVal; return v; }
#define JSONRESP_GETINT32(name, funcName, defVal) Int32 funcName() const { Int32 v; if (!this->json->GetValueAsInt32(CSTR(name), v)) return defVal; return v; }
#define JSONRESP_GETINT64(name, funcName, defVal) Int64 funcName() const { Int64 v; if (!this->json->GetValueAsInt64(CSTR(name), v)) return defVal; return v; }
#define JSONRESP_GETBOOL(name, funcName) Bool funcName() const { return this->json->GetValueAsBool(CSTR(name)); }
#define JSONRESP_GETOBJ(name, funcName, clsName) Optional<clsName> funcName() const { NN<ObjectField> f; if (Optional<ObjectField>::ConvertFrom(this->fieldMap.GetC(CSTR(name))).SetTo(f)) return Optional<clsName>::ConvertFrom(f->GetValue()); return nullptr; }
#define JSONRESP_GETARRAY_DOUBLE(name, funcName) Optional<const Data::ArrayListNative<Double>> funcName() const { NN<ArrayNativeField<Double>> f; if (Optional<ArrayNativeField<Double>>::ConvertFrom(this->fieldMap.GetC(CSTR(name))).SetTo(f)) return f->GetValue(); return nullptr; }
#define JSONRESP_GETARRAY_STR(name, funcName) Optional<const Data::ArrayListStringNN> funcName() const { NN<ArrayStrField> f; if (Optional<ArrayStrField>::ConvertFrom(this->fieldMap.GetC(CSTR(name))).SetTo(f)) return f->GetValue(); return nullptr; }
#define JSONRESP_GETARRAY_OBJ(name, funcName, clsName) Optional<const Data::ArrayListNN<clsName>> funcName() const { NN<ArrayNNField<clsName>> f; if (Optional<ArrayNNField<clsName>>::ConvertFrom(this->fieldMap.GetC(CSTR(name))).SetTo(f)) return f->GetValue(); return nullptr; }

#define JSONREQ_RET(clif, ssl, url, respType) \
	NN<Text::JSONBase> json; \
	if (!Net::HTTPJSONReader::Read(clif, ssl, url).SetTo(json)) return nullptr; \
	NN<respType> ret; \
	NEW_CLASSNN(ret, respType(json)); \
	json->EndUse(); \
	if (ret->IsValid()) return ret; \
	ret.Delete(); \
	return nullptr;
#endif
