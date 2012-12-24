/**
 * This program load a .dae (Collada) file and export it to a binary format
 * usable directly within the Mammoth3D engine.
 * 
 * Usage: dae2bin TYPE in.dae
 * 
 * TYPE can be: MESH (.mm)
 * */
 
#include "../../src/loaders/collada/collada.hpp"

int main(int argc, char **argv)
{
	std::cout << "dae2bin.exe - Mammoth3D tool\n\n";

	if (argc != 3) {
		std::cout << "Invalid usage.\n\n"
						"Usage: dae2bin TYPE in.dae\n"
						"TYPE allowed: MESH (.mm)\n\n"
						"Example: dae2bin MESH in.dae\n";
		return 0;
	}

	std::string outputType(argv[1]);
	
	loader::Collada *file = 0;
	file = new loader::Collada(argv[2]);
	if ("MESH" == outputType) {
		file->exportMeshsTo("meshs");
	}
	delete file;
	
	return 0;
}
