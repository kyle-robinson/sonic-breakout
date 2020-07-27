#include "PostProcessor.h"
#include <iostream>

PostProcessor::PostProcessor(Shader shader, unsigned int width, unsigned int height)
	: PostProcessingShader(shader), Texture(), Width(width), Height(height), Chaos(false), Confuse(false), Shake(false)
{
	glGenFramebuffers(1, &this->MSFBO);
	glGenFramebuffers(1, &this->FBO);
	glGenRenderbuffers(1, &this->RBO);

	// Initialize renderbuffer storage with a multisampled color buffer
	glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::POSPROCESSOR: Failed to initialize MSFBO!" << std::endl;

	// Initialize FBO/texture to blit multisampled color buffer
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	this->Texture.Generate(width, height, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->Texture.ID, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::POSTPROCESSOR: Failed ot initialize FBO!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Initialize render data and uniforms
	this->InitRenderData();
	this->PostProcessingShader.Bind();
	this->PostProcessingShader.SetUniform1i("scene", 0);
	float offset = 1.0f / 300.0f;
	float offsets[9][2] = {
		{ -offset, offset },
		{ 0.0f, offset },
		{ offset, offset },
		{ -offset, 0.0f },
		{ 0.0f, 0.0f },
		{ offset, 0.0f },
		{ -offset, -offset },
		{ 0.0f, -offset },
		{ offset, -offset },
	};
	glUniform2fv(glGetUniformLocation(this->PostProcessingShader.ID, "offsets"), 9, (float*)offsets);
	
	int edge_kernel[9] = {
		-1, -1, -1,
		-1,  8, -1,
		-1, -1, -1
	};
	glUniform1iv(glGetUniformLocation(this->PostProcessingShader.ID, "edge_kernel"), 9, edge_kernel);

	float blur_kernel[9] = {
		1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
		2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
		1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
	};
	glUniform1fv(glGetUniformLocation(this->PostProcessingShader.ID, "blur_kernel"), 9, blur_kernel);
}

void PostProcessor::BeginRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::EndRender()
{
	// Resolve multisampled color-buffer to intermediate FBO to store to texture
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->MSFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);
	glBlitFramebuffer(0, 0, this->Width, this->Height, 0, 0, this->Width, this->Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessor::Render(float time)
{
	this->PostProcessingShader.Bind();
	this->PostProcessingShader.SetUniform1f("time", time);
	this->PostProcessingShader.SetUniform1i("chaos", this->Chaos);
	this->PostProcessingShader.SetUniform1i("confuse", this->Confuse);
	this->PostProcessingShader.SetUniform1i("shake", this->Shake);

	// Render textured quad
	glActiveTexture(GL_TEXTURE0);
	this->Texture.Bind();
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void PostProcessor::InitRenderData()
{
	unsigned int VBO;
	float vertices[] = {
		// positions	tex coords
		-1.0f, -1.0f,	0.0f, 0.0f,
		 1.0f,  1.0f,	1.0f, 1.0f,
		-1.0f,  1.0f,	0.0f, 1.0f,

		-1.0f, -1.0f,	0.0f, 0.0f,
		 1.0f, -1.0f,	1.0f, 0.0f,
		 1.0f,  1.0f,	1.0f, 1.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}