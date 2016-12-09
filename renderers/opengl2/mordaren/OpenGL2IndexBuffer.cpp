#include "OpenGL2IndexBuffer.hpp"

#include "OpenGL2_util.hpp"

#include <GL/glew.h>

OpenGL2IndexBuffer::OpenGL2IndexBuffer(const utki::Buf<std::uint16_t> indices) :
		elementType(GL_UNSIGNED_SHORT),
		elementsCount(indices.size())
{	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffer);
	AssertOpenGLNoError();
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.sizeInBytes(), &*indices.begin(), GL_STATIC_DRAW);
	AssertOpenGLNoError();
	
	//TODO: remove this
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	AssertOpenGLNoError();
}