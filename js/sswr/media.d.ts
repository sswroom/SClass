export class StaticImage
{
	img: HTMLImageElement;
	exif: any;
	
	constructor(img: HTMLImageElement);
	setExif(exif: any): void;
}
