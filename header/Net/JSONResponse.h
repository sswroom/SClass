#ifndef _SM_NET_JSONRESPONSE
#define _SM_NET_JSONRESPONSE
#include "Data/ArrayListNN.h"
#include "Net/HTTPJSONReader.h"
#include "Text/JSON.h"

namespace Net
{
	class JSONResponse
	{
	public:
		enum class FieldType
		{
			Number,
			String,
			Object
		};

		class Field
		{
		private:
			Text::CStringNN name;
			FieldType fieldType;
			Bool optional;
			Bool allowNull;
		public:
			Field(Text::CStringNN name, FieldType fieldType, Bool optional, Bool allowNull);
			~Field();

			Text::CStringNN GetName() const;
		};
	protected:
		Bool valid;
		NotNullPtr<Text::JSONBase> json;
		Data::ArrayListNN<Field> fieldList;
		Text::CStringNN clsName;

		void FindMissingFields();
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
