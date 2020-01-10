#include "Application.hpp"

Application::Application(sf::Time &dt)
	: m_gfx(m_camera),
	  m_dt(dt),
	  m_iu(m_gfx, m_camera),
	  m_ui(m_gfx, m_iu),
	  m_camera(m_gfx, m_camera_follow, m_camera_controller),
	  m_camera_controller(m_gfx, m_camera, m_iu),
	  m_camera_follow(0.0f, 0.0f),
	  m_flockingController(100)
{
}

void Application::Go()
{
	HandleEvents();
	m_gfx.ClearFrame();
	UpdateLogic();
	RenderFrame();
	m_gfx.EndFrame();
}

void Application::HandleEvents()
{
	while (m_gfx.GetRenderWindow().pollEvent(m_event))
	{
		// Close window: exit
		if (m_event.type == sf::Event::Closed)
		{
			m_gfx.GetRenderWindow().close();
		}
		else if (m_event.type == sf::Event::MouseWheelMoved)
		{
			float delta_move = m_event.mouseWheel.delta;
			float zoom_strength = 0.03f;
			m_camera.SetZoom(m_camera.GetZoom() + sf::Vector2f(delta_move, delta_move) * zoom_strength);
		}
	}
}

void Application::UpdateLogic()
{
	m_iu.Update();
	m_camera_controller.Update(m_dt);
	m_camera.Update(m_dt);
	m_ui.Update(m_dt);

	m_flockingController.Update(m_dt);
}

void Application::RenderFrame()
{
	m_flockingController.Draw(m_gfx);
}