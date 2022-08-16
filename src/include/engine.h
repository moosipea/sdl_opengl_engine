#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_video.h>
#include <bits/chrono.h>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include "logger.h"

struct Color {
	Color(float r, float g, float b) : r(r), g(g), b(b) {}
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
};

class App {
	
	public:
		App(int width, int height, std::string title) : w(width), h(height), title(title) {
			
			logger.info("Constructing app...");

			window = SDL_CreateWindow(title.c_str(), 0, 0, width, height, window_flags);
			if(!window) {
				logger.error("Failed to init window!", 1);
			}

			context = SDL_GL_CreateContext(window);
			if(!context) {
				logger.error("Failed to init OpenGL context", 2);
			}

			GLenum err = glewInit();
			if(err != GLEW_OK) {
				logger.error("GLEW is angry", 6);
			}
			if(!GLEW_VERSION_2_1) {
				logger.error("Device does not support GLEW 2.1", 7);
			}
		}

		~App() {
			logger.info("Deconstructing app...");			
		}

		void start() {
	
			// create clock to measure fps
			std::chrono::high_resolution_clock timer;

			SDL_Event e;
			while(running) {

				// set uniforms
				// uniform 2 = resolution
				glUniform2f(2, (GLfloat)w, (GLfloat)h);

				auto timer_start = timer.now();

				while(SDL_PollEvent(&e)) {
					if(e.type == SDL_KEYDOWN) {
						switch(e.key.keysym.sym) {

							case SDLK_ESCAPE:
								running = false;
								break;

							case 'f':
								fullscreen = !fullscreen;
								if(fullscreen) {
									SDL_SetWindowFullscreen(window, window_flags | SDL_WINDOW_FULLSCREEN_DESKTOP);
								} else {
									SDL_SetWindowFullscreen(window, window_flags);
								}
								break;

							default:
								break;

						}
					} else if(e.type == SDL_QUIT) {
						running = false;
					}
				}
			
				glViewport(0, 0, w, h);
				glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				
				// create quad that covers entire screen (for shader purposes)
				glBegin(GL_QUADS);
					glVertex2f(-1.0f, -1.0f);
					glVertex2f(1.0f, -1.0f);
					glVertex2f(1.0f, 1.0f);
					glVertex2f(-1.0f, 1.0f);
				glEnd();
				
				SDL_GL_SwapWindow(window);
				
				auto timer_end = timer.now();
				delta = std::chrono::duration_cast<std::chrono::microseconds>(timer_end - timer_start).count();
				
				if(delta != 0.0f) {
					fps = 1.0f / delta;
				}

				SDL_SetWindowTitle(window, (title + " -  " + std::to_string(fps * 1000000.0f) + "fps").c_str());

			}

		}
		



		/*
		o-----------------o
		|Shader management|
		o-----------------o
		*/




		std::string load_file_as_string(const char* path) {
			std::string path_s(path);
			std::string content;
			std::fstream fileStream(path, std::ios::in);
			
			if(!fileStream.is_open()) {
				logger.error("Couldn't find file at " + path_s , 3);
				return "";
			}

			std::string line;
			while(!fileStream.eof()) {
				std::getline(fileStream, line);
				content.append(line + "\n");
			}

			fileStream.close();
			return content;
		}
		
		void use_shader(std::string vert, std::string frag) {
			logger.info("Applying shader");
			GLuint program = loadShader(vert.c_str(), frag.c_str());
			glUseProgram(program);
		}

		void no_shader() {
			glUseProgram(0);
		}

		GLuint loadShader(const char* vertex_path, const char* frag_path) {
			logger.info("Loading shader...");
			GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
			GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
			
			logger.info("Reading shader files...");
			std::string vert_str = load_file_as_string(vertex_path);
			std::string frag_str = load_file_as_string(frag_path);

			const char* vert_src = vert_str.c_str();
			const char* frag_src = frag_str.c_str();

			GLint result = GL_FALSE;
			int logLength;

			logger.info("Compiling vertex shader...");
			glShaderSource(vertex_shader, 1, &vert_src, NULL);
			glCompileShader(vertex_shader);
			glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &result);
			if(result == GL_FALSE) {
				logger.error("Failed to compile vertex shader", 4);
			}

			logger.info("Compiling fragment shader...");
			glShaderSource(frag_shader, 1, &frag_src, NULL);
			glCompileShader(frag_shader);
			glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &result);
			if(result == GL_FALSE) {
				logger.error("Failed to compile fragment shader", 5);
			}

			logger.info("Linking shader program...");
			GLuint program = glCreateProgram();
			glAttachShader(program, vertex_shader);
			glAttachShader(program, frag_shader);
			glLinkProgram(program);

			glGetProgramiv(program, GL_LINK_STATUS, &result);
			if(result == GL_FALSE) {
				logger.error("Failed to link shader", 6);
			}

			glDeleteShader(vertex_shader);
			glDeleteShader(frag_shader);

			return program;
		}

		/*
		o-----------------o
		|Drawing functions|
    o-----------------o
		*/

		void setClearColor(Color color) {
			_clearColor = color;
		}

	private:
		int w, h;
		bool running = true;
		bool fullscreen = false;
		
		Color _clearColor = Color(0.0f, 0.0f, 0.0f);
		
		float delta = 0.0f;
		float fps = 0.0f;
		
		std::string title;

		Logger logger;

		uint32_t window_flags = SDL_WINDOW_OPENGL;
		SDL_Window* window = NULL;
		SDL_GLContext context = NULL;
};
