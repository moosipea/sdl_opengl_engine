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

			if(vari::keyHeld(vari::LEFT))
				x_shift -= 0.2f * delta;
			if(vari::keyHeld(vari::RIGHT))
				x_shift += 0.2f * delta;

			if(vari::keyHeld(vari::UP))
				y_shift += 0.2f * delta;
			if(vari::keyHeld(vari::DOWN))
				y_shift -= 0.2f * delta;
			
			if(vari::keyHeld(vari::LSHIFT))
				zoom += 0.1f * delta;
			if(vari::keyHeld(vari::LCONTROL))
				zoom -= 0.1f * delta;

			myShader.setUniform2f("iResolution", 640.0f, 480.0f);
			myShader.setUniform2f("iShift", x_shift, y_shift);
			myShader.setUniform1f("iZoom", zoom);
		}

		void draw() override {
			clear(vari::VERY_DARK_CYAN);
			test_rect();
		}

	private:
		vari::Shader myShader;
		float x_shift = 0.0f;
		float y_shift = 0.0f;
		float zoom = 1.0f;
};

int main(int argc, char **argv) {
		
	myApp instance;

	instance.construct(640, 480, "test vari app");

	return 0;
}
