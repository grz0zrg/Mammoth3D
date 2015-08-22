Mammoth3D
=========

### OpenGL 3.3+ demos framework ###

To use the entire framework just include "mammoth3d.hpp".

Made it to do [demos](http://en.wikipedia.org/wiki/Demoscene) and to experiment

This framework use C++11 features to bypass the boring stuff

### Usage ###

There is a sample project in the "project" folder, this can be used as a starting point.

The sample project should compile easily on Windows/Linux using the [codelite](http://www.codelite.org/) .workspace, there is configurations for Windows and Linux.

The "lib" folder contain all the libraries the engine use, some are mingw32 x86 pre-compiled so it compile easily under Windows

This framework is specifically made for demos; texture, shader, geometry etc can (and should) be created through a general purpose loader, with the "loader" there is no need to explicitely create any objects and releasing the memory, all stuff needed for a demo can be created this way, anything used from the framework does not need to be released explicitely or created.

### Step by step guide ###

Opening a window (set false to true for fullscreen, last argument is MSAA level)

```cpp
auto app = window::Window::getInstance();
app->open("example", 800, 600, false, 2);
```
	
*By default the mouse cursor will be hidden and vsync will be requested*

Loading GLSL files and compiling a program:
	
```cpp
auto program = loader::Loader::getInstance()->getProgram("simple.vert", "simple.frag");
```
	
Creating a texture from an image file (only the PNG image format is supported):

```cpp
auto texture = loader::Loader::getInstance()->getTexture("image.png");
```
	
Creating a quad with the texture on the first slot (all textures assignated will be registered as t[id] in the shader (thus `uniform sampler2D t0;` to access the texture within the shader for this example):

```cpp
auto quad = loader::Loader::getInstance()->getNewQuad({{texture, 0}});
```
	
Creating a purple BitmapText mesh from a bitmap font file:

```cpp
auto ldr = loader::Loader::getInstance();
auto text = ldr->getNewBitmapText("font.png", "Hello World!", 255, 0, 255, 0.25f); // last argument is the alpha treshold, increasing this value make the characters bolder, decreasing it make it thinner
text->setScreenAlign(true); // set this to false to manipulate the text quad, to allow for rotations, scaling etc otherwise by default it is true and the text will be screen aligned, always visible
```
	
*The text, color, treshold, screen alignment can be updated in real-time.*

Creating a camera and setting up its position/target:

```cpp
auto camera = loader::Loader::getInstance()->getNewCamera(camera::PERSPECTIVE, 75, screenWidth / screenHeight);
camera->lookAt(0, 0, -100.0f, 0, 0, 0, 0, 1, 0);
```
	
Using the camera and adding the quad and text to the scene:

```cpp
auto renderer = renderer::Renderer::getInstance();
renderer->setCamera(camera);
renderer->add(quad);
renderer->add(text);
```
	
Main loop:

```cpp
do {
    renderer->render();
		
    app->swapBuffers();
} while(app->running() && !audio->isMusicFinished()); // see below for audio setup
```

#### Audio -> Visual synchronization ####

There is a built-in synctracker (value/event sequencer) which can be used in real-time with a gamepad or similar device to control any aspects of a demo (with chunk looping/backward/forward feature), it is possible to animate an entire demo that way in real-time without any code, generally the sync data will be imported first from an existing song but it is not necessary, there is a tool to convert [Renoise](http://www.renoise.com/) XRNS Song.xml data to allow perfect synchronization (with additional real-time edits needed, because it generate a "blank" event per note) of a song made in Renoise with the visuals.
	
Loading an audio file and playing it (only the OGG format is supported):

```cpp
auto audio = audio::Audio::getInstance()
audio->loadMusic("audio_track.ogg");
audio->playMusic(true, 0.5f); // loop play, volume halved
```

Setting up the synctracker:

```cpp
auto synctracker = sync::SyncTracker::getInstance();
// the XRNS song.xml converter is needed to get a ".mms" sync file from a song
synctracker->load("sync.mms");
// those two lines is only needed if there is a need to adjust the bpm/lpb because no sync file is used, otherwise the .mms set them when it is loaded
synctracker->setBpm(115);
synctracker->setLpb(4);
	
// setup the synchronized values, one track for each values, it will also create a track if the track name is not found
float sync_value = 0.0f;
int sync_value2 = 0;
synctracker->setTracks({{"track_name_1", &sync_value}, {"track_name_2", &sync_value2}});
	
// if you want to edit the sync in real-time with a gamepad (or similar device)
auto synctracker_controller = sync::SyncTrackerController::getInstance();
```

Now, in the main loop:

```cpp
// use sync_value and sync_value2 as you wish
	
double song_time = audio->getMusicTime();
synctracker->update(song_time);
	
synctracker_controller->update();
```
	
*There is no graphical interface to edit the synctracker data but there is still text output in the console reflecting any actions you do.*

### More stuff ###

It is easy to pass stuff to a shader program using uniform blocks:

```cpp
auto ublock = loader::Loader::getInstance()->getNewUniformBlock("BlockName", {{"r", 1.0f}, {"g", 1.0f}, {"b", 1.0f}, {"a", 1.0f}, {"stuff", 1.0f}}, program);
```
	
*There is only "float" uniform type available at the moment.*
	
Then in the GLSL shader (name does not matter here):
	
```glsl
layout (std140) uniform BlockName {
    vec4 color;
    float stuff;
};
```
	
Or you can pass large set of data using a TBO:

```cpp
auto texture_buffer = loader::Loader::getInstance()->getNewTextureBuffer(std::vector<float> {1.0f, 0.0f, 0.0f, 0.0f});
mesh->setTexture(texture_buffer, 1);
```
	
If you need to modify the TBO data you will need to call `ldr->getNewTbo` to get the TBO object then passing the pointer to `getNewTextureBuffer` and use `tbo->updateData` function to update the data
	
Then in the GLSL shader:

```glsl
uniform samplerBuffer t1; // access data using texelFetch function
```
	
It is easy to do instancing, example if you want a huge amount of objects of the same type:

```cpp
auto ldr = loader::Loader::getInstance();
auto particle_texture = ldr->getTexture("particle.png");
auto particle = ldr->getNewQuad({{particle_texture, 0}});
auto particles = particle->enableInstancing(1000); // 1000 objects will be drawn in a single call, particles is a vector of Mesh, you can change individual attributes from it for each mesh (note: only transform attributes are passed per instance right now)
```
		
*An attribute location for instances transform matrix should be specified in the vertex shader as `layout(location = 4) in mat4 imvp;`*

*Then in the vertex shader you can use `gl_InstanceID` to get the object id, combine it with a TBO and texelFetch function in the shader for any more per instance stuff like colors, uvs...*

There is also a fast way to display huge amount of static objects by merging geometries, it will be difficult to apply effects on each of them but may be faster than instancing:

```cpp
unsigned int object_count = 1000;
	
auto geom = ldr->getNewGeometry();
for (unsigned int i = 0; i < object_count; i++) {
    auto quad = new object::Quad();
    // setup object position, rotation, scale here
    core::geometryutils::merge(geom, quad);
    delete quad;
}
geom->update();
	
auto ldr = loader::Loader::getInstance();
auto quad_mesh = ldr->getNewMesh(geom, ldr->getNewMaterial());
	
// rendering quad_mesh will be very fast now
```

### Tools ###

The engine use a binary format to load scenes/meshes, converters are available in the "tools" folder

assimp2bin load and convert any meshes files that the [Assimp](http://assimp.sourceforge.net/) library support

dae2bin is deprecated, export Collada (.dae) only and only the geometry (UV/normal export is buggy)

### Others platforms ###

Just look at the .project settings in the project folder, the framework require: [PortAudio](http://www.portaudio.com/) [libogg/libvorbis](http://xiph.org/downloads) [glfw3](http://www.glfw.org/download.html)

Additionally some others libraries ([GLM](http://glm.g-truc.net), [lodepng](http://lodev.org/lodepng/), [GLEW](http://glew.sourceforge.net/), [tinyxml2](http://www.grinninglizard.com/tinyxml2/)) are provided and used, they can be found in the "lib" or "include" ([GLM](http://glm.g-truc.net)) folder, they can be compiled with the project

The assimp2bin tool require the [Assimp](http://assimp.sourceforge.net/) library

### License ###

Do what you want.

### Screenshots from experiments ###

![Alt text](http://garzul.tonsite.biz/mammoth3d/05_08_2013_small.png "mammoth3d")

![Alt text](http://garzul.tonsite.biz/mammoth3d/30_07_2013.png "mammoth3d")
