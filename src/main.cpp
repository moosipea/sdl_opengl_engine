#include "include/engine.h"
#include <iostream>
#include <string>

class myApp : public vari::App {
	public:

		myApp() {}

		void create() override {
			std::string vertex_source = vari::load_string("./vertex.glsl");
			std::string frag_source = vari::load_string("./fragment.glsl");
			myShader = vari::Shader(vertex_source, frag_source);
			std::cout << myShader.compile_log << std::endl;
			use_shader(myShader);
		}

		void update(float delta) override {
			myShader.setUniform2f("iResolution", 640.0f, 480.0f);
		}

		void draw() override {
			clear(vari::VERY_DARK_CYAN);
			test_rect();
		}

	private:
		vari::Shader myShader;
};

int main(int argc, char **argv) {
		
	myApp instance;

	instance.construct(640, 480, "test vari app");

	return 0;
}
