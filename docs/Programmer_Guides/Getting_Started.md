## CONTRIBUTING WITH GIT

If you wish to contribute patches to the game you will need to use git.

1. Create a fork of the TranscendenceDev repo
2. Clone your fork to your harddrive

If you are downloading the repos in order to submit future pull requests, it is 
worthwhile to keep repositories synced with kronosaur:

[How to sync a fork on github](https://help.github.com/articles/syncing-a-fork/)

General help on using git is available through official sources:

[Github](https://help.github.com)

Once you have made a patch you can make a pull request against the official
repo.

# BUILDING FROM SOURCE

You may build your own copies of the game from source. These versions will not
have the code to access the Kronosaur Multiverse however, but you can still
use one of the Kronosaur-provided binaries to access the multiverse.

## DEVELOPMENT ENVIRONMENT SETUP

You will need to install the February 2010 DirectX SDK release, as later versions 
do not include necessary libraries. The correct DirectX SDK can be downloaded 
here: 

[Archive.org - 2010 DirectX SDK](https://archive.org/details/dxsdk_feb10)

Note the DirectX SDK's install location on your computer.

It may be necessary to uninstall certain Microsoft Visual C++ Redistributables 
when installing DirectX SDKs:

[MSVC++ Redistributable Requirements](http://support.microsoft.com/kb/2728613)

Use Microsoft Visual Studio 2022 or later. Open `File > Open > Project/Solution` 
`<Repo Root>/Transcendence/Transcendence.sln`, which is the Transcendence 
solution file. The following warnings, if shown under `Output` from Solution, 
may be safely ignored:

    <Repo Root>\Alchemy\zlib-1.2.7\contrib\vstudio\vc10\zlibstat.vcxproj : 
    warning  : Platform 'Itanium' referenced in the project file 'zlibstat' 
    cannot be found.

    <Repo Root>\TransCore\TransCore.vcxproj : error  : Project 
    "...\TransCore\TransCore.vcxproj" could not be found.

Then go to `View>Solution Explorer` and check the pane opened to the right side.
Right click `Transcendence` and select `Set as Startup Project`. `Transcendence`
should now be bolded in the Solution Explorer. 

Ensure that the correct locations of the DirectX SDK Include and Lib folders 
are specified for the Transcendence project (under the Transcendence solution) 
by right clicking `Transcendence` and selecting `Properties` to open the 
project's Properties page, and checking the following property sets:

    Configuration Properties > VC++ Directories > General > Include Directories
    Configuration Properties > VC++ Directories > General > Library Directories
    
Always point the Library Directories variable to the \Lib\x86 folder of the SDK.

In the second from the top ribbon, there is a `Debug` dropdown. Select `Debug for
Contributors` and change the dropdown next to it from `Any CPU` to `Win32`.
Build the solution. Executables will be placed in the Transcendence/Game 
directory. These can be viewed from the file system.

Now the game can optionally be launched from Visual Studio with the `Local Windows
Debugger` in the second from the top ribbon.

For security reasons the source code does not include certain files to 
communicate with the Hexarc arcology (the cloud service). The code will compile
without it, however, as long as you add CHexarcServiceStub.cpp to the build.

In Visual Studio, selecting the "Debug For Contributors" configuration will
build the game with CHexarcServiceStub.cpp.

## Recommended Editor Configuration

To enable whitesmith indentation style in Visual Studio, open `Tools > Options`:

Then select `Text Editor > C/C++ > Tabs` and ensure that:

    Tab size = 4
    Indent size = 4
    Keep Tabs = True

Then select `Text Editor > C/C++ > Code Style > Formatting > Indentation` and
ensure that:

    Indent braces = True
    Indent each line relatively to = `Innermost paranthesis`
    Within parenthesis, align new lines when I type them = `Indent new lines`
    Indent case contents = True
    Indent braces following a case statement = True
    Indent braces of lambdas = True
    
Then select `Text Editor > C/C++ > Code Style > Formatting > Spacing` and ensure
that:

    `Spacing for operators > Pointer/reference Alignment > Leave unchanged` = True

If you find additional settings that should be changed to provide for an easier
editing experience, please update this readme with them.

# DEVELOPMENT ENVIRONMENT FOR GAME DATA ONLY

It is possible to use a precompiled version of Transcendence to develop game
data patches without needing a full development environment. This is useful if
your computer is unable to run Visual Studio but you still want to contribute
patches to the game's XML.

Be aware that:
* This is a more limited solution but allows people to contribute patches to
    the gamedata without needing a setup able to compile the game from source
* It only works if the branch you are patching is on the same API version as
    the current public builds of Transcendence

## GAME DATA DEV ENVIRONMENT SETUP

1. Download https://downloads.kronosaur.com/TranscendenceNext.zip
(this link has the latest alpha or beta version)
2. Extract TranscendenceNext.zip
3. Copy Transcendence.exe to `TranscendenceDev/Transcendence/Game`
4. You can now run the game and it will use the XML in `TranscendenceDev/Transcendence/TransCore`

Optionally you can configure the game to search an existing install of Transcendence's
Collection and Extension folders. To do this:

1. Create a folder `Collection` in `TranscendenceDev/Transcendence/Game`
2. Create a folder `Extensions` in `TranscendenceDev/Transcendence/Game`
3. Create a symlink inside `TranscendenceDev/Transcendence/Game/Collection` to
the `Collection` folder of the existing Transcendence installation
    * `mklink /D "{location of repo}/TranscendenceDev/Transcendence/Game/Collection/ExternalCollection" "{location of external transcendence}/Transcendence/Collection"`
        * Replace the text in `{}` with appropriate file paths.
        * An admin cmd prompt should not be necessary unless the folder permissions require it.
4. Create a symlink inside `TranscendenceDev/Transcendence/Game/Extensions` to
the `Extensions` folder of the existing Transcendence installation
    * `mklink /D "{location of repo}/TranscendenceDev/Transcendence/Game/Extensions/ExternalExtensions" "{location of external transcendence}/Transcendence/Extensions"`

Notes:
* For windows a basic `.lnk` shortcut is not yet supported.
* This has not been tested on Linux but it should still work
* If the game says that the API version of the Transcendence Universe is too high you need
    to download a new copy of TranscendenceNext.zip and follow the setup instructions again
    to update it.
