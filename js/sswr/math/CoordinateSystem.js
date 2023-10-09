var CoordinateSystem = function(srid, csysName)
{
	this.srid = srid;
	this.csysName = csysName;
}

CoordinateSystem.prototype.CoordinateSystemType = {
	Geographic: 0,
	MercatorProjected: 1,
	Mercator1SPProjected: 2,
	PointMapping: 3,
	GausskrugerProjected: 4
};
export default CoordinateSystem;