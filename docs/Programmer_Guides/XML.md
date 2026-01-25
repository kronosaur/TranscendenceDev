# XML

Transcendence stores much of its game data in a customized XML format.

## TRANSCENDENCE XML FILE STRUCTURE

Transcendence XML files use the following general structure

```
<?xml version="1.0" encoding="utf-8"?>

<!DOCTYPE {DocumentType}
	[
	<!ENTITY entityName                 "entityValue">
	]>

<{DocumentType}>
    ;   This is a single line comment
    <!--
        This is a multiline comment
        Nesting them can cause problems, so avoid
        having a multiline comment inside of another
        multiline comment.
    -->
<\{DocumentType}>
```

The `{DocumentType}` is replaced with the actual document types listed below:

### Document Types

There are 6 main types of Transcendence XML documents:
* TranscendenceUniverse
    * ENTITYs and Types defined in a TranscendenceUniverse are globally available
    * Must always be on the same API version as the engine
    * Required in order to start the game
    * Contains one or more CoreLibrary documents
* CoreLibrary
    * ENTITYs and Types defined in a CoreLibrary require the CoreLibrary to be referenced as a dependency
    * Use the same API version as the parent TranscendenceUniverse
    * Cannot exist independently of the TranscendenceUniverse
    * Contains one or more modules
* TranscendenceLibrary
    * ENTITYs and Types defined in a TranscendenceLibrary require the TranscendenceLibrary to be referenced as a dependency
    * Contains one or more modules
* TranscendenceAdventure
    * ENTITYs defined in a TranscendenceAdventure are not accessible outside of the adventure
        * It is recommended to instead define the ENTITIES that an adventure uses in a dedicated UNID library instead
        * An UNID library is an otherwise empty TranscendenceLibrary that exists purely to offer up a list of entities
    * Types defined in a TranscendenceAdventure are only accessible if the adventure is selected
    * Contains one or more modules
* TranscendenceExtension
    * ENTITYs defined in a TranscendenceExtension are not accessible outside of the extension
        * It is recommended to instead define the ENTITIES that an adventure uses in a dedicated UNID library instead
    * Types defined in a TranscendenceExtension are only accessible if that extension is enabled in the current adventure
    * Contains one or more modules
* TranscendenceModule
    * ENTITYs defined in a TranscendenceModule are not accessible outside of the extension
    * TranscendenceModules cannot exist independently
    * Types defined in a TranscendenceModule are only accessible if their parent library/adventure/extension is loaded

### Entities

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

## TOP LEVEL TAGS

The following tags are available at the outermost nesting level of a
Transcendence document

### Library `<Library>`

### Module `<Module>`

### Globals `<Globals>`

### Types
See the Transcencendence XML Types for more details

## TRANSCENDENCE XML TYPES

### GENERIC TYPE `<Type>`

### IMAGE TYPE `<ImageType>`

### ITEM TYPE `<ItemType>`

### SHIP CLASS `<ShipClass>`
