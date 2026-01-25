# XML

Transcendence stores much of its game data in a customized XML format.

## ABOUT THIS DOCUMENT

Be aware that this document is meant to reflect the current API, but is at this time manually updated.
Additionally, deprecated features/attributes/tags/etc are not documented here.

# TRANSCENDENCE XML FILE STRUCTURE

Transcendence XML files use the following general structure

```
<?xml version="1.0" encoding="utf-8"?>

<!DOCTYPE {DocumentType}
	[
	<!ENTITY entityName                 "entityValue">
	]>

<{DocumentType}>
    ;   This is a single line comment

    ;   Technically this comment style is used for Tlisp
    ;   everything after the semicolon is considered commented BUT it doesnt comment out Tags

    ;   Note: This codebase ONLY uses this style of comment for Tlisp script and uses XML
    ;   multiline comments everywhere else, however you may see this style in mods and
    ;   registered developer content

    <!--
        This is an XML multiline comment

        You can safely comment out XML <tag>s in here, but
        nesting other comments is NOT safe and can cause parsing
        issues.

        Note: you cannot put any type of comment inside of the < and > of a <Tag>
        Only the tag itself and any attributes are allowed in there.
    -->

    <{Tag}
            {attribute}=          "{value}"
            >
        <!-- There are different types of tags that will be explained later in this guide -->

        <!-- Note that this closing tag has a / in front of the tag name -->
    </{Tag}>

    <!--
    If you dont have inner text, tlisp, or tags, you can use a single tag stype which
    has a / at the end
    -->
    <{SingleTag} {someOtherAttribute}="{value}"/>

<\{DocumentType}>
```

The `{DocumentType}` is replaced with the actual document types listed below:

## Document Types

There are 6 main types of Transcendence XML documents:
* TranscendenceUniverse
    * ENTITYs and Types defined in a TranscendenceUniverse are globally available
    * There can only be one TranscendenceUniverse loaded
    * Must always be on the same API version as the engine
    * Required in order to start the game
    * Contains one or more CoreLibrary documents
    * Cannot be assigned an UNID - the engine always automatically assigned UNID 0x00000000
* CoreLibrary
    * ENTITYs and Types defined in a CoreLibrary require the CoreLibrary to be referenced as a dependency
    * Use the same API version as the parent TranscendenceUniverse
    * Cannot exist independently of the TranscendenceUniverse
    * Contains one or more modules
    * Must be assigned an UNID
* TranscendenceLibrary
    * ENTITYs and Types defined in a TranscendenceLibrary require the TranscendenceLibrary to be referenced as a dependency
    * Contains one or more modules
    * Must be assigned an UNID
* TranscendenceAdventure
    * ENTITYs defined in a TranscendenceAdventure are not accessible outside of the adventure
        * It is recommended to instead define the ENTITIES that an adventure uses in a dedicated UNID library instead
        * An UNID library is an otherwise empty TranscendenceLibrary that exists purely to offer up a list of entities
    * Types defined in a TranscendenceAdventure are only accessible if the adventure is selected
    * Contains one or more modules
    * Must be assigned an UNID
    * Must contain a `<AdventureDesc>`
* TranscendenceExtension
    * ENTITYs defined in a TranscendenceExtension are not accessible outside of the extension
        * It is recommended to instead define the ENTITIES that an adventure uses in a dedicated UNID library instead
    * Types defined in a TranscendenceExtension are only accessible if that extension is enabled in the current adventure
    * Contains one or more modules
    * Must be assigned an UNID
* TranscendenceModule
    * ENTITYs defined in a TranscendenceModule are not accessible outside of the extension
    * TranscendenceModules cannot exist independently
    * Types defined in a TranscendenceModule are only accessible if their parent library/adventure/extension is loaded
    * Cannot be assigned an UNID

## Entities

Transcendence supports the following XML entity types:
* Internal entities
    * These allow you to define programmer-friendly names to reusable
    values such as hexidecimal Type UNIDs or color codes.
    * Internal entities are most frequently used for storing UNIDs
    * UNIDs themselves are unsigned 32-bit integers typically represented as
    hexadecimal values. Transcendence specifically uses the `0x` prefix style
    for denoting hexadecimal values, as opposed to the `h` suffix.
* Pre-defined Internal entities
    These allow you to escape special symbols that the XML parser
    will otherwise attempt to parse as part of the XML.
    * `&amp;` --> `&`
    * `&gt;` --> `>`
    * `&ls;` --> `<`

The entities in the DocType Data (DTD) section are replced in the XML
document itself after the XML structure is parsed, but before the game
data is loaded.

# DOCUMENT TAGS

The following are the document tag types that Transcendence will recognize.

## `<TranscendenceUniverse>`

### Attributes
* apiVersion: the API version of this TranscendenceUniverse. It MUST match the API version of transcendence.exe.

## `<CoreLibrary>`

### Attributes
None

## `<TranscendenceLibrary>`

### Attributes
* UNID: the UNID of this library
* apiVersion: the API version that this library requires and should be parsed with. Must be 12 or greater (see `version` for legacy support)
* autoInclude: (optional) always included if valid to include
* autoIncludeForCompatibility: (optional) an API version at or below which this library is auto-included
* coverImageID: (recommended) UNID of a cover image to display for this library in the collections menu
* credits: (recommended) a string specifying the credits text to display on the credits scroll in the main menu. Use `;` to separate lines.
* debugOnly: (optional) "true" if this library is only able to be enabled when the game is run with /debug
* name: (recommended) the name to display for this library. If not provided, it displays the UNID number.
* obsoleteVersion: (optional) this library cannot be enabled if the loaded adventure is above this API version
* release: (optional) the release number of this library (must be an int)
* version: (recommended) a custom version string for this library
    * Note: prior to API version 12 when apiVersion was introduced, version was instead used to indicate the game version which was translated (roughly) to API version

## `<TranscendenceAdventure>`

### Attributes
* UNID: the UNID of this adventure
* apiVersion: the API version that this adventure requires and should be parsed with. Must be 12 or greater (see `version` for legacy support)
* coverImageID: (recommended) UNID of a cover image to display for this adventure in the collections menu
* credits: (recommended) a string specifying the credits text to display on the credits scroll in the main menu. Use `;` to separate lines.
* debugOnly: (optional) "true" if this adventure is only able to be enabled when the game is run with /debug
* extensionAPIVersion: (optional) prevents loading extensions below this version
* name: (recommended) the name to display for this adventure. If not provided, it displays the UNID number.
* obsoleteVersion: (optional) this adventure cannot be enabled if the loaded adventure is above this API version
* release: (optional) the release number of this adventure (must be an int)
* version: (recommended) a custom version string for this adventure
    * Note: prior to API version 12 when apiVersion was introduced, version was instead used to indicate the game version which was translated (roughly) to API version

## `<TranscendenceExtension>`

### Attributes
* UNID: the UNID of this extension
* apiVersion: the API version that this extension requires and should be parsed with. Must be 12 or greater (see `version` for legacy support)
* autoInclude: (optional) always included if valid to include
* autoIncludeForCompatibility: (optional) an API version at or below which this extension is auto-included
* coverImageID: (recommended) UNID of a cover image to display for this extension in the collections menu
* credits: (recommended) a string specifying the credits text to display on the credits scroll in the main menu. Use `;` to separate lines.
* debugOnly: (optional) "true" if this extension is only able to be enabled when the game is run with /debug
* extends: (optional) a list of UNIDs separated by spaces that this extension is allowed to be used with
* hidden: (optional) if this is only available for compatibility purposes
* name: (recommended) the name to display for this extension. If not provided, it displays the UNID number.
* obsoleteVersion: (optional) this extension cannot be enabled if the loaded adventure is above this API version
* private: (optional) does not show up in stats (libaries are always private)
* release: (optional) the release number of this extension (must be an int)
* version: (recommended) a custom version string for this extension
    * Note: prior to API version 12 when apiVersion was introduced, version was instead used to indicate the game version which was translated (roughly) to API version

## `<TranscendenceModule>`

### Attributes
None

# TOP LEVEL TAGS

The following tags are available at the outermost nesting level of a
Transcendence document

## `<CoreLibrary>`

`<CoreLibrary>` tag defines the core libaries that must be compiled with the TranscendenceUniverse into Transcendence.tdb
This tag is only available in a TranscendenceUniverse.

### Attributes
* Filename: the relative path to the Library
    * NOTE - it is not recommended to nest Libraries due to bugs with handling subdirectories
### Inner Text
None

## `<TranscendenceAdventure>`

`<TranscendenceAdventure>` tag defines the adventures that must be compiled with the TranscendenceUniverse into Transcendence.tdb
This tag is only available in a TranscendenceUniverse.

### Attributes
* Filename: the relative path to the Adventure
    * NOTE - it is not recommended to nest Adventures due to bugs with handling subdirectories
### Inner Text
None

## `<Library>`

`<Library>` tags define the libraries that this Library/Adventure/Extension depends on

### Attributes
* UNID: the UNID of a library dependency for this Library/Adventure/Extension
* type: (optional) the type of dependency
    * required: (default) a load error will occur if this dependency cannot be loaded
    * dependency: this library will be disabled if this dependency cannot be loaded
    * optional: this library will attempt to load the target library, but will ignore it if it does not exist
### Inner Text
None

## `<Module>`

`<Modules>` tags define the modules that are loaded as a part of this Universe/Library/Adventure/Extension
Modules are included in the output tdb when compiling with transcompiler.exe

### Attributes
* Filename: the relative path to the module
    * NOTE - it is not recommended to nest modules due to bugs with handling subdirectories
### Inner Text
None

## `<Globals>`

`<Globals>` tags are designed to contain tlisp in their inner text.

### Attributes
None
### Inner Text
Parsed as tlisp script.

## Types
See the Transcencendence XML Types for more details

# TRANSCENDENCE XML TYPES

These are the types that Transcendence supports. All types must be given an UNID.

## GENERIC TYPE: `<Type>`

Generic types are the basic type.
They cannot be instanced, but they can be inherited by types that can be instanced.
All other types can use the attributes and inner tags available to generic types.

### Attributes
* UNID: the UNID of this type. ALL types (including non-generic types) must specify this field.
* attributes: (optional) a list of attribute strings that can be used to filter for this type using `(typFind ...)` or `(typMatches ...)`
* excludes: (optional) this type is not loaded if any extension UNID in this list of extension UNIDs is present
* extends: (optional) this type is only loaded if at least one extension UNID in this list of extension UNIDs is present
* inherit: (optional) the UNID of a type to inherit all inner tags from
* obsoleteVersion: (optional) this type is only loaded if the adventure API version is below this API
* requiredVersion: (optional) this type is only loaded if the adventure API version is at this this API
* modifiers: (deprecated) use attributes instead.
* obsolete: (deprecated) use obsoleteVersion instead.

### Inner Text
None

### `<Language>`

The language tag specifies text that can be retrieved with `(typTranslate ...)`

This tag includes individual text tags inside of it. Each individual text tag must be given an attribute called `id` which has a string name value.

Individual text tags can be of the following types:
* `<Text>`
    * If the inner text is encapsulated in parenthesis `()` it will be parsed as tlisp.
* `<RTF>`

If the inner text is empty, it will be loaded as an empty string.

### `<Properties>`

The properties tag specifies named data that can be retrieved with `(typ@ ...)`. Types that can be instaned may have special accessors for instanced data, such as `(obj@ ...)` for space objects (instanced ShipClasses and StationTypes).

This tag includes individual property tags inside of it. Each individual property tag must be given an attribute called `id` which has a string name value.

Individual propreties can be of the following types as specified by their tags:
* `<Definition>`: Global, Evaluated on first type bind
* `<Constant>`: Global, Evaluated on every type bind
* `<Global>`: Global, Evaluated after type fully bound
* `<Variant>`: Unique per instance, Evaluated on instance creation
* `<Data>`: Unique per instance, Evaluated on instance creation
* `<DynamicGlobal>`: Global, Evaluated on access
* `<DynamicData>`: Unique per instance, Evaluated on access

If the inner text of an individual property is encapsulated in parenthesis `()` it will be parsed as tlisp.
If the inner text of an individual property is encapsulated in doublequotes `"` it will be parsed as a string.
Otherwise the engine will attempt to load it as an int, then a double, and lastly a string.
If it is empty, it will be loaded as a tlisp `Nil`

Data that is unique per instance cannot be accessed from the type, nor can it be used by a non-instanceable type such as a genertic `<Type>`, but it can still be inherited.

### `<Events>`

### `<DockScreens>`

### `<ItemEncounterDesc>`

### `<AttributeDesc>`

### `<ArmorClassDesc>`
(Available in API59)

### `<Achievements>`
Defines multiverse and steam achievements. Can only be set by types in a `<TranscendenceAdventure>`

### `<GlobalData>`
Deprecated. Use `<Properties>` instead.

### `<StaticData>`
Deprecated. Use `<Properties>` instead.

### `<DisplayAttributes>`
Deprecated. Use `<AttributeDesc>` (static) or `<Events><GetDisplayAttributes>` (evaluate on access) instead.

### `<ArmorMassDesc>`
Deprecated (as of API59). Use `<ArmorClassDesc>` instead.

## ADVENTURE: `<AdventureDesc>`

## IMAGE RESOURCES: `<ImageType>`

### Attributes
* bitmap: file path for a .bmp .jpg or .png file to use as a bitmap (color data). .png files will also load their alpha channel unless overridden by an explicit bitmask.
* bitmask: (optional) file path for a .bmp or .png file to use as a bitmask. .png files use the alpha channel for bitmask by default.
* noPM: (optional) false by default. Specifies that the bitmap has not been premultiplied.

### Inner Text
None

## ITEMS: `<ItemType>`
* name: name of the item
* level: level of the item

## SHIPS: `<ShipClass>`

### Attributes
* class: (recommended) name of the ship class
* type: (recommended) type of the ship class
* manufacturer: (recommended) name of the ship manufacturer
* achievement: (optional) if this ship class' destruction qualifies for an achievement
* level: (optional) level of the ship (will be automatically computed)
* virtual: (optional) if this type is virtual.
* shipCompartment: (optional) if this ship class defines an attached compartment of another ship (as seen on the Balin Dragon in Vault of the Galaxy) as opposed to being a standalone ship
* score: (optional) overrides the autocomputed score for this ship class
* eventHandler: (optional) UNID of event handler type to inherit additional events and data from. Typically used for custom AI.
* character: (optional) UNID of character
* characterClass: (optional, to be deprecated) UNID of player's character class when the player starts in this ship
* dockSCreen: (optional) dockscreen to initialize when player docks at a ship of this ship class
* defaultBackgroundID: (optional) UNID of background image for this ship's dockscreen

### `<Hull>`
Defines basic stats about this ship.

### `<Interior>`
Recommended:
Use on large ships that should not die from a single hull breach.
Defines internal compartments of this ship.

### `<Names>`
Optional
A list of names that can be randomly assigned to this ship class.
Typically this should not be used for stock hulls.

### `<Image>`
Recommended
Defines the sprite information for this ship class.

### `<HeroImage>`
Recommended
Defines the menu picture for this ship class.

### `<Devices>`

### `<DeviceSlots>`

### `<Armor>`

### `<Effects>`

### `<Items>`

### `<Equipment>`

### `<DriveImages>`

### `<Escorts>`

### `<Trade>`

### `<Communications>`

### `<InitialData>`

### `<PlayerSettings>`

## STATIONS: `<StationType>`

## STAR SYSTEM MAP/NETWORK: `<SystemMap>`
