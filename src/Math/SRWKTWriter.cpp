#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/ProjectedCoordinateSystem.h"
#include "Math/SRWKTWriter.h"
#include "Text/MyStringFloat.h"

Math::SRWKTWriter::SRWKTWriter()
{

}

Math::SRWKTWriter::~SRWKTWriter()
{

}

Char *Math::SRWKTWriter::WriteCSys(Math::CoordinateSystem *csys, Char *buff, UOSInt lev, Text::LineBreakType lbt)
{
	if (csys == 0)
		return 0;
	if (csys->IsProjected())
	{
		Math::ProjectedCoordinateSystem *pcsys = (Math::ProjectedCoordinateSystem*)csys;
		buff = Text::StrConcat(buff, "PROJCS[\"");
		buff = Text::StrConcat(buff, (const Char*)pcsys->GetCSysName());
		buff = Text::StrConcat(buff, "\",");
		buff = WriteNextLine(buff, lev + 1, lbt);
		buff = WriteCSys(pcsys->GetGeographicCoordinateSystem(), buff, lev + 1, lbt);
		if (pcsys->GetCoordSysType() == Math::CoordinateSystem::CoordinateSystemType::MercatorProjected)
		{
			*buff++ = ',';
			buff = WriteNextLine(buff, lev + 1, lbt);
			buff = Text::StrConcat(buff, "PROJECTION[\"Transverse_Mercator\"]");
		}
		else if (pcsys->GetCoordSysType() == Math::CoordinateSystem::CoordinateSystemType::Mercator1SPProjected)
		{
			*buff++ = ',';
			buff = WriteNextLine(buff, lev + 1, lbt);
			buff = Text::StrConcat(buff, "PROJECTION[\"Mercator_1SP\"]");
		}
		else if (pcsys->GetCoordSysType() == Math::CoordinateSystem::CoordinateSystemType::GausskrugerProjected)
		{
			*buff++ = ',';
			buff = WriteNextLine(buff, lev + 1, lbt);
			buff = Text::StrConcat(buff, "PROJECTION[\"Gauss_Kruger\"]");
		}
		*buff++ = ',';
		buff = WriteNextLine(buff, lev + 1, lbt);
		buff = Text::StrConcat(buff, "PARAMETER[\"latitude_of_origin\",");
		buff = Text::StrDouble(buff, pcsys->GetLatitudeOfOrigin());
		buff = Text::StrConcat(buff, "],");
		buff = WriteNextLine(buff, lev + 1, lbt);
		buff = Text::StrConcat(buff, "PARAMETER[\"central_meridian\",");
		buff = Text::StrDouble(buff, pcsys->GetCentralMeridian());
		buff = Text::StrConcat(buff, "],");
		buff = WriteNextLine(buff, lev + 1, lbt);
		buff = Text::StrConcat(buff, "PARAMETER[\"scale_factor\",");
		buff = Text::StrDouble(buff, pcsys->GetScaleFactor());
		buff = Text::StrConcat(buff, "],");
		buff = WriteNextLine(buff, lev + 1, lbt);
		buff = Text::StrConcat(buff, "PARAMETER[\"false_easting\",");
		buff = Text::StrDouble(buff, pcsys->GetFalseEasting());
		buff = Text::StrConcat(buff, "],");
		buff = WriteNextLine(buff, lev + 1, lbt);
		buff = Text::StrConcat(buff, "PARAMETER[\"false_northing\",");
		buff = Text::StrDouble(buff, pcsys->GetFalseNorthing());
		buff = Text::StrConcat(buff, "],");
		buff = this->WriteUnit(buff, pcsys->GetUnit(), lev + 1, lbt);
		buff = this->WriteSRID(buff, csys->GetSRID(), lev + 1, lbt);
		buff = this->WriteProjExtra(buff, csys, lev + 1, lbt);
		buff = Text::StrConcat(buff, "]");
	}
	else
	{
		Math::GeographicCoordinateSystem *gcsys = (Math::GeographicCoordinateSystem*)csys;
		buff = Text::StrConcat(buff, "GEOGCS[\"");
		buff = Text::StrConcat(buff, (const Char*)gcsys->GetCSysName());
		buff = Text::StrConcat(buff, "\",");
		buff = WriteNextLine(buff, lev + 1, lbt);
		const Math::GeographicCoordinateSystem::DatumData1 *datum = gcsys->GetDatum();
		buff = WriteDatum(datum, buff, lev + 1, lbt);
		buff = WritePrimem(buff, gcsys->GetPrimem(), lev + 1, lbt);
		buff = WriteUnit(buff, gcsys->GetUnit(), lev + 1, lbt);
		buff = this->WriteSRID(buff, csys->GetSRID(), lev + 1, lbt);
		buff = Text::StrConcat(buff, "]");
	}
	return buff;
}

Char *Math::SRWKTWriter::WriteDatum(const Math::CoordinateSystem::DatumData1 *datum, Char *buff, UOSInt lev, Text::LineBreakType lbt)
{
	buff = Text::StrConcat(buff, "DATUM[\"");
	buff = Text::StrConcat(buff, datum->name);
	buff = Text::StrConcat(buff, "\",");
	buff = WriteNextLine(buff, lev + 1, lbt);
	buff = Text::StrConcat(buff, "SPHEROID[\"");
	buff = Text::StrConcat(buff, datum->spheroid.name);
	buff = Text::StrConcat(buff, "\",");
	buff = Text::StrDouble(buff, datum->spheroid.ellipsoid->GetSemiMajorAxis());
	buff = Text::StrConcat(buff, ",");
	buff = Text::StrDouble(buff, datum->spheroid.ellipsoid->GetInverseFlattening());
	buff = this->WriteSRID(buff, datum->spheroid.srid, lev + 2, lbt);
	buff = Text::StrConcat(buff, "]");
	if (datum->cX != 0 || datum->cY != 0 || datum->cZ != 0 || datum->xAngle != 0 || datum->yAngle != 0 || datum->zAngle != 0 || datum->scale != 0)
	{
		*buff++ = ',';
		buff = WriteNextLine(buff, lev + 1, lbt);
		buff = Text::StrConcat(buff, "TOWGS84[");
		buff = Text::StrDouble(buff, datum->cX);
		buff = Text::StrConcat(buff, ",");
		buff = Text::StrDouble(buff, datum->cY);
		buff = Text::StrConcat(buff, ",");
		buff = Text::StrDouble(buff, datum->cZ);
		buff = Text::StrConcat(buff, ",");
		buff = Text::StrDouble(buff, Math::Unit::Angle::Convert(Math::Unit::Angle::AU_RADIAN, Math::Unit::Angle::AU_ARCSECOND, datum->xAngle));
		buff = Text::StrConcat(buff, ",");
		buff = Text::StrDouble(buff, Math::Unit::Angle::Convert(Math::Unit::Angle::AU_RADIAN, Math::Unit::Angle::AU_ARCSECOND, datum->yAngle));
		buff = Text::StrConcat(buff, ",");
		buff = Text::StrDouble(buff, Math::Unit::Angle::Convert(Math::Unit::Angle::AU_RADIAN, Math::Unit::Angle::AU_ARCSECOND, datum->zAngle));
		buff = Text::StrConcat(buff, "]");
	}
	buff = this->WriteSRID(buff, datum->srid, lev + 1, lbt);
	buff = Text::StrConcat(buff, "]");
	return buff;
}

Char *Math::SRWKTWriter::WriteNextLine(Char *buff, UOSInt lev, Text::LineBreakType lbt)
{
	if (lbt != Text::LineBreakType::None)
	{
		if (lbt == Text::LineBreakType::CR)
		{
			*buff++ = '\r';
		}		
		else if (lbt == Text::LineBreakType::LF)
		{
			*buff++ = '\n';
		}		
		else if (lbt == Text::LineBreakType::CRLF)
		{
			*buff++ = '\r';
			*buff++ = '\n';
		}
		while (lev-- > 0)
		{
			*buff++ = '\t';
		}
		*buff = 0;
	}
	return buff;
}
