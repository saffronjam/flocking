#include "BoidMgr.h"

BoidMgr::BoidMgr()
    : m_repulsionBorders(-Camera::GetOffset() * 12.0f, sf::Vector2f(Window::GetWidth() - 200.0f, Window::GetHeight()) * 12.0f),
      m_quadtreeRect(-Camera::GetOffset() * 12.0f, sf::Vector2f(Window::GetWidth() - 200.0f, Window::GetHeight()) * 12.0f),
      m_quadtreeBox(vl::Null<>(), sf::Vector2f(m_quadtreeRect.width, m_quadtreeRect.width) / 250.0f),
      m_quadtreeGrid(sf::PrimitiveType::Lines),
      m_paused(false),
      m_drawBody(true),
      m_drawVision(false),
      m_drawNeighbors(false),
      m_drawVelocity(false),
      m_drawAcceleration(false),
      m_drawFlocks(false),
      m_drawQuadtree(false)
{
    SetBoidCount(200);
    for (auto &boid : m_boids)
    {
        sf::Vector2f randomForce;
        while ((randomForce = Random::Vec2(-1.0f, -1.0f, 1.0f, 1.0f) * 1000.0f) == vl::Null<>())
            ;
        boid.ApplyForce(randomForce);
    }

    // Setup quadtree
    m_quadtree.resize(m_quadtreeRect.width / m_quadtreeBox.width);
    for (auto &col : m_quadtree)
    {
        col.resize(m_quadtreeRect.height / m_quadtreeBox.height);
    }

    sf::RectangleShape rectShape(sf::Vector2f(m_quadtreeBox.width, m_quadtreeBox.height));
    rectShape.setFillColor(sf::Color::Transparent);
    rectShape.setOutlineThickness(1.0f / Camera::GetZoom());
    rectShape.setOutlineColor(sf::Color(150, 150, 150, 10));

    for (size_t i = 0; i <= m_quadtree.size(); i++)
    {
        m_quadtreeGrid.append(sf::Vertex(sf::Vector2f(m_quadtreeRect.left + i * m_quadtreeBox.width, m_quadtreeRect.top), sf::Color(50, 50, 50, 15)));
        m_quadtreeGrid.append(sf::Vertex(sf::Vector2f(m_quadtreeRect.left + i * m_quadtreeBox.width, m_quadtreeRect.top + m_quadtreeRect.height), sf::Color(50, 50, 50, 15)));
    }
    for (size_t j = 0; j <= m_quadtree[0].size(); j++)
    {
        m_quadtreeGrid.append(sf::Vertex(sf::Vector2f(m_quadtreeRect.left, m_quadtreeRect.top + j * m_quadtreeBox.height), sf::Color(50, 50, 50, 15)));
        m_quadtreeGrid.append(sf::Vertex(sf::Vector2f(m_quadtreeRect.left + m_quadtreeRect.width, m_quadtreeRect.top + j * m_quadtreeBox.height), sf::Color(50, 50, 50, 15)));
    }
}

void BoidMgr::Update()
{
    ClearQuadtree();
    ComputeQuadTree();
    if (m_drawFlocks)
        ComputeFlocks();
    ComputeAllVisibleNeighbors();

    for (auto &boid : m_boids)
    {
        if (!m_paused)
        {
            boid.ApplyForce(boid.GetSeparationForce() * boid.GetSeparationMultiplier());
            boid.ApplyForce(boid.GetAlignmentForce() * boid.GetAlignmentMultiplier());
            boid.ApplyForce(boid.GetCohesionForce() * boid.GetCohesionMultiplier());
            boid.ApplyForce(GetRepulsionBorderForce(boid));
        }
        boid.ReconstructVisionShape();
    }
    if (!m_paused)
    {
        for (auto &boid : m_boids)
        {
            boid.Update();
            if (!m_drawAcceleration)
                boid.ResetForce();
        }
    }
}

void BoidMgr::Draw()
{
    if (m_drawVision)
        for (auto &boid : m_boids)
        {
            boid.DrawSight();
        }

    if (m_drawNeighbors)

        for (auto &boid : m_boids)
        {
            boid.DrawVisibleNeighbors();
        }

    if (m_drawVelocity)
        for (auto &boid : m_boids)
        {
            boid.DrawVelocity();
        }

    if (m_drawAcceleration)
        for (auto &boid : m_boids)
        {
            boid.DrawAcceleration();
            boid.ResetForce();
        }

    if (m_drawBody)
        for (auto &boid : m_boids)
        {
            boid.DrawBody();
        }

    if (m_drawQuadtree)
        DrawQuadTree();

    if (m_drawFlocks)
        DrawFlockConvexHull();
}

void BoidMgr::DrawQuadTree()
{
    Camera::Draw(m_quadtreeGrid);

    sf::RectangleShape rectShape(sf::Vector2f(m_quadtreeBox.width, m_quadtreeBox.height));
    rectShape.setFillColor(sf::Color::Transparent);
    rectShape.setOutlineThickness(1.0f / Camera::GetZoom());
    rectShape.setOutlineColor(sf::Color::Red);
    for (auto &coord : m_activeContainers)
    {
        rectShape.setPosition(coord.first * m_quadtreeBox.width + m_quadtreeRect.left,
                              coord.second * m_quadtreeBox.height + m_quadtreeRect.top);
        Camera::Draw(rectShape);
    }
}

void BoidMgr::DrawFlockConvexHull()
{
    if (true)
    {
        for (auto flock : m_flocks)
        {
            std::vector<sf::Vector2f> flockPoints;
            for (auto &boid : flock)
            {
                flockPoints.push_back(boid->GetPosition());
            }
            if (flockPoints.size() < 3)
                continue;

            std::vector<sf::Vector2f> wrapped(Lib::WrapPoints(flockPoints));

            sf::VertexArray points(sf::PrimitiveType::Lines, wrapped.size());
            for (size_t i = 0; i < wrapped.size() - 1; i++)
            {
                points[i].position = wrapped[i];
            }
            points[wrapped.size() - 1].position = wrapped.front();

            Camera::Draw(points);
        }
    }
}

void BoidMgr::Pause() noexcept
{
    m_paused = true;
}

void BoidMgr::Resume() noexcept
{
    m_paused = false;
}

void BoidMgr::SetBoidCount(size_t count)
{
    while (m_boids.size() != count)
    {
        if (m_boids.size() < count)
        {
            sf::Vector2f randomPosition = Random::Vec2(-400.0f, -400.0f, 400.0f, 400.0f);
            m_boids.push_back(Boid(randomPosition));
        }
        else
        {
            m_boids.pop_back();
        }
    }
}

void BoidMgr::SetSeparationMultiplier(float multiplier) noexcept
{
    for (auto &boid : m_boids)
    {
        boid.SetSeparationMultiplier(multiplier);
    }
}

void BoidMgr::SetAlignmentMultiplier(float multiplier) noexcept
{
    for (auto &boid : m_boids)
    {
        boid.SetAlignmentMultiplier(multiplier);
    }
}

void BoidMgr::SetCohesionMultiplier(float multiplier) noexcept
{
    for (auto &boid : m_boids)
    {
        boid.SetCohesionMultiplier(multiplier);
    }
}

void BoidMgr::SetMinSpeed(float speed) noexcept
{
    for (auto &boid : m_boids)
    {
        boid.SetMinSpeed(speed);
    }
}

void BoidMgr::SetMaxSpeed(float speed) noexcept
{
    for (auto &boid : m_boids)
    {
        boid.SetMaxSpeed(speed);
    }
}

void BoidMgr::SetSightRadius(float radius) noexcept
{
    for (auto &boid : m_boids)
    {
        boid.SetSightRadius(radius);
    }
}

void BoidMgr::SetSightAngle(float angle) noexcept
{
    for (auto &boid : m_boids)
    {
        boid.SetSightAngle(angle);
    }
}

void BoidMgr::ComputeAllVisibleNeighbors()
{
    for (auto &index : m_activeContainers)
    {
        for (auto &boid : m_quadtree[index.first][index.second])
        {
            boid->ClearNeighbors();
            boid->ClearVisibleNeighbors();

            int nLayersToCheck = static_cast<int>(std::floor(static_cast<int>(static_cast<float>(boid->GetSightRadius() / std::min(m_quadtreeBox.width, m_quadtreeBox.height)) * 2.0f) + 1));
            for (int i = 0; i < nLayersToCheck + 2; i++)
            {
                for (int j = 0; j < nLayersToCheck + 2; j++)
                {
                    size_t x = Lib::Constrain(index.first - nLayersToCheck / 2 + static_cast<size_t>(i), 0ull, m_quadtree.size() - 1);
                    size_t y = Lib::Constrain(index.second - nLayersToCheck / 2 + static_cast<size_t>(j), 0ull, m_quadtree[x].size() - 1);

                    for (auto &otherBoid : m_quadtree[x][y])
                    {
                        if (boid == otherBoid)
                            continue;

                        const sf::Vector2f _thisPos = boid->GetPosition();
                        const sf::Vector2f _otherPos = otherBoid->GetPosition();
                        const sf::Vector2f forward = boid->GetForward();
                        const auto sightBounds = boid->GetSightBounds();

                        const bool inSeeingDistance = vl::Distance(_thisPos, _otherPos) < boid->GetSightRadius();
                        const bool inPeripheral = !vl::IsLeft(_thisPos, sightBounds.first, _otherPos) && vl::IsLeft(_thisPos, sightBounds.second, _otherPos);
                        const bool inFront = vl::IsLeft(vl::Perpendicular(forward), _thisPos, _otherPos);
                        const bool angleBiggerThan180 = boid->GetSightAngle() > 180.0f;
                        const bool angleBiggerThan360 = boid->GetSightAngle() > 360.0f;

                        if (inSeeingDistance)
                        {
                            boid->AddNeighbor(otherBoid);
                        }

                        if (inSeeingDistance && (inPeripheral ||
                                                 (angleBiggerThan180 && inFront) ||
                                                 (angleBiggerThan360)))
                        {
                            boid->AddVisibleNeighbor(otherBoid);
                        }
                    }
                }
            }
        }
    }
}

void BoidMgr::ComputeFlocks()
{
    m_flocks.clear();
    for (auto &boid : m_boids)
    {
        boid.SetInFlock(false);
    }
    for (auto &boid : m_boids)
    {
        if (!boid.InFlock())
        {
            std::set<Boid *> currentFlock;
            IterativeFlockCheck(boid, currentFlock);
            m_flocks.push_back(currentFlock);
        }
    }
}

void BoidMgr::IterativeFlockCheck(const Boid &boid, std::set<Boid *> &currentFlock)
{
    for (auto &neighbor : boid.GetNeighbors())
    {
        if (currentFlock.emplace(const_cast<Boid *>(neighbor)).second)
        {
            neighbor->SetInFlock(true);
            IterativeFlockCheck(*neighbor, currentFlock);
        }
    }
}

sf::Vector2f BoidMgr::GetRepulsionBorderForce(const Boid &boid) const
{
    const sf::Vector2f position = boid.GetPosition();
    auto &box = m_repulsionBorders;

    const auto xRange = std::make_pair(box.left, box.left + box.width);
    const auto yRange = std::make_pair(box.top, box.top + box.height);

    float fromLeft = Lib::Map(position.x, xRange, std::make_pair(-1.0f, 1.0f));
    float fromTop = Lib::Map(position.y, yRange, std::make_pair(-1.0f, 1.0f));
    float fromRight = -fromLeft;
    float fromBottom = -fromTop;

    fromLeft = Lib::Constrain(fromLeft, -100.0f, 1.0f);
    fromTop = Lib::Constrain(fromTop, -100.0f, 1.0f);
    fromRight = Lib::Constrain(fromRight, -100.0f, 1.0f);
    fromBottom = Lib::Constrain(fromBottom, -100.0f, 1.0f);

    sf::Vector2f force(0.0f, 0.0f);
    float lowering = 1.0f;
    force.x += Lib::Constrain(std::pow((fromLeft - 1.0f) / 2.0f, 128.0f) - lowering, 0.0f, std::numeric_limits<float>::infinity());
    force.y += Lib::Constrain(std::pow((fromTop - 1.0f) / 2.0f, 128.0f) - lowering, 0.0f, std::numeric_limits<float>::infinity());
    force.x -= Lib::Constrain(std::pow((fromRight - 1.0f) / 2.0f, 128.0f) - lowering, 0.0f, std::numeric_limits<float>::infinity());
    force.y -= Lib::Constrain(std::pow((fromBottom - 1.0f) / 2.0f, 128.0f) - lowering, 0.0f, std::numeric_limits<float>::infinity());

    return force;
}

void BoidMgr::ComputeQuadTree()
{
    for (auto &boid : m_boids)
    {
        const sf::Vector2f &position = boid.GetPosition();
        sf::Vector2i middleIndex((position.x - m_quadtreeRect.left) / m_quadtreeBox.width, (position.y - m_quadtreeRect.top) / m_quadtreeBox.height);

        middleIndex.x = Lib::Constrain(middleIndex.x, 0, static_cast<int>(m_quadtree.size()) - 1);
        middleIndex.y = Lib::Constrain(middleIndex.y, 0, static_cast<int>(m_quadtree[middleIndex.x].size()) - 1);

        m_quadtree[middleIndex.x][middleIndex.y].push_back(&boid);
        m_activeContainers.emplace(middleIndex.x, middleIndex.y);
    }
}

void BoidMgr::ClearQuadtree() noexcept
{
    for (auto &index : m_activeContainers)
    {
        m_quadtree[index.first][index.second].clear();
    }
    m_activeContainers.clear();
}
