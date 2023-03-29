// Markus Buchholz, 2023

#include <stdafx.hpp>
#include "imgui_helper.hpp"
#include <tuple>
#include <thread>
#include <chrono>
#include <vector>
#include <math.h>
#include <list>
#include <numeric>
#include <memory>

//---------------------------------------------------------------
float SCALE = 10.0f;
int LENGTH = 50;
//---------------------------------------------------------------

struct Arc
{

	ImVec2 p0;
	ImVec2 p1;
	ImVec2 c;
	float r;
	std::vector<ImVec2> arc;

	Arc(ImVec2 a, ImVec2 b, float mod)
	{

		std::vector<ImVec2> arc;

		this->c = {(a.x + b.x) / 2.0f, (a.y + b.y) / 2.0f};
		this->r = std::sqrt(std::pow(a.x - this->c.x, 2) + std::pow(a.y - this->c.y, 2));

		if (mod == 1)
		{

			// backwards
			for (float tt = 0.0f; tt < M_PI; tt = tt + 0.005f)
			{

				float xi = c.x + r * cos(tt);
				float yi = c.y + r * sin(tt);
				this->arc.push_back({xi, yi});
			}
		}
		// forward
		else
		{

			for (float tt = M_PI; tt < 2 * M_PI; tt = tt + 0.005f)
			{

				float xi = c.x + r * cos(tt);
				float yi = c.y + r * sin(tt);
				this->arc.push_back({xi, yi});
			}
		}
	}
};

//---------------------------------------------------------------

struct RGB
{

	float r;
	float g;
	float b;
};

//---------------------------------------------------------------

struct HSV
{
	float h;
	float s;
	float v;
};

//---------------------------------------------------------------

int main(int argc, char const *argv[])
{
	auto seqX = [](int N)
	{
		int counter = N;
		std::vector<int> numbers(N, 0);
		numbers[0] = 1;
		int index = 0;
		std::vector<float> seq;
		for (int ii = 1; ii < numbers.size(); ii++)
		{
			int next = index - ii;

			if (next < 0 || numbers[next] == 1)
			{
				next = index + ii;
			}
			numbers[next] = 1;
			seq.push_back(next * SCALE);
			index = next;
		}

		return seq;
	};

	int w = 1280;
	int h = 720;
	std::string title = "Recaman sequence";
	initImgui(w, h, title);

	// simulation state
	ImVec4 clear_color = ImVec4(0.0f / 255.0, 0.0f / 255.0, 0.0f / 255.0, 1.00f);
	ImVec4 ray_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 ray_color2 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

	std::vector<float> numbers = seqX(LENGTH);

	std::vector<std::unique_ptr<Arc>> arcs;

	float y_dev = 400.0f;

	for (int ii = 0; ii < numbers.size() - 1; ii++)
	{

		std::unique_ptr<Arc> p{new Arc({numbers[ii], y_dev}, {numbers[ii + 1], y_dev}, ii % 2)};
		arcs.emplace_back(std::move(p));
	}

	bool flag = true;

	int jj = 0;

	std::vector<ImVec2> tail;

	RGB rgb;
	HSV hsv;

	// Main loop
	while (!glfwWindowShouldClose(window) && flag == true)
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		ImGuiWindowFlags window_flags = 0;
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_FirstUseEver);
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoBackground;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::Begin("Simulation", nullptr, window_flags);
		ImDrawList *draw_list = ImGui::GetWindowDrawList();

		for (auto &ii : arcs[jj]->arc)
		{
			tail.push_back({ii.x, ii.y});
		}

		for (auto &ii : tail)
		{

			float d = std::sqrt(std::pow((ii.x - 0.0f), 2) + std::pow((ii.y - y_dev), 2));
			hsv = {d / (255.0f * 5.0f), 1.0f, 1.0f};
			ImGui::ColorConvertHSVtoRGB(hsv.h, hsv.s, hsv.v, rgb.r, rgb.g, rgb.b);
			ImVec4 pix_color = ImVec4(rgb.r, rgb.g, rgb.b, 1.0f);

			draw_list->AddCircleFilled({ii.x, ii.y}, 0.3f, ImColor(pix_color));
		}

		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
		jj++;

		if (jj == LENGTH - 2)
		{
			return -1;
		}

		ImGui::End();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	termImgui();
	return 0;
}