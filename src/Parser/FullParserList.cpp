#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FullParserList.h"
#include "Parser/FileParser/AC3Parser.h"
#include "Parser/FileParser/ADXParser.h"
#include "Parser/FileParser/AFSParser.h"
#include "Parser/FileParser/ANIParser.h"
#include "Parser/FileParser/AOSParser.h"
#include "Parser/FileParser/APEParser.h"
#include "Parser/FileParser/ASN1Parser.h"
#include "Parser/FileParser/AUIParser.h"
#include "Parser/FileParser/AVIParser.h"
#include "Parser/FileParser/B3DMParser.h"
#include "Parser/FileParser/BMPParser.h"
#include "Parser/FileParser/BSAParser.h"
#include "Parser/FileParser/BurikoArcParser.h"
#include "Parser/FileParser/BurikoPackFileParser.h"
#include "Parser/FileParser/CABParser.h"
#include "Parser/FileParser/CDXAParser.h"
#include "Parser/FileParser/CFBParser.h"
#include "Parser/FileParser/CIPParser.h"
#include "Parser/FileParser/ClassParser.h"
#include "Parser/FileParser/COMParser.h"
#include "Parser/FileParser/CSVParser.h"
#include "Parser/FileParser/CUEParser.h"
#include "Parser/FileParser/DBFParser.h"
#include "Parser/FileParser/DCPackParser.h"
#include "Parser/FileParser/DTSParser.h"
#include "Parser/FileParser/ELFParser.h"
#include "Parser/FileParser/EVTXParser.h"
#include "Parser/FileParser/EXEParser.h"
#include "Parser/FileParser/GamedatPac2Parser.h"
#include "Parser/FileParser/GIFParser.h"
#include "Parser/FileParser/GLOCParser.h"
#include "Parser/FileParser/GUIImgParser.h"
#include "Parser/FileParser/GZIPParser.h"
#include "Parser/FileParser/HEIFParser.h"
#include "Parser/FileParser/HTRecParser.h"
#include "Parser/FileParser/ICOParser.h"
#include "Parser/FileParser/ID3Parser.h"
#include "Parser/FileParser/IMGParser.h"
#include "Parser/FileParser/IPACParser.h"
#include "Parser/FileParser/IS2Parser.h"
#include "Parser/FileParser/ISOParser.h"
#include "Parser/FileParser/JKSParser.h"
#include "Parser/FileParser/JP2Parser.h"
#include "Parser/FileParser/JPGParser.h"
#include "Parser/FileParser/JSONParser.h"
#include "Parser/FileParser/JXLParser.h"
#include "Parser/FileParser/LinkArcParser.h"
#include "Parser/FileParser/LOGParser.h"
#include "Parser/FileParser/LUTParser.h"
//#include "Parser/FileParser/M2VParser.h"
#include "Parser/FileParser/M2VStmParser.h"
#include "Parser/FileParser/MAIPackParser.h"
#include "Parser/FileParser/MajiroArcParser.h"
#include "Parser/FileParser/MD5Parser.h"
#include "Parser/FileParser/MDBParser.h"
#include "Parser/FileParser/MEDParser.h"
#include "Parser/FileParser/MEVParser.h"
#include "Parser/FileParser/MIMEFileParser.h"
#include "Parser/FileParser/MKVParser.h"
#include "Parser/FileParser/MLHParser.h"
#include "Parser/FileParser/MMSParser.h"
#include "Parser/FileParser/MP2Parser.h"
#include "Parser/FileParser/MPGParser.h"
#include "Parser/FileParser/MRGParser.h"
#include "Parser/FileParser/MTKGPSParser.h"
#include "Parser/FileParser/NFPParser.h"
#include "Parser/FileParser/NMEAParser.h"
#include "Parser/FileParser/NOAParser.h"
#include "Parser/FileParser/NS2Parser.h"
#include "Parser/FileParser/NWAParser.h"
#include "Parser/FileParser/OZF2Parser.h"
#include "Parser/FileParser/OziMapParser.h"
#include "Parser/FileParser/PACParser.h"
#include "Parser/FileParser/PAC2Parser.h"
#include "Parser/FileParser/PCAPNGParser.h"
#include "Parser/FileParser/PCAPParser.h"
#include "Parser/FileParser/PCXParser.h"
#include "Parser/FileParser/PDFParser.h"
#include "Parser/FileParser/PFS2Parser.h"
#include "Parser/FileParser/PLTParser.h"
#include "Parser/FileParser/PNGParser.h"
#include "Parser/FileParser/PSSParser.h"
#include "Parser/FileParser/QTParser.h"
#include "Parser/FileParser/RAR5Parser.h"
#include "Parser/FileParser/RLOCParser.h"
#include "Parser/FileParser/SakuotoArcParser.h"
#include "Parser/FileParser/SEGPackParser.h"
#include "Parser/FileParser/SFVParser.h"
#include "Parser/FileParser/SHPParser.h"
#include "Parser/FileParser/SLOCParser.h"
#include "Parser/FileParser/SM2MPXParser.h"
#include "Parser/FileParser/SMakeParser.h"
#include "Parser/FileParser/SMDLParser.h"
#include "Parser/FileParser/SPDParser.h"
#include "Parser/FileParser/SPKParser.h"
#include "Parser/FileParser/SPREDParser.h"
#include "Parser/FileParser/SQLiteParser.h"
#include "Parser/FileParser/SZSParser.h"
#include "Parser/FileParser/TARParser.h"
#include "Parser/FileParser/TGAParser.h"
#include "Parser/FileParser/TIFFParser.h"
#include "Parser/FileParser/TILParser.h"
#include "Parser/FileParser/TSPParser.h"
#include "Parser/FileParser/TsuyoshiArcParser.h"
#include "Parser/FileParser/TXTParser.h"
#include "Parser/FileParser/UDPParser.h"
#include "Parser/FileParser/VFPParser.h"
#include "Parser/FileParser/WAVParser.h"
#include "Parser/FileParser/WebPParser.h"
#include "Parser/FileParser/WPTParser.h"
#include "Parser/FileParser/X13Parser.h"
#include "Parser/FileParser/X509Parser.h"
#include "Parser/FileParser/XMLParser.h"
#include "Parser/FileParser/XPCMParser.h"
#include "Parser/FileParser/YKCParser.h"
#include "Parser/FileParser/ZIPParser.h"
#include "Parser/FileParser/ZWEIParser.h"
#include "Parser/ObjParser/C3DParser.h"
#include "Parser/ObjParser/DBITParser.h"
#include "Parser/ObjParser/FileGDB2Parser.h"
#include "Parser/ObjParser/ISO9660Parser.h"
#include "Parser/ObjParser/ITMParser.h"
#include "Parser/ObjParser/KMZParser.h"
#include "Parser/ObjParser/MPGXAParser.h"
#include "Parser/ObjParser/OSMMapParser.h"
#include "Parser/ObjParser/PKGMapParser.h"

Parser::FullParserList::FullParserList() : Parser::ParserList()
{
	NN<IO::FileParser> parser;
	NEW_CLASSNN(parser, Parser::FileParser::WAVParser())
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::BMPParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::MMSParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::AFSParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::ICOParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::ANIParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::TGAParser())
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::MRGParser())
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::IPACParser())
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::ZWEIParser())
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::X13Parser())
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::DCPackParser())
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::XPCMParser())
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::MAIPackParser())
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::SM2MPXParser())
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::EXEParser())
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::ELFParser())
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::TIFFParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::PNGParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::GIFParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::ADXParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::MPGParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::PSSParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::CSVParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::SHPParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::CIPParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::SPDParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::DBFParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::MEVParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::RLOCParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::GLOCParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::SLOCParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::SMDLParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::SPREDParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::MD5Parser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::SFVParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::ID3Parser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::AVIParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::PLTParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::WPTParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::MDBParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::QTParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::UDPParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::PCXParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::LUTParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::CFBParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::SQLiteParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::ZIPParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::MKVParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::NFPParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::MLHParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::MajiroArcParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::SZSParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::HTRecParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::IS2Parser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::TSPParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::JSONParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::OziMapParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::PCAPParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::PCAPNGParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::ClassParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::X509Parser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::JP2Parser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::WebPParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::HEIFParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::JXLParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::EVTXParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::RAR5Parser()); ///////////////////////////////
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::CABParser()); ///////////////////////////////
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::NMEAParser()); ///////////////////////////////
	this->AddFileParser(parser);
	
	NEW_CLASSNN(parser, Parser::FileParser::CUEParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::TARParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::GZIPParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::XMLParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::TXTParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::LOGParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::NS2Parser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::PFS2Parser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::B3DMParser());
	this->AddFileParser(parser);

//	NEW_CLASSNN(parser, Parser::FileParser::M2VParser());
	NEW_CLASSNN(parser, Parser::FileParser::M2VStmParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::AC3Parser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::DTSParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::MP2Parser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::CDXAParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::IMGParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::OZF2Parser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::SPKParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::TILParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::JKSParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::JPGParser());
	this->AddFileParser(parser);

	NEW_CLASSNN(parser, Parser::FileParser::GUIImgParser());
	this->AddFileParser(parser);
#if (defined(_MSC_VER) || defined(__MINGW32__)) && !defined(_WIN32_WCE)
#ifndef DISABLE_VFP
	NEW_CLASSNN(parser, Parser::FileParser::VFPParser());
	this->AddFileParser(parser);
#endif
	NEW_CLASSNN(parser, Parser::FileParser::AUIParser());
	this->AddFileParser(parser);
#endif
#ifdef ENABLE_APE
	NEW_CLASSNN(parser, Parser::FileParser::APEParser());
	this->AddFileParser(parser);
#endif

	NEW_CLASSNN(parser, Parser::FileParser::SEGPackParser())
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::COMParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::SakuotoArcParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::MEDParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::PACParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::AOSParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::BSAParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::TsuyoshiArcParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::YKCParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::GamedatPac2Parser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::LinkArcParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::PAC2Parser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::BurikoArcParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::BurikoPackFileParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::NOAParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::NWAParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::ISOParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::MIMEFileParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::SMakeParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::ASN1Parser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::MTKGPSParser());
	this->AddFileParser(parser);
	NEW_CLASSNN(parser, Parser::FileParser::PDFParser());
	this->AddFileParser(parser);

	NN<IO::ObjectParser> oparser;
	NEW_CLASSNN(oparser, Parser::ObjParser::MPGXAParser());
	this->AddObjectParser(oparser);
	NEW_CLASSNN(oparser, Parser::ObjParser::DBITParser());
	this->AddObjectParser(oparser);
	NEW_CLASSNN(oparser, Parser::ObjParser::FileGDB2Parser());
	this->AddObjectParser(oparser);
	NEW_CLASSNN(oparser, Parser::ObjParser::ISO9660Parser());
	this->AddObjectParser(oparser);
	NEW_CLASSNN(oparser, Parser::ObjParser::ITMParser());
	this->AddObjectParser(oparser);
	NEW_CLASSNN(oparser, Parser::ObjParser::KMZParser());
	this->AddObjectParser(oparser);
	NEW_CLASSNN(oparser, Parser::ObjParser::OSMMapParser());
	this->AddObjectParser(oparser);
	NEW_CLASSNN(oparser, Parser::ObjParser::PKGMapParser());
	this->AddObjectParser(oparser);
	NEW_CLASSNN(oparser, Parser::ObjParser::C3DParser());
	this->AddObjectParser(oparser);

	this->SetArcGISPRJParser(&this->prjParser);
}

Parser::FullParserList::~FullParserList()
{
}

NN<Math::ArcGISPRJParser> Parser::FullParserList::GetPRJParser()
{
	return this->prjParser;
}
