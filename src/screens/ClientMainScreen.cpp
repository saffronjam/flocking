#include "ClientMainScreen.h"
#include "AppClient.h"

ClientMainScreen::ClientMainScreen(AppClient &parent)
    : m_parent(parent)
{
}

ClientMainScreen::~ClientMainScreen()
{
}

void ClientMainScreen::Build()
{
}

void ClientMainScreen::Destroy()
{
}

void ClientMainScreen::OnEntry()
{
    LightningMgr::SetAmbientLight(sf::Color(255, 255, 255));

    // -------------- ALL LABELS ------------------
    auto labelGrid = sfg::Label::Create("Grid");
    auto labelMultipliers = sfg::Label::Create("Multipliers");
    auto labelVisionSettings = sfg::Label::Create("Vision");

    // -------------- FORCE MULTIPLIERS ------------------
    auto labelSeparation = sfg::Label::Create();
    auto scaleSeparation = sfg::Scale::Create(sfg::Scale::Orientation::HORIZONTAL);
    auto labelAlignment = sfg::Label::Create();
    auto scaleAlignment = sfg::Scale::Create(sfg::Scale::Orientation::HORIZONTAL);
    auto labelCohesion = sfg::Label::Create();
    auto scaleCohesion = sfg::Scale::Create(sfg::Scale::Orientation::HORIZONTAL);

    auto adjustmentSeparation = scaleSeparation->GetAdjustment();
    adjustmentSeparation->SetLower(0.0f);
    adjustmentSeparation->SetUpper(10.0f);
    adjustmentSeparation->SetValue(1.0f);
    adjustmentSeparation->SetMinorStep(0.1f);
    adjustmentSeparation->SetMajorStep(0.5f);
    auto adjustmentAlignment = scaleAlignment->GetAdjustment();
    adjustmentAlignment->SetLower(0.0f);
    adjustmentAlignment->SetUpper(10.0f);
    adjustmentAlignment->SetValue(1.0f);
    adjustmentAlignment->SetMinorStep(0.1f);
    adjustmentAlignment->SetMajorStep(0.5f);
    auto adjustmentCohesion = scaleCohesion->GetAdjustment();
    adjustmentCohesion->SetLower(0.0f);
    adjustmentCohesion->SetUpper(10.0f);
    adjustmentCohesion->SetValue(1.0f);
    adjustmentCohesion->SetMinorStep(0.1f);
    adjustmentCohesion->SetMajorStep(0.5f);

    adjustmentSeparation->GetSignal(sfg::Adjustment::OnChange).Connect([adjustmentSeparation, labelSeparation, this] {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << "Separation: " << adjustmentSeparation->GetValue() << " ";
        labelSeparation->SetText(oss.str());
        m_boidMgr.SetSeparationMultiplier(adjustmentSeparation->GetValue());
    });
    adjustmentAlignment->GetSignal(sfg::Adjustment::OnChange).Connect([adjustmentAlignment, labelAlignment, this] {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << "Alignment: " << adjustmentAlignment->GetValue() << " ";
        labelAlignment->SetText(oss.str());
        m_boidMgr.SetAlignmentMultiplier(adjustmentAlignment->GetValue());
    });
    adjustmentCohesion->GetSignal(sfg::Adjustment::OnChange).Connect([adjustmentCohesion, labelCohesion, this] {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << "Cohesion: " << adjustmentCohesion->GetValue() << " ";
        labelCohesion->SetText(oss.str());
        m_boidMgr.SetCohesionMultiplier(adjustmentCohesion->GetValue());
    });

    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << "Separation: " << adjustmentSeparation->GetValue() << " ";
        labelSeparation->SetText(oss.str());
        m_boidMgr.SetSeparationMultiplier(adjustmentSeparation->GetValue());
    }
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << "Alignment: " << adjustmentAlignment->GetValue() << " ";
        labelAlignment->SetText(oss.str());
        m_boidMgr.SetAlignmentMultiplier(adjustmentAlignment->GetValue());
    }
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << "Cohesion: " << adjustmentCohesion->GetValue() << " ";
        labelCohesion->SetText(oss.str());
        m_boidMgr.SetCohesionMultiplier(adjustmentCohesion->GetValue());
        oss.clear();
    }

    scaleSeparation->SetRequisition(sf::Vector2f(80.f, 20.f));
    scaleAlignment->SetRequisition(sf::Vector2f(80.f, 20.f));
    scaleCohesion->SetRequisition(sf::Vector2f(80.f, 20.f));

    auto boxSeparation = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
    boxSeparation->Pack(labelSeparation, false, false);
    boxSeparation->Pack(scaleSeparation, false, false);
    boxSeparation->SetRequisition(sf::Vector2f(150.0f, 0.0f));
    auto boxAlignment = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
    boxAlignment->Pack(labelAlignment, false, false);
    boxAlignment->Pack(scaleAlignment, false, false);
    boxAlignment->SetRequisition(sf::Vector2f(150.0f, 0.0f));
    auto boxCohesion = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
    boxCohesion->Pack(labelCohesion, false, false);
    boxCohesion->Pack(scaleCohesion, false, false);
    boxCohesion->SetRequisition(sf::Vector2f(150.0f, 0.0f));

    auto boxMultipliers = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.0f);
    boxMultipliers->Pack(labelMultipliers);
    boxMultipliers->Pack(boxSeparation);
    boxMultipliers->Pack(boxAlignment);
    boxMultipliers->Pack(boxCohesion);

    // ------------- VISION SETTINGS -------------------
    auto labelSightRadius = sfg::Label::Create();
    auto scaleSightRadius = sfg::Scale::Create(sfg::Scale::Orientation::HORIZONTAL);
    auto labelSightAngle = sfg::Label::Create();
    auto scaleSightAngle = sfg::Scale::Create(sfg::Scale::Orientation::HORIZONTAL);

    auto adjustmentSightRadius = scaleSightRadius->GetAdjustment();
    adjustmentSightRadius->SetLower(0.0f);
    adjustmentSightRadius->SetUpper(400.0f);
    adjustmentSightRadius->SetValue(400.0f);
    adjustmentSightRadius->SetMinorStep(5.0f);
    adjustmentSightRadius->SetMajorStep(20.0f);
    auto adjustmentSightAngle = scaleSightAngle->GetAdjustment();
    adjustmentSightAngle->SetLower(0.0f);
    adjustmentSightAngle->SetUpper(365.0f);
    adjustmentSightAngle->SetValue(365.0f);
    adjustmentSightAngle->SetMinorStep(5.0f);
    adjustmentSightAngle->SetMajorStep(5.0f);

    adjustmentSightRadius->GetSignal(sfg::Adjustment::OnChange).Connect([adjustmentSightRadius, labelSightRadius, this] {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << "Radius: " << adjustmentSightRadius->GetValue() << " ";
        labelSightRadius->SetText(oss.str());
        m_boidMgr.SetSightRadius(adjustmentSightRadius->GetValue());
    });
    adjustmentSightAngle->GetSignal(sfg::Adjustment::OnChange).Connect([adjustmentSightAngle, labelSightAngle, this] {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << "Angle: " << adjustmentSightAngle->GetValue() << " ";
        labelSightAngle->SetText(oss.str());
        m_boidMgr.SetSightAngle(adjustmentSightAngle->GetValue());
    });

    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << "Radius: " << adjustmentSightRadius->GetValue() << " ";
        labelSightRadius->SetText(oss.str());
        m_boidMgr.SetSightRadius(adjustmentSightRadius->GetValue());
    }
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << "Angle: " << adjustmentSightAngle->GetValue() << " ";
        labelSightAngle->SetText(oss.str());
        m_boidMgr.SetSightAngle(adjustmentSightAngle->GetValue());
    }

    scaleSightRadius->SetRequisition(sf::Vector2f(80.f, 20.f));
    scaleSightAngle->SetRequisition(sf::Vector2f(80.f, 20.f));

    auto boxSightRadius = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
    boxSightRadius->Pack(labelSightRadius, false, false);
    boxSightRadius->Pack(scaleSightRadius, false, false);
    boxSightRadius->SetRequisition(sf::Vector2f(150.0f, 0.0f));
    auto boxSightAngle = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
    boxSightAngle->Pack(labelSightAngle, false, false);
    boxSightAngle->Pack(scaleSightAngle, false, false);
    boxSightAngle->SetRequisition(sf::Vector2f(150.0f, 0.0f));

    auto boxVisionSettings = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.0f);
    boxVisionSettings->Pack(labelVisionSettings);
    boxVisionSettings->Pack(boxSightRadius);
    boxVisionSettings->Pack(boxSightAngle);

    // -------------- ALL BUTTONS ------------------
    auto buttonStart = sfg::Button::Create("Start");
    auto buttonRestart = sfg::Button::Create("Restart");
    auto buttonReset = sfg::Button::Create("Reset");
    auto buttonPause = sfg::Button::Create("Pause");
    auto buttonResume = sfg::Button::Create("Resume");

    buttonStart->GetSignal(sfg::Widget::OnLeftClick).Connect([this] {});
    buttonRestart->GetSignal(sfg::Widget::OnLeftClick).Connect([this] {});
    buttonReset->GetSignal(sfg::Widget::OnLeftClick).Connect([this] {});
    buttonPause->GetSignal(sfg::Widget::OnLeftClick).Connect([this] {});
    buttonResume->GetSignal(sfg::Widget::OnLeftClick).Connect([this] {});

    auto boxButtonRow0 = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 3.0f);
    auto boxButtonRow1 = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 3.0f);

    boxButtonRow0->Pack(buttonStart);
    boxButtonRow0->Pack(buttonRestart);
    boxButtonRow0->Pack(buttonReset);
    boxButtonRow1->Pack(buttonPause);
    boxButtonRow1->Pack(buttonResume);

    auto boxAllButtons = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);
    boxAllButtons->Pack(boxButtonRow0, false);
    boxAllButtons->Pack(boxButtonRow1, false);

    // --------------- SUB BOXES ----------------------

    // -------------- ADD TO MAIN BOX ------------------
    auto mainBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 15.0f);
    mainBox->Pack(boxAllButtons, false);
    mainBox->Pack(boxMultipliers, false);
    mainBox->Pack(boxVisionSettings, false);

    // -------------- ADD TO MAIN WINDOW ------------------
    auto window = sfg::Window::Create(sfg::Window::Style::BACKGROUND);
    window->SetPosition(sf::Vector2f(Window::GetWidth() - 200.0f, 0.0f));
    window->SetRequisition(sf::Vector2f(200.0f, Window::GetHeight()));
    window->Add(mainBox);

    GuiMgr::Add(window);
}

void ClientMainScreen::OnExit()
{
}

void ClientMainScreen::Update()
{
    m_boidMgr.Update();
    if (Keyboard::IsPressed(sf::Keyboard::Space))
        log_info("FPS: %f", Clock::GetFPS());
}

void ClientMainScreen::Draw()
{
    m_boidMgr.Draw();
}

int ClientMainScreen::GetNextScreenIndex() const
{
    return SCREENINDEX_NO_SCREEN;
}

int ClientMainScreen::GetPreviousScreenIndex() const
{
    return SCREENINDEX_NO_SCREEN;
}