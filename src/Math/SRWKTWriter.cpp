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

UTF8Char *Math::SRWKTWriter::WriteCSys(NN<Math::CoordinateSystem> csys, UTF8Char *buff, UOSInt lev, Text::LineBreakType lbt)
{
	if (csys->IsProjected())
	{
		Math::ProjectedCoordinateSystem *pcsys = (Math::ProjectedCoordinateSystem*)csys.Ptr();
		buff = Text::StrConcatC(buff, UTF8STRC("PROJCS[\""));
		buff = pcsys->GetCSysName()->ConcatTo(buff);
		buff = Text::StrConcatC(buff, UTF8STRC("\","));
		buff = WriteNextLine(buff, lev + 1, lbt);
		buff = WriteCSys(pcsys->GetGeographicCoordinateSystem(), buff, lev + 1, lbt);
		if (pcsys->GetCoordSysType() == Math::CoordinateSystem::CoordinateSystemType::MercatorProjected)
		{
			*buff++ = ',';
			buff = WriteNextLine(buff, lev + 1, lbt);
			buff = Text::StrConcatC(buff, UTF8STRC("PROJECTION[\"Transverse_Mercator\"]"));
		}
		else if (pcsys->GetCoordSysType() == Math::CoordinateSystem::CoordinateSystemType::Mercator1SPProjected)
		{
			*buff++ = ',';
			buff = WriteNextLine(buff, lev + 1, lbt);
			buff = Text::StrConcatC(buff, UTF8STRC("PROJECTION[\"Mercator_1SP\"]"));
		}
		else if (pcsys->GetCoordSysType() == Math::CoordinateSystem::CoordinateSystemType::GausskrugerProjected)
		{
			*buff++ = ',';
			buff = WriteNextLine(buff, lev + 1, lbt);
			buff = Text::StrConcatC(buff, UTF8STRC("PROJECTION[\"Gauss_Kruger\"]"));
		}
		*buff++ = ',';
		buff = WriteNextLine(buff, lev + 1, lbt);
		buff = Text::StrConcatC(buff, UTF8STRC("PARAMETER[\"latitude_of_origin\","));
		buff = Text::StrDouble(buff, pcsys->GetLatitudeOfOriginDegree());
		buff = Text::StrConcatC(buff, UTF8STRC("],"));
		buff = WriteNextLine(buff, lev + 1, lbt);
		buff = Text::StrConcatC(buff, UTF8STRC("PARAMETER[\"central_meridian\","));
		buff = Text::StrDouble(buff, pcsys->GetCentralMeridianDegree());
		buff = Text::StrConcatC(buff, UTF8STRC("],"));
		buff = WriteNextLine(buff, lev + 1, lbt);
		buff = Text::StrConcatC(buff, UTF8STRC("PARAMETER[\"scale_factor\","));
		buff = Text::StrDouble(buff, pcsys->GetScaleFactor());
		buff = Text::StrConcatC(buff, UTF8STRC("],"));
		buff = WriteNextLine(buff, lev + 1, lbt);
		buff = Text::StrConcatC(buff, UTF8STRC("PARAMETER[\"false_easting\","));
		buff = Text::StrDouble(buff, pcsys->GetFalseEasting());
		buff = Text::StrConcatC(buff, UTF8STRC("],"));
		buff = WriteNextLine(buff, lev + 1, lbt);
		buff = Text::StrConcatC(buff, UTF8STRC("PARAMETER[\"false_northing\","));
		buff = Text::StrDouble(buff, pcsys->GetFalseNorthing());
		buff = Text::StrConcatC(buff, UTF8STRC("],"));
		buff = this->WriteUnit(buff, pcsys->GetUnit(), lev + 1, lbt);
		buff = this->WriteSRID(buff, csys->GetSRID(), lev + 1, lbt);
		buff = this->WriteProjExtra(buff, csys, lev + 1, lbt);
		buff = Text::StrConcatC(buff, UTF8STRC("]"));
	}
	else
	{
		Math::GeographicCoordinateSystem *gcsys = (Math::GeographicCoordinateSystem*)csys.Ptr();
		buff = Text::StrConcatC(buff, UTF8STRC("GEOGCS[\""));
		buff = gcsys->GetCSysName()->ConcatTo(buff);
		buff = Text::StrConcatC(buff, UTF8STRC("\","));
		buff = WriteNextLine(buff, lev + 1, lbt);
		NN<const Math::GeographicCoordinateSystem::DatumData1> datum = gcsys->GetDatum();
		buff = WriteDatum(datum, buff, lev + 1, lbt);
		buff = WritePrimem(buff, gcsys->GetPrimem(), lev + 1, lbt);
		buff = WriteUnit(buff, gcsys->GetUnit(), lev + 1, lbt);
		buff = this->WriteSRID(buff, csys->GetSRID(), lev + 1, lbt);
		buff = Text::StrConcatC(buff, UTF8STRC("]"));
	}
	return buff;
}

UTF8Char *Math::SRWKTWriter::WriteDatum(NN<const Math::CoordinateSystem::DatumData1> datum, UTF8Char *buff, UOSInt lev, Text::LineBreakType lbt)
{
	buff = Text::StrConcatC(buff, UTF8STRC("DATUM[\""));
	buff = Text::StrConcatC(buff, datum->name, datum->nameLen);
	buff = Text::StrConcatC(buff, UTF8STRC("\","));
	buff = WriteNextLine(buff, lev + 1, lbt);
	buff = this->WriteSpheroid(&datum->spheroid, buff, lev + 1, lbt);
	if (datum->cX != 0 || datum->cY != 0 || datum->cZ != 0 || datum->xAngle != 0 || datum->yAngle != 0 || datum->zAngle != 0 || datum->scale != 0)
	{
		*buff++ = ',';
		buff = WriteNextLine(buff, lev + 1, lbt);
		buff = Text::StrConcatC(buff, UTF8STRC("TOWGS84["));
		buff = Text::StrDouble(buff, datum->cX);
		buff = Text::StrConcatC(buff, UTF8STRC(","));
		buff = Text::StrDouble(buff, datum->cY);
		buff = Text::StrConcatC(buff, UTF8STRC(","));
		buff = Text::StrDouble(buff, datum->cZ);
		buff = Text::StrConcatC(buff, UTF8STRC(","));
		buff = Text::StrDouble(buff, Math::Unit::Angle::Convert(Math::Unit::Angle::AU_RADIAN, Math::Unit::Angle::AU_ARCSECOND, datum->xAngle));
		buff = Text::StrConcatC(buff, UTF8STRC(","));
		buff = Text::StrDouble(buff, Math::Unit::Angle::Convert(Math::Unit::Angle::AU_RADIAN, Math::Unit::Angle::AU_ARCSECOND, datum->yAngle));
		buff = Text::StrConcatC(buff, UTF8STRC(","));
		buff = Text::StrDouble(buff, Math::Unit::Angle::Convert(Math::Unit::Angle::AU_RADIAN, Math::Unit::Angle::AU_ARCSECOND, datum->zAngle));
		buff = Text::StrConcatC(buff, UTF8STRC("]"));
	}
	buff = this->WriteSRID(buff, datum->srid, lev + 1, lbt);
	buff = Text::StrConcatC(buff, UTF8STRC("]"));
	return buff;
}

UTF8Char *Math::SRWKTWriter::WriteSpheroid(const Math::CoordinateSystem::SpheroidData *spheroid, UTF8Char *buff, UOSInt lev, Text::LineBreakType lbt)
{
	buff = Text::StrConcatC(buff, UTF8STRC("SPHEROID[\""));
	buff = Text::StrConcatC(buff, spheroid->name, spheroid->nameLen);
	buff = Text::StrConcatC(buff, UTF8STRC("\","));
	buff = Text::StrDouble(buff, spheroid->ellipsoid->GetSemiMajorAxis());
	buff = Text::StrConcatC(buff, UTF8STRC(","));
	buff = Text::StrDouble(buff, spheroid->ellipsoid->GetInverseFlattening());
	buff = this->WriteSRID(buff, spheroid->srid, lev + 1, lbt);
	buff = Text::StrConcatC(buff, UTF8STRC("]"));
	return buff;
}

UTF8Char *Math::SRWKTWriter::WriteNextLine(UTF8Char *buff, UOSInt lev, Text::LineBreakType lbt)
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
			buff[0] = '\r';
			buff[1] = '\n';
			buff += 2;
		}
		while (lev-- > 0)
		{
			*buff++ = '\t';
		}
		*buff = 0;
	}
	return buff;
}
