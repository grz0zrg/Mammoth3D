Mammoth3D
=========

### OpenGL 3.3+ demos framework ###

To use the entire framework just include "mammoth3d.hpp".

Made it to do [demos](http://en.wikipedia.org/wiki/Demoscene)

Disclaimer: The framework is kinda messy, this is my first try at a 3D engine ever, the point has always been to get it up "quickly" to do demos. :)

### Usage ###

There is a sample project which is just a test area in the "project" folder, this can be used as a starting point.

The sample project should compile easily on Windows/Linux using the [codelite](http://www.codelite.org/) .workspace, there is configurations for Windows and Linux.

The "lib" folder contain all the libraries the engine use, some are mingw32 x86 pre-compiled so it compile easily under Windows

### Tools ###

The engine use a binary format to load scenes/meshes, converters are available in the "tools" folder

assimp2bin load any meshes files that the [Assimp](http://assimp.sourceforge.net/) library support

dae2bin is deprecated, export Collada (.dae) only and only meshes vertices (UVs/normals exports are buggy)

### Others platforms ###

Just look at the .project settings in the project folder, the framework require: [PortAudio](http://www.portaudio.com/) [libogg/libvorbis](http://xiph.org/downloads) [glfw3](http://www.glfw.org/download.html)

Additionally some others libraries ([lodepng](http://lodev.org/lodepng/), [GLEW](http://glew.sourceforge.net/), [tinyxml2](http://www.grinninglizard.com/tinyxml2/)) are provided and used, they can be found in the "lib" folder, they can be compiled with the app

The assimp2bin tool require the [Assimp](http://assimp.sourceforge.net/) library

### License ###

Do what you want.

### Screenshots ###

![Alt text](http://garzul.tonsite.biz/mammoth3d/05_08_2013_small.png "mammoth3d")

![Alt text](http://garzul.tonsite.biz/mammoth3d/30_07_2013.png "mammoth3d")