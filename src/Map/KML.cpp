#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/KML.h"
#include "IO/StreamWriter.h"
#include "Text/MyString.h"
#include "Text/Encoding.h"
#include "Text/XML.h"

Bool Map::KML::GenKML(IO::FileStream *fs, Data::ArrayList<const WChar*> *names, Data::ArrayListDbl *lats, Data::ArrayListDbl *lons)
{
	Text::Encoding enc(65001);
	IO::StreamWriter *writer;
	WChar buff[256];
	if (names->GetCount() != lats->GetCount())
		return false;
	if (lats->GetCount() != lons->GetCount())
		return false;
	NEW_CLASS(writer, IO::StreamWriter(fs, &enc));
	
	writer->WriteLine(L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	writer->WriteLine(L"<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\" xmlns:kml=\"http://www.opengis.net/kml/2.2\" xmlns:atom=\"http://www.w3.org/2005/Atom\">");
	writer->WriteLine(L"  <Document>");
	writer->WriteLine(L"    <Folder>");
	writer->WriteLine(L"      <name>Items</name>");
	writer->WriteLine(L"      <open>1</open>");
	OSInt i = 0;
	OSInt j = names->GetCount();
	while (i < j)
	{
		writer->WriteLine(L"      <Placemark>");
		writer->Write(L"        <name>");
		Text::XML::ToXMLText(buff, (WChar*)names->GetItem(i));
		writer->Write(buff);
		writer->WriteLine(L"</name>");
		writer->Write(L"        <Point><coordinates>");
		Text::StrDouble(buff, lons->GetItem(i));
		writer->Write(buff);
		writer->Write(L",");
		Text::StrDouble(buff, lats->GetItem(i));
		writer->Write(buff);
		writer->WriteLine(L",0</coordinates></Point>");
		writer->WriteLine(L"      </Placemark>");
		i++;
	}

	writer->WriteLine(L"    </Folder>");
	writer->WriteLine(L"  </Document>");
	writer->WriteLine(L"</kml>");

	DEL_CLASS(writer);
	return true;
}
