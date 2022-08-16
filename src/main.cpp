#include "include/engine.h"

int main() {
	
	App myApp = App(640, 480, "Test App");

	myApp.use_shader("./assets/shaders/vertex.glsl", "./assets/shaders/fragment.glsl");

	myApp.start();


	return 0;
}
