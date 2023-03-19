#include "RaytracingScene.h"

#include "HBE.h"

using namespace HBE;
bool fullscreen = false;
Scene *main_scene;
Scene *ui_scene;

void onAppUpdate(float delta) {
	if (Input::getKeyDown(KEY_ESCAPE)) {
		Application::quit();
	}
	if (Input::getKeyDown(KEY_F11)) {
		fullscreen = !fullscreen;
		Graphics::getWindow()->setFullscreen(fullscreen);
	}
	if (Input::getKeyDown(KEY_V)) {
		Configs::setVerticalSync(!Configs::getVerticalSync());
	}

	if (Input::getKeyDown(KEY_B)) {
		if (ui_scene != nullptr) {
			ui_scene->setActive(!ui_scene->isActive());
		}
	}
	if (Input::getKeyDown(KEY_C)) {
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
	ApplicationInfo app_info{};
	app_info.hardware_flags = HARDWARE_FLAG_GPU_REQUIRE_VULKAN_1_3 |
							  HARDWARE_FLAG_GPU_REQUIRE_RTX_CAPABILITIES |
							  HARDWARE_FLAG_GPU_REQUIRE_DESCRIPTOR_INDEXING_CAPABILITIES;

	Application::init(app_info);
	//-----------------------SETUP--------------------
	Configs::setWindowTitle("Raytracer");
	{
		//-----------------------tests-------------------
		//main_scene = new TriangleScene();
		//main_scene = new CubeScene();
		//ModelScene load_model_scene = ModelScene();
		//ui_scene= new TextScene();
		//ui_scene = new OrderedRenderingScene();

		//-----------------------projects-----------------
		main_scene = new RaytracingScene();
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