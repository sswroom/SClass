import * as data from "./data";
import * as kml from "./kml";

export function parseXML(txt: string): kml.KMLFile | null;
/**
 * Parse file into data.ParsedObject
 * @param file file to be parsed
 * @returns can be kml.KMLFile, media.StaticImage. null if failed
 */
export function parseFile(file: File | Response): Promise<data.ParsedObject | null>;
