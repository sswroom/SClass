#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/KMLGenerator.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/UTF8Writer.h"
#include "Text/XML.h"

Bool Map::KMLGenerator::GenKMLPoints(IO::Stream *fs, Data::ArrayList<const UTF8Char*> *names, Data::ArrayList<Double> *lats, Data::ArrayList<Double> *lons)
{
	UTF8Char buff[256];
	UTF8Char *sptr;
	if (names->GetCount() != lats->GetCount())
		return false;
	if (lats->GetCount() != lons->GetCount())
		return false;
	Text::UTF8Writer writer(fs);
	
	writer.WriteLineC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
	writer.WriteLineC(UTF8STRC("<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\" xmlns:kml=\"http://www.opengis.net/kml/2.2\" xmlns:atom=\"http://www.w3.org/2005/Atom\">"));
	writer.WriteLineC(UTF8STRC("  <Document>"));
	writer.WriteLineC(UTF8STRC("    <Folder>"));
	writer.WriteLineC(UTF8STRC("      <name>Items</name>"));
	writer.WriteLineC(UTF8STRC("      <open>1</open>"));
	UOSInt i = 0;
	UOSInt j = names->GetCount();
	while (i < j)
	{
		writer.WriteLineC(UTF8STRC("      <Placemark>"));
		writer.WriteStrC(UTF8STRC("        <name>"));
		sptr = Text::XML::ToXMLText(buff, names->GetItem(i));
		writer.WriteStrC(buff, (UOSInt)(sptr - buff));
		writer.WriteLineC(UTF8STRC("</name>"));
		writer.WriteStrC(UTF8STRC("        <Point><coordinates>"));
		sptr = Text::StrDouble(buff, lons->GetItem(i));
		writer.WriteStrC(buff, (UOSInt)(sptr - buff));
		writer.WriteStrC(UTF8STRC(","));
		sptr = Text::StrDouble(buff, lats->GetItem(i));
		writer.WriteStrC(buff, (UOSInt)(sptr - buff));
		writer.WriteLineC(UTF8STRC(",0</coordinates></Point>"));
		writer.WriteLineC(UTF8STRC("      </Placemark>"));
		i++;
	}

	writer.WriteLineC(UTF8STRC("    </Folder>"));
	writer.WriteLineC(UTF8STRC("  </Document>"));
	writer.WriteLineC(UTF8STRC("</kml>"));
	return true;
}