# Asset File Style & Contribution Guidelines
## Images
### Sprites & 3D Models

The base game uses JPG for color data, and a separate bitmask for alpha data.

Historically, the bitmask has been a 1-bit BMP file, however in order to
support increased quality alpha map without drastically increasing the disk
size of the repo, we are switching to using PNG files for bitmasks.

#### 3D Model scene files

If you are contributing new rendered sprites, it is necessary that we be able to
rerender them in the future from their source scene files.

We are gradually moving towards using Blender as the accepted software for 3D
scene files, as Caligari TrueSpace is unsupported legacy software.

Scene files must be comptaible with:

    [Blender](https://www.blender.org/): Current stable version
    Caligari TrueSpace: Version 7.6 (Modelside Edition v2.1)

Do not pack resources in the scene files, any images that they use must be placed
into the Textures directory. See the TransArt Repository for more guidance.

### Other images

## Sounds
