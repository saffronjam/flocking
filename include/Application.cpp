#include "Application.hpp"
#include <stdlib.h>
#include <time.h>

Application::Application(sf::Time &dt)
	: m_gfx(m_camera),
	  m_dt(dt),
	  m_iu(m_gfx, m_camera),
	  m_ui(m_gfx, m_iu),
	  m_camera(m_gfx, m_camera_follow, m_camera_controller),
	  m_camera_controller(m_gfx, m_camera, m_iu),
	  m_camera_follow(0.0f, 0.0f),
	  m_flockingController(100),
	  m_UIBackground(sf::Vector2f(180, Graphics::ScreenHeight)),
	  m_textSeparation("Separation", *m_gfx.allFonts[Graphics::FontMapping::FontArial], 16),
	  m_textAlignment("Alignment", *m_gfx.allFonts[Graphics::FontMapping::FontArial], 16),
	  m_textCohesion("Cohesion", *m_gfx.allFonts[Graphics::FontMapping::FontArial], 16),
	  m_textSeeingDistance("Seeing Distance", *m_gfx.allFonts[Graphics::FontMapping::FontArial], 16),
	  m_textSeeingAngle("Seeing Angle", *m_gfx.allFonts[Graphics::FontMapping::FontArial], 16),
	  m_textSpeed("Speed", *m_gfx.allFonts[Graphics::FontMapping::FontArial], 16),
	  m_textDraw("Draw", *m_gfx.allFonts[Graphics::FontMapping::FontArial], 16)
{
	srand(time(NULL));

	m_UIBackground.setPosition(Graphics::ScreenWidth - m_UIBackground.getSize().x, 0);
	m_UIBackground.setFillColor(sf::Color(100, 100, 100, 120));
	m_textSeparation.setPosition(sf::Vector2f(Graphics::ScreenWidth - 90 - m_textSeparation.getLocalBounds().width / 2.0f, 10));
	m_textAlignment.setPosition(sf::Vector2f(Graphics::ScreenWidth - 90 - m_textAlignment.getLocalBounds().width / 2.0f, 70));
	m_textCohesion.setPosition(sf::Vector2f(Graphics::ScreenWidth - 90 - m_textCohesion.getLocalBounds().width / 2.0f, 130));
	m_textSeeingDistance.setPosition(sf::Vector2f(Graphics::ScreenWidth - 90 - m_textSeeingDistance.getLocalBounds().width / 2.0f, 210));
	m_textSeeingAngle.setPosition(sf::Vector2f(Graphics::ScreenWidth - 90 - m_textSeeingAngle.getLocalBounds().width / 2.0f, 270));
	m_textSpeed.setPosition(sf::Vector2f(Graphics::ScreenWidth - 90 - m_textSpeed.getLocalBounds().width / 2.0f, 350));
	m_textDraw.setPosition(sf::Vector2f(Graphics::ScreenWidth - 90 - m_textDraw.getLocalBounds().width / 2.0f, 420));

	m_ui.CreateStaticFloatSlider(0, m_flockingController.GetSeparationStrengthRef(), 0.0f, 5.0f, 1.0f, sf::Vector2f(Graphics::ScreenWidth - 160, 30));
	m_ui.CreateStaticFloatSlider(1, m_flockingController.GetAlignmentStrengthRef(), 0.0f, 5.0f, 1.0f, sf::Vector2f(Graphics::ScreenWidth - 160, 90));
	m_ui.CreateStaticFloatSlider(2, m_flockingController.GetCohesionStrengthRef(), 0.0f, 5.0f, 1.0f, sf::Vector2f(Graphics::ScreenWidth - 160, 150));
	m_ui.CreateStaticFloatSlider(3, m_flockingController.GetSeeingDistanceRef(), 0.0f, 250.0f, 100.0f, sf::Vector2f(Graphics::ScreenWidth - 160, 230));
	m_ui.CreateStaticFloatSlider(4, m_flockingController.GetSeeingAngleRef(), 0.0f, 2 * Math::Constants::PI + 0.185f, Math::Constants::PI / 2.0f, sf::Vector2f(Graphics::ScreenWidth - 160, 290));
	m_ui.CreateStaticFloatSlider(5, m_flockingController.GetSpeedRef(), 0.0f, 250.0f, 50.0f, sf::Vector2f(Graphics::ScreenWidth - 160, 370));

	m_ui.CreateStaticCheckbox(0, m_flockingController.GetDrawBoidsRef(), true, sf::Vector2f(Graphics::ScreenWidth - 115, 440), {}, {});
	m_ui.CreateStaticCheckbox(1, m_flockingController.GetDrawNeighborRef(), false, sf::Vector2f(Graphics::ScreenWidth - 95, 440), {}, {});
	m_ui.CreateStaticCheckbox(2, m_flockingController.GetDrawVisionRef(), false, sf::Vector2f(Graphics::ScreenWidth - 75, 440), {}, {});
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

	m_gfx.Render(m_UIBackground);
	m_gfx.Render(m_textSeparation);
	m_gfx.Render(m_textAlignment);
	m_gfx.Render(m_textCohesion);
	m_gfx.Render(m_textSeeingDistance);
	m_gfx.Render(m_textSeeingAngle);
	m_gfx.Render(m_textSpeed);
	m_gfx.Render(m_textDraw);
	m_ui.Draw();
}