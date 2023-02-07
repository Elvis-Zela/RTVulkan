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
#include "Lights.h"


using namespace Walnut;

/* ------------------------------------------------------------------------------------------------ */
/* ---------------------------------- Initialise World -------------------------------------------- */
/* ------------------------------------------------------------------------------------------------ */
void InitWorld(World& scene)
{
	/* - Creates simple Scene spheres - */

	glm::vec3 pos1(-1.0f, 0.5f, -1.0f);
	float	  rad1 = 1.0f;
	Material reflective;
	reflective.type = kReflective;
	Sphere sphere1(reflective, pos1, rad1);

	glm::vec3 pos2(2.0f, 1.5f, -3.0f);
	glm::vec3 alb2(0.845f, 0.0837f, 0.930f);
	float	  rad2 = 2.0f;
	float	  fuzz1 = 0.15f;
	Material metalMat1;
	metalMat1.Albedo = alb2;
	metalMat1.Fuzz = fuzz1;
	metalMat1.type = kMetal;
	Sphere sphere2(metalMat1, pos2, rad2);

	glm::vec3 pos3(1.5f, 0.5f, 5.0f);
	float	  rad3 = 1.0f;
	float	  ior = 1.33f;
	Material dielectric1;
	dielectric1.IndexOfRefraction = ior;
	dielectric1.type = kDielectric;
	Sphere sphere3(dielectric1, pos3, rad3);

	/* - Last sphere is the plane - */
	glm::vec3 posP(0.0f, -500.5f, 0.0f);
	glm::vec3 albP(0.154f, 0.470f, 0.0296f);
	float	  radP = 500.0f;
	Material planeMat;
	planeMat.Albedo = albP;
	planeMat.type = kLambertian;
	Sphere sphereP(planeMat, posP, radP);

	scene.AddObject(std::make_shared<Sphere>(sphere1));
	scene.AddObject(std::make_shared<Sphere>(sphere2));
	scene.AddObject(std::make_shared<Sphere>(sphere3));
	scene.AddObject(std::make_shared<Sphere>(sphereP));

	DistantLight dl1(glm::vec3(1.0f, -2.0f, -1.4f), glm::vec3(0.770f, 0.139f, 0.139f), 0.8f);
	scene.AddLight(std::make_shared<DistantLight>(dl1));

	DistantLight dl2(glm::vec3(-0.78f, -0.97f, -1.0f), glm::vec3(0.0836f, 0.726f, 0.760f), 0.75f);
	scene.AddLight(std::make_shared<DistantLight>(dl2));

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

		if (ImGui::Button("Reset"))
		{
			m_Renderer.ResetFrameIndex();
			m_Renderer.GetSettings().maxBounceDepth = 1;
		}

		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		ImGui::End();

		/* - Options menu where users can modify mouse sens and camera speed - */
		ImGui::Begin("Options");
		ImGui::DragFloat("Mouse Sensitivity", &m_Camera.MouseSensitivity, 0.005f, 0.2f, 0.5f);
		ImGui::DragFloat("Camera Fly Speed", &m_Camera.CameraFlySpeed, 0.1f, 1.5f, 10.0f);
		if (ImGui::SliderInt("Bounce Depth", &m_Renderer.GetSettings().maxBounceDepth, 1, 10)) m_Renderer.ResetFrameIndex();
		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);

		ImGui::End();

		ImGui::Begin("Sphere Options");
		/* - Allows User to Change Object Settings - */

		if (ImGui::Button("Add Sphere"))
		{
			Material mat;
			Sphere obj = Sphere(mat);
			m_World.AddObject(std::make_shared<Sphere>(obj));
			m_Renderer.ResetFrameIndex();
		}

		ImGui::Separator();

		for (size_t i = 0; i < m_World.SceneSize(); i++)
		{
			ImGui::PushID(i);

			Sphere* sphere = m_World.m_SceneGeometry[i].get();
			Material mat = sphere->mat;

			if (ImGui::BeginMenu("Material"))
			{
				if (ImGui::MenuItem("Lambertian")) { sphere->mat.type = kLambertian, m_Renderer.ResetFrameIndex(); }
				if (ImGui::MenuItem("Metal"))	   { sphere->mat.type = kMetal, m_Renderer.ResetFrameIndex(); }
				if (ImGui::MenuItem("Reflective")) { sphere->mat.type = kReflective, m_Renderer.ResetFrameIndex(); }
				if (ImGui::MenuItem("Dielectric")) { sphere->mat.type = kDielectric, m_Renderer.ResetFrameIndex(); }
				ImGui::EndMenu();
			}

			switch (mat.type)
			{
			case kLambertian:
				ImGui::Text("Lambertian");
				break;

			case kMetal:
				ImGui::Text("Metal");
				break;

			case kReflective:
				ImGui::Text("Reflective");
				break;

			case kDielectric:
				ImGui::Text("Dielectric");
				break;

			}

			if (ImGui::ColorEdit3("Albedo", glm::value_ptr((sphere->mat.Albedo)))) m_Renderer.ResetFrameIndex();
			if (ImGui::DragFloat("Radius", &sphere->m_Radius, 0.1f, 0.1f, FLOAT_LARGE)) m_Renderer.ResetFrameIndex();
			if (ImGui::DragFloat3("Center Pos", glm::value_ptr(sphere->m_Center), 0.1f)) m_Renderer.ResetFrameIndex();


			switch (sphere->mat.type)
			{
			case kMetal:
				if (ImGui::DragFloat("Metal Fuzz", &sphere->mat.Fuzz, 0.01f, 0.0f, 1.0f)) m_Renderer.ResetFrameIndex();
				break;

			case kDielectric:
				if (ImGui::DragFloat("IoR", &sphere->mat.IndexOfRefraction, 0.01f, 1.0f, 2.5f)) m_Renderer.ResetFrameIndex();
				break;
			}

			ImGui::Separator();
			ImGui::PopID();
		}

		ImGui::End();

		ImGui::Begin("Lighting Options");

		if (ImGui::Button("Add Point Light"))
		{
			glm::vec3 pos = glm::vec3(0.0f, 2.0f, 0.0f);
			glm::vec3 colour = glm::vec3(1.0f);
			float intensity = 200.0f;
			PointLight light(pos, colour, intensity);
			m_World.AddLight(std::make_shared<PointLight>(light));
		}

		ImGui::SameLine();

		if (ImGui::Button("Add Distant Light"))
		{
			glm::vec3 direction = glm::vec3(-1.0f, -0.2f, 0.3f);
			glm::vec3 colour = glm::vec3(1.0f);
			float intensity = 1.0f;
			DistantLight light(direction, colour, intensity);
			m_World.AddLight(std::make_shared<DistantLight>(light));
		}

		ImGui::Separator();

		for (size_t i = 0; i < m_World.m_Lights.size(); i++)
		{
			ImGui::PushID(i);

			Lights* light = m_World.m_Lights[i].get();

			switch (light->type)
			{
			case kDistant:
				if (ImGui::DragFloat3("Light Direction", glm::value_ptr(light->direction), 0.1f)) 
				{
					light->direction = glm::normalize(light->direction);
					m_Renderer.ResetFrameIndex();
				}
				if (ImGui::DragFloat("Intensity", &light->intensity, 0.05f, 0.0f, 1.0f)) m_Renderer.ResetFrameIndex();
				break;

			case kPoint:
				if (ImGui::DragFloat3("Light Position", glm::value_ptr(light->pos), 0.1f)) m_Renderer.ResetFrameIndex();
				if (ImGui::DragFloat("Intensity", &light->intensity, 5.0f, 0.0f, FLOAT_LARGE)) m_Renderer.ResetFrameIndex();
				break;
			}

			if (ImGui::ColorEdit3("Colour", glm::value_ptr(light->colour))) m_Renderer.ResetFrameIndex();

			ImGui::Separator();
			ImGui::PopID();
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportWidth		= (uint32_t)ImGui::GetContentRegionAvail().x;
		m_ViewportHeight	= (uint32_t)ImGui::GetContentRegionAvail().y;

		auto image		= m_Renderer.GetFinalImage();

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
