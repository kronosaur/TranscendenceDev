# C++ Style Guidelines

## Formatting

### Use Whitesmith Indentation

Wikipedia: Whitesmith Style

```cpp
int function (int x)
   {
   while (x < 0)
      {
      if (something 
            && something_else
            && yet_another_condition)
         {
         do_something();
         }
      }
   }
```

### Use Tabs

Use tabs to indent. Set the indent at 4 spaces.

### Use Whitespace to Separate Blocks

```cpp
   {
   ...

   //   This section does some stuff

   do_something();
   do_something_else();

   //   This is another block of code that is logically
   //   related.

   if (something)
      {
      ok_do_it();
      this_too();
      }

   //   We're done

   return 0;
   }
```

### Function Declaration and Calls

#### Inline documentation

Inline documentation of funcions should be before the declaration so that it
is detected as a standard docstring and displayed in mouse-over tooltips etc.

```cpp
// UpdateExtended
//
// This is called when the player re-enters a system after having been
// away. We update the system to reflect the amount of time that has passed.
//
void CUniverse::UpdateExtended (void)
	{
	}
```

NOTE - most of the codebase uses an older style with the comment description
after the declaration. These are not detected as docstrings so should be updated
to follow the new style. See: https://ministry.kronosaur.com/record.hexm?id=104374

#### Distinguish Between Declarations and Calls

Add a space between the function name and the opening parenthesis when
declaring or defining a function. Do not add space when calling the function:

```cpp
int function (int a)
//          ^
//     Space here.
   {
   return 0;
   }

function(10);

//      ^
//   No space here
```

#### Omit void if no parameters

```cpp
int function ()
//            ^
//          Empty
   {
   return 0;
   }
```

## Layering

```
+---------------+-------------------------------------+
| Transcendence | CTranscendenceController            |
|               | CTranscendenceModel                 |
|               | CTranscendenceWnd                   |
|               | CGameSession, CIntroSession, etc.   |
|               | TransCore                           |
|               | TransData, etc.                     |
+---------------+-------------------------------------+
| Mammoth       | TSUI                                |
|               |    CHumanInterface                  |
|               |    CVisualPalette                   |
|               |    IHISession                       |
|               |                                     |
|               | TSE                                 |
|               |    CUniverse                        |
|               |    CDesignCollection                |
|               |    CTopology                        |
|               |    CSystem                          |
|               |    CSpaceObject hierarchy           |
+---------------+-------------------------------------+
| Alchemy       | CodeChain                           |
|               |    CCodeChain                       |
|               |    ICCItemPtr                       |
|               |                                     |
|               | DirectXUtil                         |
|               |    CG32bitPixel                     |
|               |    CG32bitImage, CG8bitImage, etc.  |
|               |    CGDraw                           |
|               |                                     |
|               | Kernel                              |
|               |    CString                          |
|               |    TArray                           |
|               |    TSortMap                         |
|               |    IReadStream                      |
|               |    IWriteStream                     |
|               |    CObject                          |
|               |                                     |
|               | 3rd Party Libraries                 |
+---------------+-------------------------------------+
```

### Alchemy
### TSE
### TSUI
### Transcendence
### TransCore
### TransData and Utilities
## Major Components
### The Universe
### The Design Collection
### The Topology
### Star Systems
### Space Objects
### Station Objects
### Ship Objects
### Missile/Damage Objects
### Effects
### Items
## Data Structures
### Strings (Kernel::CString)
### Arrays (Kernel::TArray)
### Maps (Kernel::TMap, Kernel::TSortMap)
### Streams
### TLisp Items (CodeChain::ICCItem)
## Graphics
### Images and Pixels
### Drawing Algorithms
## User Interface
### TSUI
### AGScreen
### Reanimator
### Dock Screens
### HUD
