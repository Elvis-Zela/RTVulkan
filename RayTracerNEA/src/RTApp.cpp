#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Sphere.h"
#include "World.h"
#include "Camera.h"
#include "Renderer.h"


using namespace Walnut;

void InitWorld(World& scene)
{
	/* - Creates simple Scene with 2 spheres - */
	glm::vec3 pos1(-1.0f, 1.0f, -1.0f);
	glm::vec3 alb1(1.0f, 0.0f, 1.0f);
	float	  rad1 = 1.0f;

	glm::vec3 pos2(2.0f, 2.0f, -3.0f);
	glm::vec3 alb2(0.2f, 0.3f, 1.0f);
	float	  rad2 = 2.0f;

	/* - 3rd sphere is the plane - */
	glm::vec3 pos3(0.0f, -1000.0f, 0.0f);
	glm::vec3 alb3(0.25f, 1.0f, 0.1f);
	float	  rad3 = 1000.0f;

	scene.Add(std::make_shared<Sphere>(pos1, rad1, alb1));
	scene.Add(std::make_shared<Sphere>(pos2, rad2, alb2));
	scene.Add(std::make_shared<Sphere>(pos3, rad3, alb3));
	
}

class MainLayer : public Walnut::Layer
{
public:

	MainLayer()
		:m_Camera() 
	{

		InitWorld(world);

	}
	virtual void OnUpdate(float ts) override
	{
		m_Camera.Update(ts);
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Debug");
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		ImGui::Text("FPS: %.3f", 1000.0f / m_LastRenderTime);
		ImGui::End();
		
		/* - Options menu where users can modify mouse sens and camera speed - */
		ImGui::Begin("Options");
		ImGui::DragFloat("Mouse Sensitivity", &m_Camera.MouseSensitivity, 0.005f, 0.2f, 0.5f);
		ImGui::DragFloat("Camera Fly Speed", &m_Camera.CameraFlySpeed, 0.1f, 1.5f, 10.0f);
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportWidth		= (uint32_t)ImGui::GetContentRegionAvail().x;
		m_ViewportHeight	= (uint32_t)ImGui::GetContentRegionAvail().y;

		auto image			= m_Renderer.GetFinalImage();

		if (image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() },
				ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}

	void Render()
	{
		Timer timer;

		m_Renderer.IfResizing(m_ViewportWidth, m_ViewportHeight);
		m_Camera.IfResizing(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(world, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}

private:
	Renderer m_Renderer;
	Camera m_Camera;
	World world;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	float m_LastRenderTime = 0.0f;
};


Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracer";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<MainLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}
