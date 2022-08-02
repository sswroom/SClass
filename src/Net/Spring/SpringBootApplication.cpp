#include "Stdafx.h"
#include "IO/JavaProperties.h"
#include "Net/Spring/SpringBootApplication.h"

Net::Spring::SpringBootApplication::SpringBootApplication() : consoleLog(&console)
{
	this->log.AddLogHandler(&this->consoleLog, IO::ILogHandler::LOG_LEVEL_RAW);
	this->cfg = IO::JavaProperties::Parse(CSTR("application.properties"));
}

Net::Spring::SpringBootApplication::~SpringBootApplication()
{
	SDEL_CLASS(this->cfg);
}