#include "Stdafx.h"
#include "Net/JSONResponse.h"

Net::JSONResponse::JSONResponse(NotNullPtr<Text::JSONBase> json)
{
	this->json = json;
	this->valid = true;
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
