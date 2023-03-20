
#include <iostream>
#include "HBE.h"
#include "PongGame.h"
#include "Simulation/PongServer.h"

using namespace HBE;
using namespace PongLockstep;
namespace PongLockstep {

	bool fullscreen = false;
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
	}

	void startServer() {
		ServerInfo server_info{};
		server_info.port = 25565;
		server_info.max_clients = 2;
		server_info.max_channels = 2;
		PongServer server = PongServer(server_info);
		server.start();

		while (true) {
			server.pollEvents();
		}
	}
}

int main() {
	std::string input = "";
	std::cin >> input;
	if (input == "server") {
		PongLockstep::startServer();
		return 0;
	}
	ApplicationInfo app_info{};
	Application::init(app_info);
	//-----------------------SETUP--------------------
	Configs::setWindowTitle("Pong");
	{
		//-----------------------Games--------------------
		PongLockstep::PongGame pong = PongLockstep::PongGame(input);
		//-----------------------EVENTS------------------
		Application::onUpdate.subscribe(&PongLockstep::onAppUpdate);
		//-----------------------LOOP--------------------
		Application::run();
		//-----------------------CLEANUP------------------
		Application::onUpdate.unsubscribe(&PongLockstep::onAppUpdate);
		//-----------------------TERMINATE------------------
	}
	Application::terminate();
}