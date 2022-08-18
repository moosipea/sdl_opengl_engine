#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <string>
#include <fstream>
#include <vector>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

namespace vari {
	
	// --== Structs ==-- //
	
	// Color channels represented as 0.0 -> 1.0, no alpha
	struct Color {
		Color(float _r, float _g, float _b) : r(_r), g(_g), b(_b) {}
		float r, g, b;

		Color operator+(const Color& rhs) {
			return Color(std::max(std::min(r + rhs.r, 1.0f), 0.0f),
									 std::max(std::min(g + rhs.g, 1.0f), 0.0f),
									 std::max(std::min(b + rhs.b, 1.0f), 0.0f));
		}

		Color operator-(const Color& rhs) {
			return Color(std::max(std::min(r - rhs.r, 1.0f), 0.0f),
									 std::max(std::min(g - rhs.g, 1.0f), 0.0f),
									 std::max(std::min(b - rhs.b, 1.0f), 0.0f));
		}
		
		Color operator*(const Color& rhs) {
			return Color(std::max(std::min(r * rhs.r, 1.0f), 0.0f),
									 std::max(std::min(g * rhs.g, 1.0f), 0.0f),
									 std::max(std::min(b * rhs.b, 1.0f), 0.0f));
		}
		
		Color operator*(const float& scalar) {
			return Color(std::max(std::min(r * scalar, 1.0f), 0.0f),
									 std::max(std::min(g * scalar, 1.0f), 0.0f),
									 std::max(std::min(b * scalar, 1.0f), 0.0f));
		}

		Color operator/(const Color& rhs) {
			return Color(std::max(std::min(r / rhs.r, 1.0f), 0.0f),
									 std::max(std::min(g / rhs.g, 1.0f), 0.0f),
									 std::max(std::min(b / rhs.b, 1.0f), 0.0f));
		}

		Color operator/(const float& scalar) {
			return Color(std::max(std::min(r / scalar, 1.0f), 0.0f),
									 std::max(std::min(g / scalar, 1.0f), 0.0f),
									 std::max(std::min(b / scalar, 1.0f), 0.0f));
		}
	};
	
	template<typename T>
	struct Vec2 {
		Vec2(T _x, T _y) : x(_x), y(_y) {}
		T x, y;

		inline Vec2<T> operator+(const Vec2<T>& rhs) {return Vec2<T>(x+rhs.x, y+rhs.y);}
		inline Vec2<T> operator-(const Vec2<T>& rhs) {return Vec2<T>(x-rhs.x, y-rhs.y);}
		inline Vec2<T> operator*(const float& s) {return Vec2<T>(x*s, y*s);}
		inline Vec2<T> operator/(const float& s) {return Vec2<T>(x/s, y/s);}

		inline float mag() {
			return std::sqrt(x*x + y*y);
		}

		inline Vec2<T> normalize() {
			return Vec2<T>(x / mag(), y / mag());
		}
	};

	typedef Vec2<float> Vec2f;
	typedef Vec2<double> Vec2d;
	typedef Vec2<int> Vec2i; // risky..
	
	// Wrapper struct for shaders
	struct Shader {
		std::string vert_src, frag_src;
		std::string compile_log;
		GLuint program;

		Shader() {}

		Shader(std::string vertex, std::string fragment) : vert_src(vertex), frag_src(fragment) {
			GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
			GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
			
			const char* vert_src_c = vert_src.c_str();
			const char* frag_src_c = frag_src.c_str();
			
			GLint result = GL_FALSE;
			int log_length;

			compile_log.append("Compiling vertex shader\n");
			glShaderSource(vert_shader, 1, &vert_src_c, NULL);
			glCompileShader(vert_shader);

			glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &result);
			glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &log_length);
			std::vector<GLchar> vert_shader_error((log_length > 1) ? log_length : 1);
			glGetShaderInfoLog(vert_shader, log_length, NULL, &vert_shader_error[0]);
			for(int i = 0; i < vert_shader_error.size(); i++) {
				std::cout << vert_shader_error[i];
			}

			compile_log.append("Compiling fragment shader\n");
			glShaderSource(frag_shader, 1, &frag_src_c, NULL);
			glCompileShader(frag_shader);

			glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &result);
			glGetShaderiv(frag_shader, GL_INFO_LOG_LENGTH, &log_length);
			std::vector<GLchar> frag_shader_error((log_length > 1) ? log_length : 1);
			glGetShaderInfoLog(frag_shader, log_length, NULL, &frag_shader_error[0]);
			for(int j = 0; j < frag_shader_error.size(); j++) {
				std::cout << frag_shader_error[j];
			}

			compile_log.append("Linking program\n");
			this->program = glCreateProgram();
			glAttachShader(this->program, vert_shader);
			glAttachShader(this->program, frag_shader);
			glLinkProgram(this->program);

			glGetProgramiv(this->program, GL_LINK_STATUS, &result);
			glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &log_length);
			std::vector<GLchar> program_error((log_length > 1) ? log_length : 1);
			glGetProgramInfoLog(this->program, log_length, NULL, &program_error[0]);	
			for(int k = 0; k < program_error.size(); k++) {
				std::cout << program_error[k];
			}

			glDeleteShader(vert_shader);
			glDeleteShader(frag_shader);
		}

		void setUniform1f(std::string uniform, float value) {
			GLint uniform_loc = glGetUniformLocation(this->program, uniform.c_str());
			if(uniform_loc == -1)
				return;
			glUniform1f(uniform_loc, value);
		}
		
		void setUniform2f(std::string uniform, float x, float y) {
			GLint uniform_loc = glGetUniformLocation(this->program, uniform.c_str());
			if(uniform_loc == -1)
				return;
			glUniform2f(uniform_loc, x, y);
		}

	};

	struct Key {
		int scancode;
		Key(int sdl_scancode) : scancode(sdl_scancode) {}
	};

	// --== Constants ==-- //
	
	// Same palette as olc::PixelGameEngine
	static const Color
		GREY(0.75f, 0.75f, 0.75f), DARK_GREY(0.5f, 0.5f, 0.5f),    VERY_DARK_GREY(0.25f, 0.25f, 0.25f),
		RED(1.0f, 0.0f, 0.0f),     DARK_RED(0.5f, 0.0f, 0.0f),     VERY_DARK_RED(0.25f, 0.0f, 0.0f),
		YELLOW(1.0f, 1.0f, 0.0f),  DARK_YELLOW(0.5f, 0.5f, 0.0f),  VERY_DARK_YELLOW(0.25f, 0.25f, 0.0f),
		GREEN(0.0f, 1.0f, 0.0f),   DARK_GREEN(0.0f, 0.5f, 0.0f),   VERY_DARK_GREEN(0.0f, 0.25f, 0.0f),
		CYAN(0.0f, 1.0f, 1.0f),    DARK_CYAN(0.0f, 0.5f, 0.5f),    VERY_DARK_CYAN(0.0f, 0.25f, 0.25f),
		BLUE(0.0f, 0.0f, 1.0f),    DARK_BLUE(0.0f, 0.0f, 0.5f),    VERY_DARK_BLUE(0.0f, 0.0f, 0.25f),
		MAGENTA(1.0f, 0.0f, 1.0f), DARK_MAGENTA(0.5f, 0.0f, 0.5f), VERY_DARK_MAGENTA(0.25f, 0.0f, 0.25f),
		WHITE(1.0f, 1.0f, 1.0f),   BLACK(0.0f, 0.0f, 0.0f);
	
	static const Key
		ZERO(SDL_SCANCODE_0),              ONE(SDL_SCANCODE_1),          TWO(SDL_SCANCODE_2),   THREE(SDL_SCANCODE_3), FOUR(SDL_SCANCODE_4),
		FIVE(SDL_SCANCODE_5),              SIX(SDL_SCANCODE_6),          SEVEN(SDL_SCANCODE_7), EIGHT(SDL_SCANCODE_8), NINE(SDL_SCANCODE_9),
		A(SDL_SCANCODE_A),                 B(SDL_SCANCODE_B),            C(SDL_SCANCODE_C),     D(SDL_SCANCODE_D),     E(SDL_SCANCODE_E),
		F(SDL_SCANCODE_F),                 G(SDL_SCANCODE_G),            H(SDL_SCANCODE_H),     I(SDL_SCANCODE_I),     J(SDL_SCANCODE_J),
		K(SDL_SCANCODE_K),                 L(SDL_SCANCODE_L),            M(SDL_SCANCODE_M),     N(SDL_SCANCODE_N),     O(SDL_SCANCODE_O),
		P(SDL_SCANCODE_P),                 Q(SDL_SCANCODE_Q),            R(SDL_SCANCODE_R),     S(SDL_SCANCODE_S),     T(SDL_SCANCODE_T),
		U(SDL_SCANCODE_U),                 V(SDL_SCANCODE_V),            W(SDL_SCANCODE_W),     X(SDL_SCANCODE_X),     Y(SDL_SCANCODE_Y),
		Z(SDL_SCANCODE_Z),
		F1(SDL_SCANCODE_F1),               F2(SDL_SCANCODE_F2),          F3(SDL_SCANCODE_F3),   F4(SDL_SCANCODE_F4),   F5(SDL_SCANCODE_F5),
		F6(SDL_SCANCODE_F6),               F7(SDL_SCANCODE_F7),          F8(SDL_SCANCODE_F8),   F9(SDL_SCANCODE_F9),   F10(SDL_SCANCODE_F10),
		F11(SDL_SCANCODE_F11),             F12(SDL_SCANCODE_F12),
		BACKSPACE(SDL_SCANCODE_BACKSPACE), DELETE(SDL_SCANCODE_DELETE),  ESCAPE(SDL_SCANCODE_ESCAPE),
		LALT(SDL_SCANCODE_LALT),           LCONTROL(SDL_SCANCODE_LCTRL), LSHIFT(SDL_SCANCODE_LSHIFT),
		RALT(SDL_SCANCODE_RALT),           RCONTROL(SDL_SCANCODE_RCTRL), RSHIFT(SDL_SCANCODE_RSHIFT),
		ENTER(SDL_SCANCODE_RETURN),        SPACE(SDL_SCANCODE_SPACE),    TAB(SDL_SCANCODE_TAB),
		PGUP(SDL_SCANCODE_PAGEUP),         PGDN(SDL_SCANCODE_PAGEDOWN),
		LEFT(SDL_SCANCODE_LEFT),           RIGHT(SDL_SCANCODE_RIGHT),    UP(SDL_SCANCODE_UP),   DOWN(SDL_SCANCODE_DOWN);

	// --== Classes ==-- //
	
	class Logger {
		public:

			inline void toggle_storage() {
				store_log = !store_log;	
			}

			void error(std::string desc) {
				std::string msg = get_time_string() + " [ERROR]: " + desc;
				std::string vert_path, frag_path;	std::cout << "\e[1;31m" << msg << "\e[0m\n";
				if(store_log)
					log.append(msg + "\n");
			}

			void warn(std::string desc) {
				std::string msg = get_time_string() + " [WARNING]: " + desc;
				std::cout << "\e[0;33m" << msg << "\e[0m\n";
				if(store_log)
					log.append(msg + "\n");
			} 
			
			void inform(std::string desc) {
				std::string msg = get_time_string() + " [INFO]: " + desc;
				std::cout << "\e[0;97m" << msg << "\e[0m\n";
				if(store_log)
					log.append(msg + "\n");
			}

		private:
			bool store_log = false;
			std::string log;

			std::string get_time_string() {
				auto time = std::chrono::system_clock::now();
				std::time_t _time = std::chrono::system_clock::to_time_t(time);
				return std::ctime(&_time);
			}

	};

	class App {
		public:

			Logger logger;

			App() {}
			
			virtual void create() = 0;

			virtual void update(float delta) = 0;

			virtual void draw() = 0;
			
			// Constructs a window with the specified parameters
			void construct(int width, int height, std::string title) {
				this->width = width;
				this->height = height;
				this->window_title = title;

				this->window = SDL_CreateWindow(title.c_str(), 0, 0, width, height, this->window_flags);
				if(!this->window)
					logger.error("Window initialization failed!");
				
				this->gl_context = SDL_GL_CreateContext(this->window);
				if(!this->gl_context)
					logger.error("OpenGL context initialization failed!");

				GLenum err = glewInit();
				if(err != GLEW_OK)
					logger.error("GLEW initialization failed!");
				if(!GL_VERSION_2_1)
					logger.error("GLEW >=2.1 required!");
				
				create();
				this->start_main_loop();
			}

			inline void end() {
				this->running = false;
			}

			inline void clear(Color col) {
				glClearColor(col.r, col.g, col.b, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
			}

			inline void set_title(std::string title) {
				SDL_SetWindowTitle(this->window, title.c_str());
			}

			inline void use_shader(Shader shader) {
				glUseProgram(shader.program);
			}

			inline void no_shader() {
				glUseProgram(0);
			}

			// TODO REMOVE THIS
			//
			//
			//
			//
			//
			//
			//
			//
			//
			void test_rect() {
				glBegin(GL_QUADS);
					glVertex2f(-1.0f, -1.0f);
					glVertex2f(1.0f, -1.0f);
					glVertex2f(1.0f, 1.0f);
					glVertex2f(-1.0f, 1.0f);
				glEnd();
			}

		private:
			int width, height;
			std::string window_title;

			bool running = true;
			float delta = 0.0f;

			uint32_t window_flags = SDL_WINDOW_OPENGL;
			SDL_Window* window = NULL;
			SDL_GLContext gl_context = NULL;

			void start_main_loop() {
				
				std::chrono::high_resolution_clock timer;
				SDL_Event e;
				while(this->running) {
					
					auto timer_start = timer.now();

					update(this->delta);	

					// Polling events
					while(SDL_PollEvent(&e)) {
						if(e.type == SDL_QUIT)
							this->running = false; break;
					}
					
					// Rendering
					glViewport(0, 0, this->width, this->height);
					
					draw();

					SDL_GL_SwapWindow(this->window);

					auto timer_end = timer.now();
					this->delta = std::chrono::duration_cast<std::chrono::microseconds>(timer_end - timer_start).count() / 100000.0f;
				}
			}
	};
	
	// --== Helper functions ==-- //

	std::string load_string(std::string path) {
		std::string content;
		std::fstream file_stream(path.c_str(), std::ios::in);

		if(!file_stream.is_open())
			return ""; // Couldn't open file for some reason

		std::string line;
		while(!file_stream.eof()) {
			std::getline(file_stream, line);
			content.append(line + "\n");
		}

		file_stream.close();
		return content;
	}
	
	bool keyHeld(Key key) {
		const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);
		if(keyboard_state[key.scancode]) {
			return true;
		} else {
			return false;
		}
	}

};
