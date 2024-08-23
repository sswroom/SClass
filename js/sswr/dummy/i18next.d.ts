/**
 * The default export of the i18next module is an i18next instance ready to be initialized by calling init. You can create additional instances using the createInstance function.
 * 
 * Please read the options page for details on configuration options.
 * 
 * The callback will be called after all translations were loaded or with an error when failed (in case of using a backend).
 * @param options 
 * @param callback 
 */
export function init(options?: object, callback?: Function): void;
/**
 * Please have a look at the translation functions like interpolation, formatting and plurals for more details on using it.
 * 
 * You can specify either one key as a String or multiple keys as an Array of String. The first one that resolves will be returned.
 */
export function t(keys: string|string[], options?: object): string;
/**
 * Uses the same resolve functionality as the t function and returns true if a key exists.
 * @param key 
 * @param options 
 */
export function exists(key: string|string[], options?: object): boolean;
/**
 * Returns a t function that defaults to given language or namespace.
 * 
 * All arguments can be optional/null.
 * 
 * lng and ns params could be arrays of languages or namespaces and will be treated as fallbacks in that case.
 * 
 * The optional keyPrefix will be automatically applied to the returned t function.
 * @param lng 
 * @param ns 
 * @param keyPrefix 
 */
export function getFixedT(lng?: string|string[]|null, ns?: string|string[]|null, keyPrefix?: string|string[]|null): (keys: string|string[], options?: object) => string;
/**
 * Changes the language. The callback will be called as soon translations were loaded or an error occurs while loading.
 * 
 * Calling changeLanguage without lng uses the language detector to choose the language to set.
 * 
 * HINT: For easy testing—setting lng to 'cimode' will cause the t function to always return the key.
 * @param lng 
 * @param callback 
 */
export function changeLanguage(lng?: string, callback?: Function): void;
/**
 * Is set to the current detected or set language.
 * 
 * If you need the primary used language depending on your configuration (supportedLngs, load) you will prefer using i18next.resolvedLanguage or i18next.languages[0].
 */
export let language: string;
/**
 * Is set to an array of language codes that will be used to look up the translation value.
 * 
 * When the language is set, this array is populated with the new language codes. Unless overridden, this array is populated with less-specific versions of that code for fallback purposes, followed by the list of fallback languages.
 */
export let languages: string[];
/**
 * Is set to the current resolved language.
 * It can be used as primary used language, for example in a language switcher.
 * @since v21.0.0
 */
export let resolvedLanguage: string;
/**
 * Checks if namespace has loaded yet. i.e. used by react-i18next
 * @param ns 
 * @param options 
 */
export function hasLoadedNamespace(ns: string, options?: object): boolean;
/**
 * Loads additional namespaces not defined in init options.
 * @param ns 
 * @param callback 
 */
export function loadNamespaces(ns: string, callback?: Function): void;
/**
 * Loads additional languages not defined in init options (preload).
 * @param lngs 
 * @param callback 
 */
export function loadLanguages(lngs: string|string[], callback?: Function): void;
/**
 * Reloads resources on given state. Optionally you can pass an array of languages and namespaces as params if you don't want to reload all.
 * @param lng 
 * @param ns
 * @param callback 
 */
export function reloadResources(lng?: string|string[]|null, ns?: string|string[]|null, callback?: Function): void;
/**
 * Changes the default namespace.
 * @param ns 
 */
export function setDefaultNamespace(ns: string): void;
/**
 * Returns rtl or ltr depending on languages read direction.
 * @param lng undefined for current language
 */
export function dir(lng?: string): string;
/**
 * @since v8.4.0
 * @deprecated v21.3.0
 * Exposes interpolation.formatt function added on init.
 * 
 * For formatting used in translation files checkout the formatting doc.
 * @param data 
 * @param format 
 * @param lng 
 */
export function format(data: string, format: string, lng?: string): string;
/**
 * Will return a new i18next instance.
 * 
 * Please read the options page for details on configuration options.
 * 
 * Providing a callback will automatically call init.
 * 
 * The callback will be called after all translations were loaded or with an error when failed (in case of using a backend).
 * @param options 
 * @param callback 
 */
export function createInstance(options?: object, callback?: Function): ThisType;
/**
 * Creates a clone of the current instance. Shares store, plugins and initial configuration. Can be used to create an instance sharing storage but being independent on set language or default namespaces.
 * @param options 
 */
export function cloneInstance(options?: object): ThisType;
export function on(name: string, callback: Function): void;
export function off(name: string, callback?: Function): void;
/**
 * Gets one value by given key.
 * @param lng 
 * @param ns 
 * @param key 
 * @param options 
 */
export function getResource(lng: string, ns: string, key: string, options?: object): string;
/**
 * Adds one key/value.
 * @param lng 
 * @param ns 
 * @param key 
 * @param value 
 * @param options 
 */
export function addResource(lng: string, ns: string, key: string, value: string, options?: object): void;
/**
 * Adds multiple key/values.
 * @param lng 
 * @param ns 
 * @param resources 
 */
export function addResources(lng: string, ns: string, resources: any): void;
/**
 * Adds a complete bundle.
 * 
 * Setting deep (default false) param to true will extend existing translations in that file. Setting deep and overwrite (default false) to true it will overwrite existing translations in that file.
 * 
 * So omitting deep and overwrite will overwrite all existing translations with the one provided in resources. Using deep you can choose to keep existing nested translation and to overwrite those with the new ones.
 */
export function addResourceBundle(lng: string, ns: string, resources: any, deep?: boolean, overwrite?: boolean): void;
/**
 * Checks if a resource bundle exists.
 * @param lng 
 * @param ns 
 */
export function hasResourceBundle(lng: string, ns: string): boolean;
/**
 * Returns a resource data by language.
 * @param lng 
 */
export function getDataByLanguage(lng: string): object;
/**
 * Returns a resource bundle.
 * @param lng 
 * @param ns 
 */
export function getResourceBundle(lng: string, ns: string): object;
/**
 * Removes an existing bundle.
 * @param lng 
 * @param ns 
 */
export function removeResourceBundle(lng: string, ns: string): void;