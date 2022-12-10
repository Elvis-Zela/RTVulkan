#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Sphere.h"
#include "World.h"
#include "Camera.h"
#include "Renderer.h"
#include "Materials.h"


using namespace Walnut;

/* ------------------------------------------------------------------------------------------------ */
/* ---------------------------------- Initialise World -------------------------------------------- */
/* ------------------------------------------------------------------------------------------------ */
void InitWorld(World& scene)
{
	/* - Creates simple Scene spheres - */
	int m_idx;	//Index to appropriate material

	glm::vec3 pos1(-1.0f, 0.5f, -1.0f);
	glm::vec3 alb1(0.440f, 0.106f, 0.0396f);
	float	  rad1 = 1.0f;
	float	  rough1 = 0.38f;
	std::shared_ptr<RoughMat> roughMat1 = std::make_shared<RoughMat>(alb1, rough1);
	m_idx = scene.AddMaterial(roughMat1);
	Sphere sphere1(pos1, rad1, m_idx);

	glm::vec3 pos2(2.0f, 1.5f, -3.0f);
	glm::vec3 alb2(0.845f, 0.0837f, 0.930f);
	float	  rad2 = 2.0f;
	float	  fuzz1 = 0.15f;
	std::shared_ptr<Metal> metalMat1 = std::make_shared<Metal>(alb2, fuzz1);
	m_idx = scene.AddMaterial(metalMat1);
	Sphere sphere2(pos2, rad2, m_idx);

	/* - Last sphere is the plane - */
	glm::vec3 posP(0.0f, -500.5f, 0.0f);
	glm::vec3 albP(0.154f, 0.470f, 0.0296f);
	float	  radP = 500.0f;
	std::shared_ptr<Lambertian> planeMat = std::make_shared<Lambertian>(albP);
	m_idx = scene.AddMaterial(planeMat);
	Sphere sphereP(posP, radP, m_idx);

	scene.AddObject(std::make_shared<Sphere>(sphere1));
	scene.AddObject(std::make_shared<Sphere>(sphere2));
	scene.AddObject(std::make_shared<Sphere>(sphereP));
}
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */


class MainLayer : public Walnut::Layer
{
public:

	MainLayer()
		:m_Camera() 
	{

		InitWorld(m_World);

	}
	virtual void OnUpdate(float ts) override
	{
		if (m_Camera.Move(ts))
			m_Renderer.ResetFrameIndex();
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Debug");

		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);

		if (ImGui::Button("Reset"))
			m_Renderer.ResetFrameIndex();

		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		ImGui::Text("FPS: %.3f", 1000.0f / m_LastRenderTime);
		ImGui::End();

		/* - Options menu where users can modify mouse sens and camera speed - */
		ImGui::Begin("Options");
		ImGui::DragFloat("Mouse Sensitivity", &m_Camera.MouseSensitivity, 0.005f, 0.2f, 0.5f);
		ImGui::DragFloat("Camera Fly Speed", &m_Camera.CameraFlySpeed, 0.1f, 1.5f, 10.0f);
		ImGui::SliderInt("Bounce Depth", &m_Renderer.bounceDepth, 1, 10);
		
		/* - Allows User to Change Object Settings - */
		/* - Currently Disabled - */
		/*
		for (size_t i = 0; i < m_World.SceneSize(); i++)
		{
			ImGui::PushID(i);

			std::shared_ptr<Hittables> obj = m_World.GetObj(i);
			std::shared_ptr<Material> mat = m_World.GetMat(obj->GetMatIndex());

			ImGui::ColorEdit3("Albedo", glm::value_ptr(*(mat->GetAlbedo())));

			ImGui::Separator();
			ImGui::PopID();
		}
		*/
		/* --------------------------------------------------------------------------------- */

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

		if (m_Camera.ViewPortResized(m_ViewportWidth, m_ViewportHeight))
				m_Renderer.ResetFrameIndex();

		m_Renderer.IfResizing(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_World, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}

private:
	Renderer m_Renderer;
	Camera m_Camera;
	World m_World;
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
