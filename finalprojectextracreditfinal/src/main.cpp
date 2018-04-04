/* Lab 6 base code - transforms using local matrix functions
	to be written by students -
	based on lab 5 by CPE 471 Cal Poly Z. Wood + S. Sueda
	& Ian Dunn, Christian Eckhardt
*/
#include <iostream>
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "math.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "camera.h"
// used for helper in perspective
#include "glm/glm.hpp"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;






class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	float w;
	std::shared_ptr<Program> prog, prog2;
	std::shared_ptr<Program> prog3, prog4, prog5, prog6, prog7;

	// Shape to be used (from obj file)
	shared_ptr<Shape> shape;
	shared_ptr<Shape> skysphere;
	shared_ptr<Shape> shape3, shape4, shape5, shape6, shape7;
	
	//camera
	camera mycam;

	//texture for sim
	GLuint Texture,TextureSky;
	GLuint Texture2;
	GLuint Texture3;
	GLuint FBOtex, fb, depth_rb;
	GLuint VertexArrayIDBox, VertexBufferIDBox, VertexBufferPart, VertexBufferTex;
	// Contains vertex information for OpenGL
	GLuint VertexArrayID;
	float x = 0;
	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			mycam.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			mycam.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			mycam.a = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			mycam.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			mycam.d = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			mycam.d = 0;
		}
		if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		{
			mycam.q = 1;
		}
		if (key == GLFW_KEY_Q && action == GLFW_RELEASE)
		{
			mycam.q = 0;
		}
		if (key == GLFW_KEY_E && action == GLFW_PRESS)
		{
			mycam.e = 1;
		}
		if (key == GLFW_KEY_E && action == GLFW_RELEASE)
		{
			mycam.e= 0;
		}
		if (key == GLFW_KEY_Z && action == GLFW_PRESS)
		{
			mycam.z = 1;
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE)
		{
			mycam.z = 0;
		}
		if (key == GLFW_KEY_X && action == GLFW_PRESS)
		{
			mycam.x = 1;
		}
		if (key == GLFW_KEY_X && action == GLFW_RELEASE)
		{
			mycam.x = 0;
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string& resourceDirectory)
	{


		GLSL::checkVersion();

		
		// Set background color.
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		//culling:
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);

		//transparency
		glEnable(GL_BLEND);
		//next function defines how to mix the background color with the transparent pixel in the foreground. 
		//This is the standard:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

		// Initialize the GLSL program.
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/simple_vertsky.glsl", resourceDirectory + "/simple_fragsky.glsl");
		if (! prog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("camPos");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addAttribute("vertTex");
		
		
		
		
		//second prog for skysphere

		// Initialize the GLSL program.
		prog2 = make_shared<Program>();
		prog2->setVerbose(true);
		prog2->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		if (!prog2->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog2->init();
		prog2->addUniform("P");
		prog2->addUniform("V");
		prog2->addUniform("M");
		prog2->addUniform("w");
		prog2->addUniform("x");
		prog2->addUniform("camPos");
		prog2->addUniform("offset");
		prog2->addAttribute("vertPos");
		prog2->addAttribute("vertNor");
		prog2->addAttribute("vertTex");
		

		prog3 = make_shared<Program>();
		prog3->setVerbose(true);
		prog3->setShaderNames(resourceDirectory + "/simple_vertobj.glsl", resourceDirectory + "/simple_fragobj.glsl");
		if (!prog3->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog3->init();
		prog3->addUniform("P");
		prog3->addUniform("V");
		prog3->addUniform("M");
		prog3->addUniform("camPos");
		prog3->addAttribute("vertPos");
		prog3->addAttribute("vertNor");
		prog3->addAttribute("vertTex");



		prog4 = make_shared<Program>();
		prog4->setVerbose(true);
		prog4->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		if (!prog4->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog4->init();
		prog4->addUniform("P");
		prog4->addUniform("V");
		prog4->addUniform("M");
		prog4->addUniform("offset");
		prog4->addAttribute("vertPos");
		prog4->addAttribute("vertNor");
		prog4->addAttribute("vertTex");


		


	}

	void initGeom(const std::string& resourceDirectory)
	{


		//generate the VAO
		glGenVertexArrays(1, &VertexArrayIDBox);
		glBindVertexArray(VertexArrayIDBox);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferIDBox);

		GLfloat *ver= new GLfloat[10000 * 18];
			// front
		int verc = 0;
		float scale = 0.0;
		float factor = .104;
		//.104 is the division of 0 to 100 from 0 to 2pi.
			for (int i = 0; i < 100; i++) {
				scale = 0.0;
				for (int j = 0; j < 100; j++) {
					 
						ver[verc++] = 0.0 + j, ver[verc++] = 0.0 + i, ver[verc++] = 0.0; // ld
						ver[verc++] = 1.0 + j, ver[verc++] = 0.0 + i, ver[verc++] = 0.0; //rd
						ver[verc++] = 0.0 + j, ver[verc++] = 1.0 + i, ver[verc++] = 0.0;//lu
						ver[verc++] = 1.0 + j, ver[verc++] = 0.0 + i, ver[verc++] = 0.0;//rd
						ver[verc++] = 1.0 + j, ver[verc++] = 1.0 + i, ver[verc++] = 0.0;//ru
						ver[verc++] = 0.0 + j, ver[verc++] = 1.0 + i, ver[verc++] = 0.0;//lu
					
				}

		}
			
			
		
		
		
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, 10000*18*sizeof(float), ver, GL_STATIC_DRAW);
		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);



		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferPart);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferPart);
	
		GLfloat *cube_part = new GLfloat[10000 * 18];
		int texc = 0;
		for (int i = 0; i < 100; i++) {//row
			for (int j = 0; j < 100; j++) {//column
				int u = 0;
				float participation_y_factor = (float)j / 100. ;
				float participation_y_factor_1 = ((float)j + 1.) / 100. ;
				//participation_y_factor_1 = 1;
				//participation_y_factor = 1;
				float participation_x = (float)i / 100.;//0..1
				participation_x *= 3.1415926; //0..PI
				participation_x = -cos(participation_x + 3.1415926/2);//-1 .. 1
				participation_x = (participation_x + 1) / 2.; // 0..1
			
				float participation_x_1 = ((float)i+1) / 100.;//0..1
				participation_x_1 *= 3.1415926; //0..PI
				participation_x_1 = -cos(participation_x_1 + 3.1415926 / 2);//-1 .. 1
				participation_x_1 = (participation_x_1 + 1) / 2.; // 0..1
			/*	cube_part[texc++] = participation_x * participation_y_factor; cube_part[texc++] = participation_x; cube_part[texc++] = 0;//ld
				cube_part[texc++] = participation_x_1 * participation_y_factor_1; cube_part[texc++] = participation_x_1; cube_part[texc++] = 0;//rd
				cube_part[texc++] = participation_x * participation_y_factor; cube_part[texc++] = participation_x; cube_part[texc++] = 0;//lu
				cube_part[texc++] = participation_x_1 * participation_y_factor_1; cube_part[texc++] = participation_x_1; cube_part[texc++] = 0;//rd
				cube_part[texc++] = participation_x_1 * participation_y_factor_1; cube_part[texc++] = participation_x_1; cube_part[texc++] = 0;//ru
				cube_part[texc++] = participation_x * participation_y_factor; cube_part[texc++] = participation_x; cube_part[texc++] = 0;//lu			*/
			

				float wavepart = 10;
				float fi = ((float)max(0, j - (100- (int)wavepart)))/ wavepart;
				fi *= 2.0 * 3.14159265358;
				fi -= 3.14159265358;
				float fi_0 = (cos(fi) + 1.) / 2.0;
				fi = ((float)max(0, j - (100 - (int)wavepart) + 1)) / wavepart;
				fi *= 2.0 * 3.14159265358;
				fi -= 3.14159265358;
				float fi_1 = (cos(fi) + 1.) / 2.0;

				

				cube_part[texc++] = participation_x * participation_y_factor; cube_part[texc++] = participation_x; cube_part[texc++] = fi_0;//ld
				cube_part[texc++] = participation_x * participation_y_factor_1; cube_part[texc++] = participation_x; cube_part[texc++] = fi_1;//rd

				cube_part[texc++] = participation_x_1 * participation_y_factor; cube_part[texc++] = participation_x_1; cube_part[texc++] = fi_0;//lu
				
				cube_part[texc++] = participation_x * participation_y_factor_1; cube_part[texc++] = participation_x; cube_part[texc++] = fi_1;//rd

				cube_part[texc++] = participation_x_1 * participation_y_factor_1; cube_part[texc++] = participation_x_1; cube_part[texc++] = fi_1;//ru
				
				cube_part[texc++] = participation_x_1 * participation_y_factor; cube_part[texc++] = participation_x_1; cube_part[texc++] = fi_0;//lu			
			}
		}

		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, 10000 * 18 * sizeof(float), cube_part, GL_STATIC_DRAW);
		//we need to set up the vertex array
		glEnableVertexAttribArray(1);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferTex);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferTex);

		float t = 1. / 100.;
		GLfloat *cube_tex=new GLfloat[10000 * 18];
		texc = 0;
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < 100; j++) {
				int u = 0;
				cube_tex[texc++] = (GLfloat)j*t, cube_tex[texc++] = (GLfloat)i*t;
				cube_tex[texc++] = (GLfloat)(j + 1)*t, cube_tex[texc++] = (GLfloat)i*t;
				cube_tex[texc++] = (GLfloat)j*t, cube_tex[texc++] = (GLfloat)(i + 1)*t;
				cube_tex[texc++] = (GLfloat)(j + 1)*t, cube_tex[texc++] = 0.0 + (GLfloat)i*t;
				cube_tex[texc++] = (GLfloat)(j + 1)*t, cube_tex[texc++] = (GLfloat)(i + 1)*t;
				cube_tex[texc++] = (GLfloat)j*t, cube_tex[texc++] = (GLfloat)(i + 1)*t;
			}
		}

		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, 10000*12*sizeof(float), cube_tex, GL_STATIC_DRAW);
		//we need to set up the vertex array
		glEnableVertexAttribArray(2);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);


		// Initialize mesh.
		shape = make_shared<Shape>();
		shape->loadMesh(resourceDirectory + "/T-Rex Model.obj");
		shape->resize();
		shape->init();

		// Initialize mesh.
		skysphere = make_shared<Shape>();
		skysphere->loadMesh(resourceDirectory + "/sphere.obj");
		skysphere->resize();
		skysphere->init();

		shape3 = make_shared<Shape>();
		shape3->loadMesh(resourceDirectory + "/Astronaut.obj");
		shape3->resize();
		shape3->init();
		
		int width, height, channels;
		char filepath[1000];
		string str = resourceDirectory + "/height.png";
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		//texture sky
		/*string str = resourceDirectory + "/height.png";
		strcpy(filepath, str.c_str());
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &TextureSky);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureSky);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);*/

		//texture 1
		str = resourceDirectory + "/water.jpg";
		strcpy(filepath, str.c_str());		
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		//texture 2
		str = resourceDirectory + "/normal1.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture2);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		//astronaut texture
		str = resourceDirectory + "/normal2.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture3);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, Texture3);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		//[TWOTEXTURES]
		//set the 2 textures to the correct samplers in the fragment shader:
		GLuint Tex1Location = glGetUniformLocation(prog2->pid, "tex");//tex, tex2... sampler in the fragment shader
		GLuint Tex2Location = glGetUniformLocation(prog2->pid, "tex2");
		GLuint Tex3Location = glGetUniformLocation(prog2->pid, "tex3");
		// Then bind the uniform samplers to texture units:
		glUseProgram(prog2->pid);
		glUniform1i(Tex1Location, 0);
		glUniform1i(Tex2Location, 1);
		glUniform1i(Tex3Location, 2);
		glUseProgram(prog3->pid);
		glUniform1i(Tex1Location, 0);
		glUniform1i(Tex2Location, 1);
		
		
	}

	void render()
	{
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		// Get current frame buffer size.
		//glBindFramebuffer(GL_FRAMEBUFFER, fb);
		//glClearColor(0.0, 0.0, 0.0, 0.0);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width / (float)height;
		glViewport(0, 0, width, height);
		glm::vec2 offset(0, 0);
		auto P = std::make_shared<MatrixStack>();
		auto MV = std::make_shared<MatrixStack>();
		P->pushMatrix();	
		P->perspective(70., width, height, 0.1, 1000.0f);
		glm::mat4 M ,T, R;		
		glm:mat4 V = mycam.process();		

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		float pih = 3.1415926 / 2.0;
		// Draw mesh using GLSL
		prog->bind();		
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		
		glDisable(GL_DEPTH_TEST);
		glFrontFace(GL_CW);
		pih = 3.1415926 / 2.0;
		R = glm::rotate(glm::mat4(1), pih, glm::vec3(1, 0, 0)); 
		T = glm::translate(glm::mat4(1),-mycam.pos);
		M = T * R;
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		skysphere->draw(prog);
		glEnable(GL_DEPTH_TEST);
		glFrontFace(GL_CCW);			
		prog->unbind();	
		prog2->bind();
		offset.x = floor(-mycam.pos.x);
		offset.y = floor(mycam.pos.z);
		glUniform2fv(prog2->getUniform("offset"), 1, &offset[0]);
		glBindTexture(GL_TEXTURE_2D, Texture);			
		glm::mat4 RV = mycam.getR();
		
		x = -sin(glfwGetTime());
		T = glm::translate(glm::mat4(1), glm::vec3(-50, -2,50));
		RV = glm::transpose(RV);
		pih = -3.1415926 / 2.0;
		R = glm::rotate(glm::mat4(1), pih, glm::vec3(1, 0, 0));
		glm::mat4 S = glm::scale(glm::mat4(1), glm::vec3(0.3, 0.3, 1));
		M = T * R * S;
		w += 0.05;// sin(glfwGetTime()) / 10;
		glUniform3f(prog2->getUniform("camPos"), mycam.pos.x, mycam.pos.y, mycam.pos.z);
		glUniformMatrix4fv(prog2->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
		glUniformMatrix4fv(prog2->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog2->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform1f(prog2->getUniform("w"), w);
		glUniform1f(prog2->getUniform("x"), x);
		glBindVertexArray(VertexArrayIDBox);

		glDrawArrays(GL_TRIANGLES, 0, 10000*6);
		
		
		prog2->unbind();
		
		
	}
};
//*********************************************************************************************************
int main(int argc, char **argv)
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}


	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(resourceDir);

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();
		//application->fbrender();
		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}