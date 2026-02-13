# XML

Transcendence stores much of its game data in a customized XML format.

## ABOUT THIS DOCUMENT

Be aware that this document is meant to reflect the current API, but is at this time manually updated.

> **NOTE**
>
> Deprecated features/attributes/tags/etc are not documented here unless otherwise relevant

> **NOTE**
>
> Some fields may not technically be "required" (they can still be parsed without these fields
> present or correctly populated), however the default behavior is usually intended for
> compatibility with very old mods and is not normally desireable or intended in modern
> transcendence game data.
>
> As such, they are listed as required from the perspective of implementing modern transcendence
> xml.

# TRANSCENDENCE XML FILE STRUCTURE

Transcendence XML files use the following general structure

```xml
<?xml version="1.0" encoding="utf-8"?>

<!DOCTYPE DocumentType
	[
	<!ENTITY entityName                 "entityValue">
	]>

<DocumentType>
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

	<Tag
			attribute=          "value"
			>
		<!-- There are different types of tags that will be explained later in this guide -->

		<!-- Note that this closing tag has a / in front of the tag name -->
	</Tag>

	<!--
	If you dont have inner text, tlisp, or tags, you can use a single tag stype which
	has a / at the end
	-->
	<SingleTag anotherAttribute="value"/>

<\DocumentType>
```

The above example uses a lot of placeholder names, including:
* `DocumentType` - this is replaced with one of the actual [transcendence doctypes](#document-types)
* `entityName` - this is replaced with actual entity names, which are usually alises for actual UNID numbers like `itLaserCannon`
* `Tag` - this is replaced with a valid tag name for its context: see [top level tags](#top-level-tags) and [types](#types)
* `attribute` - this is replaced with the actual attribute name you want to assign a value to. Most attributes are optional. See the tag's section for more information.
* `SingleTag` - see `Tag` above.
* `anotherAttribute` - see `attribute` above.

The occurences of `DocumentType` is replaced with the actual document types listed below:

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
- **UNID**:

	the UNID of this library

- **apiVersion**:

	the API version that this library requires and should be parsed with. Must be 12 or greater (see `version` for legacy support)

- **autoInclude**: (optional)

	always included if valid to include

- **autoIncludeForCompatibility**: (optional)

	an API version at or below which this library is auto-included

- **coverImageID**: (recommended)

	UNID of a cover image to display for this library in the collections menu

- **credits**: (recommended)

	a string specifying the credits text to display on the credits scroll in the main menu. Use `;` to separate lines.

- **debugOnly**: (optional)

	"true" if this library is only able to be enabled when the game is run with /debug
	
- **name**: (recommended)

	the name to display for this library. If not provided, it displays the UNID number.

- **obsoleteVersion**: (optional)

	this library cannot be enabled if the loaded adventure is above this API version

- **release**: (optional)

	the release number of this library (must be an int)

- **version**: (recommended)

	a custom version string for this library
	> [!Note]
	>
	> prior to API version 12 when apiVersion was introduced, version was instead used to indicate the game version which was translated (roughly) to API version

## `<TranscendenceAdventure>`

### Attributes
- **UNID**:

	the UNID of this adventure

- **apiVersion**:

	the API version that this adventure requires and should be parsed with. Must be 12 or greater (see `version` for legacy support)

- **coverImageID**: (recommended)

	UNID of a cover image to display for this adventure in the collections menu

- **credits**: (recommended)

	a string specifying the credits text to display on the credits scroll in the main menu. Use `;` to separate lines.

- **debugOnly**: (optional)

	"true" if this adventure is only able to be enabled when the game is run with /debug

- **extensionAPIVersion**: (optional)

	prevents loading extensions below this version

- **name**: (recommended)

	the name to display for this adventure. If not provided, it displays the UNID number.

- **obsoleteVersion**: (optional)

	this adventure cannot be enabled if the loaded adventure is above this API version

- **release**: (optional)

	the release number of this adventure (must be an int)

- **version**: (recommended)

	a custom version string for this adventure
	> **Note**
	>
	> prior to API version 12 when apiVersion was introduced, version was instead used to indicate the game version which was translated (roughly) to API version

## `<TranscendenceExtension>`

### Attributes
- **UNID**:

	the UNID of this extension

- **apiVersion**:

	the API version that this extension requires and should be parsed with. Must be 12 or greater (see `version` for legacy support)

- **autoInclude**: (optional)

	always included if valid to include

- **autoIncludeForCompatibility**: (optional)

	an API version at or below which this extension is auto-included

- **coverImageID**: (recommended)

	UNID of a cover image to display for this extension in the collections menu

- **credits**: (recommended)

	a string specifying the credits text to display on the credits scroll in the main menu. Use `;` to separate lines.

- **debugOnly**: (optional)

	"true" if this extension is only able to be enabled when the game is run with /debug

- **extends**: (optional)

	a list of UNIDs separated by spaces that this extension is allowed to be used with

- **hidden**: (optional)

	if this is only available for compatibility purposes

- **name**: (recommended)

	the name to display for this extension. If not provided, it displays the UNID number.

- **obsoleteVersion**: (optional)

	this extension cannot be enabled if the loaded adventure is above this API version

- **private**: (optional)

	does not show up in stats (libaries are always private)

- **release**: (optional)

	the release number of this extension (must be an int)

- **version**: (recommended)

	a custom version string for this extension

	> **Note**
	>
	> prior to API version 12 when apiVersion was introduced, version was instead used to indicate the game version which was translated (roughly) to API version

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
- **Filename**:

	the relative path to the Library

	> **NOTE**
	>
	> it is not recommended to nest Libraries due to bugs with handling subdirectories

### Inner Text
None

## `<TranscendenceAdventure>`

`<TranscendenceAdventure>` tag defines the adventures that must be compiled with the TranscendenceUniverse into Transcendence.tdb
This tag is only available in a TranscendenceUniverse.

### Attributes
- **Filename**: the relative path to the Adventure

	> **NOTE**
	>
	> it is not recommended to nest Adventures due to bugs with handling subdirectories

### Inner Text
None

## `<Library>`

`<Library>` tags define the libraries that this Library/Adventure/Extension depends on

### Attributes
- **UNID**:
	
	the UNID of a library dependency for this Library/Adventure/Extension

- **type**: (optional)

	the type of dependency

	- **required**: (default)

		a load error will occur if this dependency cannot be loaded

	- **dependency**:
	
		this library will be disabled if this dependency cannot be loaded

	- **optional**:
	
		this library will attempt to load the target library, but will ignore it if it does not exist

### Inner Text
None

## `<Module>`

`<Modules>` tags define the modules that are loaded as a part of this Universe/Library/Adventure/Extension
Modules are included in the output tdb when compiling with transcompiler.exe

### Attributes
- **Filename**: the relative path to the module
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
- **UNID**:
	
	the UNID of this type. ALL types (including non-generic types) must specify this field.

- **attributes**: (optional)

	a list of attribute strings that can be used to filter for this type using `(typFind ...)` or `(typMatches ...)`

- **excludes**: (optional)

	this type is not loaded if any extension UNID in this list of extension UNIDs is present

- **extends**: (optional)

	this type is only loaded if at least one extension UNID in this list of extension UNIDs is present

- **inherit**: (optional)

	the UNID of a type to inherit all inner tags from

- **obsoleteVersion**: (optional)

	this type is only loaded if the adventure API version is below this API

- **requiredVersion**: (optional)

	this type is only loaded if the adventure API version is at this this API

- **modifiers**: (deprecated)

	use attributes instead.

- **obsolete**: (deprecated)

	use obsoleteVersion instead.

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

- **bitmap**:

	file path for a .bmp .jpg or .png file to use as a bitmap (color data). .png files will also load their alpha channel unless overridden by an explicit bitmask.

	> **NOTE**
	>
	> it is not recommended to nest Libraries due to bugs with handling subdirectories

- **bitmask**: (optional)

	file path for a .bmp or .png file to use as a bitmask. .png files use the alpha channel for bitmask by default.

	> **NOTE**
	>
	> it is not recommended to nest Libraries due to bugs with handling subdirectories

- **noPM**: (optional)

	false by default. Specifies that the bitmap has not been premultiplied.

### Inner Text

None

## ITEMS: `<ItemType>`
- **name**:

	name of the item

- **level**:

	level of the item

## SHIPS: `<ShipClass>`

### Attributes

- **class**: (recommended)

	name of the ship class

- **type**: (recommended)

	type of the ship class

- **manufacturer**: (recommended)

	name of the ship manufacturer

- **achievement**: (optional)

	if this ship class' destruction qualifies for an achievement

- **level**: (optional)

	level of the ship (will be automatically computed)

- **virtual**: (optional)

	if this type is virtual.

- **shipCompartment**: (optional)

	if this ship class defines an attached compartment of another ship (as seen on the Balin Dragon in Vault of the Galaxy)
	as opposed to being a standalone ship

- **score**: (optional)

	overrides the autocomputed score for this ship class

- **eventHandler**: (optional)

	UNID of event handler type to inherit additional events and data from. Typically used for custom AI.

- **character**: (optional)

	UNID of character

- **characterClass**: (optional, to be deprecated)

	UNID of player's character class when the player starts in this ship

- **dockSCreen**: (optional)

	dockscreen to initialize when player docks at a ship of this ship class

- **defaultBackgroundID**: (optional)

	UNID of background image for this ship's dockscreen

Due to practical implementation constraints around reusing a single hull for
different purposes, we have a notion of a 'stock' ship hull vs ship hulls that
may be named, given alterantive equipment loadouts, etc.

The stock hull is meant to represent a factory fresh ship with the sort of
loadout that the manufacturer would market it as having.

With few exceptions, only the stock hull should define the following tags:
* Hull
* Interior
* DeviceSlots
* Effects
* Image
* HeroImage

Additionally the stock hull should define the following tags:
* Armor (Required)
* Devices (for default devices)
* Equipment (for default ship software)

Optionally the stock hull can define the following tags
* Items (default items in cargohold)

Variant hulls should inherit the stock hull (or another variant hull) to get
the data for those tags.

Variant hulls may define the following tags:
* Armor (if they change the armor type installed. Do not change the armor count. Armor count in this XML element is being deprecated.)
* Devices (if they change the devices)
* Items (items in the cargohold)
* Names
* PlayerSettings (if this variant is a starting playership)

### `<Hull>`

Required (Stock hull only)

Defines basic stats about this ship.

#### Attributes
	
- **mass**: (required)

	mass of the ship in metric tons
	
- **cargoSpace**: (recommended)

	the cargo capacity of this ship in CBM (cubic meters)

	> **NOTE**
	>
	> Before API 59, this specified metric tons.
	> This is converted at a 1:1 ratio by default unless an adventure specifies otherwise

- **size**: (recommended)

	longest axis of the ship in meters

- **maxCargoSpace**: (recommended)

	the cargo capacity of this ship in CBM (cubic meters)

	> **NOTE**
	>
	> Before API 59, this specified metric tons.
	> This is converted at a 1:1 ratio by default unless an adventure specifies otherwise

- **maxDevices**: (required)

	maximum devices that can be installed

	> **NOTE**
	>
	> Until the device slot rework is in, this is unfortunately a required field.
	> It will be optional at that point, as the game will be able to auto-compute
	> max devices based on the named slots.
	> It will persist as a non-deprecated field to allow for niche ships that
	> cannot use all of their named slots simultaneously.

- **maxWeapons**: (optional)

	maximum weapon-class devices that can be installed

- **maxNonWeapons**: (optional)

	maximum devices that can be installed which are not of the weapon-class

- **maxArmor**: (optional, not recommended)

	maximum armor class that can be installed
	
	It is recommended to use `<ArmorLimits>` instead for explicit control
	over the speed bonus/penalty that is applied. The game will otherwise
	attempt to auto-compute bonuses/penalties.

- **stdArmor**: (optional, not recommended)

	standard armor class that can be installed (where there is no bonus
	or penalty)
	
	It is recommended to use `<ArmorLimits>` instead for explicit control
	over the speed bonus/penalty that is applied. The game will otherwise
	attempt to auto-compute bonuses/penalties.

#### Inner Tags

- `<ArmorLimits>`

	This tag defines an explicit armor limit and speed bonus/penalty

	**Attributes**

	- **massClass**: (required)

		The armor class that this speed bonus/penalty is applied to

		> **NOTE**
		>
		> in API59 this attribute name will be deprecated, and switched to `armorClass`

	- **speedAdj**: (required)

		The speed bonus or penalty applied to this ship in increments of 0.01c (1% of lightspeed)

	**Example**    
	
	The following series of `<ArmorLimits>` defines a ship that gets:
	* +.02c from ultralight armor
	* +.01c from light armor
	* no bonus from medium armor
	* treats heavy armor as standard (highest armor class without a penalty)
	* -.01c from super-heavy armor, which is also its maximum armor as the highest class listed

	```
	<ArmorLimits massClass="ultraLight"     speedAdj="2"/>
	<ArmorLimits massClass="light"          speedAdj="1"/>
	<ArmorLimits massClass="medium"         speedAdj="0"/>
	<ArmorLimits massClass="heavy"          speedAdj="0"/>
	<ArmorLimits massClass="superHeavy"     speedAdj="-1"/>
	```

### `<Interior>`

Recommended (Stock hull only)
Use on large ships that should not die from a single hull breach.

Defines internal compartments of this ship.

### `<Maneuver>`

Required (Stock hull only)

#### Attributes

- **maxRotationRate** (required)

	Accepts floating point numbers.
	The number of degrees per 2 simulation seconds that the ship is able to rotate

- **rotationAccel** (optional)

	The maximum increase in rotation rate per 2 simulation seconds
	Accepts floating point numbers.

	> **WARNING**
	>
	> If this value is less than half of maxRotationRate, it can cause issues with the AI steering

- **rotationStopAccel** (optional)

	The maximum decrease in rotation rate per 2 simulation seconds
	Accepts floating point numbers.

	> **WARNING**
	>
	> If this value is less than half of maxRotationRate, it can cause issues with the AI steering
	> In particular this can cause major issues with oversteering

### `<Drive>`

Required (Stock hull only)

#### Attributes

- **maxSpeed** (required)

	Maximum velocity in increments of .01c (1% of lightspeed)

- **thrust** (required)

	Thrust
	(a thrust ratio of around 1 can be achieved by setting thrust to the same value as mass. Note that equipment mass also factors into this.)

- **powerUseRatio** (recommended)

	Autocalculates a power use based on the value in thrust.
	The formula is: `13 * powerUseRatio * (thrust / 100) ^ 1.2 = powerUse (as a multiple of 100kW)`
	Accepts floating point numbers.

- **powerUse** (optional)

	Forces a specific power use. (Specified in whole multiples of 100kW)

### `<Image>`

Required (Stock hull only)

Defines the sprite information for this ship class.
Will be invisible, unhittable, and non-moving if not defined.
Virtual shipclasses do not directly use this, as their instances are not spawned into the world.
Inheriting a virtual shipclass that defines `<Image>` will allow that image definition to be used.
Image coordinates start at `(0,0)` from the top left of the image.

#### Attributes

- **imageWidth** (required)

	Number of pixels wide an individual facing of this sprite should be

- **imageHeight** (required)

	Number of pixels tall an individual facing of this sprite should be

- **rotationCount** (required)

	Total number of facings that are available for this sprite

- **rotationColums** (required)

	Total number of columns that the rotation facings are split up across

- **viewportRatio** (required)

	Viewport ratio that this sprite was rendered with to ensure that the weapon
	and engine effects can be lined up appropriately with the sprite as the ship
	turns.

	This assumes a specific camera angle that transcendence is designed to work with.

	If the scene is setup with:
	* the camera `N` units behind the origin
	* the camera `N*2` units above the origin
	* the model rotates around the origin

	The viewport ratio is `W/(N*2)` where `W` is the width of the viewport in the
	scene file's units.
	
	If using Arisaya's default blender scene, this value is always `0.2`, because the
	scene is setup to have a camera with fixed lens properties moved closer or farther
	from the origin to fit the ship.

	A value of `0` means that the ship was rendered with isometric projection.

- **imageX** (optional)

	Number of pixels to the right of the leftmost pixel of the image the upper left corner of the sprite's 0 facing should be
	Defaults to 0

- **imageY** (optional)

	Number of pixels below the topmost pixel of the image the upper left corner of the sprite's 0 facing should be
	Defaults to 0

### `<HeroImage>`

Recommended (Stock hull only)

Defines the menu picture for this ship class.
Image coordinates start at `(0,0)` from the top left of the image.

#### Attributes

- **imageWidth** (required)

	Number of pixels wide this sprite should be

- **imageHeight** (required)

	Number of pixels tall this sprite should be

- **imageX** (optional)

	Number of pixels to the right of the leftmost pixel of the image the upper left corner of the sprite should be
	Defaults to 0

- **imageY** (optional)

	Number of pixels below the topmost pixel of the image the upper left corner of the sprite should be
	Defaults to 0

### `<DeviceSlots>`

Required (Stock hull only)
This should be defined on any stock ship.

Defines the device slots installed on this ship.

> **Note**
>
> The legacy slot systems of nameless slots and generic slots (present on legacy ships)
> are being deprecated as part of 2.0
>
> 2.0 will automatically generate and place named slots on legacy ships to support modern
> gameplay elements, but this may result in undesirable slot placement.
>
> Thus it is officially recommended to define all new stock ships hulls with named device
> slots, and is required for newly contributed stock hulls in this repo.

#### Inner Tags

- `<DeviceSlot>` (required)

	This tag defines a device slot on the ship

	**Attributes**

	- **Criteria** (required)
	
		A criteria string that filters what items can be installed in this slot

		Some recommended example values are:
	
		- `w`: any weapon
		- `w~l`: any non-launcher weapon
		- `l`: any launcher
		- `r`: any reactor
		- `s`: any shield
		- `c`: any cargo device
		- `v`: any drive device
		- `b`: any miscellanious device (not one of the prior mentioned categories)
		- `d`: any device
		- `d~w`: any non-weapon device

		Although item criteria technically allows broader criteria that includes non-device
		type items, only devices can actually be installed on a ship.
		
		Certain classes of devices cannot be installed on a ship more than once,
		or if they are forceably installed via script, the extra items will not have any
		effect.

		- shields
		- reactors
		- cargo
		- drive

	- **sID** (required)
	
		A name for this slot

	- **posAngle** (recommended)
	
		An angle in degrees (counter-clockwise) from the front of the ship (0 degrees) that this device is positioned at

	- **posRadius** (recommended)
	
		The number of pixels away from the center of the sprite that this device is positioned

	- **posZ** (optional)
	
		The number of pixels in the vertical access to offset this device's rotation.

		If the device position seems to 'slide out of place' when the ship rotates, it
		means you need to adjust posZ to account for the vertical offset.
		
		This should be used in cases where a device may be mounted above or below the
		Z=0 height of the ship.

		If using Blender or Truespace, this is the same Z-up axis used in this programs

		This is slightly adjusted for camera perspective and uses viewportRatio to
		calculate the exact posiitioning.

	- **fireAngle** (optional)

		For weapons, adjusts the default angle at which this weapon shoots.
		Specified as an angle in degrees (counter-clockwise) from the front of the ship (0 degrees).

	- **fireArc** (optional)

		For weapons, this gives the mounted weapon a swivel with this many degrees of arc,
		centered around `fireAngle`
		
		If a weapon has its own swivel or omnidirectional properties, those take precendence if their
		arc is bigger.

	- **minFireArc** (optional)

		For weapons, this specifies the minimum fire arc that this weapon can fire.
		Specified as an angle in degrees (counter-clockwise) from the front of the ship (0 degrees).
		Must be specified with maxFireArc as well.
		
		Weapon will fire from minFireArc to maxFireArc - if a gun is firing on the wrong side, it probably
		means that you got min and max fire arc reversed.
		
		If a weapon has its own swivel or omnidirectional properties, those take precendence if their
		arc is bigger.

	- **maxFireArc** (optional)

		For weapons, this specifies the maximum fire arc that this weapon can fire.
		Specified as an angle in degrees (counter-clockwise) from the front of the ship (0 degrees).
		Must be specified with minFireArc as well.
		
		If a weapon has its own swivel or omnidirectional properties, those take precendence if their
		arc is bigger.

	- **omnidirectional** (optional)

		For weapons, this specifies that it should be able to fire in any direction.

	- **secondaryWeapon** (optional)

		For weapons, this specifies if the weapon is a secondary weapon. This primarily impacts the AI,
		but for the player it means that it cannot be directly selected.

		For the player, it is recommended to also specify `<EnhancementAbilities>` to go with this.

	- **cannotBeEmpty** (optional)

		This slot must have a device in it at all times. To remove the existing device, a compatible
		device must be installed in its place.

	**Inner Tags**

	- `<EnhancementAbilities>` (optional)

		Enhanccement abilities allow for slot enhancements to be applied to the device in this slot,
		and these can be both buffs or nerfs in order to better balance the slot.

		For example, a slot that grants automatic fire arc should have a nerf for non-omnidirectional
		weapons in it, in order to balance those weapons for the automatic turreting they get.

		**Inner Tags**

		- `<Enhance>` (recommended)

			This tag defines a single enhancement. A slot may have multiple enhancements.

			**Attributes**

			- **type** (required)
			
				An UNID of a virtual device to use as the icon for this enhancement

			- **enhance** (required)

				The enhancement to apply to the device in this slot if it matches the criteria

			- **criteria** (optional)

				A criteria string to check the item against. If the installed item matches
				the criteria, the enhancement will be applied.

				If not supplied, it will always apply the enhancement.

Here is an example of a freighter's device slot definitions.
In this example, the freighter is given:

* 1 main weapon
* 2 secondary turrets that have linked fire, and if they are not omni, have their fire rate reduced to balance them for the advantage of being turreted.
* 1 cargo bay
* 1 shield
* 1 reactor
* 1 drive
* 2 miscellaneous slots

By specifying coordinates for the non-weapon slots, it means they can interact properly with the future physicalized component system
rather than relying on auto-assigned locations which may not make sense.

```xml
<DeviceSlots>
	<DeviceSlot id="main"			criteria="w" posAngle="0" posRadius="52" posZ="4"/>
	<DeviceSlot id="fore_turret"	criteria="w~l" posAngle="0" posRadius="47" posZ="5"	fireAngle="0" fireArc="240" secondaryWeapon="true">
		<EnhancementAbilities>
			<Enhance type=			"&vtTurretSlotWithNonOmni;"
						enhancement=	"-speed:182;"
						criteria=		"w -property:omnidirectional;"
						/>
			<Enhance type=			"&vtAntaresIITurretSlot;"
						enhancement=	"+linkedFire:whenInFireArc;"
						/>
		</EnhancementAbilities>
	</DeviceSlot>
	<DeviceSlot id="aft_turret"		criteria="w~l" posAngle="180" posRadius="47" posZ="7"	fireAngle="180" fireArc="240" secondaryWeapon="true">
		<EnhancementAbilities>
			<Enhance type=			"&vtTurretSlotWithNonOmni;"
						enhancement=	"-speed:182;"
						criteria=		"w -property:omnidirectional;"
						/>
			<Enhance type=			"&vtAntaresIITurretSlot;"
						enhancement=	"+linkedFire:whenInFireArc;"
						/>
		</EnhancementAbilities>
	</DeviceSlot>
	<DeviceSlot id="cargo"			criteria="c" posAngle="0" posRadius="0" posZ="0" cannotBeEmpty="true"/>
	<DeviceSlot id="shield"			criteria="s" posAngle="0" posRadius="45" posZ="0"/>
	<DeviceSlot id="reactor"		criteria="r" posAngle="180" posRadius="42" posZ="0" cannotBeEmpty="true"/>
	<DeviceSlot id="drive"			criteria="v" posAngle="180" posRadius="55" posZ="0" cannotBeEmpty="true"/>
	<DeviceSlot id="misc1"			criteria="b" posAngle="0" posRadius="30" posZ="0"/>
	<DeviceSlot id="misc2"			criteria="b" posAngle="0" posRadius="40" posZ="0"/>
</DeviceSlots>
```

### `<AISettings>`

#### Attributes

- **combatStyle** (recommended)

	Accepts the following combat styles (default is standard)
	* standard
	* advanced (attempts flanking if faster, has a few extra behaviors)
	* flyby (simple non-flanking style style)
	* standoff (simple style that tries to keep range)
	* chase (simple style that tries to stay behind the target)
	* noRetreat (simple aggressive style that attempts flanking if faster)

- **perception** (optional)

	Determines how well a ship type (including the player) can see other ships. 4 = standard

	> **NOTE**
	>
	> This feature will be moved from AI settings to a different tag in 2.0

- **flockingStyle** (optional)

	Accepts the following flocking formations (default is none)
	* cloud
	* compact
	* random
	* none

- **reactToAttack** (optional, avoid on stock)

	How an NPC ship of this type should react to attacks.

	> **NOTE**
	>
	> This overrides the standard reactions provided by orders
	>
	> Do not use this on stock ships, it will interfere with expected
	> behavior

	Accepts the following reactions:
	* chase
	* chaseFromBase
	* default (defers to orders)
	* destroy
	* destroyAndRetaliate
	* deter
	* deterNoManeuvers (only uses secondary weapons to deter)
	* gate (leaves the system)
	* none (does nothing)

- **reactToThreat** (optional, avoid on stock)

	If an NPC ship of this type should react to hostiles (default is)

	> **NOTE**
	>
	> This overrides the standard reactions provided by orders
	>
	> Do not use this on stock ships, it will interfere with expected
	> behavior

	Accepts the following reactions:
	* chase
	* chaseFromBase
	* default (defers to orders)
	* destroy
	* destroyAndRetaliate
	* deter
	* deterNoManeuvers (only uses secondary weapons to deter)
	* gate (leaves the system)
	* none (does nothing)

- **ascendOnGate** (do not use on stock ships, use only on persistent ships)

	If a ship should be ascended instead of deleted when it enters a gate
	
	This is necessary for persistent ships (Ex, named characters, or known fleets
	that can show up elsewhere)

	This should never be used on stock ships, as you can end up accidentally
	ascending (persisting) a large quantity of data over the course of a game.

- **noShieldRetreat** (optional)

	If a ship should not retreat when its shields go down

	Ignored in certain cases (combat styles, non-regenerating shields)

	> **NOTE**
	>
	> This overrides the standard reactions provided by some combatStyles
	>
	> This is overridden by other combat styles which never retreat

- **noDogfights** (optional, avoid on stock)

	Ship does not engage in dogfighting type maneuvers. (Needs to be verified)

	> **NOTE**
	>
	> This overrides the standard reactions provided by orders
	>
	> Do not use this on stock ships, it will interfere with expected
	> behavior

- **nonCombatant** (optional, avoid on stock)

	Ship does not engage in combat.

	> **NOTE**
	>
	> This overrides the standard reactions provided by orders
	>
	> Do not use this on stock ships, it will interfere with expected
	> behavior

- **noFriendlyFire** (optional, avoid on stock)

	Ship cannot hit non-enemy targets unless deliberately targeting them.
	This causes shots to pass through without collision detection.

	> **NOTE**
	>
	> This overrides the standard reactions provided by orders
	>
	> Do not use this on stock ships, it will interfere with expected
	> behavior
	>
	> If set on a station, guards also get this automatically, so generally
	> this is needed only for specific variants that are encountered on their
	> own

- **aggressor** (optional, avoid on stock)

	Ship will aggro on enemy ships nearby

	> **NOTE**
	>
	> This overrides the standard reactions provided by orders
	>
	> Do not use this on stock ships, it will interfere with expected
	> behavior

- **noAttackOnThreat** (optional, avoid on stock)

	Does not attack when threatened

	> **NOTE**
	>
	> This overrides the standard reactions provided by orders
	>
	> Do not use this on stock ships, it will interfere with expected
	> behavior

- **noTargetsOfOpportunity** (optional, avoid on stock)

	Ship will not aggro on targets of opportunity

	> **NOTE**
	>
	> This overrides the standard reactions provided by orders
	>
	> Do not use this on stock ships, it will interfere with expected
	> behavior

- **noFriendlyFireCheckAttrib** (optional, avoid on stock)

	Ship does not check for friendly targets in the way before shooting

	> **NOTE**
	>
	> This overrides the standard reactions provided by orders
	>
	> Do not use this on stock ships, it will interfere with expected
	> behavior

- **noNavPaths** (optional, avoid on stock)

	Ship does not use nav paths and will not avoid enemy stations enroute to
	its destination

	> **NOTE**
	>
	> This overrides the standard reactions provided by orders
	>
	> Do not use this on stock ships, it will interfere with expected
	> behavior

- **noOrderGiver** (optional, avoid on stock)

	This ship is treated as responsible for its own actions, rather than
	an order giver (ex, the player, a station, or a carrier)

	> **NOTE**
	>
	> This overrides the standard ai reactions of other ai
	>
	> Do not use this on stock ships, it will interfere with expected
	> behavior

- **useAllPrimaryWeapons** (optional, avoid on stock, mostly deprecated)

	NPCs of this ship class can link-fire all primary weapons
	This is primarily a legacy hack before linked-fire slots were added

	However it remains a non-deprecated option for some niche applications,
	for example when a named character's ship that doesnt have linked-fire
	slots needs to act like it does.

	> **NOTE**
	>
	> This overrides the standard ship balance of slots
	>
	> Do not use this on stock ships, it will interfere with expected
	> behavior, and its not something the player is allowed to do

- **targetsStations** (optional, avoid on stock)

	Determines if NPCs of this ship class will determine stations owned by
	a hostile sovereign to be threats, even if the station is not attacking them.

	> **NOTE**
	>
	> This overrides the standard reactions provided by orders
	>
	> Do not use this on stock ships, it will interfere with expected
	> behavior

- **fireRangeAdj** (optional, avoid on stock)

	Adjusts what a ship computes is its optimal firing range by this factor
	expressed as a %.

	> **NOTE**
	>
	> This overrides the standard aiming from combat styles
	>
	> Do not use this on stock ships, it will interfere with expected
	> behavior

- **fireRangeAccuracy** (optional, avoid on stock)

	If less than 100, has a chance to just not shoot even if the ship has
	a firing solution.

	This should generally not be used and may be deprecated by upcoming
	crew mechanics.

	> **NOTE**
	>
	> This overrides the standard aiming from combat styles
	>
	> Do not use this on stock ships, it will interfere with expected
	> behavior

- **fireRateAdj** (Do not use: deprecated & buggy)

	Adjusts the fire delay of the weapons on a ship in 10% increments.
	Ex, 9 = 90% of the stock fire delay, meaning that it shoots faster.

	> **NOTE**
	>
	> Do not use this attribute. It is included only to enable people to
	> understand the math conversion needed to migrate off of it.
	>
	> It has bugs that are not easily fixed due to the extremely brute-forced
	> mechanics of its implementation, and introduces many balance problems
	> for adventures and mods that allow the player to acquire NPC ships,
	> as this system only applies to the AI. Additionally, for NPC fuel
	> consumption mechanics, this introduces other problems by drastically
	> altering their efficiency in ways that are not easily fixed/accounted for.
	>
	> Instead, the enhancement system is the intended means for balancing
	> ships with many weapons, and for (de)tuning enemy ships.
	>
	> 1. Slots can be individually given speed buffs or debuffs via enhancements
	>       * This enables powerful auto-turret slots on capital ships to be
	>       selectively detuned based on the benefits given from being an auto-turret
	> 2. A ship-wide 0-slot virtual enhancer that cannot be removed via normal means
	> can bbe used to detune a ship that is supposed to have poorer-than-stock
	> performance.

### `<Armor>`

Required

Defines the armor type and configuration on this ship

> **NOTE**
>
> this system will split out the armorID from
> the rest of the attributes in the near feature

#### Attributes

- **count** (required)

	The number of armor segments that this ship should have

- **armorID** (required)

	The UNID of the armor to install on this ship

- **startAt** (optional)

	The number of degrees offset from the front of the ship (0 degrees) the first armor segment should be

- **fortificationWMDAdj** (optional)

	The amount of WMD fortification that this armor should have.
	Accepts floating point numbers.
	Default: 0

	> **NOTE**
	>
	> This is not a standard feature in the core game, and is being used for
	> balance prototyping in the Chronicles adventure.
	> 
	> Also, A new system called "physicalized damage methods" is in development,
	> and will replace the existing WMD system.

### `<Effects>`

Recommended

Defines the location of engine effects on this ship

#### Inner Tags

- `<Effect>` (required)

	This tag defines a device slot on the ship

	**Attributes**

	- **Type** (required)
	
		should be "thrustMain"
		other effect types are currently not in use

	- **posAngle** (recommended)
	
		An angle in degrees (counter-clockwise) from the front of the ship (0 degrees) that this effect is positioned at

	- **posRadius** (recommended)
	
		The number of pixels away from the center of the sprite that this effect is positioned

	- **posZ** (optional)
	
		The number of pixels in the vertical access to offset this effect's rotation.

		If the effect position seems to 'slide out of place' when the ship rotates, it
		means you need to adjust posZ to account for the vertical offset.
		
		This should be used in cases where an effect may be placed above or below the
		Z=0 height of the ship.

		If using Blender or Truespace, this is the same Z-up axis used in this programs

		This is slightly adjusted for camera perspective and uses viewportRatio to
		calculate the exact posiitioning.

	- **rotation** (optional)

		The angle at which the effect should be emit.
		Specified as an angle in degrees (counter-clockwise) from the front of the ship (0 degrees).

		For engines, thiis is normally 180

	- **effect** (recommended)

		The UNID of the effect to emit.
		
		For Stars of the Pilgrim, some useful values are:

		* `&efMainThrusterDefault;`
		* `&efMainThrusterLarge;`


### `<Devices>`

Recommended

Defines the devices installed on this ship

### `<Names>`

Optional
Typically this should not be used for stock hulls.

A list of names that can be randomly assigned to this ship class.

### `<Items>`

Optional

Allows the inclusion of items in a ship's cargo hold, such as fuel.

### `<Equipment>`

Optional

Allows pre-installed software like a targeting rom.

### `<DriveImages>`

Deprecated

Use `<Effects>` instead

### `<Escorts>`

### `<Trade>`

### `<Communications>`

### `<InitialData>`

### `<PlayerSettings>`

## STATIONS: `<StationType>`

## STAR SYSTEM MAP/NETWORK: `<SystemMap>`
