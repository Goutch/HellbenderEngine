#include "HBE.h"
#include "Raytracer/Raytracer.h"
#include "Pong/Pong.h"
#include "Examples/Instancing.h"
#include "Pathfinder/Pathfinder.h"

using namespace HBE;

void onAppUpdate(float delta) {
	if (Input::getKeyDown(KEY::ESCAPE)) {
		Application::quit();
	}
	if (Input::getKeyDown(KEY::V)) {
		Configs::setVerticalSync(!Configs::getVerticalSync());
	}
}


int main() {
	Application::init();

	//-----------------------SETUP--------------------
	Configs::setWindowTitle("Hellbender");
	//Pathfinder *pathfinder = new Pathfinder();
	Raytracer::init();
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