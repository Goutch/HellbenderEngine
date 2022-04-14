#include "HBE.h"
#include "VoxelModule/VoxelModule.h"
#include "Pong/Pong.h"
#include "Examples/Instancing.h"
#include "Pathfinder/Pathfinder.h"
#include "Raytracing/RaytracingExemple.h"
using namespace HBE;
bool fullscreen = false;
bool controller = false;

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
		if (controller) {
			controller = false;
			Application::getScene()->getCameraEntity()->detach<CameraController>();
		} else {
			controller = true;
			Application::getScene()->getCameraEntity()->attach<CameraController>();
		}

	}
}


int main() {
	Application::init();
	//-----------------------SETUP--------------------
	Configs::setWindowTitle("Hellbender");
	//Pathfinder *pathfinder = new Pathfinder();

	{
		RaytracingExemple raytracer;

		//VoxelModule::init();
		//Pong::init();
		//Instancing::init();
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