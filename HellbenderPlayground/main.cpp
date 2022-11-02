#include <Examples/FontRendering.h>
#include "HBE.h"
#include "VoxelModule/VoxelModule.h"
#include "Pathfinder/Pathfinder.h"
#include "Examples/LoadModel.h"
#include "Examples/MeshRendering.h"
#include "Examples/Raytracing.h"
#include "Examples/FontRendering.h"
using namespace HBE;
bool fullscreen = false;

void onAppUpdate(float delta) {
	if (Input::getKeyDown(KEY::ESCAPE)) {
		Application::quit();
	}
	if (Input::getKeyDown(KEY::F11)) {
		fullscreen = !fullscreen;
		Graphics::getWindow()->setFullscreen(fullscreen);

	}
	if (Input::getKeyDown(KEY::V)) {
		Configs::setVerticalSync(!Configs::getVerticalSync());
	}

	if (Input::getKeyDown(KEY::C)) {
		Entity cam = Application::getScene()->getCameraEntity();
		if (cam.valid()) {
			if (cam.has<CameraController>()) {
				Input::setCursorVisible(true);
				cam.detach<CameraController>();
			} else {
				Input::setCursorVisible(false);
				cam.attach<CameraController>();
			}
		}
	}

}

/*#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

void generateBlueNoiseRGB() {
	FILE *file = fopen((RESOURCE_PATH + std::string("textures/BlueNoise.png")).c_str(), "rb");
	int width, height, nb_channels;
	unsigned char *data = stbi_load_from_file(file, &width, &height, &nb_channels, 0);
	unsigned char *out = new unsigned char[width * height * 4];
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			int i = ((y * width) + x);
			int index = i * 4;

			out[index] = data[(i * nb_channels)];

			int i2 = (i + (width * 1) + 1) % (width * height);
			out[index + 1] = data[(i2 * nb_channels)];

			int i3 = (i + (width * 2) + 2) % (width * height);
			out[index + 2] = data[(i3 * nb_channels)];

			out[index + 3] = 255;
		}
	}
	stbi_write_png((RESOURCE_PATH + std::string("textures/BlueNoise8.png")).c_str(), width, height, 4, out, width * 4);
	stbi_image_free(data);
	delete out;
}*/

int main() {
	Application::init();
	//-----------------------SETUP--------------------
	Configs::setWindowTitle("Hellbender");
	//Pathfinder *pathfinder = new Pathfinder();

	{
		//RaytracingExemple raytracer;
		//Pathfinder pathfinder;
		MeshRendering::init();
		//LoadModelExemple::init();
		//FontRenderingScene font_rendering_scene = FontRenderingScene();
		//RaytracingScene rts = RaytracingScene();
		//-----------------------EVENTS------------------
		Application::onUpdate.subscribe(&onAppUpdate);
		//-----------------------LOOP--------------------
		Application::run();
		//-----------------------CLEANUP------------------
		Application::onUpdate.unsubscribe(&onAppUpdate);
		//-----------------------TERMINATE------------------
	}
	//delete pathfinder;
	Application::terminate();
}