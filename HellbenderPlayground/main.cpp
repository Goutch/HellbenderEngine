#include "HBE.h"
#include "VoxelModule/VoxelModule.h"
#include "Examples/Instancing.h"
#include "Pathfinder/Pathfinder.h"
#include "Raytracing/RaytracingExemple.h"
#include "Examples/LoadModel.h"
#include "Examples/MeshRendering.h"
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
		Entity cam = *Application::getScene()->getCameraEntity();
		if (cam.has<CameraController>()) {
			cam.detach<CameraController>();
		} else {
			cam.attach<CameraController>();
		}
	}
}


int main() {
	Application::init();
	//-----------------------SETUP--------------------
	Configs::setWindowTitle("Hellbender");
	//Pathfinder *pathfinder = new Pathfinder();

	{
		//RaytracingExemple raytracer;
		//Pathfinder pathfinder;
		//MeshRendering::init();
		LoadModelExemple::init();
		//Instancing::init();
		//VoxelModule::init();
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