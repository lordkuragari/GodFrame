#include "GLObjects/GLTexture.h"

GLTexture::GLTexture( GLuint shader, glm::mat4 projection, const char * filePath ) {
	shader_Program = shader;
	m4_mvp = m4_move = m4_rotate = m4_scale = glm::mat4( 1.f );
	m4_projection = projection;

	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );

	float vertices[] = {
		-0.5, 0.5,
		0.5, 0.5,
		0.5, -0.5,
		-0.5, -0.5,
	};

	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

	GLuint elements[] = {
		0, 1, 2,
		0, 2, 3,
	};

	glGenBuffers( 1, &ebo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( elements ), elements, GL_STATIC_DRAW );

	uvData[0] = 0.f;
	uvData[1] = 0.f;
	uvData[2] = 1.f;
	uvData[3] = 0.f;
	uvData[4] = 1.f;
	uvData[5] = 1.f;
	uvData[6] = 0.f;
	uvData[7] = 1.f;

	glGenBuffers( 1, &uvo );
	glBindBuffer( GL_ARRAY_BUFFER, uvo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( uvData ), uvData, GL_DYNAMIC_DRAW );

	texture = SOIL_load_OGL_texture( filePath, SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT );

	Scale( 64.f, 64.f );
	Color( 1.f, 1.f, 1.f, 1.f );
}
GLTexture::~GLTexture( void ) {
	glDeleteTextures( 1, &texture );
	glDeleteBuffers( 1, &uvo );
	glDeleteBuffers( 1, &ebo );
	glDeleteBuffers( 1, &vbo );
	glDeleteVertexArrays( 1, &vao );
}
void GLTexture::Render( void ) {
	assert( glIsProgram( shader_Program ) == GL_TRUE );

	m4_mvp = m4_projection * m4_move * m4_rotate * m4_scale;

	glUseProgram( shader_Program );
	glBindVertexArray( vao );

	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	GLuint positionAttrib = glGetAttribLocation( shader_Program, "v2_position" );
	glEnableVertexAttribArray( positionAttrib );
	glVertexAttribPointer( positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0 );

	glBindBuffer( GL_ARRAY_BUFFER, uvo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( uvData ), uvData, GL_DYNAMIC_DRAW );

	GLuint textureAttrib = glGetAttribLocation( shader_Program, "v2_texcoord" );
	glEnableVertexAttribArray( textureAttrib );

	CheckGLError( "test" );
	glVertexAttribPointer( textureAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0 );

	glBindTexture( GL_TEXTURE_2D, texture );
	glActiveTexture( GL_TEXTURE0 );
	glUniform1i( glGetUniformLocation( shader_Program, "texID" ), 0 );

	glUniformMatrix4fv( glGetUniformLocation( shader_Program, "m4_mvp" ), 1, GL_FALSE, glm::value_ptr( m4_mvp ) );
	glUniform4fv( glGetUniformLocation( shader_Program, "v4_color" ), 1, glm::value_ptr( v4_color ) );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glDisableVertexAttribArray( positionAttrib );
	glDisableVertexAttribArray( textureAttrib );

	CheckGLError( "GLTexture::Render - end" );
}
void GLTexture::Rotate( float radians ) {
	m4_rotate = glm::rotate( glm::mat4( ), radians, glm::vec3( 0.f, 0.f, 1.f ) );
}
void GLTexture::Scale( float scale_x, float scale_y ) {
	glm::vec3 v3_scale = glm::vec3( scale_x, scale_y, 1.f );
	m4_scale = glm::scale( glm::mat4( ), v3_scale );
}
void GLTexture::Move( float move_X, float move_y ) {
	glm::vec3 v3_position = glm::vec3( move_X, move_y, 1.f );
	m4_move = glm::translate( glm::mat4( ), v3_position );
}
void GLTexture::Color( float color_r, float color_g, float color_b, float color_a ) {
	v4_color = glm::vec4( color_r, color_g, color_b, color_a );
}
void GLTexture::UV( float x, float y, float w, float h ) {
	uvData[0] = x;
	uvData[1] = y;
	uvData[2] = x + w;
	uvData[3] = y;
	uvData[4] = x + w;
	uvData[5] = y + h;
	uvData[6] = x;
	uvData[7] = y + h;
}