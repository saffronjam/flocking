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
    auto labelSpeedControls = sfg::Label::Create("Speed");
    auto labelDraw = sfg::Label::Create("Draw");

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
    adjustmentSeparation->SetMinorStep(0.1f);
    adjustmentSeparation->SetMajorStep(0.5f);
    auto adjustmentAlignment = scaleAlignment->GetAdjustment();
    adjustmentAlignment->SetLower(0.0f);
    adjustmentAlignment->SetUpper(10.0f);
    adjustmentAlignment->SetMinorStep(0.1f);
    adjustmentAlignment->SetMajorStep(0.5f);
    auto adjustmentCohesion = scaleCohesion->GetAdjustment();
    adjustmentCohesion->SetLower(0.0f);
    adjustmentCohesion->SetUpper(10.0f);
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

    adjustmentSeparation->SetValue(1.0f);
    adjustmentAlignment->SetValue(1.0f);
    adjustmentCohesion->SetValue(1.0f);

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
    adjustmentSightRadius->SetMinorStep(1.0f);
    adjustmentSightRadius->SetMajorStep(20.0f);
    auto adjustmentSightAngle = scaleSightAngle->GetAdjustment();
    adjustmentSightAngle->SetLower(0.0f);
    adjustmentSightAngle->SetUpper(365.0f);
    adjustmentSightAngle->SetMinorStep(1.0f);
    adjustmentSightAngle->SetMajorStep(5.0f);

    adjustmentSightRadius->GetSignal(sfg::Adjustment::OnChange).Connect([adjustmentSightRadius, labelSightRadius, this] {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(0) << "Radius: " << adjustmentSightRadius->GetValue() << " ";
        labelSightRadius->SetText(oss.str());
        m_boidMgr.SetSightRadius(adjustmentSightRadius->GetValue());
    });
    adjustmentSightAngle->GetSignal(sfg::Adjustment::OnChange).Connect([adjustmentSightAngle, labelSightAngle, this] {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(0) << "Angle: " << adjustmentSightAngle->GetValue() << " ";
        labelSightAngle->SetText(oss.str());
        m_boidMgr.SetSightAngle(adjustmentSightAngle->GetValue());
    });

    adjustmentSightRadius->SetValue(150.0f);
    adjustmentSightAngle->SetValue(365.0f);

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

    // -------------- SPEED CONTROLS -----------------
    auto labelMinSpeed = sfg::Label::Create();
    auto scaleMinSpeed = sfg::Scale::Create(sfg::Scale::Orientation::HORIZONTAL);
    auto labelMaxSpeed = sfg::Label::Create();
    auto scaleMaxSpeed = sfg::Scale::Create(sfg::Scale::Orientation::HORIZONTAL);

    auto adjustmentMinSpeed = scaleMinSpeed->GetAdjustment();
    adjustmentMinSpeed->SetLower(0.0f);
    adjustmentMinSpeed->SetUpper(1000.0f);
    adjustmentMinSpeed->SetMinorStep(1.0f);
    adjustmentMinSpeed->SetMajorStep(25.0f);
    auto adjustmentMaxSpeed = scaleMaxSpeed->GetAdjustment();
    adjustmentMaxSpeed->SetLower(0.0f);
    adjustmentMaxSpeed->SetUpper(1000.0f);
    adjustmentMaxSpeed->SetMinorStep(1.0f);
    adjustmentMaxSpeed->SetMajorStep(25.0f);

    adjustmentMinSpeed->GetSignal(sfg::Adjustment::OnChange).Connect([adjustmentMinSpeed, adjustmentMaxSpeed, labelMinSpeed, this] {
        float minSpeedVal = adjustmentMinSpeed->GetValue();
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(0) << "Min speed: " << minSpeedVal << " ";
        labelMinSpeed->SetText(oss.str());
        m_boidMgr.SetMinSpeed(minSpeedVal);
        if (adjustmentMaxSpeed->GetValue() < minSpeedVal)
            adjustmentMaxSpeed->SetValue(minSpeedVal);
    });
    adjustmentMaxSpeed->GetSignal(sfg::Adjustment::OnChange).Connect([adjustmentMinSpeed, adjustmentMaxSpeed, labelMaxSpeed, this] {
        float maxSpeedVal = adjustmentMaxSpeed->GetValue();
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(0) << "Max speed: " << maxSpeedVal << " ";
        labelMaxSpeed->SetText(oss.str());
        m_boidMgr.SetMaxSpeed(maxSpeedVal);
        if (adjustmentMinSpeed->GetValue() > maxSpeedVal)
            adjustmentMinSpeed->SetValue(maxSpeedVal);
    });

    adjustmentMinSpeed->SetValue(1.0f);
    adjustmentMaxSpeed->SetValue(100.0f);

    scaleMinSpeed->SetRequisition(sf::Vector2f(80.f, 20.f));
    scaleMaxSpeed->SetRequisition(sf::Vector2f(80.f, 20.f));

    auto boxMinSpeed = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
    boxMinSpeed->Pack(labelMinSpeed, false, false);
    boxMinSpeed->Pack(scaleMinSpeed, false, false);
    boxMinSpeed->SetRequisition(sf::Vector2f(150.0f, 0.0f));
    auto boxMaxSpeed = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
    boxMaxSpeed->Pack(labelMaxSpeed, false, false);
    boxMaxSpeed->Pack(scaleMaxSpeed, false, false);
    boxMaxSpeed->SetRequisition(sf::Vector2f(150.0f, 0.0f));

    auto boxSpeedControls = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.0f);
    boxSpeedControls->Pack(labelSpeedControls);
    boxSpeedControls->Pack(boxMinSpeed);
    boxSpeedControls->Pack(boxMaxSpeed);

    // -------------- DRAW CHECKBUTTON -----------------
    auto checkButtonDrawBody = sfg::CheckButton::Create("Body");
    auto checkButtonDrawVision = sfg::CheckButton::Create("Vision");
    auto checkButtonDrawNeighbors = sfg::CheckButton::Create("Neighbors");
    auto checkButtonDrawVelocity = sfg::CheckButton::Create("Velocity");
    auto checkButtonDrawAcceleration = sfg::CheckButton::Create("Acceleration");
    auto checkButtonDrawFlocks = sfg::CheckButton::Create("Flocks");
    auto checkButtonDrawGrid = sfg::CheckButton::Create("Grid");

    checkButtonDrawBody->GetSignal(sfg::CheckButton::OnToggle).Connect([this, checkButtonDrawBody] { m_boidMgr.SetDrawBody(checkButtonDrawBody->IsActive()); });
    checkButtonDrawVision->GetSignal(sfg::CheckButton::OnToggle).Connect([this, checkButtonDrawVision] { m_boidMgr.SetDrawVision(checkButtonDrawVision->IsActive()); });
    checkButtonDrawNeighbors->GetSignal(sfg::CheckButton::OnToggle).Connect([this, checkButtonDrawNeighbors] { m_boidMgr.SetDrawNeighbors(checkButtonDrawNeighbors->IsActive()); });
    checkButtonDrawVelocity->GetSignal(sfg::CheckButton::OnToggle).Connect([this, checkButtonDrawVelocity] { m_boidMgr.SetDrawVelocity(checkButtonDrawVelocity->IsActive()); });
    checkButtonDrawAcceleration->GetSignal(sfg::CheckButton::OnToggle).Connect([this, checkButtonDrawAcceleration] { m_boidMgr.SetDrawAcceleration(checkButtonDrawAcceleration->IsActive()); });
    checkButtonDrawFlocks->GetSignal(sfg::CheckButton::OnToggle).Connect([this, checkButtonDrawFlocks] { m_boidMgr.SetDrawFlocks(checkButtonDrawFlocks->IsActive()); });
    checkButtonDrawGrid->GetSignal(sfg::CheckButton::OnToggle).Connect([this, checkButtonDrawGrid] { m_boidMgr.SetDrawQuadtree(checkButtonDrawGrid->IsActive()); });

    checkButtonDrawBody->SetActive(true);
    checkButtonDrawVision->SetActive(false);
    checkButtonDrawNeighbors->SetActive(false);
    checkButtonDrawVelocity->SetActive(false);
    checkButtonDrawAcceleration->SetActive(false);
    checkButtonDrawFlocks->SetActive(false);
    checkButtonDrawGrid->SetActive(false);

    auto boxDraw = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.0f);
    boxDraw->Pack(labelDraw);
    boxDraw->Pack(checkButtonDrawBody);
    boxDraw->Pack(checkButtonDrawVision);
    boxDraw->Pack(checkButtonDrawNeighbors);
    boxDraw->Pack(checkButtonDrawVelocity);
    boxDraw->Pack(checkButtonDrawAcceleration);
    boxDraw->Pack(checkButtonDrawFlocks);
    boxDraw->Pack(checkButtonDrawGrid);

    // --------------- SUB BOXES ----------------------

    // -------------- ADD TO MAIN BOX ------------------
    auto mainBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 15.0f);
    mainBox->Pack(boxMultipliers, false);
    mainBox->Pack(boxVisionSettings, false);
    mainBox->Pack(boxSpeedControls, false);
    mainBox->Pack(boxDraw, false);

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