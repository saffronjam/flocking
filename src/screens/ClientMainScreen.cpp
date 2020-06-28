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

    // -------------- SLEEP DELAY ------------------
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

    auto multipliersBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.0f);
    multipliersBox->Pack(labelMultipliers);
    multipliersBox->Pack(boxSeparation);
    multipliersBox->Pack(boxAlignment);
    multipliersBox->Pack(boxCohesion);

    // -------------- ALL BUTTONS ------------------
    auto startButton = sfg::Button::Create("Start");
    auto restartButton = sfg::Button::Create("Restart");
    auto resetButton = sfg::Button::Create("Reset");
    auto pauseButton = sfg::Button::Create("Pause");
    auto resumeButton = sfg::Button::Create("Resume");

    startButton->GetSignal(sfg::Widget::OnLeftClick).Connect([this] {});
    restartButton->GetSignal(sfg::Widget::OnLeftClick).Connect([this] {});
    resetButton->GetSignal(sfg::Widget::OnLeftClick).Connect([this] {});
    pauseButton->GetSignal(sfg::Widget::OnLeftClick).Connect([this] {});
    resumeButton->GetSignal(sfg::Widget::OnLeftClick).Connect([this] {});

    auto buttonsBoxRow0 = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 3.0f);
    auto buttonsBoxRow1 = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 3.0f);

    buttonsBoxRow0->Pack(startButton);
    buttonsBoxRow0->Pack(restartButton);
    buttonsBoxRow0->Pack(resetButton);
    buttonsBoxRow1->Pack(pauseButton);
    buttonsBoxRow1->Pack(resumeButton);

    auto allButtonBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);
    allButtonBox->Pack(buttonsBoxRow0, false);
    allButtonBox->Pack(buttonsBoxRow1, false);

    // --------------- SUB BOXES ----------------------

    // -------------- ADD TO MAIN BOX ------------------
    auto mainBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 15.0f);
    mainBox->Pack(allButtonBox, false);
    mainBox->Pack(multipliersBox, false);

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