#ifndef _SM_TEXT_JSON
#define _SM_TEXT_JSON
#include "Data/ArrayListNN.hpp"
#include "Data/ArrayListObj.hpp"
#include "Data/FastStringMapNN.hpp"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	enum class JSONType
	{
		Object,
		Array,
		Number,
		String,
		BOOL,
		Null,
		INT32,
		INT64
	};

	class JSONObject;
	class JSONArray;
	
	class JSONBase
	{
	private:
		OSInt useCnt;

	protected:
		JSONBase();
	protected:
		virtual ~JSONBase();

	public:
		virtual JSONType GetType() = 0;
		virtual void ToJSONString(NN<Text::StringBuilderUTF8> sb) = 0;
		virtual void ToJSONStringWF(NN<Text::StringBuilderUTF8> sb, UOSInt level) = 0;
		virtual Bool Equals(Text::CStringNN s) = 0;
		virtual Bool Identical(NN<JSONBase> obj) = 0;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) = 0;
		void BeginUse();
		void EndUse();
		Bool IsString();

		Optional<JSONBase> GetValue(Text::CStringNN path);
		Optional<Text::String> GetValueString(Text::CStringNN path);
		Optional<Text::String> GetValueNewString(Text::CStringNN path);
		Int32 GetValueAsInt32(Text::CStringNN path);
		Bool GetValueAsInt32(Text::CStringNN path, OutParam<Int32> val);
		Int64 GetValueAsInt64(Text::CStringNN path);
		Bool GetValueAsInt64(Text::CStringNN path, OutParam<Int64> val);
		Double GetValueAsDoubleOrNAN(Text::CStringNN path) { NN<Text::JSONBase> json; return this->GetValue(path).SetTo(json)?json->GetAsDoubleOrNAN():NAN; }
		Double GetValueAsDoubleOr(Text::CStringNN path, Double v) { NN<Text::JSONBase> json; return this->GetValue(path).SetTo(json)?json->GetAsDoubleOrNAN():v; }
		Bool GetValueAsDouble(Text::CStringNN path, OutParam<Double> val);
		Bool GetValueAsBool(Text::CStringNN path);
		Optional<Text::JSONArray> GetValueArray(Text::CStringNN path);
		Optional<Text::JSONObject> GetValueObject(Text::CStringNN path);
		JSONType GetValueType(Text::CStringNN path);
		Int32 GetAsInt32();
		Bool GetAsInt32(OutParam<Int32> val);
		Int64 GetAsInt64();
		Bool GetAsInt64(OutParam<Int64> val);
		Double GetAsDoubleOrNAN();
		Double GetAsDoubleOr(Double v);
		Bool GetAsDouble(OutParam<Double> val);
		Bool GetAsBool();

		static Optional<JSONBase> ParseJSONStr(Text::CStringNN jsonStr);
		static Optional<JSONBase> ParseJSONBytes(UnsafeArray<const UInt8> jsonBytes, UOSInt len);
		static Optional<JSONBase> ParseJSONBytes(const Data::ByteArrayR &jsonBytes);

	private:
		static UnsafeArray<const UTF8Char> ClearWS(UnsafeArray<const UTF8Char> jsonStr);
		static UnsafeArrayOpt<const UTF8Char> ParseJSString(UnsafeArray<const UTF8Char> jsonStr, NN<Text::StringBuilderUTF8> sb);
		static UnsafeArrayOpt<const UTF8Char> ParseJSNumber(UnsafeArray<const UTF8Char> jsonStr, OutParam<Double> val, OptOut<Bool> noDecimal);
		static Optional<JSONBase> ParseJSONStr2(UnsafeArray<const UTF8Char> jsonStr, UnsafeArray<const UTF8Char> jsonStrEnd, OutParam<UnsafeArrayOpt<const UTF8Char>> jsonStrEndOut, NN<Text::StringBuilderUTF8> sbEnv);
		static Bool Str2Bool(NN<Text::String> s);
	};

	class JSONNumber : public JSONBase
	{
	private:
		Double val;

	public:
		JSONNumber(Double val);
	private:
		virtual ~JSONNumber();

	public:
		virtual JSONType GetType();
		virtual void ToJSONString(NN<Text::StringBuilderUTF8> sb);
		virtual void ToJSONStringWF(NN<Text::StringBuilderUTF8> sb, UOSInt level);
		virtual Bool Equals(Text::CStringNN s);
		virtual Bool Identical(NN<JSONBase> obj);
		virtual void ToString(NN<Text::StringBuilderUTF8> sb);
		Double GetValue();
	};

	class JSONInt32 : public JSONBase
	{
	private:
		Int32 val;

	public:
		JSONInt32(Int32 val);
	private:
		virtual ~JSONInt32();

	public:
		virtual JSONType GetType();
		virtual void ToJSONString(NN<Text::StringBuilderUTF8> sb);
		virtual void ToJSONStringWF(NN<Text::StringBuilderUTF8> sb, UOSInt level);
		virtual Bool Equals(Text::CStringNN s);
		virtual Bool Identical(NN<JSONBase> obj);
		virtual void ToString(NN<Text::StringBuilderUTF8> sb);
		Int32 GetValue();
	};

	class JSONInt64 : public JSONBase
	{
	private:
		Int64 val;

	public:
		JSONInt64(Int64 val);
	private:
		virtual ~JSONInt64();

	public:
		virtual JSONType GetType();
		virtual void ToJSONString(NN<Text::StringBuilderUTF8> sb);
		virtual void ToJSONStringWF(NN<Text::StringBuilderUTF8> sb, UOSInt level);
		virtual Bool Equals(Text::CStringNN s);
		virtual Bool Identical(NN<JSONBase> obj);
		virtual void ToString(NN<Text::StringBuilderUTF8> sb);
		Int64 GetValue();
	};

	class JSONString : public JSONBase
	{
	private:
		NN<Text::String> val;

	public:
		JSONString(NN<Text::String> val);
		JSONString(Text::CStringNN val);
	private:
		virtual ~JSONString();

	public:
		virtual JSONType GetType();
		virtual void ToJSONString(NN<Text::StringBuilderUTF8> sb);
		virtual void ToJSONStringWF(NN<Text::StringBuilderUTF8> sb, UOSInt level);
		virtual Bool Equals(Text::CStringNN s);
		virtual Bool Identical(NN<JSONBase> obj);
		virtual void ToString(NN<Text::StringBuilderUTF8> sb);
		NN<Text::String> GetValue();
	};

	class JSONBool : public JSONBase
	{
	private:
		Bool val;

	public:
		JSONBool(Bool val);
	private:
		virtual ~JSONBool();

	public:
		virtual JSONType GetType();
		virtual void ToJSONString(NN<Text::StringBuilderUTF8> sb);
		virtual void ToJSONStringWF(NN<Text::StringBuilderUTF8> sb, UOSInt level);
		virtual Bool Equals(Text::CStringNN s);
		virtual Bool Identical(NN<JSONBase> obj);
		virtual void ToString(NN<Text::StringBuilderUTF8> sb);
		Bool GetValue();
	};

	class JSONObject : public JSONBase
	{
	private:
		Data::FastStringMapNN<Text::JSONBase> objVals;
	public:
		JSONObject();
	private:
		virtual ~JSONObject();

	public:
		virtual JSONType GetType();
		virtual void ToJSONString(NN<Text::StringBuilderUTF8> sb);
		virtual void ToJSONStringWF(NN<Text::StringBuilderUTF8> sb, UOSInt level);
		virtual Bool Equals(Text::CStringNN s);
		virtual Bool Identical(NN<JSONBase> obj);
		virtual void ToString(NN<Text::StringBuilderUTF8> sb);
		NN<JSONObject> SetObjectValue(Text::CStringNN name, Optional<JSONBase> val);
		NN<JSONObject> SetObjectValueAndRelease(Text::CStringNN name, NN<JSONBase> val);
		NN<JSONObject> SetObjectInt32(Text::CStringNN name, Int32 val);
		NN<JSONObject> SetObjectInt64(Text::CStringNN name, Int64 val);
		NN<JSONObject> SetObjectDouble(Text::CStringNN name, Double val);
		NN<JSONObject> SetObjectString(Text::CStringNN name, Text::CStringNN val);
		NN<JSONObject> SetObjectString(Text::CStringNN name, Optional<Text::String> val);
		NN<JSONObject> SetObjectString(Text::CStringNN name, NN<Text::String> val);
		NN<JSONObject> SetObjectBool(Text::CStringNN name, Bool val);
		Optional<JSONBase> GetObjectValue(Text::CStringNN name);
		Optional<JSONArray> GetObjectArray(Text::CStringNN name);
		Optional<JSONObject> GetObjectObject(Text::CStringNN name);
		void GetObjectNames(NN<Data::ArrayListNN<Text::String>> names);
		Optional<Text::String> GetObjectString(Text::CStringNN name);
		Optional<Text::String> GetObjectNewString(Text::CStringNN name);
		Double GetObjectDoubleOrNAN(Text::CStringNN name);
		Double GetObjectDoubleOr(Text::CStringNN name, Double v);
		Int32 GetObjectInt32(Text::CStringNN name);
		Int64 GetObjectInt64(Text::CStringNN name);
		Bool GetObjectInt64(Text::CStringNN name, OutParam<Int64> v);
		Bool GetObjectBool(Text::CStringNN name);
		Bool GetObjectBool(Text::CStringNN name, OutParam<Bool> v);
		void RemoveObject(Text::CStringNN name);
		static NN<Text::JSONObject> New();
	};

	class JSONArray : public JSONBase
	{
	private:
		Data::ArrayListObj<Optional<Text::JSONBase>> arrVals;
	public:
		JSONArray();
	private:
		virtual ~JSONArray();

	public:
		virtual JSONType GetType();
		virtual void ToJSONString(NN<Text::StringBuilderUTF8> sb);
		virtual void ToJSONStringWF(NN<Text::StringBuilderUTF8> sb, UOSInt level);
		virtual Bool Equals(Text::CStringNN s);
		virtual Bool Identical(NN<JSONBase> obj);
		virtual void ToString(NN<Text::StringBuilderUTF8> sb);
		void SetArrayValue(UOSInt index, Optional<Text::JSONBase> val);
		void AddArrayValue(Optional<Text::JSONBase> val);
		NN<JSONArray> AddArrayString(Text::CStringNN val);
		JSONType GetArrayType(UOSInt index);
		Optional<JSONBase> GetArrayValue(UOSInt index);
		Optional<JSONObject> GetArrayObject(UOSInt index);
		Double GetArrayDoubleOrNAN(UOSInt index);
		Double GetArrayDoubleOr(UOSInt index, Double v);
		Optional<Text::String> GetArrayString(UOSInt index);
		UOSInt GetArrayLength();
		void RemoveArrayItem(UOSInt index);

		static NN<JSONArray> New();
	};

	class JSONNull : public JSONBase
	{
	public:
		JSONNull();
	private:
		virtual ~JSONNull();

	public:
		virtual JSONType GetType();
		virtual void ToJSONString(NN<Text::StringBuilderUTF8> sb);
		virtual void ToJSONStringWF(NN<Text::StringBuilderUTF8> sb, UOSInt level);
		virtual Bool Equals(Text::CStringNN s);
		virtual Bool Identical(NN<JSONBase> obj);
		virtual void ToString(NN<Text::StringBuilderUTF8> sb);
	};

	Text::CStringNN JSONTypeGetName(JSONType t);
}
#endif
