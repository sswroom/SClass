var Coord2D = function(x, y)
{
	this.x = x;
	this.y = y;
}

Coord2D.prototype.getLat = function()
{
	return this.y;
}

Coord2D.prototype.getLon = function()
{
	return this.x;
}
export default Coord2D;