#include "Stdafx.h"
#include "Net/JSONResponse.h"
#include <stdio.h>

Net::JSONResponse::Field::Field(Text::CStringNN name, FieldType fieldType, Bool optional, Bool allowNull)
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

void Net::JSONResponse::FindMissingFields()
{
	if (this->json->GetType() == Text::JSONType::Object)
	{
		NotNullPtr<Text::JSONObject> obj = NotNullPtr<Text::JSONObject>::ConvertFrom(this->json);

		Data::FastStringMap<Int32> fieldMap;
		Data::ArrayIterator<NotNullPtr<Field>> it = this->fieldList.Iterator();
		while (it.HasNext())
		{
			fieldMap.PutC(it.Next()->GetName(), 1);
		}

		Data::ArrayListNN<Text::String> nameList;
		obj->GetObjectNames(nameList);
		NotNullPtr<Text::String> name;
		Data::ArrayIterator<NotNullPtr<Text::String>> itName = nameList.Iterator();
		while (itName.HasNext())
		{
			name = itName.Next();
			if (fieldMap.GetNN(name) == 0)
			{
				Text::JSONBase *val = obj->GetObjectValue(name->ToCString());
				printf("JSONResponse: %s.%s is not supported, type is %s\r\n", this->clsName.v, name->v, Text::JSONTypeGetName(val?val->GetType():Text::JSONType::Null).v);
				this->valid = false;
			}
		}
	}
}

Net::JSONResponse::JSONResponse(NotNullPtr<Text::JSONBase> json, Text::CStringNN clsName)
{
	this->clsName = clsName;
	this->json = json;
	this->valid = this->json->GetType() == Text::JSONType::Object;
	if (!this->valid)
	{
		printf("JSONResponse: %s is not object\r\n", clsName.v);
	}
	printf("JSONResponse: class is %s\r\n", clsName.v);
	this->json->BeginUse();
}

Net::JSONResponse::~JSONResponse()
{
	this->json->EndUse();
}

Bool Net::JSONResponse::IsValid() const
{
	return this->valid;
}
