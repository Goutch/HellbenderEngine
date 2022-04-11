#include "HBE.h"
#include "VoxelModule/Raytracer.h"
#include "Pong/Pong.h"
#include "Examples/Instancing.h"
#include "Pathfinder/Pathfinder.h"

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
	Raytracer::init();
	Application::getScene()->getCameraEntity()->attach<CameraController>();
	//Pong::init();
	//Instancing::init();
	//-----------------------EVENTS------------------
	Application::onUpdate.subscribe(&onAppUpdate);
	//-----------------------LOOP--------------------
	Application::run();
	//-----------------------CLEANUP------------------
	Application::onUpdate.unsubscribe(&onAppUpdate);
	//-----------------------TERMINATE------------------
	//delete pathfinder;
	Application::terminate();
}