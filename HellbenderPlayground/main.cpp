#include "HBE.h"
#include "Raytracer/Raytracer.h"
#include "Pong/Pong.h"
#include "Examples/Instancing.h"
#include "Pathfinder/Pathfinder.h"

using namespace HBE;

void onAppUpdate(float delta) {
	//Shut down app if escape key is pressed
	if (Input::getKeyDown(KEY::ESCAPE)) {
		Application::quit();
	}
	//change vertical sync if V key is pressed
	if (Input::getKeyDown(KEY::V)) {
		Configs::setVerticalSync(!Configs::getVerticalSync());
	}
}


int main() {


	Application::init();

	//-----------------------SETUP--------------------
	Configs::setWindowTitle("Hellbender");
	Pathfinder pathfinder;
	//Raytracer::init();
	//Pong::init();
	//Instancing::init();
	//-----------------------EVENTS------------------
	Application::onUpdate.subscribe(&onAppUpdate);
	//-----------------------LOOP--------------------
	Application::run();
	//-----------------------CLEANUP------------------
	Application::onUpdate.unsubscribe(&onAppUpdate);
	//-----------------------TERMINATE------------------

	Application::terminate();
}