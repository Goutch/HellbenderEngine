#include <Examples/TextScene.h>
#include "HBE.h"
#include "VoxelModule/VoxelModule.h"
#include "Pathfinder/Pathfinder.h"
#include "Examples/ModelScene.h"
#include "Examples/RaytracingScene.h"
#include "Examples/TextScene.h"
#include "Examples/CubeScene.h"
#include "Examples/TriangleScene.h"
#include "Examples/OrderedRenderingScene.h"

using namespace HBE;
bool fullscreen = false;
Scene *main_scene;
Scene *ui_scene;

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

	if (Input::getKeyDown(KEY::B)) {
		if (ui_scene != nullptr) {
			ui_scene->setActive(!ui_scene->isActive());
		}
	}
	if (Input::getKeyDown(KEY::C)) {
		Entity cam = main_scene->getCameraEntity();
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

void onAppPresent() {
	std::vector<Texture *> present_images;
	if (main_scene != nullptr && main_scene->isActive() && main_scene->getMainCameraTexture() != nullptr) {
		present_images.push_back(main_scene->getMainCameraTexture());
	}
	if (ui_scene != nullptr && ui_scene->isActive() && ui_scene->getMainCameraTexture() != nullptr) {
		present_images.push_back(ui_scene->getMainCameraTexture());
	}
	PresentCmdInfo present_info;
	present_info.image_count = present_images.size();
	present_info.images = present_images.data();
	Graphics::present(present_info);

}

int main() {
	Application::init();
	//-----------------------SETUP--------------------
	Configs::setWindowTitle("Hellbender");
	//Pathfinder *pathfinder = new Pathfinder();

	{
		//-----------------------tests-------------------
		//main_scene = new TriangleScene();
		main_scene = new CubeScene();
		//ModelScene load_model_scene = ModelScene();
		//ui_scene= new TextScene();
		ui_scene = new OrderedRenderingScene();

		//-----------------------projects-----------------
		//RaytracingScene rts = RaytracingScene();
		//Pathfinder pathfinder;
		//-----------------------EVENTS------------------
		Application::onUpdate.subscribe(&onAppUpdate);
		Application::onPresent.subscribe(&onAppPresent);
		//-----------------------LOOP--------------------
		Application::run();
		//-----------------------CLEANUP------------------
		Application::onUpdate.unsubscribe(&onAppUpdate);
		Application::onPresent.unsubscribe(&onAppPresent);
		//-----------------------TERMINATE------------------

		if (ui_scene != nullptr)
			delete ui_scene;
		if (main_scene != nullptr)
			delete main_scene;
	}
	//delete pathfinder;
	Application::terminate();
}