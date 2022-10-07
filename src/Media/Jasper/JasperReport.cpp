#include "Stdafx.h"
#include "Media/Jasper/JasperReport.h"

Media::Jasper::JasperReport::JasperReport(Text::String *sourceName) : IO::ParsedObject(sourceName)
{

}

Media::Jasper::JasperReport::JasperReport(Text::CString sourceName) : IO::ParsedObject(sourceName)
{

}

Media::Jasper::JasperReport::~JasperReport()
{

}

IO::ParserType Media::Jasper::JasperReport::GetParserType() const
{
	return IO::ParserType::JasperReport;
}
