#include "Stdafx.h"
#include "Exporter/PEMExporter.h"
#include "IO/FileStream.h"
#include "Net/ASN1Data.h"

Exporter::PEMExporter::PEMExporter()
{

}

Exporter::PEMExporter::~PEMExporter()
{

}

Int32 Exporter::PEMExporter::GetName()
{
	return (Int32)"PEME";
}

IO::FileExporter::SupportType Exporter::PEMExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_ASN1_DATA)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	Net::ASN1Data *asn1 = (Net::ASN1Data*)pobj;
	if (asn1->GetASN1Type() != Net::ASN1Data::AT_X509)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::PEMExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	switch (index)
	{
	case 0:
		Text::StrConcat(nameBuff, (const UTF8Char*)"PEM file");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.pem");
		return true;
	case 1:
		Text::StrConcat(nameBuff, (const UTF8Char*)"Cert file");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.crt");
		return true;
	case 2:
		Text::StrConcat(nameBuff, (const UTF8Char*)"Key file");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.key");
		return true;
	}
	return false;
}

Bool Exporter::PEMExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_ASN1_DATA)
	{
		return false;
	}
	Net::ASN1Data *asn1 = (Net::ASN1Data*)pobj;
	if (asn1->GetASN1Type() != Net::ASN1Data::AT_X509)
	{
		return false;
	}
	return ExportStream(stm, (Crypto::Cert::X509File*)asn1);
}

Bool Exporter::PEMExporter::ExportStream(IO::SeekableStream *stm, Crypto::Cert::X509File *x509)
{
	//////////////////////////////
	return false;
}

Bool Exporter::PEMExporter::ExportFile(const UTF8Char *fileName, Crypto::Cert::X509File *x509)
{
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	Bool succ = ExportStream(fs, x509);
	DEL_CLASS(fs);
	return succ;
}
