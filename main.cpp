#include <iostream>
#include <chrono>
#include <iomanip>
#include <string>
#include <Windows.h>

#include "MAI_SGL/data_structures.h"
#include "MAI_SGL/shader/color_shader.h"
#include "MAI_SGL/shader/texture_shader.h"
#include "MAI_SGL/shader/color_shader_2D.h"
#include "MAI_SGL/core/gpu.h"
#include "MAI_SGL/resource/texture.h"
#include "MAI_SGL/resource/image.h"
#include "MAI_SGL/math/math.h"

#include "application/application.h"
#include "application/camera.h"

#include "MAIframework/geometry.h"

#include "ui/imgui_layer.h"

#ifndef SOFTWARE_RENDERING_ROOT_DIR
#define SOFTWARE_RENDERING_ROOT_DIR "."
#endif



#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup" )//更改main入口

Camera* camera = nullptr;

uint32_t window_width = 1080;
uint32_t window_height = 720;

mai::Image* image = nullptr;
uint32_t texture = 0;

mai::Geometry* geometry = nullptr;

mai::TextureShader* texture_shader = nullptr;
mai::ColorShader* color_shader = nullptr;
mai::ColorShader2D* color_shader_2d = nullptr;

mai::mat4f model_matrix;

float angle = 0.0f;
float delta_time = 0.0f;
float current_fps = 0.0f;
float smoothed_delta_time = 0.0f;
float smoothed_fps = 0.0f;

constexpr float rotation_speed = 0.6f;
constexpr float max_delta_time = 0.1f;
constexpr float fps_smoothing = 0.1f;
constexpr uint32_t fps_print_interval = 60;


void transform(float delta_time)
{
	angle += rotation_speed * delta_time;
	model_matrix = mai::rotate(mai::mat4f(1.0f), angle, mai::vec3f{ 0.0f, 1.0f, 0.0f });
}

void on_render(float delta_time)
{
	//transform(delta_time);
	
	const mai::mat4f& view_matrix = camera->get_view_matrix();
	const mai::mat4f& projection_matrix = camera->get_projection_matrix();

	texture_shader->_model_matrix = model_matrix;
	texture_shader->_view_matrix = view_matrix;
	texture_shader->_projection_matrix = projection_matrix;
	texture_shader->_diffuse_texture = texture;


	MAI_SGL->clear();
	MAI_SGL->draw_dimension(MAI_DRAW_3D);
	MAI_SGL->enable(MAI_DEPTH_TEST);

	MAI_SGL->bind_vertex_array(geometry->get_VAO());
	MAI_SGL->bind_buffer(MAI_ELEMENT_ARRAY_BUFFER, geometry->get_EBO());

	MAI_SGL->use_program(texture_shader);
	MAI_SGL->draw_element(MAI_DRAW_TRIANGLES, 0, geometry->get_indices_count());
}

bool prepare()
{
	if (!init_imgui_for_MAI_SGL())
		return false;

	//init camera
	camera = new Camera(60.0f, (float)window_width / (float)window_height, 0.1f, 100.0f, { 0.0f, 1.0f, 0.0f });
	camera->set_position({ 0.0f, 0.0f, 3.0f });
	MAI_APP->set_camera(camera);

	//init texture
	texture_shader = new mai::TextureShader();
	image = mai::Image::create_image("assets/textures/mai.png");
	if (image == nullptr)
		std::cerr << "false" << std::endl;
	if (image != nullptr)
	{
		texture = MAI_SGL->gen_texture();
		MAI_SGL->bind_texture(texture);
		MAI_SGL->tex_image_2D(image->_width, image->_height, image->_data);
		MAI_SGL->tex_parameter(MAI_TEXTURE_FILTER, MAI_TEXTURE_FILTER_LINEAR);
		MAI_SGL->tex_parameter(MAI_TEXTURE_WRAP_U, MAI_TEXTURE_WRAP_REPEAT);
		MAI_SGL->tex_parameter(MAI_TEXTURE_WRAP_V, MAI_TEXTURE_WRAP_REPEAT);
		MAI_SGL->bind_texture(0);
	}

	geometry = mai::Geometry::create_box(1.0f);

	return true;
}

void print_debug_data()
{
	std::cout << "-------------------------------------------------------------------\n";
	MAI_SGL->print_frame_stats();
	std::cout
		<< std::fixed << std::setprecision(2)
		<< "FPS: " << current_fps
		<< " | Smoothed FPS: " << smoothed_fps
		<< '\n';
}

void print_summary_debug_data()
{
	MAI_SGL->print_summary_stats();
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

	const std::string stats_output_path = std::string(SOFTWARE_RENDERING_ROOT_DIR) + "/doc/logs/logs";
	if (!MAI_SGL->set_stats_output_path(stats_output_path))
		std::cerr << "Failed to open GPU stats output files." << std::endl;

	if (!prepare())
		return -1;


	bool active = true;
	uint64_t frame_index = 0;
	using Clock = std::chrono::steady_clock;
	auto previous_frame_time = Clock::now();
	const auto fps_stats_start_time = previous_frame_time;

	while (active)
	{
		auto current_frame_time = Clock::now();
		std::chrono::duration<float> frame_duration = current_frame_time - previous_frame_time;
		previous_frame_time = current_frame_time;

		const float raw_delta_time = frame_duration.count();

		delta_time = raw_delta_time;
		if (delta_time > max_delta_time)
			delta_time = max_delta_time;

		if (raw_delta_time > 0.0f)
		{
			current_fps = 1.0f / raw_delta_time;
			if (smoothed_delta_time <= 0.0f)
				smoothed_delta_time = raw_delta_time;
			else
				smoothed_delta_time += (raw_delta_time - smoothed_delta_time) * fps_smoothing;

			smoothed_fps = 1.0f / smoothed_delta_time;
		}

		active = MAI_APP->peek_message();
		camera->update(delta_time);
		on_render(delta_time);
		rend_imgui();
		MAI_APP->show();
		print_debug_data();

		++frame_index;
	}


	print_summary_debug_data();
	const auto fps_stats_end_time = Clock::now();
	const std::chrono::duration<float> total_frame_duration = fps_stats_end_time - fps_stats_start_time;
	const float total_frame_seconds = total_frame_duration.count();
	if (total_frame_seconds > 0.0f)
	{
		const float average_fps = static_cast<float>(frame_index) / total_frame_seconds;
		std::cout << "Average FPS: " << average_fps << '\n';
	}

	shutdown_imgui_for_MAI_SGL();
	MAI_SGL->delete_texture(texture);
	mai::Image::destroy_image(image);

	return 0;
}

