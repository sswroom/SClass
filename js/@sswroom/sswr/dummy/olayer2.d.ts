export const ImgPath: string;
export const VERSION_NUMBER: string;
/**
 * Instances of this class represent bounding boxes.  Data stored as left, bottom, right, top floats.  All values are initialized to null, however, you should make sure you set them before using the bounds for anything.
 */
export class Bounds
{
	/**
	 * Construct a new bounds object.  Coordinates can either be passed as four arguments, or as a single argument.
	 * @param left The left bounds of the box.  Note that for width calculations, this is assumed to be less than the right value.
	 * @param bottom The bottom bounds of the box.  Note that for height calculations, this is assumed to be less than the top value.
	 * @param right The right bounds.
	 * @param top The top bounds.
	 */
	constructor(left: number, bottom: number, right: number, top: number);
	/**
	 * Construct a new bounds object.  Coordinates can either be passed as four arguments, or as a single argument.
	 * @param bounds 
	 */
	constructor(bounds: number[]);
	/**
	 * Returns a string representation of the bounds object.
	 * @returns String representation of bounds object.
	 */
	toString(): string;
	/**
	 * Returns an array representation of the bounds object.
	 * 
	 * Returns an array of left, bottom, right, top properties, or -- when the optional parameter is true -- an array of the bottom, left, top, right properties.
	 * @param reverseAxisOrder Should we reverse the axis order?
	 * @returns array of left, bottom, right, top
	 */
	toArray(reverseAxisOrder?: boolean): number[];
	/**
	 * Returns a boundingbox-string representation of the bounds object.
	 * @param decimal How many decimal places in the bbox coords?  Default is 6
	 * @param reverseAxisOrder Should we reverse the axis order?
	 * @returns Simple String representation of bounds object.  (e.g.  “5,42,10,45”)
	 */
	toBBOX(decimal?: number, reverseAxisOrder?: boolean): string;
	/**
	 * Create a new polygon geometry based on this bounds.
	 * @returns A new polygon with the coordinates of this bounds.
	 */
	toGeometry(): Geometry.Polygon;
	/**
	 * Returns the width of the bounds.
	 * @returns The width of the bounds (right minus left).
	 */
	getWidth(): number;
	/**
	 * Returns the height of the bounds.
	 * @returns The height of the bounds (top minus bottom).
	 */
	getHeight(): number;
	/**
	 * Returns an OpenLayers.Size object of the bounds.
	 * @returns The size of the bounds.
	 */
	getSize(): Size;
	/**
	 * Returns the OpenLayers.Pixel object which represents the center of the bounds.
	 * @returns The center of the bounds in pixel space.
	 */
	getCenterPixel(): Pixel;
	/**
	 * Returns the OpenLayers.LonLat object which represents the center of the bounds.
	 * @returns The center of the bounds in map space.
	 */
	getCenterLonLat(): LonLat;
	/**
	 * Scales the bounds around a pixel or lonlat.  Note that the new bounds may return non-integer properties, even if a pixel is passed.
	 * @param ratio 
	 * @param origin Default is center.
	 * @returns A new bounds that is scaled by ratio from origin.
	 */
	scale(ratio: number, origin?: Pixel|LonLat): Bounds;
	/**
	 * Shifts the coordinates of the bound by the given horizontal and vertical deltas.
	 * 
	 * This method will throw a TypeError if it is passed null as an argument.
	 * @param x horizontal delta
	 * @param y vertical delta
	 * @returns A new bounds whose coordinates are the same as this, but shifted by the passed-in x and y values.
	 */
	add(x: number, y: number): Bounds;
	/**
	 * Extend the bounds to include the OpenLayers.LonLat, OpenLayers.Geometry.Point or OpenLayers.Bounds specified.
	 * 
	 * Please note that this function assumes that left < right and bottom < top.
	 * @param object The object to be included in the new bounds object.
	 */
	extend(object: LonLat|Geometry.Point|Bounds): void;
	/**
	 * Extend the bounds to include the XY coordinate specified.
	 * @param x The X part of the the coordinate.
	 * @param y The Y part of the the coordinate.
	 */
	extendXY(x: number, y: number): void;
	/**
	 * Returns whether the bounds object contains the given OpenLayers.LonLat.
	 * @param ll OpenLayers.LonLat or an object with a ‘lon’ and ‘lat’ properties.
	 * @param options Optional parameters
	 * @returns The passed-in lonlat is within this bounds.
	 */
	containsLonLat(ll: LonLat|{lon:number,lat:number}, options?: {inclusive?: boolean, worldBounds?: Bounds}): boolean;
	/**
	 * Returns whether the bounds object contains the given OpenLayers.Pixel.
	 * @param px 
	 * @param inclusive Whether or not to include the border.  Default is true.
	 * @returns The passed-in pixel is within this bounds.
	 */
	containsPixel(px: Pixel, inclusive?: boolean): boolean;
	/**
	 * Returns whether the bounds object contains the given x and y.
	 * @param x 
	 * @param y 
	 * @param inclusive Whether or not to include the border.  Default is true.
	 * @returns Whether or not the passed-in coordinates are within this bounds.
	 */
	contains(x: number, y: number, inclusive?: boolean): boolean;
	/**
	 * Determine whether the target bounds intersects this bounds.  Bounds are considered intersecting if any of their edges intersect or if one bounds contains the other.
	 * @param bounds The target bounds.
	 * @param options Optional parameters.
	 * @returns The passed-in bounds object intersects this bounds.
	 */
	intersectsBounds(bounds: Bounds, options?: {inclusive?: boolean, worldBounds?: Bounds}): boolean;
	/**
	 * Returns whether the bounds object contains the given OpenLayers.Bounds.
	 * @param bounds The target bounds.
	 * @param partial If any of the target corners is within this bounds consider the bounds contained.  Default is false.  If false, the entire target bounds must be contained within this bounds.
	 * @param inclusive Treat shared edges as contained.  Default is true.
	 * @returns The passed-in bounds object is contained within this bounds.
	 */
	containsBounds(bounds: Bounds, partial?: boolean, inclusive?: boolean): boolean;
	/**
	 * Returns the the quadrant (“br”, “tr”, “tl”, “bl”) in which the given OpenLayers.LonLat lies.
	 * @param lonlat 
	 * @returns The quadrant (“br” “tr” “tl” “bl”) of the bounds in which the coordinate lies.
	 */
	determineQuadrant(lonlat: LonLat): string;
	/**
	 * Transform the Bounds object from source to dest.
	 * @param source Source projection.
	 * @param dest Destination projection.
	 * @returns Itself, for use in chaining operations.
	 */
	transform(source: Projection, dest: Projection): Bounds;
	/**
	 * Alternative constructor that builds a new OpenLayers.Bounds from a parameter string.
	 * @param str Comma-separated bounds string.  (e.g.  “5,42,10,45”)
	 * @param reverseAxisOrder Does the string use reverse axis order?
	 * @returns New bounds object built from the passed-in String.
	 */
	static fromString(str: string, reverseAxisOrder?: boolean): Bounds;
	/**
	 * Alternative constructor that builds a new OpenLayers.Bounds from an array.
	 * @param bbox Array of bounds values (e.g.  [5,42,10,45])
	 * @param reverseAxisOrder Does the array use reverse axis order?
	 * @returns New bounds object built from the passed-in Array.
	 */
	static fromArray(bbox: number[], reverseAxisOrder?: boolean): Bounds;
	/**
	 * Alternative constructor that builds a new OpenLayers.Bounds from a size.
	 * @param size OpenLayers.Size or an object with both ‘w’ and ‘h’ properties.
	 * @returns New bounds object built from the passed-in size.
	 */
	static fromSize(size: Size|{w: number, h: number}): Bounds;
}

/**
 * Controls affect the display or behavior of the map.  They allow everything from panning and zooming to displaying a scale indicator.  Controls by default are added to the map they are contained within however it is possible to add a control to an external div by passing the div in the options parameter.
 * @example The following example shows how to add many of the common controls to a map.
 * The next code fragment is a quick example of how to intercept shift-mouse click to display the extent of the bounding box dragged out by the user.  Usually controls are not created in exactly this manner.  See the source for a more complete example:
 */
export class Control
{
	/** The element that contains the control, if not present the control is placed inside the map. */
	div: HTMLElement;
	/** Controls can have a ‘type’.  The type determines the type of interactions which are possible with them when they are placed in an OpenLayers.Control.Panel. */
	type: number;
	/**  This property is used for showing a tooltip over the Control. */
	title: string;
	/** Activate the control when it is added to a map.  Default is false. */
	autoActivate: boolean;
	/** The control is active (read-only).  Use activate and deactivate to change control state. */
	active: boolean;
	/** If set as an option at construction, the eventListeners object will be registered with OpenLayers.Events.on.  Object structure must be a listeners object as shown in the example for the events.on method. */
	eventListeners: object;
	/** Events instance for listeners and triggering control specific events. */
	events: Events;
	/**
	 * Create an OpenLayers Control.  The options passed as a parameter directly extend the control.  For example passing the following:
	 * Overrides the default div attribute value of null.
	 * @param options 
	 */
	constructor(options: object);
	/**
	 * Explicitly activates a control and its associated handler if one has been set.  Controls can be deactivated by calling the deactivate() method.
	 * @returns  True if the control was successfully activated or false if the control was already active.
	 */
	activate(): boolean;
	/**
	 * Deactivates a control and its associated handler if any.  The exact effect of this depends on the control itself.
	 * @returns True if the control was effectively deactivated or false if the control was already inactive.
	 */
	deactivate(): boolean;
	static readonly TYPE_BUTTON: number;
	static readonly TYPE_TOGGLE: number;
	static readonly TYPE_TOOL: number;
}

namespace Control
{
	/**
	 * The navigation control handles map browsing with mouse events (dragging, double-clicking, and scrolling the wheel).  Create a new navigation control with the OpenLayers.Control.Navigation control.
	 * 
	 * Note that this control is added to the map by default (if no controls array is sent in the options object to the OpenLayers.Map constructor).
	 */
	export class Navigation extends Control
	{
		/** Options passed to the DragPan control. */
		dragPanOptions: object;
		/** Options passed to the PinchZoom control. */
		pinchZoomOptions: object;
		/** Allow panning of the map by dragging outside map viewport.  Default is false. */
		documentDrag: boolean;
		/** Whether the user can draw a box to zoom */
		zoomBoxEnabled: boolean;
		/** Whether the mousewheel should zoom the map */
		zoomWheelEnabled: boolean;
		/** Whether or not to handle right clicks.  Default is false. */
		handleRightClicks: boolean;
		/** OpenLayers.Handler key code of the key, which has to be pressed, while drawing the zoom box with the mouse on the screen.  You should probably set handleRightClicks to true if you use this with MOD_CTRL, to disable the context menu for machines which use CTRL-Click as a right click.  Default: <OpenLayers.Handler.MOD_SHIFT> */
		zoomBoxKeyMask: number;
		/** Activate the control when it is added to a map.  Default is true. */
		autoActivate: boolean;
		/**
		 * Create a new navigation control
		 * @param options An optional object whose properties will be set on the control
		 */
		constructor(options?: object);
	}

	/**
	 * The PanZoom is a visible control, composed of a OpenLayers.Control.PanPanel and a OpenLayers.Control.ZoomPanel.  By default it is drawn in the upper left corner of the map.
	 */
	export class PanZoom extends Control
	{
		/** Number of pixels by which we’ll pan the map in any direction on clicking the arrow buttons.  If you want to pan by some ratio of the map dimensions, use slideRatio instead. */
		slideFactor: number;
		/** The fraction of map width/height by which we’ll pan the map on clicking the arrow buttons.  Default is null.  If set, will override slideFactor.  E.g. if slideRatio is .5, then the Pan Up button will pan up half the map height. */
		slideRatio: number;
		constructor(options?: object);
		destroy(): void;
		readonly X: number;
		readonly Y: number;
	}

	/**
	 * The PanZoomBar is a visible control composed of a OpenLayers.Control.PanPanel and a <OpenLayers.Control.ZoomBar>.  By default it is displayed in the upper left corner of the map as 4 directional arrows above a vertical slider.
	 */
	export class PanZoomBar extends PanZoom
	{
		zoomStopWidth: number;
		zoomStopHeight: number;
		zoomWorldIcon: boolean;
		/** Set this property to false not to display the pan icons.  If false the zoom world icon is placed under the zoom bar.  Defaults to true. */
		panIcons: boolean;
		/** Force a fixed zoom level even though the map has fractionalZoom */
		forceFixedZoomLevel: boolean;
		constructor();
		destroy(): void;
	}
}

/**
 * Utility functions for event handling.
 */
export class Events
{
	static readonly KEY_SPACE: number;
	static readonly KEY_BACKSPACE: number;
	static readonly KEY_TAB: number;
	static readonly KEY_RETURN: number;
	static readonly KEY_ESC: number;
	static readonly KEY_LEFT: number;
	static readonly KEY_UP: number;
	static readonly KEY_RIGHT: number;
	static readonly KEY_DOWN: number;
	static readonly KEY_DELETE: number;
	static readonly BROWSER_EVENTS: string[];

	fallThrough: boolean;
	/**
	 * Should the .xy property automatically be created for browser mouse events?  In general, this should be false.  If it is true, then mouse events will automatically generate a ‘.xy’ property on the event object that is passed.  (Prior to OpenLayers 2.7, this was true by default.)  Otherwise, you can call the getMousePosition on the relevant events handler on the object available via the ‘evt.object’ property of the evt object.  So, for most events, you can call: function named(evt) { this.xy = this.object.events.getMousePosition(evt) }
	 * 
	 * This option typically defaults to false for performance reasons
	 * 
	 * when creating an events object whose primary purpose is to manage relatively positioned mouse events within a div, it may make sense to set it to true.
	 * 
	 * This option is also used to control whether the events object caches offsets.  If this is false, it will not: the reason for this is that it is only expected to be called many times if the includeXY property is set to true.  If you set this to true, you are expected to clear the offset cache manually (using this.clearMouseCache()) if: the border of the element changes the location of the element in the page changes
	 */
	includeXY: boolean;
	/**
	 * Event extensions registered with this instance.  Keys are event types, values are {OpenLayers.Events.*} extension instances or {Boolean} for events that an instantiated extension provides in addition to the one it was created for.
	 * 
	 * Extensions create an event in addition to browser events, which usually fires when a sequence of browser events is completed.  Extensions are automatically instantiated when a listener is registered for an event provided by an extension.
	 * 
	 * Extensions are created in the OpenLayers.Events namespace using OpenLayers.Class, and named after the event they provide.  The constructor receives the target OpenLayers.Events instance as argument.  Extensions that need to capture browser events before they propagate can register their listeners events using register, with {extension: true} as 4th argument.
	 * 
	 * If an extension creates more than one event, an alias for each event type should be created and reference the same class.  The constructor should set a reference in the target’s extensions registry to itself.
	 * 
	 * Below is a minimal extension that provides the “foostart” and “fooend” event types, which replace the native “click” event type if clicked on an element with the css class “foo”:
	 */
	extensions: object;
	/**
	 * Construct an OpenLayers.Events object.
	 * @param object The js object to which this Events object is being added
	 * @param element A dom element to respond to browser events
	 * @param eventType Deprecated.  Array of custom application events.  A listener may be registered for any named event, regardless of the values provided here.
	 * @param fallThrough Allow events to fall through after these have been handled?
	 * @param options Options for the events object.
	 */
	constructor(object: object, element: HTMLElement, eventType: string[], fallThrough: boolean, options: object);
	destroy(): void;
	/**
	 * Deprecated.  Any event can be triggered without adding it first.
	 * @param eventName 
	 */
	addEventType(eventName: string): void;
	/**
	 * Convenience method for registering listeners with a common scope.  Internally, this method calls register as shown in the examples below.
	 * @param object 
	 */
	on(object: object): void;
	/**
	 * Register an event on the events object.
	 * 
	 * When the event is triggered, the ‘func’ function will be called, in the context of ‘obj’.  Imagine we were to register an event, specifying an OpenLayers.Bounds Object as ‘obj’.  When the event is triggered, the context in the callback function will be our Bounds object.  This means that within our callback function, we can access the properties and methods of the Bounds object through the “this” variable.  So our callback could execute something like:
	 * @param type Name of the event to register
	 * @param obj The object to bind the context to for the callback#.  If no object is specified, default is the Events’s ‘object’ property.
	 * @param func The callback function.  If no callback is specified, this function does nothing.
	 * @param priority If true, adds the new listener to the front of the events queue instead of to the end.
	 */
	register(type: string, obj: object, func: Function, priority: boolean|object): void;
	/**
	 * Same as register() but adds the new listener to the front of the events queue instead of to the end.
	 * TODO: get rid of this in 3.0	Decide whether listeners should be called in the order they were registered or in reverse order.
	 * @param type Name of the event to register
	 * @param obj The object to bind the context to for the callback#.  If no object is specified, default is the Events’s ‘object’ property.
	 * @param func The callback function.  If no callback is specified, this function does nothing.
	 */
	registerPriority(type: string, obj: object, func: Function): void;
	/**
	 * Convenience method for unregistering listeners with a common scope.  Internally, this method calls unregister as shown in the examples below.
	 * @param object 
	 */
	un(object: object): void;
	/**
	 * @param type 
	 * @param obj If none specified, defaults to this.object
	 * @param func 
	 */
	unregister(type: string, obj: object, func: Function): void;
	/**
	 * Trigger a specified registered event.
	 * @param type 
	 * @param evt will be passed to the listeners.
	 * @returns The last listener return.  If a listener returns false, the chain of listeners will stop getting called.
	 */
	triggerEvent(type: string, evt: Event|object): boolean;
	/**
	 * Clear cached data about the mouse position.  This should be called any time the element that events are registered on changes position within the page.
	 */
	clearMouseCache(): void;
}

export abstract class Feature
{
	popupClass: Class;
	constructor(layer: Layer, lonlat: LonLat, data: object);
}

export class Style
{
	fill?: boolean;
	fillColor?: string;
	fillOpacity?: number;
	stroke?: boolean;
	strokeColor?: string;
	strokeOpacity?: number;
	strokeWidth?: number;
	strokeLinecap?: string;
	strokeDashstyle?: string;
	graphic?: boolean;
	pointRadius?: number;
	pointerEvents?: string;
	cursor?: string;
	externalGraphic?: string;
	graphicWidth?: number;
	graphicHeight?: number;
	graphicOpacity?: number;
	graphicXOffset?: number;
	graphicYOffset?: number;
	rotation?: number;
	graphicZIndex?: number;
	graphicName?: string;
	graphicTitle?: string;
	title?: string;
	backgroundGraphic?: string;
	backgroundGraphicZIndex?: number;
	backgroundXOffset?: number;
	backgroundYOffset?: number;
	backgroundHeight?: number;
	backgroundWidth?: number;
	label?: string;
	labelAlign?: string;
	labelXOffset?: number;
	labelYOffset?: number;
	labelSelect?: boolean;
	labelOutlineColor?: string;
	labelOutlineWidth?: number;
	labelOutlineOpacity?: number;
	fontColor?: string;
	fontOpacity?: number;
	fontFamily?: string;
	fontSize?: string;
	fontStyle?: string;
	fontWeight?: string;
	display?: string;
}

namespace Feature
{
	export class Vector extends Feature
	{
		geometry: Geometry;
		attributes?: any;
		style?: Style;
		url: string;
		modified: object;
		constructor(geometry: Geometry, attributes?: any, style?: Style);
	}
}

/**
 * A Geometry is a description of a geographic object.  Create an instance of this class with the OpenLayers.Geometry constructor.  This is a base class, typical geometry types are described by subclasses of this class.
 * 
 * Note that if you use the <OpenLayers.Geometry.fromWKT> method, you must explicitly include the OpenLayers.Format.WKT in your build.
 */
export abstract class Geometry
{
	/**
	 * Create a clone of this geometry.  Does not set any non-standard properties of the cloned geometry.
	 * @returns An exact clone of this geometry.
	 */
	clone(): Geometry;
	/**
	 * Get the bounds for this Geometry.  If bounds is not set, it is calculated again, this makes queries faster.
	 * @returns 
	 */
	getBounds(): Bounds;
	/**
	 * Recalculate the bounds for the geometry.
	 */
	calculateBounds(): void;
	/**
	 * Calculate the closest distance between two geometries (on the x-y plane).
	 * @param geometry The target geometry.
	 * @param options Optional properties for configuring the distance calculation. Valid options depend on the specific geometry type.
	 * @returns The distance between this geometry and the target.  If details is true, the return will be an object with distance, x0, y0, x1, and y1 properties.  The x0 and y0 properties represent the coordinates of the closest point on this geometry.  The x1 and y1 properties represent the coordinates of the closest point on the target geometry.
	 */
	distanceTo(geometry: Geometry, options?: any): number|{distance: number, x0: number, y0: number, x1: number, y1: number};
	/**
	 * Return a list of all points in this geometry.
	 * @param nodes For lines, only return vertices that are endpoints.  If false, for lines, only vertices that are not endpoints will be returned.  If not provided, all vertices will be returned.
	 * @returns A list of all vertices in the geometry.
	 */
	getVertices(nodes: boolean): Geometry.Point[];
	/**
	 * Calculate the centroid of this geometry.  This method is defined in subclasses.
	 * @returns The centroid of the collection
	 */
	getCentroid(weighted?: boolean): Geometry.Point;
}

namespace Geometry
{
	/**
	 * Point geometry class.
	 */
	export class Point extends Geometry
	{
		x: number;
		y: number;
		/**
		 * Construct a point geometry.
		 * @param x 
		 * @param y 
		 */
		constructor(x: number, y: number);
		/**
		 * @returns An exact clone of this OpenLayers.Geometry.Point
		 */
		clone(): Point;
		/**
		 * Calculate the closest distance between two geometries (on the x-y plane).
		 * @param geometry The target geometry.
		 * @param options Optional properties for configuring the distance calculation.
		 * @returns The distance between this geometry and the target.  If details is true, the return will be an object with distance, x0, y0, x1, and y1 properties.  The x0 and y0 properties represent the coordinates of the closest point on this geometry.  The x1 and y1 properties represent the coordinates of the closest point on the target geometry.
		 */
		distanceTo(geometry: Geometry, options?: {details?: boolean, edge?: boolean}): number|{distance: number, x0: number, y0: number, x1: number, y1: number};
		/**
		 * Determine whether another geometry is equivalent to this one.  Geometries are considered equivalent if all components have the same coordinates.
		 * @param geom The geometry to test.
		 * @returns The supplied geometry is equivalent to this geometry.
		 */
		equals(geom: Point): boolean;
		/**
		 * Moves a geometry by the given displacement along positive x and y axes.  This modifies the position of the geometry and clears the cached bounds.
		 * @param x Distance to move geometry in positive x direction.
		 * @param y Distance to move geometry in positive y direction.
		 */
		move(x: number, y: number): void;
		/**
		 * Rotate a point around another.
		 * @param angle Rotation angle in degrees (measured counterclockwise from the positive x-axis)
		 * @param origin Center point for the rotation
		 */
		rotate(angle: number, origin: Point): void;
		/**
		 * @returns The centroid of the collection
		 */
		getCentroid(): Point;
		/**
		 * Resize a point relative to some origin.  For points, this has the effect of scaling a vector (from the origin to the point).  This method is more useful on geometry collection subclasses.
		 * @param scale Ratio of the new distance from the origin to the old distance from the origin.  A scale of 2 doubles the distance between the point and origin.
		 * @param origin Point of origin for resizing
		 * @param ratio Optional x:y ratio for resizing.  Default ratio is 1.
		 * @returns The current geometry.
		 */
		resize(scale: number, origin: Point, ratio?: number): Geometry;
		/**
		 * Determine if the input geometry intersects this one.
		 * @param geometry Any type of geometry.
		 * @returns The input geometry intersects this one.
		 */
		intersects(geometry: Geometry): boolean;
		/**
		 * Translate the x,y properties of the point from source to dest.
		 * @param source 
		 * @param dest 
		 */
		transform(source: Projection, dest: Projection): Geometry;
		/**
		 * Return a list of all points in this geometry.
		 * @param nodes For lines, only return vertices that are endpoints.  If false, for lines, only vertices that are not endpoints will be returned.  If not provided, all vertices will be returned.
		 * @returns A list of all vertices in the geometry.
		 */
		getVertices(nodes: boolean): Point[];
	}

	export class Collection extends Geometry
	{
		/**
		 *  The component parts of this geometry
		 */
		components: Geometry[];
		/**
		 * Creates a Geometry Collection -- a list of geoms.
		 * @param components Optional array of geometries
		 */
		constructor(components: Geometry[]);
		/**
		 * Destroy this geometry.
		 */
		destroy(): void;
		/**
		 * Clone this geometry.
		 * @returns An exact clone of this collection
		 */
		clone(): Collection;
		/**
		 * Recalculate the bounds by iterating through the components and calling calling extendBounds() on each item.
		 */
		calculateBounds(): void;
		/**
		 * Add components to this geometry.
		 * @param components An array of geometries to add
		 */
		addComponents(components: Geometry[]): void;
		/**
		 * Remove components from this geometry
		 * @param components The components to be removed
		 * @returns A component was removed.
		 */
		removeComponents(components: Geometry[]): boolean;
		/**
		 * Calculate the length of this geometry
		 * @returns The length of the geometry
		 */
		getLength(): number;
		/**
		 * Calculate the area of this geometry.  Note how this function is overridden in OpenLayers.Geometry.Polygon.
		 * @returns The area of the collection by summing its parts
		 */
		getArea(): number;
		/**
		 * Calculate the approximate area of the polygon were it projected onto the earth.
		 * @param projection The spatial reference system for the geometry coordinates.  If not provided, Geographic/WGS84 is assumed.
		 * @external Robert.  G. Chamberlain and William H.  Duquette, “Some Algorithms for Polygons on a Sphere”, JPL Publication 07-03, Jet Propulsion Laboratory, Pasadena, CA, June 2007 http://trs-new.jpl.nasa.gov/dspace/handle/2014/40409
		 * @returns The approximate geodesic area of the geometry in square meters.
		 */
		getGeodesicArea(projection?: Projection): number;
		/**
		 * Compute the centroid for this geometry collection.
		 * @param weighted Perform the getCentroid computation recursively, returning an area weighted average of all geometries in this collection.
		 * @returns The centroid of the collection
		 */
		getCentroid(weighted?: boolean): Point;
		/**
		 * Calculate the approximate length of the geometry were it projected onto the earth.
		 * @param projection The spatial reference system for the geometry coordinates.  If not provided, Geographic/WGS84 is assumed.
		 * @returns The appoximate geodesic length of the geometry in meters.
		 */
		getGeodesicLength(projection?: Projection): number;
		/**
		 * Moves a geometry by the given displacement along positive x and y axes.  This modifies the position of the geometry and clears the cached bounds.
		 * @param x Distance to move geometry in positive x direction.
		 * @param y Distance to move geometry in positive y direction.
		 */
		move(x: number, y: number): void;
		/**
		 * Rotate a geometry around some origin
		 * @param angle Rotation angle in degrees (measured counterclockwise from the positive x-axis)
		 * @param origin Center point for the rotation
		 */
		rotate(angle: number, origin: Point): void;
		/**
		 * Resize a geometry relative to some origin.  Use this method to apply a uniform scaling to a geometry.
		 * @param scale Factor by which to scale the geometry.  A scale of 2 doubles the size of the geometry in each dimension (lines, for example, will be twice as long, and polygons will have four times the area).
		 * @param origin Point of origin for resizing
		 * @param ratio Optional x:y ratio for resizing.  Default ratio is 1.
		 * @return The current geometry.
		 */
		resize(scale: number, origin: Point, ratio: number): Geometry;
		/**
		 * Calculate the closest distance between two geometries (on the x-y plane).
		 * @param geometry The target geometry.
		 * @param options Optional properties for configuring the distance calculation.
		 * @returns The distance between this geometry and the target.  If details is true, the return will be an object with distance, x0, y0, x1, and y1 properties.  The x0 and y0 properties represent the coordinates of the closest point on this geometry.  The x1 and y1 properties represent the coordinates of the closest point on the target geometry.
		 */
		distanceTo(geometry: Geometry, options: {details?: boolean, edge?: boolean}): number|{distance:number,x0:number,y0:number,x1:number,y1:number};
		/**
		 * Determine whether another geometry is equivalent to this one.  Geometries are considered equivalent if all components have the same coordinates.
		 * @param geometry The geometry to test.
		 * @returns The supplied geometry is equivalent to this geometry.
		 */
		equals(geometry: Geometry): boolean;
		/**
		 * Reproject the components geometry from source to dest.
		 * @param source 
		 * @param dest 
		 */
		transform(source: Projection, dest: Projection): Geometry;
		/**
		 * Determine if the input geometry intersects this one.
		 * @param geometry Any type of geometry.
		 * @returns The input geometry intersects this one.
		 */
		intersects(geometry: Geometry): boolean;
		/**
		 * Return a list of all points in this geometry.
		 * @param nodes For lines, only return vertices that are endpoints.  If false, for lines, only vertices that are not endpoints will be returned.  If not provided, all vertices will be returned.
		 * @returns A list of all vertices in the geometry.
		 */
		getVertices(nodes?: boolean): Point[];
	}

	export class MultiPoint extends Collection
	{
		/**
		 * Create a new MultiPoint Geometry
		 * @param components 
		 */
		constructor(components: Point[]);
		/**
		 * Wrapper for <OpenLayers.Geometry.Collection.addComponent>
		 * @param point Point to be added
		 * @param index Optional index
		 */
		addPoint(point: Point, index?: number): void;
		/**
		 * Wrapper for <OpenLayers.Geometry.Collection.removeComponent>
		 * @param point Point to be removed
		 */
		removePoint(point: Point): void;
	}

	export class Curve extends MultiPoint
	{
		/**
		 * 
		 * @param point 
		 */
		constructor(point: Point[]);
		/**
		 * @returns The length of the curve
		 */
		getLength(): number;
		/**
		 * Calculate the approximate length of the geometry were it projected onto the earth.
		 * @param projection The spatial reference system for the geometry coordinates.  If not provided, Geographic/WGS84 is assumed.
		 * @returns The appoximate geodesic length of the geometry in meters.
		 */
		getGeodesicLength(projection?: Projection): number;
	}

	/**
	 * A LineString is a Curve which, once two points have been added to it, can never be less than two points long.
	 */
	export class LineString extends Curve
	{
		/**
		 * Create a new LineString geometry
		 * @param points An array of points used to generate the linestring
		 */
		constructor(points: Point[]);
		/**
		 * Only allows removal of a point if there are three or more points in the linestring.  (otherwise the result would be just a single point)
		 * @param point The point to be removed
		 * @returns The component was removed.
		 */
		removeComponent(point: Point): boolean;
		/**
		 * Test for instersection between two geometries.  This is a cheapo implementation of the Bently-Ottmann algorigithm.  It doesn’t really keep track of a sweep line data structure.  It is closer to the brute force method, except that segments are sorted and potential intersections are only calculated when bounding boxes intersect.
		 * @param geometry 
		 * @returns The input geometry intersects this geometry.
		 */
		intersects(geometry: Geometry): boolean;
		/**
		 * Return a list of all points in this geometry.
		 * @param nodes For lines, only return vertices that are endpoints.  If false, for lines, only vertices that are not endpoints will be returned.  If not provided, all vertices will be returned.
		 * @returns A list of all vertices in the geometry.
		 */
		getVertices(nodes: boolean): Point[];
		/**
		 * Calculate the closest distance between two geometries (on the x-y plane).
		 * @param geometry The target geometry.
		 * @param options Optional properties for configuring the distance calculation.
		 * @returns The distance between this geometry and the target.  If details is true, the return will be an object with distance, x0, y0, x1, and y1 properties.  The x0 and y0 properties represent the coordinates of the closest point on this geometry.  The x1 and y1 properties represent the coordinates of the closest point on the target geometry.
		 */
		distanceTo(geometry: Geometry, options?: {details?: boolean, edge?: boolean}): number|{distance: number, x0: number, y0: number, x1: number, y1: number};
		/**
		 * This function will return a simplified LineString.  Simplification is based on the Douglas-Peucker algorithm.
		 * @param tolerance threshold for simplification in map units
		 * @returns the simplified LineString
		 */
		simplify(tolerance: number): LineString;
	}

	/**
	 * A Linear Ring is a special LineString which is closed.  It closes itself automatically on every addPoint/removePoint by adding a copy of the first point as the last point.
	 * 
	 * Also, as it is the first in the line family to close itself, a getArea() function is defined to calculate the enclosed area of the linearRing
	 */
	export class LinearRing extends LineString
	{
		/**
		 * Linear rings are constructed with an array of points.  This array can represent a closed or open ring.  If the ring is open (the last point does not equal the first point), the constructor will close the ring.  If the ring is already closed (the last point does equal the first point), it will be left closed.
		 * @param points points
		 */
		constructor(points: Point[]);
		/**
		 * Adds a point to geometry components.  If the point is to be added to the end of the components array and it is the same as the last point already in that array, the duplicate point is not added.  This has the effect of closing the ring if it is not already closed, and doing the right thing if it is already closed.  This behavior can be overridden by calling the method with a non-null index as the second argument.
		 * @param point 
		 * @param index Index into the array to insert the component
		 * @returns Was the Point successfully added?
		 */
		addComponents(point: Point, index?: number): boolean;
		/**
		 * Removes a point from geometry components.
		 * @param point 
		 * @returns The component was removed.
		 */
		removeComponent(point: Point): boolean;
		/**
		 * Moves a geometry by the given displacement along positive x and y axes.  This modifies the position of the geometry and clears the cached bounds.
		 * @param x Distance to move geometry in positive x direction.
		 * @param y Distance to move geometry in positive y direction.
		 */
		move(x: number, y: number): void;
		/**
		 * Rotate a geometry around some origin
		 * @param angle Rotation angle in degrees (measured counterclockwise from the positive x-axis)
		 * @param origin Center point for the rotation
		 */
		rotate(angle: number, origin: Point): void;
		/**
		 * Resize a geometry relative to some origin.  Use this method to apply a uniform scaling to a geometry.
		 * @param scale Factor by which to scale the geometry.  A scale of 2 doubles the size of the geometry in each dimension (lines, for example, will be twice as long, and polygons will have four times the area).
		 * @param origin Point of origin for resizing
		 * @param ratio Optional x:y ratio for resizing.  Default ratio is 1.
		 */
		resize(scale: number, origin: Point, ratio?: number): Geometry;
		/**
		 * Reproject the components geometry from source to dest.
		 * @param source 
		 * @param dest 
		 */
		transform(source: Projection, dest: Projection): Geometry;
		/**
		 * @param weighted 
		 * @returns The centroid of the collection
		 */
		getCentroid(weighted?: boolean): Point;
		/**
		 * @description The area is positive if the ring is oriented CW, otherwise it will be negative.
		 * @returns The signed area for a ring.
		 */
		getArea(): number;
		/**
		 * Calculate the approximate area of the polygon were it projected onto the earth.  Note that this area will be positive if ring is oriented clockwise, otherwise it will be negative.
		 * @param projection The spatial reference system for the geometry coordinates.  If not provided, Geographic/WGS84 is assumed.
		 * @description Robert.  G. Chamberlain and William H.  Duquette, “Some Algorithms for Polygons on a Sphere”, JPL Publication 07-03, Jet Propulsion Laboratory, Pasadena, CA, June 2007 http://trs-new.jpl.nasa.gov/dspace/handle/2014/40409
		 * @returns The approximate signed geodesic area of the polygon in square meters.
		 */
		getGeodesicArea(projection?: Projection): number;
		/**
		 * Determine if the input geometry intersects this one.
		 * @param geometry Any type of geometry.
		 * @returns The input geometry intersects this one.
		 */
		intersects(geometry: Geometry): boolean;
		/**
		 * Return a list of all points in this geometry.
		 * @param nodes For lines, only return vertices that are endpoints.  If false, for lines, only vertices that are not endpoints will be returned.  If not provided, all vertices will be returned.
		 * @returns A list of all vertices in the geometry.
		 */
		getVertices(nodes: boolean): Point[];
	}

	/**
	 * Polygon is a collection of Geometry.LinearRings.
	 */
	export class Polygon extends Collection
	{
		/**
		 * Constructor for a Polygon geometry.  The first ring (this.component[0])is the outer bounds of the polygon and all subsequent rings (this.component[1-n]) are internal holes.
		 * @param components 
		 */
		constructor(components: LinearRing[]);
		/**
		 * Calculated by subtracting the areas of the internal holes from the area of the outer hole.
		 * @returns The area of the geometry
		 */
		getArea(): number;
		/**
		 * Calculate the approximate area of the polygon were it projected onto the earth.
		 * @param projection The spatial reference system for the geometry coordinates.  If not provided, Geographic/WGS84 is assumed.
		 * @description Robert.  G. Chamberlain and William H.  Duquette, “Some Algorithms for Polygons on a Sphere”, JPL Publication 07-03, Jet Propulsion Laboratory, Pasadena, CA, June 2007 http://trs-new.jpl.nasa.gov/dspace/handle/2014/40409
		 * @returns The approximate geodesic area of the polygon in square meters.
		 */
		getGeodesicArea(projection: Projection): number;
		/**
		 * Determine if the input geometry intersects this one.
		 * @param geometry Any type of geometry.
		 * @returns The input geometry intersects this one.
		 */
		intersects(geometry: Geometry): boolean;
		/**
		 * Calculate the closest distance between two geometries (on the x-y plane).
		 * @param geometry The target geometry.
		 * @param options Optional properties for configuring the distance calculation.
		 * @returns The distance between this geometry and the target.  If details is true, the return will be an object with distance, x0, y0, x1, and y1 properties.  The x0 and y0 properties represent the coordinates of the closest point on this geometry.  The x1 and y1 properties represent the coordinates of the closest point on the target geometry.
		 */
		distanceTo(geometry: Geometry, options: { details?: boolean; edge?: boolean; }): number | { distance: number; x0: number; y0: number; x1: number; y1: number; };
		/**
		 * Create a regular polygon around a radius.  Useful for creating circles and the like.
		 * @param origin center of polygon.
		 * @param radius distance to vertex, in map units.
		 * @param sides Number of sides.  20 approximates a circle.
		 * @param rotation original angle of rotation, in degrees.
		 */
		static createRegularPolygon(origin: Point, radius: number, sides: number, rotation: number): Polygon;
	}

	/**
	 * MultiPolygon is a geometry with multiple OpenLayers.Geometry.Polygon components.  Create a new instance with the OpenLayers.Geometry.MultiPolygon constructor.
	 */
	export class MultiPolygon extends Collection
	{
		/**
		 * Create a new MultiPolygon geometry
		 * @param components An array of polygons used to generate the MultiPolygon
		 */
		constructor(components: Polygon[]);
	}
}

/**
 * Base class to construct a higher-level handler for event sequences.  All handlers have activate and deactivate methods.  In addition, they have methods named like browser events.  When a handler is activated, any additional methods named like a browser event is registered as a listener for the corresponding event.  When a handler is deactivated, those same methods are unregistered as event listeners.
 * 
 * Handlers also typically have a callbacks object with keys named like the abstracted events or event sequences that they are in charge of handling.  The controls that wrap handlers define the methods that correspond to these abstract events - so instead of listening for individual browser events, they only listen for the abstract events defined by the handler.
 * 
 * Handlers are created by controls, which ultimately have the responsibility of making changes to the the state of the application.  Handlers themselves may make temporary changes, but in general are expected to return the application in the same state that they found it.
 */
export class Handler
{
	/** The control that initialized this handler.  The control is assumed to have a valid map property - that map is used in the handler’s own setMap method. */
	control: Control;
	/** Use bitwise operators and one or more of the OpenLayers.Handler constants to construct a keyMask.  The keyMask is used by <checkModifiers>.  If the keyMask matches the combination of keys down on an event, checkModifiers returns true. */
	keyMask: number;
	/**
	 * Construct a handler.
	 * @param control The control that initialized this handler.  The control is assumed to have a valid map property; that map is used in the handler’s own setMap method.  If a map property is present in the options argument it will be used instead.
	 * @param callbacks An object whose properties correspond to abstracted events or sequences of browser events.  The values for these properties are functions defined by the control that get called by the handler.
	 * @param options An optional object whose properties will be set on the handler.
	 */
	constructor(control: Control, callbacks?: object, options?: object);
	/**
	 * Turn on the handler.  Returns false if the handler was already active.
	 * @returns The handler was activated.
	 */
	activate(): boolean;
	/**
	 * Turn off the handler.  Returns false if the handler was already inactive.
	 * @returns The handler was deactivated.
	 */
	deactivate(): boolean;
	/** If set as the keyMask, <checkModifiers> returns false if any key is down. */
	static readonly MOD_NONE: number;
	/** If set as the keyMask, <checkModifiers> returns false if Shift is down. */
	static readonly MOD_SHIFT: number;
	/** If set as the keyMask, <checkModifiers> returns false if Ctrl is down. */
	static readonly MOD_CTRL: number;
	/** If set as the keyMask, <checkModifiers> returns false if Alt is down. */
	static readonly MOD_ALT: number;
	/** If set as the keyMask, <checkModifiers> returns false if Cmd is down. */
	static readonly MOD_META: number;
}

namespace Handler
{
	/**
	 * A handler for mouse clicks.  The intention of this handler is to give controls more flexibility with handling clicks.  Browsers trigger click events twice for a double-click.  In addition, the mousedown, mousemove, mouseup sequence fires a click event.  With this handler, controls can decide whether to ignore clicks associated with a double click.  By setting a pixelTolerance, controls can also ignore clicks that include a drag.  Create a new instance with the OpenLayers.Handler.Click constructor.
	 */
	export class Click extends Handler
	{
		/** Number of milliseconds between clicks before the event is considered a double-click. */
		delay: number;
		/** Handle single clicks.  Default is true.  If false, clicks will not be reported.  If true, single-clicks will be reported. */
		single: boolean;
		/** Handle double-clicks.  Default is false. */
		double: boolean;
		/** Maximum number of pixels between mouseup and mousedown for an event to be considered a click.  Default is 0.  If set to an integer value, clicks with a drag greater than the value will be ignored.  This property can only be set when the handler is constructed. */
		pixelTolerance: number;
		/** Maximum distance in pixels between clicks for a sequence of events to be considered a double click.  Default is 13.  If the distance between two clicks is greater than this value, a double- click will not be fired. */
		dblclickTolerance: number;
		/** Stop other listeners from being notified of clicks.  Default is false.  If true, any listeners registered before this one for click or rightclick events will not be notified. */
		stopSingle: boolean;
		/**
		 * Stop other listeners from being notified of double-clicks.  Default is false.  If true, any click listeners registered before this one will not be notified of any double-click events.
		 * 
		 * The one caveat with stopDouble is that given a map with two click handlers, one with stopDouble true and the other with stopSingle true, the stopSingle handler should be activated last to get uniform cross-browser performance.  Since IE triggers one click with a dblclick and FF triggers two, if a stopSingle handler is activated first, all it gets in IE is a single click when the second handler stops propagation on the dblclick.
		 */
		stopDouble: boolean;
		/**
		 * Create a new click handler.
		 * @param control The control that is making use of this handler.  If a handler is being used without a control, the handler’s setMap method must be overridden to deal properly with the map.
		 * @param callbacks An object with keys corresponding to callbacks that will be called by the handler.  The callbacks should expect to receive a single argument, the click event.  Callbacks for ‘click’ and ‘dblclick’ are supported.
		 * @param options Optional object whose properties will be set on the handler.
		 */
		constructor(control: Control, callbacks?: object, options: object);
		/**
		 * Deactivate the handler.
		 * @returns The handler was successfully deactivated.
		 */
		deactivate(): boolean;
	}
}

/**
 * The icon represents a graphical icon on the screen.  Typically used in conjunction with a OpenLayers.Marker to represent markers on a screen.
 * 
 * An icon has a url, size and position.  It also contains an offset which allows the center point to be represented correctly.  This can be provided either as a fixed offset or a function provided to calculate the desired offset.
 */
export class Icon
{
	url: string;
	size: Size;
	offset: Pixel;
	imageDiv: HTMLImageElement;

	/**
	 * Creates an icon, which is an image tag in a div.
	 * @param url 
	 * @param size An OpenLayers.Size or an object with a ‘w’ and ‘h’ properties.
	 * @param offset An OpenLayers.Pixel or an object with a ‘x’ and ‘y’ properties.
	 * @param calculateOffset 
	 */
	constructor(url: string, size: Size|{w: number, h: number}, offset: Pixel|{x: number, y: number}, calculateOffset?: ()=>void);
	/**
	 * @returns Whether or not the icon is drawn.
	 */
	isDrawn(): boolean;
}

export class Layer
{
	id: string;
	name: string;
	div: HTMLDivElement;
	/** The layer’s opacity.  Float number between 0.0 and 1.0.  Default is 1. */
	opacity?: number;
	/**
	 * If a layer’s display should not be scale-based, this should be set to true.  This will cause the layer, as an overlay, to always be ‘active’, by always returning true from the calculateInRange() function.
	 * 
	 * If not explicitly specified for a layer, its value will be determined on startup in initResolutions() based on whether or not any scale-specific properties have been set as options on the layer.  If no scale-specific options have been set on the layer, we assume that it should always be in range.
	 * 
	 * See #987 for more info.
	 */
	alwaysInRange: boolean;
	/** The properties that are used for calculating resolutions information. */
	static readonly RESOLUTION_PROPERTIES: Array;
	/** Listeners will be called with a reference to an event object.  The properties of this event depends on exactly what happened. */
	events: Events;
	/** This variable is set when the layer is added to the map, via the accessor function setMap(). */
	map: Map;
	/** Whether or not the layer is a base layer.  This should be set individually by all subclasses.  Default is false */
	isBaseLayer?: boolean;
	/** Display the layer’s name in the layer switcher.  Default is true. */
	displayInLayerSwitcher?: boolean;
	/** The layer should be displayed in the map.  Default is true. */
	visibility?: boolean;
	/**
	 * Attribution information, displayed when an OpenLayers.Control.Attribution has been added to the map.
	 * 
	 * An object is required to store the full attribution information from a WMS capabilities response.  Example attribution object: {title:””,href:””,logo:{format:””,width:10,height:10,href:””}}
	 */
	attribution: string|{title: string, href: string, logo: {format: string, width: number, height: number, href: string}};
	/** If set as an option at construction, the eventListeners object will be registered with OpenLayers.Events.on.  Object structure must be a listeners object as shown in the example for the events.on method. */
	eventListeners: object;
	/** Determines the width (in pixels) of the gutter around image tiles to ignore.  By setting this property to a non-zero value, images will be requested that are wider and taller than the tile size by a value of 2 x gutter.  This allows artifacts of rendering at tile edges to be ignored.  Set a gutter value that is equal to half the size of the widest symbol that needs to be displayed.  Defaults to zero.  Non-tiled layers always have zero gutter. */
	gutter: number;
	/** 
	 * Specifies the projection of the layer.  Can be set in the layer options.  If not specified in the layer options, it is set to the default projection specified in the map, when the layer is added to the map.  Projection along with default maxExtent and resolutions are set automatically with commercial baselayers in EPSG:3857, such as Google, Bing and OpenStreetMap, and do not need to be specified.  Otherwise, if specifying projection, also set maxExtent, maxResolution or resolutions as appropriate.  When using vector layers with strategies, layer projection should be set to the projection of the source data if that is different from the map default.
	 * 
	 * Can be either a string or an OpenLayers.Projection object; if a string is passed, will be converted to an object when the layer is added to the map.
	 */
	projection: Projection|string;
	/** The layer map units.  Defaults to null.  Possible values are ‘degrees’ (or ‘dd’), ‘m’, ‘ft’, ‘km’, ‘mi’, ‘inches’.  Normally taken from the projection.  Only required if both map and layers do not define a projection, or if they define a projection which does not define units. */
	units: string;
	/** An array of map scales in descending order.  The values in the array correspond to the map scale denominator.  Note that these values only make sense if the display (monitor) resolution of the client is correctly guessed by whomever is configuring the application.  In addition, the units property must also be set.  Use resolutions instead wherever possible. */
	scales: number[];
	/** A list of map resolutions (map units per pixel) in descending order.  If this is not set in the layer constructor, it will be set based on other resolution related properties (maxExtent, maxResolution, maxScale, etc.). */
	resolutions: number[];
	/** 
	 * If provided as an array, the array should consist of four values (left, bottom, right, top).  The maximum extent for the layer.  Defaults to null.
	 * 
	 * The center of these bounds will not stray outside of the viewport extent during panning.  In addition, if displayOutsideMaxExtent is set to false, data will not be requested that falls completely outside of these bounds.
	 */
	maxExtent?: Bounds|number[];
	/** If provided as an array, the array should consist of four values (left, bottom, right, top).  The minimum extent for the layer.  Defaults to null. */
	minExtent?: Bounds|number[];
	/** Default max is 360 deg / 256 px, which corresponds to zoom level 0 on gmaps.  Specify a different value in the layer options if you are not using the default OpenLayers.Map.tileSize and displaying the whole world. */
	maxResolution: number;
	minResolution: number;
	numZoomLevels: number;
	minScale: number;
	maxScale: number;
	/** Request map tiles that are completely outside of the max extent for this layer.  Defaults to false. */
	displayOutsideMaxExtent?: boolean;
	/** Wraps the world at the international dateline, so the map can be panned infinitely in longitudinal direction.  Only use this on the base layer, and only if the layer’s maxExtent equals the world bounds.  #487 for more info. */
	wrapDateLine: boolean;
	/**
	 * 
	 * @param name The layer name
	 * @param options Hashtable of extra options to tag onto the layer
	 */
	constructor(name: string, options: object);
	/**
	 * Sets the new layer name for this layer.  Can trigger a changelayer event on the map.
	 * @param newName The new name.
	 */
	setName(newName: string): void;
	/**
	 * @param newOptions 
	 * @param reinitialize If set to true, and if resolution options of the current baseLayer were changed, the map will be recentered to make sure that it is displayed with a valid resolution, and a changebaselayer event will be triggered.
	 */
	addOptions(newOptions: object, reinitialize: boolean): void;
	/**
	 * This function can be implemented by subclasses
	 */
	onMapResize(): void;
	/**
	 * Redraws the layer.  Returns true if the layer was redrawn, false if not.
	 * @returns The layer was redrawn.
	 */
	redraw(): boolean;
	/**
	 * Just as setMap() allows each layer the possibility to take a personalized action on being added to the map, removeMap() allows each layer to take a personalized action on being removed from it.  For now, this will be mostly unused, except for the EventPane layer, which needs this hook so that it can remove the special invisible pane.
	 * @param map 
	 */
	removeMap(map: Map): void;
	/**
	 * @param bounds optional tile bounds, can be used by subclasses that have to deal with different tile sizes at the layer extent edges (e.g.  Zoomify)
	 * @returns The size that the image should be, taking into account gutters.
	 */
	getImageSize(bounds?: Bounds): Size;
	/**
	 * Set the tile size based on the map size.  This also sets layer.imageSize or use by Tile.Image.
	 * @param size 
	 */
	setTileSize(size: Size): void;
	/**
	 * @returns The layer should be displayed (if in range).
	 */
	getVisibility(): boolean;
	/**
	 * Set the visibility flag for the layer and hide/show & redraw accordingly.  Fire event unless otherwise specified
	 * 
	 * Note that visibility is no longer simply whether or not the layer’s style.display is set to “block”.  Now we store a ‘visibility’ state property on the layer class, this allows us to remember whether or not we desire for a layer to be visible.  In the case where the map’s resolution is out of the layer’s range, this desire may be subverted.
	 * @param visibility Whether or not to display the layer (if in range)
	 */
	setVisibility(visibility: boolean): void;
	/**
	 * Hide or show the Layer.  This is designed to be used internally, and is not generally the way to enable or disable the layer.  For that, use the setVisibility function instead..
	 * @param display 
	 */
	display(display: boolean): void;
	/**
	 * @returns The layer is displayable at the current map’s current resolution.  Note that if ‘alwaysInRange’ is true for the layer, this function will always return true.
	 */
	calculateInRange(): boolean;
	setIsBaseLayer(isBaseLayer: boolean): void;
	/**
	 * @returns The currently selected resolution of the map, taken from the resolutions array, indexed by current zoom level.
	 */
	getResolution(): number;
	/**
	 * @returns A Bounds object which represents the lon/lat bounds of the current viewPort.
	 */
	getExtent(): Bounds;
	/**
	 * @param extent 
	 * @param closest Find the zoom level that most closely fits the specified bounds.  Note that this may result in a zoom that does not exactly contain the entire extent.  Default is false.
	 * @returns The index of the zoomLevel (entry in the resolutions array) for the passed-in extent.  We do this by calculating the ideal resolution for the given extent (based on the map size) and then calling getZoomForResolution(), passing along the ‘closest’ parameter.
	 */
	getZoomForExtent(extent: Bounds, closest: boolean): number;
	/**
	 * @param zoom 
	 * @returns A suitable resolution for the specified zoom.
	 */
	getResolutionForZoom(zoom: number): number;
	/**
	 * @param extent 
	 * @param closest Find the zoom level that corresponds to the absolute closest resolution, which may result in a zoom whose corresponding resolution is actually smaller than we would have desired (if this is being called from a getZoomForExtent() call, then this means that the returned zoom index might not actually contain the entire extent specified... but it’ll be close).  Default is false.
	 * @returns The index of the zoomLevel (entry in the resolutions array) that corresponds to the best fit resolution given the passed in value and the ‘closest’ specification.
	 */
	getZoomForResolution(extent: Bounds, closest: boolean): number;
	/**
	 * An OpenLayers.LonLat which is the passed-in view port OpenLayers.Pixel, translated into lon/lat by the layer.
	 * @param viewPortPx An OpenLayers.Pixel or an object with a ‘x’ and ‘y’ properties.
	 * @returns An OpenLayers.LonLat which is the passed-in view port OpenLayers.Pixel, translated into lon/lat by the layer.
	 */
	getLonLatFromViewPortPx(viewPortPx: Pixel|{x: number, y: number}): LonLat;
	/**
	 * Returns a pixel location given a map location.  This method will return fractional pixel values.
	 * @param lonlat An OpenLayers.LonLat or an object with a ‘lon’ and ‘lat’ properties.
	 * @param resolution 
	 * @returns An OpenLayers.Pixel which is the passed-in lonlat translated into view port pixels.
	 */
	getViewPortPxFromLonLat(lonlat: LonLat, resolution: number): Pixel;
	/**
	 * Sets the opacity for the entire layer (all images)
	 * @param opacity 
	 */
	setOpacity(opacity: number): void;
}

namespace Layer
{
	export class HTTPRequest extends Layer
	{
		events: Events;
		/** Used to hash URL param strings for multi-WMS server selection.  Set to the Golden Ratio per Knuth’s recommendation. */
		static readonly URL_HASH_FACTOR: number;
		/**
		 * @deprecated See http://docs.openlayers.org/library/spherical_mercator.html for information on the replacement for this functionality.
		 * Whether layer should reproject itself based on base layer locations.  This allows reprojection onto commercial layers.  Default is false: Most layers can’t reproject, but layers which can create non-square geographic pixels can, like WMS.
		 */
		reproject?: boolean;
		/**
		 * @param name 
		 * @param url 
		 * @param params 
		 * @param options Hashtable of extra options to tag onto the layer
		 */
		constructor(name: string, url: string|string[], params: object, options: object);
		destroy(): void;
		/**
		 * 
		 * @param obj 
		 * @returns An exact clone of this OpenLayers.Layer.HTTPRequest
		 */
		clone(obj: object): HTTPRequest;
		setUrl(newUrl: string): void;
		/**
		 * @param newParams 
		 * @returns whether the layer was actually redrawn.
		 */
		mergeNewParams(newParams: object): boolean;
		/**
		 * Redraws the layer.  Returns true if the layer was redrawn, false if not.
		 * @param force Force redraw by adding random parameter.
		 * @returns The layer was redrawn.
		 */
		redraw(force: boolean): boolean;
	}

	/**
	 * Base class for layers that use a lattice of tiles.  Create a new grid layer with the OpenLayers.Layer.Grid constructor.
	 */
	export class Grid extends HTTPRequest
	{
		tileSize: Size;
		/** Optional origin for aligning the grid of tiles.  If provided, requests for tiles at all resolutions will be aligned with this location (no tiles shall overlap this location).  If not provided, the grid of tiles will be aligned with the layer’s maxExtent.  Default is ``null``. */
		tileOrigin: LonLat|null;
		/** optional configuration options for OpenLayers.Tile instances created by this Layer, if supported by the tile class. */
		tileOptions: object;
		/** The tile class to use for this layer.  Defaults is OpenLayers.Tile.Image. */
		tileClass: Tile;
		/** Moves the layer into single-tile mode, meaning that one tile will be loaded.  The tile’s size will be determined by the ‘ratio’ property.  When the tile is dragged such that it does not cover the entire viewport, it is reloaded. */
		singleTile: boolean;
		/** Used only when in single-tile mode, this specifies the ratio of the size of the single tile to the size of the map.  Default value is 1.5. */
		ratio: number;
		/** Used only when in gridded mode, this specifies the number of extra rows and columns of tiles on each side which will surround the minimum grid tiles to cover the map.  For very slow loading layers, a larger value may increase performance somewhat when dragging, but will increase bandwidth use significantly. */
		buffer: number;
		/**
		 * The transition effect to use when the map is zoomed.  Two possible values:
		 * ”resize”	Existing tiles are resized on zoom to provide a visual effect of the zoom having taken place immediately.  As the new tiles become available, they are drawn on top of the resized tiles (this is the default setting).
		 * ”map-resize”	Existing tiles are resized on zoom and placed below the base layer.  New tiles for the base layer will cover existing tiles.  This setting is recommended when having an overlay duplicated during the transition is undesirable (e.g. street labels or big transparent fills).
		 * null	No transition effect.
		 * Using “resize” on non-opaque layers can cause undesired visual effects.  Set transitionEffect to null in this case.
		 */
		transitionEffect: string;
		/** How many tiles are still loading? */
		numLoadingTiles: number;
		/** Delay for removing the backbuffer when all tiles have finished loading.  Can be set to 0 when no css opacity transitions for the olTileImage class are used.  Default is 0 for singleTile layers, 2500 for tiled layers.  See className for more information on tile animation. */
		removeBackBufferDelay: number;
		/** Name of the class added to the layer div.  If not set in the options passed to the constructor then className defaults to “olLayerGridSingleTile” for single tile layers (see singleTile), and “olLayerGrid” for non single tile layers. */
		className: string;
		/**
		 * Create a new grid layer
		 * @param name 
		 * @param url 
		 * @param params 
		 * @param options Hashtable of extra options to tag onto the layer
		 */
		constructor(name: string, url: string, params: object, options: object);
		/** Deconstruct the layer and clear the grid. */
		destroy(): void;
		/**
		 * Refetches tiles with new params merged, keeping a backbuffer.  Each loading new tile will have a css class of ‘.olTileReplacing’.  If a stylesheet applies a ‘display: none’ style to that class, any fade-in transition will not apply, and backbuffers for each tile will be removed as soon as the tile is loaded.
		 * @param newParams 
		 * @returns whether the layer was actually redrawn.
		 */
		mergeNewParams(newParams: object): boolean;
		/**
		 * @param newOptions 
		 * @param reinitialize If set to true, and if resolution options of the current baseLayer were changed, the map will be recentered to make sure that it is displayed with a valid resolution, and a changebaselayer event will be triggered.
		 */
		addOptions(newOptions: object, reinitialize: boolean): void;
		/**
		 * Create a clone of this layer
		 * @param obj Is this ever used?
		 * @returns An exact clone of this OpenLayers.Layer.Grid
		 */
		clone(obj: object): Grid;
		/**
		 * Check if we are in singleTile mode and if so, set the size as a ratio of the map size (as specified by the layer’s ‘ratio’ property).
		 * @param size 
		 */
		setTileSize(size: Size): void;
		/**
		 * Return the bounds of the tile grid.
		 * @returns A Bounds object representing the bounds of all the currently loaded tiles (including those partially or not at all seen onscreen).
		 */
		getTilesBounds(): Bounds;
		/**
		 * Create a tile, initialize it, and add it to the layer div.
		 * @param bounds 
		 * @param position 
		 * @returns The added OpenLayers.Tile
		 */
		addTile(bounds: Bounds, position: Pixel): void;
		/**
		 * Returns The tile bounds for a layer given a pixel location.
		 * @param viewPortPx The location in the viewport.
		 * @returns Bounds of the tile at the given pixel location.
		 */
		getTileBounds(viewPortPx: Pixel): Bounds;
	}

	export class XYZ extends Grid
	{
		/** Default is true, as this is designed to be a base tile source. */
		isBaseLayer: boolean;
		/** Whether the tile extents should be set to the defaults for spherical mercator.  Useful for things like OpenStreetMap.  Default is false, except for the OSM subclass. */
		sphericalMercator: boolean;
		/** If your cache has more zoom levels than you want to provide access to with this layer, supply a zoomOffset.  This zoom offset is added to the current map zoom level to determine the level for a requested tile.  For example, if you supply a zoomOffset of 3, when the map is at the zoom 0, tiles will be requested from level 3 of your cache.  Default is 0 (assumes cache level and map zoom are equivalent).  Using zoomOffset is an alternative to setting serverResolutions if you only want to expose a subset of the server resolutions. */
		zoomOffset: number;
		/** A list of all resolutions available on the server.  Only set this property if the map resolutions differ from the server.  This property serves two purposes.  (a) serverResolutions can include resolutions that the server supports and that you don’t want to provide with this layer; you can also look at zoomOffset, which is an alternative to serverResolutions for that specific purpose.  (b) The map can work with resolutions that aren’t supported by the server, i.e. that aren’t in serverResolutions.  When the map is displayed in such a resolution data for the closest server-supported resolution is loaded and the layer div is stretched as necessary. */
		serverResolutions: number[];
		/**
		 * 
		 * @param name 
		 * @param url 
		 * @param options Hashtable of extra options to tag onto the layer
		 */
		constructor(name: string, url: string, options: object);
		/**
		 * Create a clone of this layer
		 * @param obj Is this ever used?
		 * @returns An exact clone of this OpenLayers.Layer.XYZ
		 */
		clone(obj: object): XYZ;
		/**
		 * When the layer is added to a map, then we can fetch our origin (if we don’t have one.)
		 * @param map 
		 */
		setMap(map: Map): void;
	}

	/**
	 * This layer allows accessing OpenStreetMap tiles.  By default the OpenStreetMap hosted tile.openstreetmap.org Mapnik tileset is used.  If you wish to use a different layer instead, you need to provide a different URL to the constructor.  Here’s an example for using OpenCycleMap:
	 */
	export class OSM extends XYZ
	{
		/** The layer name.  Defaults to “OpenStreetMap” if the first argument to the constructor is null or undefined. */
		name: string;
		/**
		 * The tileset URL scheme.  Defaults to (protocol relative url)
		 * [a|b|c].tile.openstreetmap.org/${z}/${x}/${y}.png (the official OSM tileset) if the second argument to the constructor is null or undefined.  To use another tileset you can have something like this:
		 */
		url: string;
		/** optional configuration options for OpenLayers.Tile instances created by this Layer.  Default is */
		tileOptions: object;
		/**
		 * @param name The layer name.
		 * @param url The tileset URL scheme.
		 * @param options Configuration options for the layer.  Any inherited layer option can be set in this object (e.g.  OpenLayers.Layer.Grid.buffer).
		 */
		constructor(name: string, url: string|string[], options?: object);
	}

	/**
	 * Instances of OpenLayers.Layer.ArcGIS93Rest are used to display data from ESRI ArcGIS Server 9.3 (and up?)  Mapping Services using the REST API.  Create a new ArcGIS93Rest layer with the OpenLayers.Layer.ArcGIS93Rest constructor.  More detail on the REST API is available at http://sampleserver1.arcgisonline.com/ArcGIS/SDK/REST/index.html ; specifically, the URL provided to this layer should be an export service URL: http://sampleserver1.arcgisonline.com/ArcGIS/SDK/REST/export.html
	 */
	export class ArcGIS93Rest extends Grid
	{
		/** Hashtable of default parameter key/value pairs */
		static readonly DEFAULT_PARAMS: object;
		/** Default is true for ArcGIS93Rest layer */
		isBaseLayer: boolean;
		/**
		 * Create a new ArcGIS93Rest layer object.
		 * @param name A name for the layer
		 * @param url Base url for the ArcGIS server REST service
		 * @param params An object with key/value pairs representing the query string parameters and values.
		 * @param options Hashtable of extra options to tag onto the layer.
		 */
		constructor(name: string, url: string|string[], params?: object, options?: object);
		/**
		 * Catch changeParams and uppercase the new params to be merged in before calling changeParams on the super class.
		 * 
		 * Once params have been changed, the tiles will be reloaded with the new parameters.
		 * @param newParams Hashtable of new params to use
		 */
		mergeNewParams(newParams: object): boolean;
	}

	/**
	 * Instances of OpenLayers.Layer.WMS are used to display data from OGC Web Mapping Services.  Create a new WMS layer with the OpenLayers.Layer.WMS constructor.
	 */
	export class WMS extends Grid
	{
		/** Hashtable of default parameter key/value pairs */
		static readonly DEFAULT_PARAMS: object;
		/** Default is true for WMS layer */
		isBaseLayer: boolean;
		/** Should the BBOX commas be encoded?  The WMS spec says ‘no’, but some services want it that way.  Default false. */
		encodeBBOX: boolean;
		/** If true, the image format will not be automagicaly switched from image/jpeg to image/png or image/gif when using TRANSPARENT=TRUE.  Also isBaseLayer will not changed by the constructor.  Default false. */
		noMagic: boolean;
		/**
		 * Create a new WMS layer object
		 * 
		 * The code below creates a simple WMS layer using the image/jpeg format.
		 * 
		 * Note the 3rd argument (params).  Properties added to this object will be added to the WMS GetMap requests used for this layer’s tiles.  The only mandatory parameter is “layers”.  Other common WMS params include “transparent”, “styles” and “format”.  Note that the “srs” param will always be ignored.  Instead, it will be derived from the baseLayer’s or map’s projection.
		 * The code below creates a transparent WMS layer with additional options.
		 * 
		 * Note that by default, a WMS layer is configured as baseLayer.  Setting the “transparent” param to true will apply some magic (see noMagic).  The default image format changes from image/jpeg to image/png, and the layer is not configured as baseLayer.
		 * @param name A name for the layer
		 * @param url Base url for the WMS (e.g.  http://wms.jpl.nasa.gov/wms.cgi)
		 * @param params An object with key/value pairs representing the GetMap query string parameters and parameter values.
		 * @param objects Hashtable of extra options to tag onto the layer.  These options include all properties listed above, plus the ones inherited from superclasses.
		 */
		constructor(name: string, url: string, params?: object, objects?: object);
		/**
		 * Returns true if the axis order is reversed for the WMS version and projection of the layer.
		 * @returns true if the axis order is reversed, false otherwise.
		 */
		reverseAxisOrder(): boolean;
		/**
		 * Catch changeParams and uppercase the new params to be merged in before calling changeParams on the super class.
		 * 
		 * Once params have been changed, the tiles will be reloaded with the new parameters.
		 * @param newParams Hashtable of new params to use
		 */
		mergeNewParams(newParams: object): boolean;
		/**
		 * Combine the layer’s url with its params and these newParams.
		 * 
		 * Add the SRS parameter from projection -- this is probably more eloquently done via a setProjection() method, but this works for now and always.
		 * @param newParams 
		 * @param altUrl Use this as the url instead of the layer’s url
		 */
		getFullRequestString(newParams: object, altUrl: string): string;
	}

	/**
	 * Instances of OpenLayers.Layer.Vector are used to render vector data from a variety of sources.  Create a new vector layer with the OpenLayers.Layer.Vector constructor.
	 */
	export class Vector extends Layer
	{
		/**
		 * Listeners will be called with a reference to an event object.  The properties of this event depends on exactly what happened.
		 */
		events: Events;
		/** The layer is a base layer.  Default is false.  Set this property in the layer options. */
		isBaseLayer: boolean;
		/** Whether the layer remains in one place while dragging the map.  Note that setting this to true will move the layer to the bottom of the layer stack. */
		isFixed: boolean;
		features: Feature.Vector[];
		/** report friendly error message when loading of renderer fails. */
		reportError: boolean;
		/** Default style for the layer */
		style: object;
		/** Options for the renderer.  See {OpenLayers.Renderer} for supported options. */
		rendererOptions: object;
		/** geometryType allows you to limit the types of geometries this layer supports.  This should be set to something like “OpenLayers.Geometry.Point” to limit types. */
		geometryType: string;
		/** This specifies the ratio of the size of the visiblity of the Vector Layer features to the size of the map. */
		ratio: number;
		/**
		 * Create a new vector layer
		 * @param name A name for the layer
		 * @param options Optional object with non-default properties to set on the layer.
		 */
		constructor(name: string, options?: object);
		/**
		 * Destroy this layer
		 */
		destroy(): void;
		/**
		 * Hide or show the Layer
		 * @param display 
		 */
		display(display: boolean): void;
		/**
		 * Add Features to the layer.
		 * @param features 
		 * @param options 
		 */
		addFeatures(features: Feature.Vector[], options?: object): void;
		/**
		 * Remove features from the layer.  This erases any drawn features and removes them from the layer’s control.  The beforefeatureremoved and featureremoved events will be triggered for each feature.  The featuresremoved event will be triggered after all features have been removed.  To suppress event triggering, use the silent option.
		 * @param features List of features to be removed.
		 * @param options Optional properties for changing behavior of the removal.
		 */
		removeFeatures(features: Feature.Vector[], options?: object): void;
		/**
		 * Remove all features from the layer.
		 * @param options Optional properties for changing behavior of the removal.
		 */
		removeAllFeatures(options?: object): void;
		/**
		 * Erase and destroy features on the layer.
		 * @param features An optional array of features to destroy.  If not supplied, all features on the layer will be destroyed.
		 * @param options 
		 */
		destroyFeatures(features: Feature.Vector[], options?: object): void;
		/**
		 * Draw (or redraw) a feature on the layer.  If the optional style argument is included, this style will be used.  If no style is included, the feature’s style will be used.  If the feature doesn’t have a style, the layer’s style will be used.
		 * 
		 * This function is not designed to be used when adding features to the layer (use addFeatures instead).  It is meant to be used when the style of a feature has changed, or in some other way needs to visually updated after it has already been added to a layer.  You must add the feature to the layer for most layer-related events to happen.
		 * @param feature 
		 * @param style Named render intent or full symbolizer object.
		 */
		drawFeature(feature: Feature.Vector, style: string|object): void;
		/**
		 * Given a property value, return the feature if it exists in the features array
		 * @param property 
		 * @param value 
		 * @returns A feature corresponding to the given property value or null if there is no such feature.
		 */
		getFeatureBy(property: string, value: string): Feature.Vector|null;
		/**
		 * Given a feature id, return the feature if it exists in the features array
		 * @param featureId 
		 * @returns A feature corresponding to the given featureId or null if there is no such feature.
		 */
		getFeatureById(featureId: string): Feature.Vector|null;
		/**
		 * Given a feature fid, return the feature if it exists in the features array
		 * @param featureFid 
		 * @returns A feature corresponding to the given featureFid or null if there is no such feature.
		 */
		getFeatureByFid(featureFid: string): Feature.Vector|null;
		/**
		 * Returns an array of features that have the given attribute key set to the given value.  Comparison of attribute values takes care of datatypes, e.g. the string ‘1234’ is not equal to the number 1234.
		 * @param attrName 
		 * @param attrValue 
		 * @returns An array of features that have the passed named attribute set to the given value.
		 */
		getFeaturesByAttribute(attrName: string, attrValue: string): Feature.Vector[]|null;
		/**
		 * method called after a feature is inserted.  Does nothing by default.  Override this if you need to do something on feature updates.
		 * @param feature 
		 */
		onFeatureInsert(feature: Feature.Vector): void;
		/**
		 * method called before a feature is inserted.  Does nothing by default.  Override this if you need to do something when features are first added to the layer, but before they are drawn, such as adjust the style.
		 * @param feature 
		 */
		preFeatureInsert(feature: Feature.Vector): void;
		/**
		 * Calculates the max extent which includes all of the features.
		 * @returns or null if the layer has no features with geometries.
		 */
		getDataExtent(): Bounds|null;
	}

	export class Markers extends Layer
	{
		/** Markers layer is never a base layer. */
		isBaseLayer: boolean;
		/** internal marker list */
		markers: Marker[];
		/**
		 * Create a Markers layer.
		 * @param name 
		 * @param options Hashtable of extra options to tag onto the layer
		 */
		constructor(name: string, options?: object);
		destroy(): void;
		/**
		 * Sets the opacity for all the markers.
		 * @param opacity 
		 */
		setOpacity(opacity: number): void;
		addMarker(marker: Marker): void;
		removeMarker(marker: Marker): void;
		/**
		 * Calculates the max extent which includes all of the markers.
		 */
		getDataExtent(): Bounds;
	}
}

/**
 * This class represents a longitude and latitude pair
 */
export class LonLat
{
	/** The x-axis coodinate in map units */
	lon: number;
	/** The y-axis coordinate in map units */
	lat: number;
	/**
	 * Create a new map location.  Coordinates can be passed either as two arguments, or as a single argument.
	 * @param lon The x-axis coordinate in map units.  If your map is in a geographic projection, this will be the Longitude.  Otherwise, it will be the x coordinate of the map location in your map units.
	 * @param lat The y-axis coordinate in map units.  If your map is in a geographic projection, this will be the Latitude.  Otherwise, it will be the y coordinate of the map location in your map units.
	 */
	constructor(lon: number, lat: number);
	/**
	 * @returns Shortened String representation of OpenLayers.LonLat object.  (e.g.  <i>”5, 42”</i>)
	 */
	toShortString(): string;
	/**
	 * @returns New OpenLayers.LonLat object with the same lon and lat values
	 */
	clone(): LonLat;
	/**
	 * 
	 * @param lon 
	 * @param lat 
	 * @returns A new OpenLayers.LonLat object with the lon and lat passed-in added to this’s.
	 */
	add(lon: number, lat: number): LonLat;
	/**
	 * 
	 * @param ll 
	 * @returns Boolean value indicating whether the passed-in OpenLayers.LonLat object has the same lon and lat components as this.  Note: if ll passed in is null, returns false
	 */
	equals(ll: LonLat|null):boolean;
	/**
	 * Transform the LonLat object from source to dest.  This transformation is in place: if you want a new lonlat, use .clone() first.
	 * @param source Source projection.
	 * @param dest Destination projection.
	 * @returns Itself, for use in chaining operations.
	 */
	transform(source: Projection, dest: Projection): LonLat;
	/**
	 * 
	 * @param maxExtent 
	 * @returns A copy of this lonlat, but wrapped around the “dateline” (as specified by the borders of maxExtent)
	 */
	wrapDateLine(maxExtent: Bounds): LonLat;
}

/**
 * Instances of OpenLayers.Map are interactive maps embedded in a web page.  Create a new map with the OpenLayers.Map constructor.
 * 
 * On their own maps do not provide much functionality.  To extend a map it’s necessary to add controls (OpenLayers.Control) and layers (OpenLayers.Layer) to the map.
 */
export class Map
{
	/** Base z-indexes for different classes of thing */
	static readonly Z_INDEX_BASE: object;
	/** An events object that handles all events on the map */
	events: Events;
	/**
	 * Allow the map to function with “overlays” only.  Defaults to false.  If true, the lowest layer in the draw order will act as the base layer.  In addition, if set to true, all layers will have isBaseLayer set to false when they are added to the map.
	 * 
	 * If you set map.allOverlays to true, then you cannot use map.setBaseLayer or layer.setIsBaseLayer.  With allOverlays true, the lowest layer in the draw layer is the base layer.  So, to change the base layer, use setLayerIndex or raiseLayer to set the layer index to 0.
	 */
	allOverlays: boolean;
	/**
	 * The element that contains the map (or an id for that element).  If the OpenLayers.Map constructor is called with two arguments, this should be provided as the first argument.  Alternatively, the map constructor can be called with the options object as the only argument.  In this case (one argument), a div property may or may not be provided.  If the div property is not provided, the map can be rendered to a container later using the render method.
	 * 
	 * If you are calling render after map construction, do not use maxResolution auto.  Instead, divide your maxExtent by your maximum expected dimension.
	 */
	div: HTMLElement|string;
	/** Ordered list of layers in the map */
	layers: Layer[];
	/**
	 * List of controls associated with the map.
	 * 
	 * If not provided in the map options at construction, the map will by default be given the following controls if present in the build:
	 */
	controls: Control[];
	/** The currently selected base layer.  This determines min/max zoom level, projection, etc. */
	baseLayer: Layer;
	/** The options object passed to the class constructor.  Read-only. */
	options: object;
	/** Set in the map options to override the default tile size for this map. */
	tileSize: Size;
	/** Set in the map options to specify the default projection for layers added to this map.  When using a projection other than EPSG:4326 (CRS:84, Geographic) or EPSG:3857 (EPSG:900913, Web Mercator), also set maxExtent, maxResolution or resolutions.  Default is “EPSG:4326”.  Note that the projection of the map is usually determined by that of the current baseLayer (see baseLayer and getProjectionObject). */
	projection: string;
	/** The map units.  Possible values are ‘degrees’ (or ‘dd’), ‘m’, ‘ft’, ‘km’, ‘mi’, ‘inches’.  Normally taken from the projection.  Only required if both map and layers do not define a projection, or if they define a projection which does not define units */
	units: string;
	/** A list of map resolutions (map units per pixel) in descending order.  If this is not set in the layer constructor, it will be set based on other resolution related properties (maxExtent, maxResolution, maxScale, etc.). */
	resolutions: number[];
	/** Required if you are not displaying the whole world on a tile with the size specified in tileSize. */
	maxResolution: number;
	minResolution: number;
	maxScale: number;
	minScale: number;
	/** If provided as an array, the array should consist of four values (left, bottom, right, top).  The maximum extent for the map.  Default depends on projection; if this is one of those defined in OpenLayers.Projection.defaults (EPSG:4326 or web mercator), maxExtent will be set to the value defined there; else, defaults to null.  To restrict user panning and zooming of the map, use restrictedExtent instead.  The value for maxExtent will change calculations for tile URLs. */
	maxExtent: Bounds|number[];
	/** If provided as an array, the array should consist of four values (left, bottom, right, top).  The minimum extent for the map.  Defaults to null. */
	minExtent: Bounds|number[]|null;
	/** If provided as an array, the array should consist of four values (left, bottom, right, top).  Limit map navigation to this extent where possible.  If a non-null restrictedExtent is set, panning will be restricted to the given bounds.  In addition, zooming to a resolution that displays more than the restricted extent will center the map on the restricted extent.  If you wish to limit the zoom level or resolution, use maxResolution. */
	restrictedExtent: Bounds|number[];
	/** Number of zoom levels for the map.  Defaults to 16.  Set a different value in the map options if needed. */
	numZoomLevels: number;
	/** Relative path to a CSS file from which to load theme styles.  Specify null in the map options (e.g.  {theme: null}) if you want to get cascading style declarations - by putting links to stylesheets or style declarations directly in your page. */
	theme: string;
	/** Requires proj4js support for projections other than EPSG:4326 or EPSG:900913/EPSG:3857.  Projection used by several controls to display data to user.  If this property is set, it will be set on any control which has a null displayProjection property at the time the control is added to the map. */
	displayProjection: Projection;
	/** By default, and if the build contains TileManager.js, the map will use the TileManager to queue image requests and to cache tile image elements.  To create a map without a TileManager configure the map with tileManager: null.  To create a TileManager with non-default options, supply the options instead or alternatively supply an instance of {OpenLayers.TileManager}. */
	tileManager: TileManager|object;
	/** Should OpenLayers allow events on the map to fall through to other elements on the page, or should it swallow them?  (#457) Default is to swallow. */
	fallThrough: boolean;
	/** Should OpenLayers automatically update the size of the map when the resize event is fired.  Default is true. */
	autoUpdateSize: boolean;
	/** If set as an option at construction, the eventListeners object will be registered with OpenLayers.Events.on.  Object structure must be a listeners object as shown in the example for the events.on method. */
	eventListeners: object;
	/** The Easing function to be used for tweening.  Default is OpenLayers.Easing.Expo.easeOut.  Setting this to ‘null’ turns off animated panning. */
	panMethod: Function;
	/** The Easing function to be used for tweening.  Default is OpenLayers.Easing.Quad.easeOut.  Setting this to ‘null’ turns off animated zooming. */
	zoomMethod: Function;
	/**
	 * Constructor for a new OpenLayers.Map instance.  There are two possible ways to call the map constructor.  See the examples below.
	 * @param div  The element or id of an element in your page that will contain the map.  May be omitted if the div option is provided or if you intend to call the render method later.
	 * @param options Optional object with properties to tag onto the map.
	 */
	constructor(div: HTMLElement|string, options?: object);
	/**
	 * Get the DOMElement representing the view port.
	 */
	getViewport(): HTMLElement;
	/**
	 * Render the map to a specified container.
	 * @param div The container that the map should be rendered to.  If different than the current container, the map viewport will be moved from the current to the new container.
	 */
	render(div: string|HTMLElement): void;
	/**
	 * Destroy this map.  Note that if you are using an application which removes a container of the map from the DOM, you need to ensure that you destroy the map before this happens; otherwise, the page unload handler will fail because the DOM elements that map.destroy() wants to clean up will be gone.  
	 */
	destroy(): void;
	/**
	 * Change the map options
	 * @param options Hashtable of options to tag to the map
	 */
	setOptions(options: object): void;
	/**
	 * Get the tile size for the map
	 */
	getTileSize(): Size;
	/**
	 * Get a list of objects given a property and a match item.
	 * @param array A property on the map whose value is an array.
	 * @param property A property on each item of the given array.
	 * @param match A string to match.  Can also be a regular expression literal or object.  In addition, it can be any object with a method named test.  For reqular expressions or other, if match.test(map[array][i][property]) evaluates to true, the item will be included in the array returned.  If no items are found, an empty array is returned.
	 * @returns An array of items where the given property matches the given criteria.
	 */
	getBy(array: string, property: string, match: string|object): Array;
	/**
	 * Get a list of layers with properties matching the given criteria.
	 * @param property A layer property to be matched.
	 * @param match A string to match.  Can also be a regular expression literal or object.  In addition, it can be any object with a method named test.  For reqular expressions or other, if match.test(layer[property]) evaluates to true, the layer will be included in the array returned.  If no layers are found, an empty array is returned.
	 * @returns A list of layers matching the given criteria.  An empty array is returned if no matches are found.
	 */
	getLayersBy(property: string, match: string|object): Layer[];
	/**
	 * Get a list of layers with names matching the given name.
	 * @param match A layer name.  The name can also be a regular expression literal or object.  In addition, it can be any object with a method named test.  For reqular expressions or other, if name.test(layer.name) evaluates to true, the layer will be included in the list of layers returned.  If no layers are found, an empty array is returned.
	 * @returns A list of layers matching the given name.  An empty array is returned if no matches are found.
	 */
	getLayersByName(match: string|object): Layer[];
	/**
	 * Get a list of layers of a given class (CLASS_NAME).
	 * @param match A layer class name.  The match can also be a regular expression literal or object.  In addition, it can be any object with a method named test.  For reqular expressions or other, if type.test(layer.CLASS_NAME) evaluates to true, the layer will be included in the list of layers returned.  If no layers are found, an empty array is returned.
	 * @returns A list of layers matching the given class.  An empty array is returned if no matches are found.
	 */
	getLayersByClass(match: string|object): Layer[];
	/**
	 * Get a list of controls with properties matching the given criteria.
	 * @param property A control property to be matched.
	 * @param match A string to match.  Can also be a regular expression literal or object.  In addition, it can be any object with a method named test.  For reqular expressions or other, if match.test(layer[property]) evaluates to true, the layer will be included in the array returned.  If no layers are found, an empty array is returned.
	 * @returns A list of controls matching the given criteria.  An empty array is returned if no matches are found.
	 */
	getControlsBy(property: string, match: string|object): Control[];
	/**
	 * Get a list of controls of a given class (CLASS_NAME).
	 * @param match A control class name.  The match can also be a regular expression literal or object.  In addition, it can be any object with a method named test.  For reqular expressions or other, if type.test(control.CLASS_NAME) evaluates to true, the control will be included in the list of controls returned.  If no controls are found, an empty array is returned.
	 * @returns A list of controls matching the given class.  An empty array is returned if no matches are found.
	 */
	getControlsByClass(match: string|object): Control[];
	/**
	 * Get a layer based on its id
	 * @param id A layer id
	 * @returns The Layer with the corresponding id from the map’s layer collection, or null if not found.
	 */
	getLayer(id: string): Layer|null;
	/**
	 * @param layer 
	 * @returns True if the layer has been added to the map.
	 */
	addLayer(layer: Layer): boolean;
	addLayers(layers: Layer[]): void;
	/**
	 * Removes a layer from the map by removing its visual element (the layer.div property), then removing it from the map’s internal list of layers, setting the layer’s map property to null.
	 * 
	 * a “removelayer” event is triggered.
	 * 
	 * very worthy of mention is that simply removing a layer from a map will not cause the removal of any popups which may have been created by the layer. this is due to the fact that it was decided at some point that popups would not belong to layers. thus there is no way for us to know here to which layer the popup belongs.
	 * 
	 * A simple solution to this is simply to call destroy() on the layer. the default OpenLayers.Layer class’s destroy() function automatically takes care to remove itself from whatever map it has been attached to.
	 * 
	 * The correct solution is for the layer itself to register an event-handler on “removelayer” and when it is called, if it recognizes itself as the layer being removed, then it cycles through its own personal list of popups, removing them from the map.
	 * @param layer 
	 * @param setNewBaseLayer Default is true
	 */
	removeLayer(layer: Layer, setNewBaseLayer?: boolean): void;
	/**
	 * @returns The number of layers attached to the map.
	 */
	getNumLayers(): number;
	/**
	 * @param layer 
	 * @returns The current (zero-based) index of the given layer in the map’s layer stack.  Returns -1 if the layer isn’t on the map.
	 */
	getLayerIndex(layer: Layer): number;
	/**
	 * Move the given layer to the specified (zero-based) index in the layer list, changing its z-index in the map display.  Use map.getLayerIndex() to find out the current index of a layer.  Note that this cannot (or at least should not) be effectively used to raise base layers above overlays.
	 * @param layer 
	 * @param idx 
	 */
	setLayerIndex(layer: Layer, idx: number): void;
	/**
	 * Change the index of the given layer by delta.  If delta is positive, the layer is moved up the map’s layer stack; if delta is negative, the layer is moved down.  Again, note that this cannot (or at least should not) be effectively used to raise base layers above overlays.
	 * @param layer 
	 * @param delta 
	 */
	raiseLayer(layer: Layer, delta: number): void;
	/**
	 * Allows user to specify one of the currently-loaded layers as the Map’s new base layer.
	 * @param newBaseLayer 
	 */
	setBaseLayer(newBaseLayer: Layer): void;
	/**
	 * Add the passed over control to the map.  Optionally position the control at the given pixel.
	 * @param control 
	 * @param px 
	 */
	addControl(control: Control, px?: Pixel): void;
	/**
	 * Add all of the passed over controls to the map.  You can pass over an optional second array with pixel-objects to position the controls.  The indices of the two arrays should match and you can add null as pixel for those controls you want to be autopositioned.
	 * @param controls 
	 * @param pixels 
	 */
	addControls(controls: Control[], pixels: Pixel[]): void;
	/**
	 * 
	 * @param id ID of the control to return.
	 * @returns The control from the map’s list of controls which has a matching ‘id’.  If none found, returns null.
	 */
	getControl(id: string): Control|null;
	/**
	 * Remove a control from the map.  Removes the control both from the map object’s internal array of controls, as well as from the map’s viewPort (assuming the control was not added outsideViewport)
	 * @param control The control to remove.
	 */
	removeControl(control: Control): void;
	/**
	 * @param popup 
	 * @param exclusive If true, closes all other popups first
	 */
	addPopup(popup: Popup, exclusive: boolean): void;
	removePopup(popup: Popup): void;
	/**
	 * @returns An OpenLayers.Size object that represents the size, in pixels, of the div into which OpenLayers has been loaded.  Note - A clone() of this locally cached variable is returned, so as not to allow users to modify it.
	 */
	getSize(): Size;
	/**
	 * This function should be called by any external code which dynamically changes the size of the map div (because mozilla wont let us catch the “onresize” for an element)
	 */
	updateSize(): void;
	getCenter(): LonLat;
	getZoom(): number;
	/**
	 * Allows user to pan by a value of screen pixels
	 * @param dx 
	 * @param dy 
	 * @param options Options to configure panning:
	 */
	pan(dx: number, dy: number, options?: object): void;
	/**
	 * Allows user to pan to a new lonlat.  If the new lonlat is in the current extent the map will slide smoothly
	 * @param lonlat 
	 */
	panTo(lonlat: LonLat): void;
	/**
	 * Set the map center (and optionally, the zoom level).
	 * @param lonlat The new center location.  If provided as array, the first value is the x coordinate, and the 2nd value is the y coordinate.
	 * @param zoom Optional zoom level.
	 * @param dragging Specifies whether or not to trigger movestart/end events
	 * @param forceZoomChange Specifies whether or not to trigger zoom change events (needed on baseLayer change)
	 */
	setCenter(lonlat: LonLat, zoom: number, dragging: boolean, forceZoomChange: boolean): void;
	/**
	 * Returns the minimum zoom level for the current map view.  If the base layer is configured with <wrapDateLine> set to true, this will be the first zoom level that shows no more than one world width in the current map viewport.  Components that rely on this value (e.g. zoom sliders) should also listen to the map’s “updatesize” event and call this method in the “updatesize” listener.
	 * @returns Minimum zoom level that shows a map not wider than its baseLayer’s maxExtent.  This is an Integer value, unless the map is configured with <fractionalZoom> set to true.
	 */
	getMinZoom(): number;
	/**
	 * This method returns a string representing the projection.  In the case of projection support, this will be the srsCode which is loaded -- otherwise it will simply be the string value that was passed to the projection at startup.
	 * 
	 * FIXME: In 3.0, we will remove getProjectionObject, and instead return a Projection object from this function.
	 * @returns The Projection string from the base layer or null.
	 */
	getProjection(): string;
	/**
	 * Returns the projection obect from the baselayer.
	 * @returns The Projection of the base layer.
	 */
	getProjectionObject(): Projection;
	/**
	 * @returns The Map’s Maximum Resolution
	 */
	getMaxResolution(): string;
	/**
	 * 
	 * @param options 
	 * @returns The maxExtent property as set on the current baselayer, unless the ‘restricted’ option is set, in which case the ‘restrictedExtent’ option from the map is returned (if it is set).
	 */
	getMaxExtent(options: object): Bounds;
	/**
	 * @returns The total number of zoom levels that can be displayed by the current baseLayer.
	 */
	getNumZoomLevels(): number;
	/**
	 * @returns A Bounds object which represents the lon/lat bounds of the current viewPort.  If no baselayer is set, returns null.
	 */
	getExtent(): Bounds;
	/**
	 * @returns The current resolution of the map.  If no baselayer is set, returns null.
	 */
	getResolution(): number;
	/**
	 * @returns The current units of the map.  If no baselayer is set, returns null.
	 */
	getUnits(): number;
	/**
	 * @returns The current scale denominator of the map.  If no baselayer is set, returns null.
	 */
	getScale(): number;
	/**
	 * 
	 * @param bounds 
	 * @param closest Find the zoom level that most closely fits the specified bounds.  Note that this may result in a zoom that does not exactly contain the entire extent.  Default is false.
	 * @returns A suitable zoom level for the specified bounds.  If no baselayer is set, returns null.
	 */
	getZoomForExtent(bounds: Bounds, closest: boolean): number;
	/**
	 * 
	 * @param zoom 
	 * @returns A suitable resolution for the specified zoom.  If no baselayer is set, returns null.
	 */
	getResolutionForZoom(zoom: number): number;
	/**
	 * 
	 * @param resolution 
	 * @param closest Find the zoom level that corresponds to the absolute closest resolution, which may result in a zoom whose corresponding resolution is actually smaller than we would have desired (if this is being called from a getZoomForExtent() call, then this means that the returned zoom index might not actually contain the entire extent specified... but it’ll be close).  Default is false.
	 * @returns A suitable zoom level for the specified resolution.  If no baselayer is set, returns null.
	 */
	getZoomForResolution(resolution: number, closest: boolean): number;
	/**
	 * Zoom to a specific zoom level.  Zooming will be animated unless the map is configured with {zoomMethod: null}.  To zoom without animation, use setCenter without a lonlat argument.
	 * @param zoom 
	 */
	zoomTo(zoom: number): void;
	zoomIn(): void;
	zoomOut(): void;
	/**
	 * Zoom to the passed in bounds, recenter
	 * @param bounds If provided as an array, the array should consist of four values (left, bottom, right, top).
	 * @param closest Find the zoom level that most closely fits the specified bounds.  Note that this may result in a zoom that does not exactly contain the entire extent.  Default is false.
	 */
	zoomToExtent(bounds: Bounds, closest: boolean): void;
	/**
	 * Zoom to the full extent and recenter.
	 * @param options 
	 */
	zoomToMaxExtent(options: object): void;
	/**
	 * Zoom to a specified scale
	 * @param scale 
	 * @param closest Find the zoom level that most closely fits the specified scale.  Note that this may result in a zoom that does not exactly contain the entire extent.  Default is false.
	 */
	zoomToScale(scale: number, closest: boolean): void;
	/**
	 * 
	 * @param lonlat 
	 * @returns An OpenLayers.Pixel which is the passed-in OpenLayers.LonLat, translated into view port pixels by the current base layer.
	 */
	getViewPortPxFromLonLat(lonlat: LonLat): Pixel;
	getLonLatFromViewPortPx(viewPortPx: Pixel): LonLat;
	/**
	 * 
	 * @param px An OpenLayers.Pixel or an object with a ‘x’ and ‘y’ properties.
	 * @returns An OpenLayers.LonLat corresponding to the given OpenLayers.Pixel, translated into lon/lat by the current base layer
	 */
	getLonLatFromPixel(px: Pixel): LonLat;
	/**
	 * Returns a pixel location given a map location.  The map location is translated to an integer pixel location (in viewport pixel coordinates) by the current base layer.
	 * @param lonlat A map location.
	 * @returns An OpenLayers.Pixel corresponding to the OpenLayers.LonLat translated into view port pixels by the current base layer.
	 */
	getPixelFromLonLat(lonlat: LonLat): Pixel;
	/**
	 * 
	 * @param layerPx 
	 * @returns Layer Pixel translated into ViewPort Pixel coordinates
	 */
	getViewPortPxFromLayerPx(layerPx: Pixel): Pixel;
	/**
	 * 
	 * @param viewPortPx 
	 * @returns ViewPort Pixel translated into Layer Pixel coordinates
	 */
	getLayerPxFromViewPortPx(viewPortPx: Pixel): Pixel;
	/**
	 * 
	 * @param lonlat lonlat
	 * @returns An OpenLayers.Pixel which is the passed-in OpenLayers.LonLat, translated into layer pixels by the current base layer
	 */
	getLayerPxFromLonLat(lonlat: LonLat): Pixel;
	static readonly TILE_WIDTH: number;
	static readonly TILE_HEIGHT: number;
}
export class Marker
{
	lonlat: LonLat;
	icon: Icon;

	constructor(lonlat: LonLat, icon: Icon);
	destroy(): void;
	isDrawn(): boolean;
}

/**
 * This class represents a screen coordinate, in x and y coordinates
 */
export class Pixel
{
	/** The x coordinate */
	x: number;
	/** The y coordinate */
	y: number;
	/**
	 * Create a new OpenLayers.Pixel instance
	 * @param x The x coordinate
	 * @param y The y coordinate
	 */
	constructor(x: number, y: number);
	/**
	 * Return a clone of this pixel object
	 * @returns A clone pixel
	 */
	clone(): Pixel;
	/**
	 * Determine whether one pixel is equivalent to another
	 * @param px An OpenLayers.Pixel or an object with a ‘x’ and ‘y’ properties.
	 * @returns The point passed in as parameter is equal to this.  Note that if px passed in is null, returns false.
	 */
	equals(px: Pixel|null): boolean;
	/**
	 * Returns the distance to the pixel point passed in as a parameter.
	 * @param px 
	 * @returns The pixel point passed in as parameter to calculate the distance to.
	 */
	distanceTo(px: Pixel): number;
	/**
	 * 
	 * @param x 
	 * @param y 
	 * @returns A new Pixel with this pixel’s x&y augmented by the values passed in.
	 */
	add(x: number, y: number): Pixel;
	/**
	 * Parameters px - {<OpenLayers.Pixel>|Object} An OpenLayers.Pixel or an object with a ‘x’ and ‘y’ properties.
	 * @param px 
	 * @returns A new Pixel with this pixel’s x&y augmented by the x&y values of the pixel passed in.
	 */
	offset(px: Pixel|{x: number, y: number}): Pixel;
}

/**
 * A popup is a small div that can opened and closed on the map.  Typically opened in response to clicking on a marker.  See OpenLayers.Marker.  Popup’s don’t require their own layer and are added the the map using the OpenLayers.Map.addPopup method.
 */
export class Popup
{
	/** Resize the popup to auto-fit the contents.  Default is false. */
	autoSize: boolean;
	/** Minimum size allowed for the popup’s contents. */
	minSize: Size;
	/** Maximum size allowed for the popup’s contents. */
	maxSize: Size;
	/** When drawn, pan map such that the entire popup is visible in the current viewport (if necessary).  Default is false. */
	panMapIfOutOfView: boolean;
	/**
	 * If panMapIfOutOfView is false, and this property is true, contrain the popup such that it always fits in the available map space.  By default, this is not set on the base class.  If you are creating popups that are near map edges and not allowing pannning, and especially if you have a popup which has a fixedRelativePosition, setting this to false may be a smart thing to do.  Subclasses may want to override this setting.
	 * 
	 * Default is false.
	 */
	keepInMap: boolean;
	/**
	 * When map pans, close the popup.  Default is false.
	 */
	closeOnMove: boolean;
	/**
	 * Create a popup.
	 * @param id a unqiue identifier for this popup.  If null is passed an identifier will be automatically generated.
	 * @param lonlat  The position on the map the popup will be shown.
	 * @param contentSize The size of the content.
	 * @param contentHTML  An HTML string to display inside the popup.
	 * @param closeBox  Whether to display a close box inside the popup.
	 * @param closeBoxCallback  Function to be called on closeBox click.
	 */
	constructor(id: string, lonlat: LonLat, contentSize: Size|null, contentHTML: string, closeBox: boolean, closeBoxCallback: Function);
	/**
	 * Auto size the popup so that it precisely fits its contents (as determined by this.contentDiv.innerHTML).  Popup size will, of course, be limited by the available space on the current map
	 */
	updateSize(): void;
	/**
	 * 
	 * @param size Desired size to make the popup.
	 * @returns A size to make the popup which is neither smaller than the specified minimum size, nor bigger than the maximum size (which is calculated relative to the size of the viewport).
	 */
	getSafeContentSize(size: Size): Size;
}

namespace Popup
{
	export class Anchored extends Popup
	{
		/**
		 * If panMapIfOutOfView is false, and this property is true, contrain the popup such that it always fits in the available map space.  By default, this is set.  If you are creating popups that are near map edges and not allowing pannning, and especially if you have a popup which has a fixedRelativePosition, setting this to false may be a smart thing to do.
		 * 
		 * For anchored popups, default is true, since subclasses will usually want this functionality.
		 */
		keepInMap: boolean;
		/**
		 * 
		 * @param id 
		 * @param lonlat 
		 * @param contentSize 
		 * @param contentHTML 
		 * @param anchor Object which must expose a ‘size’ OpenLayers.Size and ‘offset’ OpenLayers.Pixel (generally an OpenLayers.Icon).
		 * @param closeBox 
		 * @param closeBoxCallback Function to be called on closeBox click.
		 */
		constructor(id: string, lonlat: LonLat, contentSize: Size|null, contentHTML: string, anchor: object, closeBox: boolean, closeBoxCallback: Function);
		destroy(): void;
		/**
		 * Overridden from Popup since user might hide popup and then show() it in a new location (meaning we might want to update the relative position on the show)
		 */
		show(): void;
		/**
		 * 
		 * @param contentSize the new size for the popup’s contents div (in pixels).
		 */
		setSize(contentSize: Size): void;
	}

	export class Framed extends Anchored
	{
		/** The image has some alpha and thus needs to use the alpha image hack.  Note that setting this to true will have no noticeable effect in FF or IE7 browsers, but will all but crush the ie6 browser.  Default is false. */
		isAlphaImage: boolean;
		/** We want the framed popup to work dynamically placed relative to its anchor but also in just one fixed position.  A well designed framed popup will have the pixels and logic to display itself in any of the four relative positions, but (understandably), this will not be the case for all of them.  By setting this property to ‘true’, framed popup will not recalculate for the best placement each time it’s open, but will always open the same way.  Note that if this is set to true, it is generally advisable to also set the ‘panIntoView’ property to true so that the popup can be scrolled into view (since it will often be offscreen on open) Default is false. */
		fixedRelativePosition: boolean;
		/**
		 * 
		 * @param id 
		 * @param lonlat 
		 * @param contentSize 
		 * @param contentHTML 
		 * @param anchor Object which must expose a ‘size’ OpenLayers.Size and ‘offset’ OpenLayers.Pixel (generally an OpenLayers.Icon).
		 * @param closeBox 
		 * @param closeBoxCallback Function to be called on closeBox click.
		 */
		constructor(id: string, lonlat: LonLat, contentSize: Size|null, contentHTML: string, anchor: object, closeBox: boolean, closeBoxCallback: Function);
		destroy(): void;
		setBackgroundColor(color: string): void;
		setBorder(): void;
		/**
		 * Overridden here, because we need to update the blocks whenever the size of the popup has changed.
		 * @param contentSize the new size for the popup’s contents div (in pixels).
		 */
		setSize(contentSize: Size): void;
	}

	export class FramedCloud extends Framed
	{
		/** Framed Cloud is autosizing by default. */
		autoSize: boolean;
		/** Framed Cloud does pan into view by default. */
		panMapIfOutOfView: boolean;
		imageSize: Size;
		/** The FramedCloud does not use an alpha image (in honor of the good ie6 folk out there) */
		isAlphaImage: boolean;
		/** The Framed Cloud popup works in just one fixed position. */
		fixedRelativePosition: boolean;
		minSize: Size;
		maxSize: Size;
		/**
		 * 
		 * @param id 
		 * @param lonlat 
		 * @param contentSize 
		 * @param contentHTML 
		 * @param anchor Object which must expose a ‘size’ OpenLayers.Size and ‘offset’ OpenLayers.Pixel (generally an OpenLayers.Icon).
		 * @param closeBox 
		 * @param closeBoxCallback Function to be called on closeBox click.
		 */
		constructor(id: string, lonlat: LonLat, contentSize: Size|null, contentHTML: string, anchor: object|null, closeBox: boolean|null, closeBoxCallback?: Function);
	}
}

/**
 * Methods for coordinate transforms between coordinate systems.  By default, OpenLayers ships with the ability to transform coordinates between geographic (EPSG:4326) and web or spherical mercator (EPSG:900913 et al.) coordinate reference systems.  See the transform method for details on usage.
 * 
 * Additional transforms may be added by using the proj4js library.  If the proj4js library is included, the transform method will work between any two coordinate reference systems with proj4js definitions.
 * 
 * If the proj4js library is not included, or if you wish to allow transforms between arbitrary coordinate reference systems, use the addTransform method to register a custom transform method.
 */
export class Projection
{
	/**
	 * This class offers several methods for interacting with a wrapped pro4js projection object.
	 * @param projCode A string identifying the Well Known Identifier for the projection.
	 * @param options An optional object to set additional properties on the projection.
	 * @returns A projection object.
	 */
	constructor(projCode: string, options?: {});
	/**
	 * Get the string SRS code.
	 * @returns The SRS code.
	 */
	getCode(): string;
	/**
	 * Get the units string for the projection -- returns null if proj4js is not available.
	 * @returns The units abbreviation.
	 * 
	 */
	getUnits(): string;
	/**
	 * Defaults for the SRS codes known to OpenLayers (currently EPSG:4326, CRS:84, urn:ogc:def:crs:EPSG:6.6:4326, EPSG:900913, EPSG:3857, EPSG:102113, EPSG:102100 and OSGEO:41001).  Keys are the SRS code, values are units, maxExtent (the validity extent for the SRS in projected coordinates), worldExtent (the world’s extent in EPSG:4326) and yx (true if this SRS is known to have a reverse axis order).
	 */
	static defaults: Projection;
	/**
	 * Set a custom transform method between two projections.  Use this method in cases where the proj4js lib is not available or where custom projections need to be handled.
	 * @param from The code for the source projection
	 * @param to the code for the destination projection
	 * @param method A function that takes a point as an argument and transforms that point from the source to the destination projection in place.  The original point should be modified.
	 */
	static addTransform(from: string, to: string, method: (point: Point) => void): void;
	/**
	 * Transform a point coordinate from one projection to another.  Note that the input point is transformed in place.
	 * @param point An object with x and y properties representing coordinates in those dimensions.
	 * @param source Source map coordinate system
	 * @param dest Destination map coordinate system
	 * @returns A transformed coordinate.  The original point is modified.
	 */
	static transform(point: Point|{x: number, y: number}, source: Projection, dest: Projection): Point;
	/**
	 * A null transformation useful for defining projection aliases when proj4js is not available:
	 * @param point 
	 */
	static nullTransform(point: Point): void;
}

/**
 * Instances of this class represent a width/height pair
 */
export class Size
{
	/** width */
	w: number;
	/** height */
	h: number;
	/**
	 * Create an instance of OpenLayers.Size
	 * @param w width
	 * @param h height
	 */
	constructor(w: number, h: number);
	/**
	 * Create a clone of this size object
	 * @returns A new OpenLayers.Size object with the same w and h values
	 */
	clone(): Size;
	/**
	 * Determine where this size is equal to another
	 * @param sz An OpenLayers.Size or an object with a ‘w’ and ‘h’ properties.
	 * @returns The passed in size has the same h and w properties as this one.  Note that if sz passed in is null, returns false.
	 */
	equals(sz: Size|null): boolean;
}

export class Util
{
	/**
	 * Given two objects representing points with geographic coordinates, this calculates the distance between those points on the surface of an ellipsoid.
	 * @param p1 (or any object with both .lat, .lon properties)
	 * @param p2 (or any object with both .lat, .lon properties)
	 * @returns The distance (in km) between the two input points as measured on an ellipsoid.  Note that the input point objects must be in geographic coordinates (decimal degrees) and the return distance is in kilometers.
	 */
	static distVincenty(p1: LonLat, p2: LonLat): number;
	/**
	 * Calculate destination point given start point lat/long (numeric degrees), bearing (numeric degrees) & distance (in m).  Adapted from Chris Veness work, see http://www.movable-type.co.uk/scripts/latlong-vincenty-direct.html
	 * @param lonlat (or any object with both .lat, .lon properties) The start point.
	 * @param brng The bearing (degrees).
	 * @param dist The ground distance (meters).
	 * @returns The destination point.
	 */
	static destinationVincenty(lonlat: LonLat, brng: number, dist: number): LonLat;
	/** Constant inches per unit -- borrowed from MapServer mapscale.c derivation of nautical miles from http://en.wikipedia.org/wiki/Nautical_mile Includes the full set of units supported by CS-MAP (http://trac.osgeo.org/csmap/) and PROJ.4 (http://trac.osgeo.org/proj/) The hardcoded table is maintain in a CS-MAP source code module named CSdataU.c The hardcoded table of PROJ.4 units are in pj_units.c. */
	static readonly INCHES_PER_UNIT: object;
	/** 72 (A sensible default) */
	static readonly DOTS_PER_INCH: number;
	/** True if the userAgent reports the browser to use the Gecko engine */
	static readonly IS_GECKO: boolean;
	/** True if canvas 2d is supported. */
	static readonly CANVAS_SUPPORTED: boolean;
	/** 
	 * A substring of the navigator.userAgent property.  Depending on the userAgent property, this will be the empty string or one of the following: 
	 * ”opera” -- Opera
	 * ”msie” -- Internet Explorer
	 * ”safari” -- Safari
	 * ”firefox” -- Firefox
	 * ”mozilla” -- Mozilla
	*/
	static readonly BROWSER_NAME: string;
	/**
	 * This function has been modified by the OpenLayers from the original version, written by Matthew Eernisse and released under the Apache 2 license here:
	 * 
	 * http://www.fleegix.org/articles/2006/05/30/getting-the-scrollbar-width-in-pixels
	 * 
	 * It has been modified simply to cache its value, since it is physically impossible that this code could ever run in more than one browser at once.
	 */
	static getScrollbarWidth(): number;
	/**
	 * This function will return latitude or longitude value formatted as
	 * @param coordinate the coordinate value to be formatted
	 * @param axis value of either ‘lat’ or ‘lon’ to indicate which axis is to to be formatted (default = lat)
	 * @param dmsOption specify the precision of the output can be one of: ‘dms’ show degrees minutes and seconds ‘dm’ show only degrees and minutes ‘d’ show only degrees
	 * @returns the coordinate value formatted as a string
	 */
	static getFormattedLonLat(coordinate: number, axis: string, dmsOption: string): string;
}