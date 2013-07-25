Mammoth3D
=========

![Alt text](http://garzul.tonsite.biz/mammoth3d/14_05_2013.png "mammoth3d")

### OpenGL 3.x demos framework ###

To use the entire framework just include "mammoth3d.hpp".

I use it to do [demos](http://en.wikipedia.org/wiki/Demoscene)

### Usage ###

All projects should compile easily on Windows using the [codelite](http://www.codelite.org/) .workspace.

The "lib" folder contain all the libraries the engine use, some are mingw32 x86 precompiled

### Tools ###

The engine use its own binary meshes format, converters are availables in the "tools" folder:

assimp2bin load any meshes files that the assimp library support [Assimp](http://assimp.sourceforge.net/)

dae2bin is deprecated, export collada (.dae) only and only meshe vertices (UVs/normals are supported but does not export correctly)

### Others platforms ###

Just look at the .project settings in the project folder to compile the sample program, the framework require: [PortAudio](http://www.portaudio.com/) [libogg/libvorbis](http://xiph.org/downloads) [glfw](http://www.glfw.org/download.html)

The assimp2bin tool require the [Assimp](http://assimp.sourceforge.net/) library

### License ###

Do what you want.