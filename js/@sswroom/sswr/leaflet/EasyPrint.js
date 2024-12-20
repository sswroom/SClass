import * as data from "../data.js";
import * as math from "../math.js";
import * as web from "../web.js";
import * as domtoimage from "../domtoimage/index.js";

export class EasyPrint
{
	/**
	 * @param {L.map} map
	 * @param {{title?: string, position?: string, sizeModes?: string[], filename?: string, exportOnly?: boolean, hidden?: boolean, tileLayer?: L.tileLayer|L.tileLayer[],tileWait?: number, hideControlContainer?: boolean, hideClasses?: string[], hideIds?: string[], pageBorderTopHTML?: string, pageBorderBottomHTML?: string, pageBorderHeight?: number, overlayHTML?: string, spinnerBgCOlor?: string, customSpinnerClass?: string}} options
	 */
	constructor(map, options)
	{
		this._a4PageSize = {
			height: 715,
			width: 1045
		};
	
		this._a4PaperSize = {
			width: 1123.660266,
			height: 794.547794
		};
	
		this._a3PaperSize =  {
			width: 1589.095588,
			height: 1123.660266
		};
	
		this._pageMargin = {
			x: 40,
			y: 40
		};

		this.blankDiv = null;
		this._map = map;
		this.options = data.mergeOptions(options, {
			title: 'Print map',
			position: 'topleft',
			sizeModes: ['Current','A4Landscape','A4Portrait','A3Landscape','A3Portrait'],
			sizeModeObjs: [],
			filename: 'map',
			exportOnly: false,
			hidden: false,
			tileLayer: null, //Layer[] | Layer
			tileWait: 500,
			hideControlContainer: true,
			hideClasses: [],
			hideIds: [],
			onclick: null,
			pageBorderTopHTML: null,
			pageBorderBottomHTML: null,
			pageBorderHeight: 0,
			overlayHTML: null,
			customWindowTitle: window.document.title,
			spinnerBgCOlor: '#0DC5C1',
			customSpinnerClass: 'epLoader',
			defaultSizeTitles: {
				Current: 'Current Size',
				A4Landscape: 'A4 Landscape',
				A4Portrait: 'A4 Portrait',
				A3Landscape: 'A3 Landscape',
				A3Portrait: 'A3 Portrait'
			}
		});
		this.mapContainer = map.getContainer();
		this.originalState = {
			mapWidth: this.mapContainer.style.width,
			widthWasAuto: false,
			widthWasPercentage: false,
			mapHeight: this.mapContainer.style.height,
			zoom: this._map.getZoom(),
			center: this._map.getCenter()
		};
	}

	_sizeModeObjs()
	{
		this.options.sizeModeObjs = this.options.sizeModes.map(function (sizeMode) {
			if (sizeMode === 'Current') {
				return {
					name: this.options.defaultSizeTitles.Current,
					className: 'CurrentSize'
				}
			}
			if (sizeMode === 'A4Landscape') {
				return {
					height: this._a4PageSize.height,
					width: this._a4PageSize.width,
					name: this.options.defaultSizeTitles.A4Landscape,
					className: 'A4Landscape page',
					paperSize: 'A4'
				}
			}
			if (sizeMode === 'A4Portrait') {
				return {
					height: this._a4PageSize.width,
					width: this._a4PageSize.height,
					name: this.options.defaultSizeTitles.A4Portrait,
					className: 'A4Portrait page',
					paperSize: 'A4'
				}
			};
			if (sizeMode === 'A3Landscape') {
				return {
					height: this._a3PaperSize.height - this._pageMargin.y * 2,
					width: this._a3PaperSize.width - this._pageMargin.x * 2,
					name: this.options.defaultSizeTitles.A3Landscape,
					className: 'A3Landscape page',
					paperSize: 'A3'
				}
			}
			if (sizeMode === 'A3Portrait') {
				return {
					height: this._a3PaperSize.width - this._pageMargin.x * 2,
					width: this._a3PaperSize.height - this._pageMargin.y * 2,
					name: this.options.defaultSizeTitles.A3Portrait,
					className: 'A3Portrait page',
					paperSize: 'A3'
				}
			};
			return sizeMode;
		}, this);		
	}
	/**
	 * @param {string|{ target: { className: any; }; }} event
	 * @param {string} filename
	 */
	printMap(event, filename) {
		this.resolve = null;
		this.reject = null;
		this.format = "PNG";
		if (filename) {
			this.options.filename = filename
		}
		if (!this.options.exportOnly) {
			this._page = window.open("", "_blank", 'toolbar=no,status=no,menubar=no,scrollbars=no,resizable=no,left=10, top=10, width=200, height=250, visible=none');
			this._page.document.write(this._createSpinner(this.options.customWindowTitle, this.options.customSpinnerClass, this.options.spinnerBgCOlor));
		}
		this._sizeModeObjs();
		this.originalState = {
			mapWidth: this.mapContainer.style.width,
			widthWasAuto: false,
			widthWasPercentage: false,
			mapHeight: this.mapContainer.style.height,
			zoom: this._map.getZoom(),
			center: this._map.getCenter()
		};
		if (this.originalState.mapWidth === 'auto') {
			this.originalState.mapWidth = this._map.getSize().x  + 'px'
			this.originalState.widthWasAuto = true
		} else if (this.originalState.mapWidth.includes('%')) {
			this.originalState.percentageWidth = this.originalState.mapWidth
			this.originalState.widthWasPercentage = true
			this.originalState.mapWidth = this._map.getSize().x  + 'px'
		}
		this._map.fire("easyPrint-start", { event: event });
		if (!this.options.hidden) {
			this._togglePageSizeButtons({type: null});
		}
		if (this.options.hideControlContainer) {
			this._toggleControls();    
		}
		if (this.options.hideClasses) {
			this._toggleClasses(this.options.hideClasses);
		}
		if (this.options.hideIds) {
			this._toggleIds(this.options.hideIds);
		}
		let sizeMode = typeof event !== 'string' ? event.target.className : event;
		if (sizeMode === 'CurrentSize') {
			return this._printOpertion(sizeMode);
		}
		this.outerContainer = this._createOuterContainer(this.mapContainer)
		if (this.originalState.widthWasAuto) {
			this.outerContainer.style.width = this.originalState.mapWidth
		}
		this._createImagePlaceholder(sizeMode)
	}

	/**
	 * @param {math.Coord2D} size
	 * @returns {Promise<Blob>}
	 */
	toPNG(size) {
		this.resolve = null;
		this.reject = null;
		this.format = "PNG";
		this._sizeModeObjs();
		this.originalState = {
			mapWidth: this.mapContainer.style.width,
			widthWasAuto: false,
			widthWasPercentage: false,
			mapHeight: this.mapContainer.style.height,
			zoom: this._map.getZoom(),
			center: this._map.getCenter()
		};
		if (this.originalState.mapWidth === 'auto') {
			this.originalState.mapWidth = this._map.getSize().x  + 'px'
			this.originalState.widthWasAuto = true
		} else if (this.originalState.mapWidth.includes('%')) {
			this.originalState.percentageWidth = this.originalState.mapWidth
			this.originalState.widthWasPercentage = true
			this.originalState.mapWidth = this._map.getSize().x  + 'px'
		}
		let plugin = this;
		return new Promise((resolve, reject) => {
			plugin.resolve = resolve;
			plugin.reject = reject;
			if (!plugin.options.hidden) {
				plugin._togglePageSizeButtons({type: null});
			}
			if (plugin.options.hideControlContainer) {
				plugin._toggleControls();    
			}
			if (plugin.options.hideClasses) {
				plugin._toggleClasses(plugin.options.hideClasses);
			}
			if (plugin.options.hideIds) {
				plugin._toggleIds(plugin.options.hideIds);
			}
			if (size == null) {
				return plugin._printOpertion('CurrentSize');
			}
			plugin.outerContainer = plugin._createOuterContainer(plugin.mapContainer)
			if (plugin.originalState.widthWasAuto) {
				plugin.outerContainer.style.width = plugin.originalState.mapWidth
			}
			plugin._createImagePlaceholder(size)
		});
	}

	/**
	 * @param {math.Coord2D} size
	 * @returns {Promise<Blob>}
	 */
	toSVG(size) {
		this.resolve = null;
		this.reject = null;
		this.format = "SVG";
		this._sizeModeObjs();
		this.originalState = {
			mapWidth: this.mapContainer.style.width,
			widthWasAuto: false,
			widthWasPercentage: false,
			mapHeight: this.mapContainer.style.height,
			zoom: this._map.getZoom(),
			center: this._map.getCenter()
		};
		if (this.originalState.mapWidth === 'auto') {
			this.originalState.mapWidth = this._map.getSize().x  + 'px'
			this.originalState.widthWasAuto = true
		} else if (this.originalState.mapWidth.includes('%')) {
			this.originalState.percentageWidth = this.originalState.mapWidth
			this.originalState.widthWasPercentage = true
			this.originalState.mapWidth = this._map.getSize().x  + 'px'
		}
		let plugin = this;
		return new Promise((resolve, reject) => {
			plugin.resolve = resolve;
			plugin.reject = reject;
			if (!plugin.options.hidden) {
				plugin._togglePageSizeButtons({type: null});
			}
			if (plugin.options.hideControlContainer) {
				plugin._toggleControls();    
			}
			if (plugin.options.hideClasses) {
				plugin._toggleClasses(plugin.options.hideClasses);
			}
			if (plugin.options.hideIds) {
				plugin._toggleIds(plugin.options.hideIds);
			}
			if (size == null) {
				return plugin._printOpertion('CurrentSize');
			}
			plugin.outerContainer = plugin._createOuterContainer(plugin.mapContainer)
			if (plugin.originalState.widthWasAuto) {
				plugin.outerContainer.style.width = plugin.originalState.mapWidth
			}
			plugin._createImagePlaceholder(size)
		});
	}
	
	/**
	 * @param {string} name
	 * @param {any} value
	 */
	updateOptions(name, value) {
		this.options[name] = value;
	}

	/**
	 * @param {string|math.Coord2D} sizeMode
	 */
	async _createImagePlaceholder(sizeMode) {
		let plugin = this;
		let dataUrl = await domtoimage.toPng(this.mapContainer, {
				width: parseInt(this.originalState.mapWidth.replace('px')),
				height: parseInt(this.originalState.mapHeight.replace('px')),
				imagePlaceholder: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAAC4jAAAuIwF4pT92AAAAB3RJTUUH6AMECQMVtyBSbwAAABl0RVh0Q29tbWVudABDcmVhdGVkIHdpdGggR0lNUFeBDhcAAAALSURBVAjXY2AAAgAABQAB4iYFmwAAAABJRU5ErkJggg=="
			});
		try
		{
			plugin.blankDiv = document.createElement("div");
			let blankDiv = plugin.blankDiv;
			plugin.outerContainer.parentElement.insertBefore(blankDiv, plugin.outerContainer);
			blankDiv.className = 'epHolder';
			blankDiv.style.backgroundImage = 'url("' + dataUrl + '")';
			blankDiv.style.position = 'absolute';
			blankDiv.style.zIndex = 1011;
			blankDiv.style.display = 'initial';
			blankDiv.style.width = plugin.originalState.mapWidth;
			blankDiv.style.height = plugin.originalState.mapHeight;
			plugin._resizeAndPrintMap(sizeMode);
		}
		catch (error) {
			console.error('oops, something went wrong!', error);
			let dataUrl = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAAC4jAAAuIwF4pT92AAAAB3RJTUUH6AMECQMVtyBSbwAAABl0RVh0Q29tbWVudABDcmVhdGVkIHdpdGggR0lNUFeBDhcAAAALSURBVAjXY2AAAgAABQAB4iYFmwAAAABJRU5ErkJggg==";
			plugin.blankDiv = document.createElement("div");
			let blankDiv = plugin.blankDiv;
			plugin.outerContainer.parentElement.insertBefore(blankDiv, plugin.outerContainer);
			blankDiv.className = 'epHolder';
			blankDiv.style.backgroundImage = 'url("' + dataUrl + '")';
			blankDiv.style.position = 'absolute';
			blankDiv.style.zIndex = 1011;
			blankDiv.style.display = 'initial';
			blankDiv.style.width = plugin.originalState.mapWidth;
			blankDiv.style.height = plugin.originalState.mapHeight;
			plugin._resizeAndPrintMap(sizeMode);
		}
	}

	/**
	 * @param {string|math.Coord2D} sizeMode
	 */
	_resizeAndPrintMap(sizeMode) {
		this.outerContainer.style.opacity = 0;
		let pageSize;
		if (typeof sizeMode == "string")
		{
			pageSize = this.options.sizeModeObjs.filter(function (item) {
				return item.className.indexOf(sizeMode) > -1;
			});
			pageSize = pageSize[0];
		}
		else
		{
			pageSize = {width: sizeMode.x, height: sizeMode.y, className: "Custom", name: "Custom"};
		}
		if (pageSize == null)
		{
			console.log(this.options.sizeModeObjs, sizeMode);
			throw new Error("sizeMode not found");
		}
		let pageBorderHeight = 0;
		if (this.options.pageBorderHeight)
			pageBorderHeight = this.options.pageBorderHeight + 4;
		this.mapContainer.style.width = pageSize.width + 'px';
		this.mapContainer.style.height = (pageSize.height - pageBorderHeight) + 'px';
		if (pageSize.width < pageSize.height) {
			this.orientation = 'portrait';
		} else {
			this.orientation = 'landscape';
		}
		this.paperSize = pageSize.paperSize;
		this._map.setView(this.originalState.center);
		this._map.setZoom(this.originalState.zoom);
		this._map.invalidateSize();
		if (this.options.tileLayer) {
			this._pausePrint(sizeMode)
		} else {
			this._printOpertion(sizeMode)
		}
	}

	/**
	 * @param {string|math.Coord2D} sizeMode
	 */
	_pausePrint(sizeMode) {
		let plugin = this;
		let loadingTest = setInterval(function () { 
			let loading = false;
			let tileLayer = plugin.options.tileLayer;
			if (tileLayer.constructor === Array)
			{
				let i = 0;
				let j = tileLayer.length;
				while (i < j)
				{
					if (tileLayer[i].isLoading())
					{
						loading = true;
						break;
					}
					i++;
				}
			}
			else
			{
				loading = tileLayer.isLoading();
			}
			if (!loading)
			{
				clearInterval(loadingTest);
				plugin._printOpertion(sizeMode)
			}
		}, plugin.options.tileWait);
	}

	/**
	 * @param {string|math.Coord2D} sizemode
	 */
	async _printOpertion(sizemode) {
		let plugin = this;
		let widthForExport = this.mapContainer.style.width
		if (this.originalState.widthWasAuto && sizemode === 'CurrentSize' || this.originalState.widthWasPercentage && sizemode === 'CurrentSize') {
			widthForExport = this.originalState.mapWidth
		}
		let dataUrl;
		if (this.format == "SVG")
		{
			dataUrl = await domtoimage.toSvg(plugin.mapContainer, {
				width: parseInt(widthForExport),
				height: parseInt(plugin.mapContainer.style.height.replace('px')),
				imagePlaceholder: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAAC4jAAAuIwF4pT92AAAAB3RJTUUH6AMECQMVtyBSbwAAABl0RVh0Q29tbWVudABDcmVhdGVkIHdpdGggR0lNUFeBDhcAAAALSURBVAjXY2AAAgAABQAB4iYFmwAAAABJRU5ErkJggg=="
			});
		}
		else
		{
			dataUrl = await domtoimage.toPng(plugin.mapContainer, {
				width: parseInt(widthForExport),
				height: parseInt(plugin.mapContainer.style.height.replace('px')),
				imagePlaceholder: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAAC4jAAAuIwF4pT92AAAAB3RJTUUH6AMECQMVtyBSbwAAABl0RVh0Q29tbWVudABDcmVhdGVkIHdpdGggR0lNUFeBDhcAAAALSURBVAjXY2AAAgAABQAB4iYFmwAAAABJRU5ErkJggg=="
			});
		}
		try
		{
			let blob = data.dataURI2Blob(dataUrl);
			if (plugin.resolve) {
				plugin.resolve(blob);
			} else if (plugin.options.exportOnly) {
				web.openData(blob, blob.type, plugin.options.filename + '.png');
			} else {
				plugin._sendToBrowserPrint(dataUrl, plugin.orientation, plugin.paperSize);
			}
			plugin._toggleControls(true);
			plugin._toggleClasses(plugin.options.hideClasses, true);
			plugin._toggleIds(plugin.options.hideIds, true);

			if (plugin.outerContainer) {
				if (plugin.originalState.widthWasAuto) {
					plugin.mapContainer.style.width = 'auto'
				} else if (plugin.originalState.widthWasPercentage) {
					plugin.mapContainer.style.width = plugin.originalState.percentageWidth
				}
				else {
					plugin.mapContainer.style.width = plugin.originalState.mapWidth;              
				}
				plugin.mapContainer.style.height = plugin.originalState.mapHeight;
				plugin._removeOuterContainer(plugin.mapContainer, plugin.outerContainer, plugin.blankDiv)
				plugin._map.invalidateSize();
				plugin._map.setView(plugin.originalState.center);
				plugin._map.setZoom(plugin.originalState.zoom);
			}
			plugin._map.fire("easyPrint-finished");
		}
		catch (error) {
			console.error('Print operation failed', error);
			if (plugin.reject)
			{
				plugin.reject("Failed");
			}

			plugin._page.document.close();  
			plugin._toggleControls(true);
			plugin._toggleClasses(plugin.options.hideClasses, true);
			plugin._toggleIds(plugin.options.hideIds, true);

			if (plugin.outerContainer) {
				if (plugin.originalState.widthWasAuto) {
					plugin.mapContainer.style.width = 'auto'
				} else if (plugin.originalState.widthWasPercentage) {
					plugin.mapContainer.style.width = plugin.originalState.percentageWidth
				}
				else {
					plugin.mapContainer.style.width = plugin.originalState.mapWidth;              
				}
				plugin.mapContainer.style.height = plugin.originalState.mapHeight;
				plugin._removeOuterContainer(plugin.mapContainer, plugin.outerContainer, plugin.blankDiv)
				plugin._map.invalidateSize();
				plugin._map.setView(plugin.originalState.center);
				plugin._map.setZoom(plugin.originalState.zoom);
			}
			plugin._map.fire("easyPrint-finished");
		}
	}

	/**
	 * @param {string} img
	 * @param {string} orientation
	 * @param {any} paperSize
	 */
	_sendToBrowserPrint(img, orientation, paperSize) {
		this._page.resizeTo(600, 800); 
		let pageContent = this._createNewWindow(img, orientation, this, paperSize)
		this._page.document.body.innerHTML = ''
		this._page.document.write(pageContent);
		this._page.document.close();  
	}

	/**
	 * @param {string} title
	 * @param {string} spinnerClass
	 * @param {string} spinnerColor
	 */
	_createSpinner(title, spinnerClass, spinnerColor) {
		return `<html><head><title>`+ title + `</title></head><body><style>
			body{
				background: ` + spinnerColor + `;
			}
			.epLoader,
			.epLoader:before,
			.epLoader:after {
				border-radius: 50%;
			}
			.epLoader {
				color: #ffffff;
				font-size: 11px;
				text-indent: -99999em;
				margin: 55px auto;
				position: relative;
				width: 10em;
				height: 10em;
				box-shadow: inset 0 0 0 1em;
				-webkit-transform: translateZ(0);
				-ms-transform: translateZ(0);
				transform: translateZ(0);
			}
			.epLoader:before,
			.epLoader:after {
				position: absolute;
				content: '';
			}
			.epLoader:before {
				width: 5.2em;
				height: 10.2em;
				background: #0dc5c1;
				border-radius: 10.2em 0 0 10.2em;
				top: -0.1em;
				left: -0.1em;
				-webkit-transform-origin: 5.2em 5.1em;
				transform-origin: 5.2em 5.1em;
				-webkit-animation: load2 2s infinite ease 1.5s;
				animation: load2 2s infinite ease 1.5s;
			}
			.epLoader:after {
				width: 5.2em;
				height: 10.2em;
				background: #0dc5c1;
				border-radius: 0 10.2em 10.2em 0;
				top: -0.1em;
				left: 5.1em;
				-webkit-transform-origin: 0px 5.1em;
				transform-origin: 0px 5.1em;
				-webkit-animation: load2 2s infinite ease;
				animation: load2 2s infinite ease;
			}
			@-webkit-keyframes load2 {
				0% {
					-webkit-transform: rotate(0deg);
					transform: rotate(0deg);
				}
				100% {
					-webkit-transform: rotate(360deg);
					transform: rotate(360deg);
				}
			}
			@keyframes load2 {
				0% {
					-webkit-transform: rotate(0deg);
					transform: rotate(0deg);
				}
				100% {
					-webkit-transform: rotate(360deg);
					transform: rotate(360deg);
				}
			}
			</style>
		<div class="`+spinnerClass+`">Loading...</div></body></html>`;
	}

	/**
	 * @param {string} img
	 * @param {string} orientation
	 * @param {this} plugin
	 * @param {string|null} paperSize
	 */
	_createNewWindow(img, orientation, plugin, paperSize) {
		let strs =  new Array();
		strs.push(`<html><head>
				<style>@media print {
					img { max-width: 98%!important; max-height: 98%!important; }
					@page { size: ` + (paperSize?paperSize:'')+' '+ orientation + `;}}
				</style>
				<script>function step1(){
				setTimeout('step2()', 10);}
				function step2(){window.print();window.close()}
				</script></head><body onload='step1()' style="margin: 0px;">`);
		if (plugin.options.pageBorderTopHTML || plugin.options.pageBorderBottomHTML)
		{
			strs.push("<table border=\"0\" width=\"100%\" height=\"100%\">");
			if (plugin.options.pageBorderTopHTML)
			{
				strs.push("<tr><td>"+plugin.options.pageBorderTopHTML+"</td></tr>");
			}
			strs.push(`<tr><td>`);
			if (plugin.options.overlayHTML)
				strs.push(plugin.options.overlayHTML);
			strs.push(`<img src="` + img + `" style="display:block; margin:auto;"></td></tr>`);
			if (plugin.options.pageBorderBottomHTML)
			{
				strs.push("<tr><td>"+plugin.options.pageBorderBottomHTML+"</td></tr>");
			}
			strs.push("</table>");
		}
		else
		{
			if (plugin.options.overlayHTML)
				strs.push(plugin.options.overlayHTML);
			strs.push(`<img src="` + img + `" style="display:block; margin:auto;">`);
		}
		strs.push(`</body></html>`);
		return strs.join("");
	}

	/**
	 * @param {HTMLDivElement} mapDiv
	 */
	_createOuterContainer(mapDiv) {
		let outerContainer = document.createElement('div'); 
		mapDiv.parentNode.insertBefore(outerContainer, mapDiv); 
		mapDiv.parentNode.removeChild(mapDiv);
		outerContainer.appendChild(mapDiv);
		outerContainer.style.width = mapDiv.style.width;
		outerContainer.style.height = mapDiv.style.height;
		outerContainer.style.display = 'inline-block'
		outerContainer.style.overflow = 'hidden';
		return outerContainer;
	}

	/**
	 * @param {HTMLDivElement} mapDiv
	 * @param {HTMLDivElement} outerContainer
	 * @param {HTMLDivElement} blankDiv
	 */
	_removeOuterContainer(mapDiv, outerContainer, blankDiv) {
		if (outerContainer.parentNode) {
			outerContainer.parentNode.insertBefore(mapDiv, outerContainer);
			outerContainer.parentNode.removeChild(blankDiv);
			outerContainer.parentNode.removeChild(outerContainer);      
		}
	}

	_addCss() {
		let css = document.createElement("style");
		css.type = "text/css";
		css.innerHTML = `.leaflet-control-easyPrint-button { 
			background-image: url(data:image/svg+xml;utf8;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iaXNvLTg4NTktMSI/Pgo8IS0tIEdlbmVyYXRvcjogQWRvYmUgSWxsdXN0cmF0b3IgMTYuMC4wLCBTVkcgRXhwb3J0IFBsdWctSW4gLiBTVkcgVmVyc2lvbjogNi4wMCBCdWlsZCAwKSAgLS0+CjwhRE9DVFlQRSBzdmcgUFVCTElDICItLy9XM0MvL0RURCBTVkcgMS4xLy9FTiIgImh0dHA6Ly93d3cudzMub3JnL0dyYXBoaWNzL1NWRy8xLjEvRFREL3N2ZzExLmR0ZCI+CjxzdmcgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIiB4bWxuczp4bGluaz0iaHR0cDovL3d3dy53My5vcmcvMTk5OS94bGluayIgdmVyc2lvbj0iMS4xIiBpZD0iQ2FwYV8xIiB4PSIwcHgiIHk9IjBweCIgd2lkdGg9IjE2cHgiIGhlaWdodD0iMTZweCIgdmlld0JveD0iMCAwIDUxMiA1MTIiIHN0eWxlPSJlbmFibGUtYmFja2dyb3VuZDpuZXcgMCAwIDUxMiA1MTI7IiB4bWw6c3BhY2U9InByZXNlcnZlIj4KPGc+Cgk8cGF0aCBkPSJNMTI4LDMyaDI1NnY2NEgxMjhWMzJ6IE00ODAsMTI4SDMyYy0xNy42LDAtMzIsMTQuNC0zMiwzMnYxNjBjMCwxNy42LDE0LjM5OCwzMiwzMiwzMmg5NnYxMjhoMjU2VjM1Mmg5NiAgIGMxNy42LDAsMzItMTQuNCwzMi0zMlYxNjBDNTEyLDE0Mi40LDQ5Ny42LDEyOCw0ODAsMTI4eiBNMzUyLDQ0OEgxNjBWMjg4aDE5MlY0NDh6IE00ODcuMTk5LDE3NmMwLDEyLjgxMy0xMC4zODcsMjMuMi0yMy4xOTcsMjMuMiAgIGMtMTIuODEyLDAtMjMuMjAxLTEwLjM4Ny0yMy4yMDEtMjMuMnMxMC4zODktMjMuMiwyMy4xOTktMjMuMkM0NzYuODE0LDE1Mi44LDQ4Ny4xOTksMTYzLjE4Nyw0ODcuMTk5LDE3NnoiIGZpbGw9IiMwMDAwMDAiLz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8L3N2Zz4K);
			background-size: 16px 16px; 
			cursor: pointer; 
		}
		.leaflet-control-easyPrint-button-export { 
			background-image: url(data:image/svg+xml;utf8;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iaXNvLTg4NTktMSI/Pgo8IS0tIEdlbmVyYXRvcjogQWRvYmUgSWxsdXN0cmF0b3IgMTYuMC4wLCBTVkcgRXhwb3J0IFBsdWctSW4gLiBTVkcgVmVyc2lvbjogNi4wMCBCdWlsZCAwKSAgLS0+CjwhRE9DVFlQRSBzdmcgUFVCTElDICItLy9XM0MvL0RURCBTVkcgMS4xLy9FTiIgImh0dHA6Ly93d3cudzMub3JnL0dyYXBoaWNzL1NWRy8xLjEvRFREL3N2ZzExLmR0ZCI+CjxzdmcgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIiB4bWxuczp4bGluaz0iaHR0cDovL3d3dy53My5vcmcvMTk5OS94bGluayIgdmVyc2lvbj0iMS4xIiBpZD0iQ2FwYV8xIiB4PSIwcHgiIHk9IjBweCIgd2lkdGg9IjE2cHgiIGhlaWdodD0iMTZweCIgdmlld0JveD0iMCAwIDQzMy41IDQzMy41IiBzdHlsZT0iZW5hYmxlLWJhY2tncm91bmQ6bmV3IDAgMCA0MzMuNSA0MzMuNTsiIHhtbDpzcGFjZT0icHJlc2VydmUiPgo8Zz4KCTxnIGlkPSJmaWxlLWRvd25sb2FkIj4KCQk8cGF0aCBkPSJNMzk1LjI1LDE1M2gtMTAyVjBoLTE1M3YxNTNoLTEwMmwxNzguNSwxNzguNUwzOTUuMjUsMTUzeiBNMzguMjUsMzgyLjV2NTFoMzU3di01MUgzOC4yNXoiIGZpbGw9IiMwMDAwMDAiLz4KCTwvZz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8L3N2Zz4K);
			background-size: 16px 16px; 
			cursor: pointer; 
		}
		.easyPrintHolder a {
			background-size: 16px 16px;
			cursor: pointer;
		}
		.easyPrintHolder .CurrentSize{
			background-image: url(data:image/svg+xml;utf8;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0idXRmLTgiPz4KPCFET0NUWVBFIHN2ZyBQVUJMSUMgIi0vL1czQy8vRFREIFNWRyAxLjEvL0VOIiAiaHR0cDovL3d3dy53My5vcmcvR3JhcGhpY3MvU1ZHLzEuMS9EVEQvc3ZnMTEuZHRkIj4KPHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHhtbG5zOnhsaW5rPSJodHRwOi8vd3d3LnczLm9yZy8xOTk5L3hsaW5rIiB3aWR0aD0iMTZweCIgdmVyc2lvbj0iMS4xIiBoZWlnaHQ9IjE2cHgiIHZpZXdCb3g9IjAgMCA2NCA2NCIgZW5hYmxlLWJhY2tncm91bmQ9Im5ldyAwIDAgNjQgNjQiPgogIDxnPgogICAgPGcgZmlsbD0iIzFEMUQxQiI+CiAgICAgIDxwYXRoIGQ9Ik0yNS4yNTUsMzUuOTA1TDQuMDE2LDU3LjE0NVY0Ni41OWMwLTEuMTA4LTAuODk3LTIuMDA4LTIuMDA4LTIuMDA4QzAuODk4LDQ0LjU4MiwwLDQ1LjQ4MSwwLDQ2LjU5djE1LjQwMiAgICBjMCwwLjI2MSwwLjA1MywwLjUyMSwwLjE1NSwwLjc2N2MwLjIwMywwLjQ5MiwwLjU5NCwwLjg4MiwxLjA4NiwxLjA4N0MxLjQ4Niw2My45NDcsMS43NDcsNjQsMi4wMDgsNjRoMTUuNDAzICAgIGMxLjEwOSwwLDIuMDA4LTAuODk4LDIuMDA4LTIuMDA4cy0wLjg5OC0yLjAwOC0yLjAwOC0yLjAwOEg2Ljg1NWwyMS4yMzgtMjEuMjRjMC43ODQtMC43ODQsMC43ODQtMi4wNTUsMC0yLjgzOSAgICBTMjYuMDM5LDM1LjEyMSwyNS4yNTUsMzUuOTA1eiIgZmlsbD0iIzAwMDAwMCIvPgogICAgICA8cGF0aCBkPSJtNjMuODQ1LDEuMjQxYy0wLjIwMy0wLjQ5MS0wLjU5NC0wLjg4Mi0xLjA4Ni0xLjA4Ny0wLjI0NS0wLjEwMS0wLjUwNi0wLjE1NC0wLjc2Ny0wLjE1NGgtMTUuNDAzYy0xLjEwOSwwLTIuMDA4LDAuODk4LTIuMDA4LDIuMDA4czAuODk4LDIuMDA4IDIuMDA4LDIuMDA4aDEwLjU1NmwtMjEuMjM4LDIxLjI0Yy0wLjc4NCwwLjc4NC0wLjc4NCwyLjA1NSAwLDIuODM5IDAuMzkyLDAuMzkyIDAuOTA2LDAuNTg5IDEuNDIsMC41ODlzMS4wMjctMC4xOTcgMS40MTktMC41ODlsMjEuMjM4LTIxLjI0djEwLjU1NWMwLDEuMTA4IDAuODk3LDIuMDA4IDIuMDA4LDIuMDA4IDEuMTA5LDAgMi4wMDgtMC44OTkgMi4wMDgtMi4wMDh2LTE1LjQwMmMwLTAuMjYxLTAuMDUzLTAuNTIyLTAuMTU1LTAuNzY3eiIgZmlsbD0iIzAwMDAwMCIvPgogICAgPC9nPgogIDwvZz4KPC9zdmc+Cg==)
		}
		.easyPrintHolder .page {
			background-image: url(data:image/svg+xml;utf8;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iaXNvLTg4NTktMSI/Pgo8IS0tIEdlbmVyYXRvcjogQWRvYmUgSWxsdXN0cmF0b3IgMTguMS4xLCBTVkcgRXhwb3J0IFBsdWctSW4gLiBTVkcgVmVyc2lvbjogNi4wMCBCdWlsZCAwKSAgLS0+CjxzdmcgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIiB4bWxuczp4bGluaz0iaHR0cDovL3d3dy53My5vcmcvMTk5OS94bGluayIgdmVyc2lvbj0iMS4xIiBpZD0iQ2FwYV8xIiB4PSIwcHgiIHk9IjBweCIgdmlld0JveD0iMCAwIDQ0NC44MzMgNDQ0LjgzMyIgc3R5bGU9ImVuYWJsZS1iYWNrZ3JvdW5kOm5ldyAwIDAgNDQ0LjgzMyA0NDQuODMzOyIgeG1sOnNwYWNlPSJwcmVzZXJ2ZSIgd2lkdGg9IjUxMnB4IiBoZWlnaHQ9IjUxMnB4Ij4KPGc+Cgk8Zz4KCQk8cGF0aCBkPSJNNTUuMjUsNDQ0LjgzM2gzMzQuMzMzYzkuMzUsMCwxNy03LjY1LDE3LTE3VjEzOS4xMTdjMC00LjgxNy0xLjk4My05LjM1LTUuMzgzLTEyLjQ2N0wyNjkuNzMzLDQuNTMzICAgIEMyNjYuNjE3LDEuNywyNjIuMzY3LDAsMjU4LjExNywwSDU1LjI1Yy05LjM1LDAtMTcsNy42NS0xNywxN3Y0MTAuODMzQzM4LjI1LDQzNy4xODMsNDUuOSw0NDQuODMzLDU1LjI1LDQ0NC44MzN6ICAgICBNMzcyLjU4MywxNDYuNDgzdjAuODVIMjU2LjQxN3YtMTA4LjhMMzcyLjU4MywxNDYuNDgzeiBNNzIuMjUsMzRoMTUwLjE2N3YxMzAuMzMzYzAsOS4zNSw3LjY1LDE3LDE3LDE3aDEzMy4xNjd2MjI5LjVINzIuMjVWMzR6ICAgICIgZmlsbD0iIzAwMDAwMCIvPgoJPC9nPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+Cjwvc3ZnPgo=);
		}
		.easyPrintHolder .A4Landscape { 
			transform: rotate(-90deg);
		}
		.easyPrintHolder .A3Landscape { 
			transform: rotate(-90deg);
		}

		.leaflet-control-easyPrint-button{
			display: inline-block;
		}
		.easyPrintHolder{
			margin-top:-31px;
			margin-bottom: -5px;
			margin-left: 30px;
			padding-left: 0px;
			display: none;
		}

		.easyPrintSizeMode {
			display: inline-block;
		}
		.easyPrintHolder .easyPrintSizeMode a {
			border-radius: 0px;
		}

		.easyPrintHolder .easyPrintSizeMode:last-child a{
			border-top-right-radius: 2px;
			border-bottom-right-radius: 2px;
			margin-left: -1px;
		}

		.easyPrintPortrait:hover, .easyPrintLandscape:hover{
			background-color: #757570;
			cursor: pointer;
		}`;
		document.body.appendChild(css);
	}

	/**
	 * @param {{ type: any; }} e
	 */
	_togglePageSizeButtons(e) {
/*		let holderStyle = this.holder.style
		let linkStyle = this.link.style
		if (e.type === 'mouseover') {
			holderStyle.display = 'block';
			linkStyle.borderTopRightRadius = '0'
			linkStyle.borderBottomRightRadius = '0'
		} else {
			holderStyle.display = 'none';
			linkStyle.borderTopRightRadius = '2px'
			linkStyle.borderBottomRightRadius = '2px'      
		}*/
	}

	/**
	 * @param {boolean | undefined} [show]
	 */
	_toggleControls(show) {
		let controlContainer = document.getElementsByClassName("leaflet-control-container")[0];
		if (show)
			controlContainer.style.display = 'block';
		else
			controlContainer.style.display = 'none';
	}

	/**
	 * @param {any[]} classes
	 * @param {boolean | undefined} [show]
	 */
	_toggleClasses(classes, show) {
		classes.forEach(function (/** @type {string} */ className) {
			let divs = document.getElementsByClassName(className);
			let i;
			for (i in divs)
			{
				if (divs[i].style)
				{
					divs[i].style.display = show?'':'none';
				}
			}
		});
	}
	/**
	 * @param {any[]} ids
	 * @param {boolean | undefined} [show]
	 */
	_toggleIds(ids, show) {
		ids.forEach(function (/** @type {string} */ id) {
			let div = document.getElementById(id);
			if (div && div.style)
			{
				div.style.display = show?'':'none';
			}
		});
	}
}
