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
	
	writer.WriteLine(CSTR("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
	writer.WriteLine(CSTR("<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\" xmlns:kml=\"http://www.opengis.net/kml/2.2\" xmlns:atom=\"http://www.w3.org/2005/Atom\">"));
	writer.WriteLine(CSTR("  <Document>"));
	writer.WriteLine(CSTR("    <Folder>"));
	writer.WriteLine(CSTR("      <name>Items</name>"));
	writer.WriteLine(CSTR("      <open>1</open>"));
	UOSInt i = 0;
	UOSInt j = names->GetCount();
	while (i < j)
	{
		writer.WriteLine(CSTR("      <Placemark>"));
		writer.Write(CSTR("        <name>"));
		sptr = Text::XML::ToXMLText(buff, names->GetItem(i));
		writer.WriteStrC(buff, (UOSInt)(sptr - buff));
		writer.WriteLine(CSTR("</name>"));
		writer.Write(CSTR("        <Point><coordinates>"));
		sptr = Text::StrDouble(buff, lons->GetItem(i));
		writer.WriteStrC(buff, (UOSInt)(sptr - buff));
		writer.Write(CSTR(","));
		sptr = Text::StrDouble(buff, lats->GetItem(i));
		writer.WriteStrC(buff, (UOSInt)(sptr - buff));
		writer.WriteLine(CSTR(",0</coordinates></Point>"));
		writer.WriteLine(CSTR("      </Placemark>"));
		i++;
	}

	writer.WriteLine(CSTR("    </Folder>"));
	writer.WriteLine(CSTR("  </Document>"));
	writer.WriteLine(CSTR("</kml>"));
	return true;
}
