var unit = {
	Angle: {
		AngleUnit: {
			RADIAN: 1,
			GRADIAN: 2,
			TURN: 3,
			DEGREE: 4,
			ARCMINUTE: 5,
			ARCSECOND: 6,
			MILLIARCSECOND: 7,
			MICROARCSECOND: 8
		},

		getUnitRatio: function(u)
		{
			switch (u)
			{
			case unit.Angle.AngleUnit.RADIAN:
				return 1;
			case unit.Angle.AngleUnit.GRADIAN:
				return Math.PI / 200.0;
			case unit.Angle.AngleUnit.TURN:
				return Math.PI * 2.0;
			case unit.Angle.AngleUnit.DEGREE:
				return Math.PI / 180.0;
			case unit.Angle.AngleUnit.ARCMINUTE:
				return Math.PI / 10800.0;
			case unit.Angle.AngleUnit.ARCSECOND:
				return Math.PI / 648000.0;
			case unit.Angle.AngleUnit.MILLIARCSECOND:
				return Math.PI / 648000000.0;
			case unit.Angle.AngleUnit.MICROARCSECOND:
				return Math.PI / 648000000000.0;
			}
			return 1;
		}
	},

	Distance: {
		DistanceUnit: {
			METER: 0,
			CENTIMETER: 1,
			MILLIMETER: 2,
			MICROMETER: 3,
			NANOMETER: 4,
			PICOMETER: 5,
			KILOMETER: 6,
			INCH: 7,
			FOOT: 8,
			YARD: 9,
			MILE: 10,
			NAUTICAL_MILE: 11,
			AU: 12,
			LIGHTSECOND: 13,
			LIGHTMINUTE: 14,
			LIGHTHOUR: 15,
			LIGHTDAY: 16,
			LIGHTWEEK: 17,
			LIGHTYEAR: 18,
			EMU: 19,
			POINT: 20,
			PIXEL: 21,
			TWIP: 22
		},

		getUnitRatio: function(u)
		{
			switch (u)
			{
			case unit.Distance.DistanceUnit.METER:
				return 1.0;
			case unit.Distance.DistanceUnit.CENTIMETER:
				return 0.01;
			case unit.Distance.DistanceUnit.MILLIMETER:
				return 0.001;
			case unit.Distance.DistanceUnit.MICROMETER:
				return 0.000001;
			case unit.Distance.DistanceUnit.NANOMETER:
				return 0.000000001;
			case unit.Distance.DistanceUnit.PICOMETER:
				return 0.000000000001;
			case unit.Distance.DistanceUnit.KILOMETER:
				return 1000.0;
			case unit.Distance.DistanceUnit.INCH:
				return 0.0254;
			case unit.Distance.DistanceUnit.FOOT:
				return 0.0254 * 12.0;
			case unit.Distance.DistanceUnit.YARD:
				return 0.0254 * 36.0;
			case unit.Distance.DistanceUnit.MILE:
				return 0.0254 * 12.0 * 5280;
			case unit.Distance.DistanceUnit.NAUTICAL_MILE:
				return 1852.0;
			case unit.Distance.DistanceUnit.AU:
				return 149597870700.0;
			case unit.Distance.DistanceUnit.LIGHTSECOND:
				return 299792458.0;
			case unit.Distance.DistanceUnit.LIGHTMINUTE:
				return 17987547480.0;
			case unit.Distance.DistanceUnit.LIGHTHOUR:
				return 299792458.0 * 3600.0;
			case unit.Distance.DistanceUnit.LIGHTDAY:
				return 299792458.0 * 86400.0;
			case unit.Distance.DistanceUnit.LIGHTWEEK:
				return 299792458.0 * 604800.0;
			case unit.Distance.DistanceUnit.LIGHTYEAR:
				return 299792458.0 * 31557600.0;
			case unit.Distance.DistanceUnit.EMU:
				return 1 / 36000000.0;
			case unit.Distance.DistanceUnit.POINT:
				return 0.0254 / 72.0;
			case unit.Distance.DistanceUnit.PIXEL:
				return 0.0254 / 96.0;
			case unit.Distance.DistanceUnit.TWIP:
				return 0.0254 / 1440.0;
			}
			return 1;
		},

		convert: function(fromUnit, toUnit, fromValue)
		{
			return fromValue * unit.Distance.getUnitRatio(fromUnit) / unit.Distance.getUnitRatio(toUnit);
		}
	}
};
export default unit;