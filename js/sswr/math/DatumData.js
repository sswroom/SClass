import unit from "./unit";
var DatumData = function(srid, spheroid, name, x0, y0, z0, cX, cY, cZ, xAngle, yAngle, zAngle, scale, aunit)
{
	this.srid = srid;
	this.spheroid = spheroid;
	this.name = name;
	this.x0 = x0;
	this.y0 = y0;
	this.z0 = z0;
	this.cX = cX;
	this.cY = cY;
	this.cZ = cZ;
	var aratio = unit.Angle.getUnitRatio(aunit);
	this.xAngle = xAngle * aratio;
	this.yAngle = yAngle * aratio;
	this.zAngle = zAngle * aratio;
	this.scale = scale;
	this.aunit = unit.Angle.AngleUnit.RADIAN;
};
export default DatumData;