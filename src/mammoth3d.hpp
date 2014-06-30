#ifndef MAMMOTH3D_HPP
#define MAMMOTH3D_HPP

	#ifndef GLFW_INCLUDE_GL3
		#define GLFW_INCLUDE_GL3
	#endif

	#include "../include/glm/glm.hpp"
	#include "../include/glm/gtx/random.hpp"

	#include "core/math.hpp"
	#include "core/utils.hpp"
	#include "core/texture.hpp"
	#include "core/uniform_block.hpp"
	#include "core/geometry_utils.hpp"
	#include "renderer/renderer.hpp"
	#include "scenegraph/node.hpp"
	#include "scenegraph/meshnode.hpp"
	#include "window/window.hpp"
	#include "audio/audio.hpp"
	#include "objects/mesh.hpp"
	#include "loaders/loader.hpp"
	#include "cameras/camera.hpp"
	#include "programs/program.hpp"
	#include "font/bitmapfont.hpp"
	#include "objects/quad.hpp"
	#include "objects/bitmaptext.hpp"
	#include "joystick/joystick.hpp"
	#include "synctracker/synctracker.hpp"
	#include "synctracker/synctracker_controller.hpp"
	#include "synctracker/synctracker_xrns.hpp"
	
#endif