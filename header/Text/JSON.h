#ifndef _SM_TEXT_JSON
#define _SM_TEXT_JSON
#include "Data/ArrayList.h"
#include "Data/FastStringMap.h"
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
		INT64,
		StringWO
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
		virtual void ToJSONString(NotNullPtr<Text::StringBuilderUTF8> sb) = 0;
		virtual Bool Equals(Text::CString s) = 0;
		virtual Bool Identical(NotNullPtr<JSONBase> obj) = 0;
		virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb) = 0;
		void BeginUse();
		void EndUse();
		Bool IsString();

		JSONBase *GetValue(Text::CStringNN path);
		Text::String *GetValueString(Text::CStringNN path);
		Text::String *GetValueNewString(Text::CStringNN path);
		Int32 GetValueAsInt32(Text::CStringNN path);
		Int64 GetValueAsInt64(Text::CStringNN path);
		Double GetValueAsDouble(Text::CStringNN path);
		Bool GetValueAsDouble(Text::CStringNN path, OutParam<Double> val);
		Bool GetValueAsBool(Text::CStringNN path);
		Text::JSONArray *GetValueArray(Text::CStringNN path);
		Text::JSONObject *GetValueObject(Text::CStringNN path);
		Int32 GetAsInt32();
		Int64 GetAsInt64();
		Double GetAsDouble();
		Bool GetAsDouble(OutParam<Double> val);
		Bool GetAsBool();

		static JSONBase *ParseJSONStr(Text::CStringNN jsonStr);
		static JSONBase *ParseJSONBytes(const UInt8 *jsonBytes, UOSInt len);
		static JSONBase *ParseJSONBytes(const Data::ByteArrayR &jsonBytes);

	private:
		static const UTF8Char *ClearWS(const UTF8Char *jsonStr);
		static const UTF8Char *ParseJSString(const UTF8Char *jsonStr, NotNullPtr<Text::StringBuilderUTF8> sb);
		static const UTF8Char *ParseJSNumber(const UTF8Char *jsonStr, OutParam<Double> val);
		static JSONBase *ParseJSONStr2(const UTF8Char *jsonStr, const UTF8Char *jsonStrEnd, OutParam<const UTF8Char *> jsonStrEndOut, NotNullPtr<Text::StringBuilderUTF8> sbEnv);
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
		virtual void ToJSONString(NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual Bool Equals(Text::CString s);
		virtual Bool Identical(NotNullPtr<JSONBase> obj);
		virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb);
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
		virtual void ToJSONString(NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual Bool Equals(Text::CString s);
		virtual Bool Identical(NotNullPtr<JSONBase> obj);
		virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb);
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
		virtual void ToJSONString(NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual Bool Equals(Text::CString s);
		virtual Bool Identical(NotNullPtr<JSONBase> obj);
		virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb);
		Int64 GetValue();
	};

	class JSONString : public JSONBase
	{
	private:
		Text::String *val;

	public:
		JSONString(Text::String *val);
		JSONString(NotNullPtr<Text::String> val);
		JSONString(Text::CString val);
	private:
		virtual ~JSONString();

	public:
		virtual JSONType GetType();
		virtual void ToJSONString(NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual Bool Equals(Text::CString s);
		virtual Bool Identical(NotNullPtr<JSONBase> obj);
		virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb);
		Text::String *GetValue();
	};

	class JSONStringW : public JSONBase
	{
	private:
		const WChar *val;

	public:
		JSONStringW(const WChar *val);
	private:
		virtual ~JSONStringW();

	public:
		virtual JSONType GetType();
		virtual void ToJSONString(NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual Bool Equals(Text::CString s);
		virtual Bool Identical(NotNullPtr<JSONBase> obj);
		virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb);
		const WChar *GetValue();
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
		virtual void ToJSONString(NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual Bool Equals(Text::CString s);
		virtual Bool Identical(NotNullPtr<JSONBase> obj);
		virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb);
		Bool GetValue();
	};

	class JSONObject : public JSONBase
	{
	private:
		Data::FastStringMap<Text::JSONBase *> objVals;
	public:
		JSONObject();
	private:
		virtual ~JSONObject();

	public:
		virtual JSONType GetType();
		virtual void ToJSONString(NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual Bool Equals(Text::CString s);
		virtual Bool Identical(NotNullPtr<JSONBase> obj);
		virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb);
		void SetObjectValue(Text::CStringNN name, JSONBase *val);
		void SetObjectInt32(Text::CStringNN name, Int32 val);
		void SetObjectInt64(Text::CStringNN name, Int64 val);
		void SetObjectDouble(Text::CStringNN name, Double val);
		void SetObjectString(Text::CStringNN name, Text::CString val);
		void SetObjectString(Text::CStringNN name, Text::String *val);
		void SetObjectString(Text::CStringNN name, NotNullPtr<Text::String> val);
		void SetObjectBool(Text::CStringNN name, Bool val);
		JSONBase *GetObjectValue(Text::CStringNN name);
		JSONArray *GetObjectArray(Text::CStringNN name);
		JSONObject *GetObjectObject(Text::CStringNN name);
		void GetObjectNames(NotNullPtr<Data::ArrayList<Text::String *>> names);
		Text::String *GetObjectString(Text::CStringNN name);
		Text::String *GetObjectNewString(Text::CStringNN name);
		Double GetObjectDouble(Text::CStringNN name);
		Int32 GetObjectInt32(Text::CStringNN name);
		Int64 GetObjectInt64(Text::CStringNN name);
		Bool GetObjectBool(Text::CStringNN name);
		void RemoveObject(Text::CStringNN name);
	};

	class JSONArray : public JSONBase
	{
	private:
		Data::ArrayList<Text::JSONBase *> arrVals;
	public:
		JSONArray();
	private:
		virtual ~JSONArray();

	public:
		virtual JSONType GetType();
		virtual void ToJSONString(NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual Bool Equals(Text::CString s);
		virtual Bool Identical(NotNullPtr<JSONBase> obj);
		virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb);
		void SetArrayValue(UOSInt index, Text::JSONBase *val);
		void AddArrayValue(Text::JSONBase *val);
		JSONBase *GetArrayValue(UOSInt index);
		JSONObject *GetArrayObject(UOSInt index);
		Double GetArrayDouble(UOSInt index);
		Text::String *GetArrayString(UOSInt index);
		UOSInt GetArrayLength();
		void RemoveArrayItem(UOSInt index);
	};

	class JSONNull : public JSONBase
	{
	public:
		JSONNull();
	private:
		virtual ~JSONNull();

	public:
		virtual JSONType GetType();
		virtual void ToJSONString(NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual Bool Equals(Text::CString s);
		virtual Bool Identical(NotNullPtr<JSONBase> obj);
		virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb);
	};

	Text::CString JSONTypeGetName(JSONType t);
}
#endif
