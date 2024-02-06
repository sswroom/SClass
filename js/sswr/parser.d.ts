import * as kml from "./kml";
import * as media from "./media";

export function parseXML(txt: string): kml.Feature | kml.NetworkLinkControl | null;
export function parseFile(file: File | Response): Promise<kml.Feature | kml.NetworkLinkControl | media.StaticImage | null>;
