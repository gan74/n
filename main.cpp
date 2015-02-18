#define ALL
#ifdef ALL
#include <iostream>

#include <n/core/Timer.h>

#include <n/math/Plane.h>
#include <n/io/File.h>

#include <SDL2/SDL.h>
#include <n/graphics/ImageLoader.h>
#include <n/graphics/GLContext.h>
#include <n/graphics/Texture.h>
#include <n/graphics/Scene.h>
#include <n/graphics/GL.h>
#include <n/graphics/StaticBuffer.h>
#include <n/graphics/TriangleBuffer.h>
#include <n/graphics/VertexArrayObject.h>
#include <n/graphics/ShaderCombinaison.h>
#include <n/graphics/Camera.h>
#include <n/graphics/StaticMesh.h>
#include <n/graphics/Material.h>
#include <n/graphics/MeshLoader.h>
#include <n/graphics/MaterialLoader.h>

#include <n/math/StaticConvexVolume.h>
#include <n/math/ConvexVolume.h>

using namespace n;
using namespace n::graphics;
using namespace n::math;
using namespace n::core;

SDL_Window *createWindow() {
	SDL_Window *mainWindow = 0;
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		fatal("Unable to initialize SDL");
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	if(!(mainWindow = SDL_CreateWindow("n 2.1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN))) {
		fatal("Unable to create window");
	}
	SDL_GL_CreateContext(mainWindow);
	SDL_GL_SetSwapInterval(0);

	GLContext::getContext();

	return mainWindow;
}

bool run(SDL_Window *mainWindow) {
	SDL_GL_SwapWindow(mainWindow);
	SDL_Event e;
	bool cc = true;
	while(SDL_PollEvent(&e)) {
		if(e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
			cc = false;
			break;
		}
	}
	return cc;
}

class Cube : public StaticMesh
{
	public:
		Cube() : StaticMesh(MeshLoader::load<String>("sphere.obj")) {
			axis = ((Vec3(random(), random(), random()) - 0.5) * 2).normalized();
		}

		virtual void render(RenderQueue &qu) override {
			static Timer timer;
			static double x = 0;
			double t = timer.reset();
			x += t;
			Quaternion<> q = Quaternion<>::fromAxisAngle(axis.cross(Vec3(1, 0, 0)).cross(axis), t);
			axis = q(axis);
			setRotation(Quaternion<>::fromAxisAngle(axis, x));
			if(!getMeshInstance().isValid()) {
				fatal("Unable to load mesh");
			}
			StaticMesh::render(qu);
		}

	private:
		Vec3 axis;
};

int main(int, char **) {

	SDL_Window *win = createWindow();

	Shader<VertexShader> vert("#version 420 core\n"
										"layout(location = 0) in vec3 n_VertexPosition;"
										"layout(location = 1) in vec3 n_VertexNormal;"
										"uniform mat4 n_ViewProjectionMatrix;"
										"uniform mat4 n_ModelMatrix;"
										"uniform vec3 n_Camera;"

										"out vec3 normal;"
										"out vec3 view;"

										"void main() {"
											"vec4 model = n_ModelMatrix * vec4(n_VertexPosition, 1.0);"
											"gl_Position = n_ViewProjectionMatrix * model;"
											"view = n_Camera - model.xyz;"
											"normal = (n_ModelMatrix * vec4(n_VertexNormal, 1.0)).xyz;"
										"}");

	Shader<FragmentShader> frag("#version 420 core\n"
										"layout(location = 0) out vec4 n_FragColor;"
										"in vec3 normal;"
										"in vec3 view;"
										"uniform vec4 n_Color;"
										"uniform float n_Roughness;"

										"void main() {"
											"vec3 L = normalize(vec3(0, 0, 1));"
											"float o2 = n_Roughness * n_Roughness;"
											"float A = 1.0 - 0.5 * o2 / (o2 + 0.33);"
											"float B = (0.45 * o2) / (o2 + 0.09);"
											"float vn = dot(view, normal);"
											"float ln = dot(L, normal);"
											"float I = max(0.0, ln);"
											"float avn = acos(vn);"
											"float aln = acos(ln);"
											"float diff = max(0.0, dot(normalize(view - normal * vn), normalize(L - normal * ln)));"
											"float alpha = max(avn, aln);"
											"float beta = min(avn, aln);"
											"n_FragColor = n_Color * I * A + B * diff * sin(alpha) * tan(beta);"
											//"n_FragColor = n_Color * dot(L, normal);"

										"}");

	ShaderCombinaison shader(&frag, &vert, 0);
	if(!shader.isValid()) {
		std::cerr<<shader.getLogs()<<std::endl;
		std::cerr<<frag.getLogs()<<std::endl;
		std::cerr<<vert.getLogs()<<std::endl;
		fatal("Unable to compile shader.");
	}

	shader.bind();

	Camera cam;
	cam.setPosition(Vec3(0, 0, 2));
	shader["n_Camera"] = cam.getPosition();
	cam.setRotation(Quaternion<>::fromEuler(0, toRad(90), 0));

	GLContext::getContext()->setProjectionMatrix(cam.getProjectionMatrix());
	GLContext::getContext()->setViewMatrix(cam.getViewMatrix());
	Scene scene;
	/*for(uint i = 0; i != 1000; i++) {
		Cube *cube = new Cube();
		cube->setPosition(Vec3(random(), random(), random()) * 100 - 50);
		scene.insert(cube);
	}*/

	scene.insert(new Cube());
	Timer timer;
	while(run(win)) {
		gl::glClear(GL_COLOR_BUFFER_BIT);
		core::Array<StaticMesh *> meshes = scene.query<StaticMesh>(cam);
		RenderQueue queue;
		for(Renderable *m : meshes) {
			m->render(queue);
		}
		for(const auto &q : queue.getBatches()) {
			q();
		}
		GLContext::getContext()->processTasks();
		if(GLContext::checkGLError()) {
			fatal("GL error");
		}
		shader["n_Roughness"] = (1 + sin(timer.elapsed())) / 2;
		gl::glFlush();
	}
	return 0;
}


#else

#include <n/core/Array.h>

int main(int, char **) {
	return 0;
}

#endif

