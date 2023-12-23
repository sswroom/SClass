#ifndef _SM_NET_JSONRESPONSE
#define _SM_NET_JSONRESPONSE
#include "Data/ArrayListNN.h"
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

		};
	protected:
		Bool valid;
		NotNullPtr<Text::JSONBase> json;
		Data::ArrayListNN<Field> fieldList;

	public:
		JSONResponse(NotNullPtr<Text::JSONBase> json);
		virtual ~JSONResponse();

		Bool IsValid() const;
	};
}

#define JSONRESP_BEGIN(clsName) class clsName : public Net::JSONResponse \
	{ \
		clsName(NotNullPtr<Text::JSONBase> json) : Net::JSONResponse(json) \
		{
#define JSONRESP_SEC_GET } \
		virtual ~clsName() {} \

#define JSONRESP_END }

#endif
