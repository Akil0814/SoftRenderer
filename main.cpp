#include <iostream>
#include <chrono>
#include <Windows.h>

#include "MAIframework/geometry.h"

#include "application/application.h"
#include "application/camera.h"

#include "MAI_SGL/data_structures.h"

#include "MAI_SGL/shader/color_shader.h"
#include "MAI_SGL/shader/texture_shader.h"

#include "MAI_SGL/core/gpu.h"

#include "MAI_SGL/resource/texture.h"
#include "MAI_SGL/resource/image.h"

#include "MAI_SGL/math/math.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup" )//更改main入口

Camera* camera = nullptr;

uint32_t window_width = 1080;
uint32_t window_height = 720;

mai::Image* image = nullptr;
mai::Geometry* test_geometry= nullptr;

uint32_t texture = 0;

//三个属性对应vbo
uint32_t position_vbo = 0;
uint32_t color_vbo = 0;
uint32_t uv_vbo = 0;

//三角形的indices
uint32_t ebo = 0;

//本三角形专属vao
uint32_t vao = 0;

mai::TextureShader* texture_shader = nullptr;
mai::ColorShader* color_shader = nullptr;

mai::mat4f model_matrix;
mai::mat4f view_matrix;
mai::mat4f perspective_matrix;

float angle = 0.0f;
float camera_pos = 5.0f;
float speed = 0.01;

float camera_z = 3.0f;
float delta_time = 0.0f;
float current_fps = 0.0f;
float smoothed_fps = 0.0f;

constexpr float rotation_speed = 0.6f;
constexpr float camera_speed = 0.6f;
constexpr float max_delta_time = 0.1f;
constexpr float fps_smoothing = 0.1f;

void transform(float delta_time)
{
	angle += rotation_speed * delta_time;
	camera_z -= camera_speed * delta_time;
	//模型变换
	model_matrix = mai::rotate(mai::mat4f(1.0f), angle, mai::vec3f{ 0.0f, 1.0f, 0.0f });
}

void on_render(float delta_time)
{
	transform(delta_time);

	texture_shader->_model_matrix = model_matrix;
	texture_shader->_view_matrix = view_matrix;
	texture_shader->_projection_matrix = perspective_matrix;
	texture_shader->_diffuse_texture = texture;

	color_shader->_model_matrix = model_matrix;
	color_shader->_view_matrix = view_matrix;
	color_shader->_projection_matrix = perspective_matrix;

	MAI_SGL->clear();

	MAI_SGL->bind_vertex_array(vao);
	MAI_SGL->bind_buffer(MAI_ELEMENT_ARRAY_BUFFER, ebo);

	MAI_SGL->use_program(texture_shader);
	MAI_SGL->draw_element(MAI_DRAW_TRIANGLES, 0, 6);

	MAI_SGL->use_program(color_shader);
	MAI_SGL->draw_element(MAI_DRAW_TRIANGLES, 6, 3);

	//MAI_SGL->bind_vertex_array(test_geometry->get_VAO());
	//MAI_SGL->bind_buffer(MAI_ELEMENT_ARRAY_BUFFER, test_geometry->get_EBO());
	//MAI_SGL->use_program(color_shader);
	//MAI_SGL->draw_element(MAI_DRAW_TRIANGLES, 0, test_geometry->get_indices_count());

}

void prepare()
{

	camera = new Camera(60.0f, (float)window_width / (float)window_height, 0.1f, 100.0f, { 0.0f, 1.0f, 0.0f });
	MAI_APP->set_camera(camera);

	texture_shader = new mai::TextureShader();
	color_shader = new mai::ColorShader();

	test_geometry = mai::Geometry::create_box(1.0f);

	image = mai::Image::create_image("assets/textures/mai.png");
	if (image == nullptr)
		std::cerr << "false" << std::endl;

	if (image != nullptr)
	{
		texture = MAI_SGL->get_texture();
		MAI_SGL->bind_texture(texture);
		MAI_SGL->tex_image_2D(image->_width, image->_height, image->_data);
		MAI_SGL->tex_parameter(MAI_TEXTURE_FILTER, MAI_TEXTURE_FILTER_LINEAR);
		MAI_SGL->tex_parameter(MAI_TEXTURE_WRAP_U, MAI_TEXTURE_WRAP_REPEAT);
		MAI_SGL->tex_parameter(MAI_TEXTURE_WRAP_V, MAI_TEXTURE_WRAP_REPEAT);
		MAI_SGL->bind_texture(0);
	}


	perspective_matrix = mai::perspective(60.0f, (float)window_width / (float)window_height, 0.1f, 100.0f);

	auto camera_model_matrix = mai::translate(mai::mat4f(1.0f), mai::vec3f{ 0.0f, 0.0f, 3.0f });
	view_matrix = mai::inverse(camera_model_matrix);



	MAI_SGL->disable(MAI_CULL_FACE);
	MAI_SGL->enable(MAI_BLENDING);
	//MAI_SGL->disable(MAI_DEPTH_TEST);

	float positions[] =
	{
		-0.85f, -0.75f, 0.25f,
		-0.85f,  0.75f, 0.25f,
		 0.65f,  0.75f, 0.25f,
		 0.65f, -0.75f, 0.25f,

		-0.35f, -0.55f, -0.25f,
		0.15f, 0.45f, -0.25f,
		0.70f, -0.25f, -0.25f,
	};

	float colors[] =
	{
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,

		0.2f, 0.4f, 1.0f, 0.3f,
		0.2f, 0.8f, 1.0f, 0.3f,
		0.5f, 1.0f, 0.9f, 0.3f,
	};

	float uvs[] =
	{
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,

		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
	};

	uint32_t indices[] = { 0, 1, 2, 0, 2, 3, 4, 5, 6 };

	//生成indices对应ebo
	ebo = MAI_SGL->gen_buffer();
	MAI_SGL->bind_buffer(MAI_ELEMENT_ARRAY_BUFFER, ebo);
	MAI_SGL->buffer_data(MAI_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 9, indices);
	MAI_SGL->bind_buffer(MAI_ELEMENT_ARRAY_BUFFER, 0);

	//生成vao并且绑定
	vao = MAI_SGL->gen_vertex_array();
	MAI_SGL->bind_vertex_array(vao);

	//生成每个vbo，绑定后，设置属性ID及读取参数
	position_vbo = MAI_SGL->gen_buffer();
	MAI_SGL->bind_buffer(MAI_ARRAY_BUFFER, position_vbo);
	MAI_SGL->buffer_data(MAI_ARRAY_BUFFER, sizeof(float) * 21, positions);
	MAI_SGL->vertex_attribute_pointer(0, 3, 3 * sizeof(float), 0);

	color_vbo = MAI_SGL->gen_buffer();
	MAI_SGL->bind_buffer(MAI_ARRAY_BUFFER, color_vbo);
	MAI_SGL->buffer_data(MAI_ARRAY_BUFFER, sizeof(float) * 28, colors);
	MAI_SGL->vertex_attribute_pointer(1, 4, 4 * sizeof(float), 0);

	uv_vbo = MAI_SGL->gen_buffer();
	MAI_SGL->bind_buffer(MAI_ARRAY_BUFFER, uv_vbo);
	MAI_SGL->buffer_data(MAI_ARRAY_BUFFER, sizeof(float) * 14, uvs);
	MAI_SGL->vertex_attribute_pointer(2, 2, 2 * sizeof(float), 0);

	MAI_SGL->bind_buffer(MAI_ARRAY_BUFFER, 0);
	MAI_SGL->bind_vertex_array(0);

	MAI_SGL->printVAO(vao);
}


int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance, //本应用程序实例句柄，唯一指代当前程序
	_In_opt_ HINSTANCE hPrevInstance, //本程序前一个实例，一般是null
	_In_ LPWSTR lpCmdLine, //应用程序运行参数
	_In_ int nCmdShow) //窗口如何显示（最大化、最小化、隐藏），不需理会
{
	if (!MAI_APP->init(hInstance, 1080, 720))
		return -1;

	//将bmp指向的内存配置到sgl当中
	MAI_SGL->init_surface(MAI_APP->get_width(), MAI_APP->get_height(), MAI_APP->get_canvas());

	prepare();


	bool active = true;
	using Clock = std::chrono::steady_clock;
	auto previous_frame_time = Clock::now();
	while (active)
	{
		auto current_frame_time = Clock::now();
		std::chrono::duration<float> frame_duration = current_frame_time - previous_frame_time;
		previous_frame_time = current_frame_time;

		delta_time = frame_duration.count();
		if (delta_time > max_delta_time)
			delta_time = max_delta_time;

		if (delta_time > 0.0f)
		{
			current_fps = 1.0f / delta_time;
			if (smoothed_fps <= 0.0f)
				smoothed_fps = current_fps;
			else
				smoothed_fps += (current_fps - smoothed_fps) * fps_smoothing;
		}

		active = MAI_APP->peek_message();
		camera->update();
		on_render(delta_time);
		MAI_APP->show();
	}

	MAI_SGL->delete_texture(texture);
	mai::Image::destroy_image(image);

	return 0;
}
