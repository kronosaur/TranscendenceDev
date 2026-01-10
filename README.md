# TRANSCENDENCE SOURCE CODE

Copyright (c) 2003-2026 by Kronosaur Productions, LLC.

https://transcendence.kronosaur.com

transcendence@kronosaur.com

This repo contains the source code for building the Transcendence.exe client,
which is used to run any Transcendence adventure, including **Stars of the Pilgrim**
from the **Domina & Oracus** saga.

Note that there are several other repositories that are relevant to this
project but are not necessary for building it from scratch.

Art Assets: [TransArt](https://github.com/kronosaur/TranscendenceArt)

Official community content and tools:
[Registered Developer Github Organization](https://github.com/TranscendenceRegisteredDevelopment)

## DOWNLOAD & INSTALL

You can choose to clone this repo with git and built from source, or download
a prebuilt binary.

### PREBUILT BINARIES

Prebuilt binaries can be obtained from Kronosaur Productions for free. These
versions are able to access the Kronosaur Multiverse, and download expansions
and mods from the Multiverse.

[Kronosaur Multiverse](https://multiverse.kronosaur.com)

#### STABLE CHANNEL RELEASE

This is the latest stable version of the game.

[Latest Stable Release](https://downloads.kronosaur.com/Transcendence.zip)

#### DEVELOPMENT CHANNEL RELEASE

These are alpha/beta/release candidate releases. If none is available, the download
will simply provide the latest stable version of the game.

[Latest Alpha/Beta Release](https://downloads.kronosaur.com/TranscendenceNext.zip)

#### TRANSDATA

Transdata is a tool for performing various functionality such as decompiling
.tdb files, running diagnostics, running stats/balance tools, and running
tlisp scripts

[Latest Transdata Release](https://downloads.kronosaur.com/TransData.zip)

#### CHANGELOGS

Summarized changelogs are available in the appropriate version's announcement
thread on the Kronosaur Productions forums.

[Announcement Forum](https://forums.kronosaur.com/viewforum.php?f=15)

### BUILDING FROM SOURCE

You may build your own copies of the game from source. These versions will not
have the code to access the Kronosaur Multiverse however, but you can still
use one of the Kronosaur-provided binaries to access the multiverse.

If you are downloading the repos in order to submit future pull requests, it is 
worthwhile to keep repositories synced with kronosaur:

[How to sync a fork on github](https://help.github.com/articles/syncing-a-fork/)

General help on using git is available through official sources:

[Github](https://help.github.com)

You will need to install the February 2010 DirectX SDK release, as later versions 
do not include necessary libraries. The correct DirectX SDK can be downloaded 
here: 

[Archive.org - 2010 DirectX SDK](https://archive.org/details/dxsdk_feb10)

Note the DirectX SDK's install location on your computer.

It may be necessary to uninstall certain Microsoft Visual C++ Redistributables 
when installing DirectX SDKs:

[MSVC++ Redistributable Requirements](http://support.microsoft.com/kb/2728613)

Use Microsoft Visual Studio 2026 or later. Open `File > Open > Project/Solution` 
`<Repo Root>/Transcendence/Transcendence.sln`, which is the Transcendence 
solution file. The following warnings, if shown under `Output` from Solution, 
may be safely ignored:

```
    <Repo Root>\Alchemy\zlib-1.2.7\contrib\vstudio\vc10\zlibstat.vcxproj : 
    warning  : Platform 'Itanium' referenced in the project file 'zlibstat' 
    cannot be found.

    <Repo Root>\TransCore\TransCore.vcxproj : error  : Project 
    "...\TransCore\TransCore.vcxproj" could not be found.
```

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

## CONTRIBUTION GUIDELINES

Please see the contribution guidelines for information about best practices and
style guides.

[General Contribution Guidelines](docs/CONTRIBUTING.md)

Style & Contribution Guidelines:

* [C++ Style Guide](docs/Style_Guides/C++.md)
* [TLisp Style Guide](docs/Style_Guides/TLisp.md)
* [XML Style Guide](docs/Style_Guides/XML.md)
* [Markdown (Documentation) Style Guide](docs/Style_Guides/Markdown.md)
* [In-game Text Style Guide](docs/Style_Guides/Ingame_Text.md)
* [Asset Style Guide](docs/Style_Guides/Assets.md)

### PULL REQUESTS

Please submit pull requests against the appropriate branch - if you have made an
API change, ensure that your pull request is against the appropriate API branch.
For example, if you add a new tlisp function or a new XML attribute field, this
would be considered a new API version, and you should use the appropriate
`integration/API##` branch as the merge target, where `API##` is the next highest
API number after the current API version in the alpha/beta branches.

If your Pull Request is not ready for merge, please mark it as a draft, this will
reduce unnecessary confusion when reviewing pull requests.

It is recommended to split up multiple unrelated (or extremely large)
changes into multiple PRs to make it easier to review. We have a limited amount
of time each week to review PRs, so oversized PRs may take multiple weeks to
review or outright rejected as too big to review.

### RECOMMENDED EDITOR CONFIGURATION

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

## LINKS

[Official Website](https://transcendence.kronosaur.com)

[Forums](https://forums.kronosaur.com)

[Discord](https://forums.kronosaur.com/viewforum.php?f=61)

## LICENSE

[Kronosaur Productions License](LICENSE)
