#include <gtc/type_ptr.hpp>
#include <iostream>
//#include <string>
//#include <chrono>
#include <gtc/constants.hpp>
#include <gtc/matrix_transform.hpp>

#include "main.hpp"
//#include "opengl.hpp"

using namespace std;
using namespace glm;


void basic_model::draw(const glm::mat4& view, const glm::mat4 proj) {
	mat4 modelview = view * modelTransform;

	glUseProgram(shader); // load shader and variables
	glUniform1i(glGetUniformLocation(shader, "textureMap"), 0);
	glUniform1i(glGetUniformLocation(shader, "normalMap"), 3);
	glUniformMatrix4fv(glGetUniformLocation(shader, "uProjectionMatrix"), 1, false, value_ptr(proj));
	glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false, value_ptr(modelview));
	glUniform3fv(glGetUniformLocation(shader, "uColor"), 1, value_ptr(color));

	mesh.draw(); // draw
}

void drawAxis(const glm::mat4& view, const glm::mat4& proj) {

	const char* axis_shader_source = R"(
	#version 330 core
	uniform mat4 uProjectionMatrix;
	uniform mat4 uModelViewMatrix;
#ifdef _VERTEX_
	flat out int v_instanceID;
	void main() {
		v_instanceID = gl_InstanceID;
	}
#endif
#ifdef _GEOMETRY_
	layout(points) in;
	layout(line_strip, max_vertices = 2) out;
	flat in int v_instanceID[];
	out vec3 v_color;
	const vec3 dir[] = vec3[](
		vec3(1, 0, 0),
		vec3(-.5, 0, 0),
		vec3(0, 1, 0),
		vec3(0, -.5, 0),
		vec3(0, 0, 1),
		vec3(0, 0, -.5)
	);
	void main() {
		v_color = abs(dir[v_instanceID[0]]);
		gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(0.0, 0.0, 0.0, 1.0);
		EmitVertex();
		v_color = abs(dir[v_instanceID[0]]);
		gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(normalize(dir[v_instanceID[0]]) * 1000, 1.0);
		EmitVertex();
		EndPrimitive();
	}
#endif
#ifdef _FRAGMENT_
	in vec3 v_color;
	out vec3 f_color;
	void main() {
		gl_FragDepth = gl_FragCoord.z - 0.000001;
		f_color = v_color;
	}
#endif)";
	static GLuint axis_shader = 1;
	/*if (!axis_shader) {
		mesh_data::shader_builder prog;
		prog.set_shader_source(GL_VERTEX_SHADER, axis_shader_source);
		prog.set_shader_source(GL_GEOMETRY_SHADER, axis_shader_source);
		prog.set_shader_source(GL_FRAGMENT_SHADER, axis_shader_source);
		axis_shader = prog.build();
	}*/

	glUseProgram(axis_shader);
	glUniformMatrix4fv(glGetUniformLocation(axis_shader, "uProjectionMatrix"), 1, false, value_ptr(proj));
	glUniformMatrix4fv(glGetUniformLocation(axis_shader, "uModelViewMatrix"), 1, false, value_ptr(view));
	//mesh_data::draw_dummy(6);
}


void drawGrid(const glm::mat4& view, const glm::mat4& proj) {

	const char* grid_shader_source = R"(
	#version 330 core
	uniform mat4 uProjectionMatrix;
	uniform mat4 uModelViewMatrix;
#ifdef _VERTEX_
	flat out int v_instanceID;
	void main() {
		v_instanceID = gl_InstanceID;
	}
#endif
#ifdef _GEOMETRY_
	layout(points) in;
	layout(line_strip, max_vertices = 2) out;
	flat in int v_instanceID[];
	void main() {
		gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(v_instanceID[0] - 10, 0, -10, 1);
		EmitVertex();
		gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(v_instanceID[0] - 10, 0, 10, 1);
		EmitVertex();
		EndPrimitive();
	}
#endif
#ifdef _FRAGMENT_
	out vec3 f_color;
	void main() {
		f_color = vec3(0.5, 0.5, 0.5);
	}
#endif)";
	static GLuint grid_shader = 1;
	/*if (!grid_shader) {
		mesh_data::shader_builder prog;
		prog.set_shader_source(GL_VERTEX_SHADER, grid_shader_source);
		prog.set_shader_source(GL_GEOMETRY_SHADER, grid_shader_source);
		prog.set_shader_source(GL_FRAGMENT_SHADER, grid_shader_source);
		grid_shader = prog.build();
	}*/

	const glm::mat4 rot = glm::rotate(glm::mat4(1), glm::pi<float>() / 2.f, glm::vec3(0, 1, 0));

	glUseProgram(grid_shader);
	glUniformMatrix4fv(glGetUniformLocation(grid_shader, "uProjectionMatrix"), 1, false, value_ptr(proj));
	glUniformMatrix4fv(glGetUniformLocation(grid_shader, "uModelViewMatrix"), 1, false, value_ptr(view));
	//mesh_data::draw_dummy(21);
	glUniformMatrix4fv(glGetUniformLocation(grid_shader, "uModelViewMatrix"), 1, false, value_ptr(view * rot));
	//mesh_data::draw_dummy(21);
}

void render() {
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);


	// TODO: change to actual model
	// coordinates of a triangle
	static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f,
	};

	// This will identify our vertex buffer
	GLuint vertexbuffer;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	// draw triangle on screen
	glDrawArrays(GL_TRIANGLES, 0, 3); // 3 verticies starting from 0
	glDisableVertexAttribArray(0);

	

	// draw the model
	//m_model.draw(view, proj); // TODO: set model so model can be drawn
}

int main() {
	if (!glfwInit()) {
		std::cout << "failed to initialise glfw";
		return -1;
	}

	// set up window
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window;
	window = glfwCreateWindow(700, 600, "OpenGL", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	
	glfwInit();

	glfwMakeContextCurrent(window);

	// check for escape key
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	gladLoadGL(); // make sure glad works with opengl

	do {
		// Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
		glClear(GL_COLOR_BUFFER_BIT);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);


	render();

    return 0;
}