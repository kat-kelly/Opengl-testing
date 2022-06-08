#pragma once

//#include <GL/glew.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <iostream>

#include "mesh.hpp"

struct basic_model {
	GLuint shader = 0;
	mesh_data::gl_mesh mesh;// = mesh_data::sphere_latlong(m_subdiv).build();
	glm::vec3 color{ 1.0 };
	glm::mat4 modelTransform{ 1.0 };
	GLuint texture;

	void draw(const glm::mat4& view, const glm::mat4 proj);
};

// subdivisions
int m_subdiv = 10;

// orientation of viewspace and object
glm::mat4 view { 0 };
glm::mat4 proj { 0 };

// geometry
basic_model m_model;