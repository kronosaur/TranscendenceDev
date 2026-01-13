# XML

Transcendence stores much of its game data in XML.

## CONTRIBUTION GUIDE

There are two options for developing patches to XML:

1. Build the entire game from source and it will use the XML in `TranscendenceDev/Transcendence/TransCore`
    * This is the most simple solution if you can build the executables from source
2. Use an external copy of Transcompiler to build Transcendence.tdb and apply it to an external copy of Transcendence
    * This is a more limited solution but allows people to contribute patches to the gamedata without needing a setup able to compile the game from source
    * It only works if the branch you are patching is on the same API version as the current public builds of Transcendence
    1. Download https://downloads.kronosaur.com/TransData.zip
    2. Extract TransData.zip: inside is TransData.exe and TransCompiler.exe
    3. Copy TransCompiler.exe to `TranscendenceDev/Transcendence/Game`
    4. In that same folder there is a bat file called `compileCore.bat`. Execute this batch script.
    5. This will create a new file called `transcendence.tdb`
    6. If you have not done so already download and extract https://downloads.kronosaur.com/TranscendenceNext.zip (this link has the latest alpha or beta version)
        * Backup the `transcendence.tdb` in this folder by naming it `transcendence.tdb.{version}`
    7. Copy `transcendence.tdb` from `TranscendenceDev/Transcendence/Game` into the extracted TranscendenceNext folder from step 6
    8. You can now start Transcendence.exe in that folder and it will attempt to load your new version of `transcendence.tdb`
        * This lets you test your patch before submitting it

## TRANSCENDENCE XML FILES

## TRANSCENDENCE XML TYPES
