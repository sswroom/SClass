import * as kml from "./kml";

export function parseXML(txt: string): kml.Feature | kml.NetworkLinkControl | null;
export function parseFile(file: File): Promise<kml.Feature | kml.NetworkLinkControl | null>;
