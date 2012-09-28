#include "collada.hpp"

const char *loader::Collada::libNames[] = { "asset", "scene",
									"library_animations",
									"library_controllers",
									"library_cameras",
									"library_effects",
									"library_geometries",
									"library_images",
									"library_lights",
									"library_materials",
									"library_visual_scenes"};
													
const loader::Collada::e_libType loader::Collada::libTypes[] = { 
										ASSET, SCENE, ANIMATIONS,
										CONTROLLERS, CAMERAS, EFFECTS,
										GEOMETRIES, IMAGES, LIGHTS,
										MATERIALS, VISUAL_SCENES, NONE};