#pragma once

#include <memory>

#include "Boid.hpp"
#include "InputUtility.hpp"

class FlockingController
{
public:
    class InteractivePoint
    {
    public:
        InteractivePoint(sf::Vector2f position, float const &strength)
            : position(position),
              strength(strength) {}
        sf::Vector2f position;
        float strength;
    };

public:
    FlockingController(InputUtility &iu, int const &nBoids);

    void Update(sf::Time const &dt);
    void Draw(Graphics &gfx);

    void AddGlobalAttractionPoint(InteractivePoint const &point) { m_globalAttractionPoints.push_back(point); }
    void AddGlobalRepulsionPoint(InteractivePoint const &point) { m_globalRepulsionPoints.push_back(point); }
    void RemoveGlobalAttractionPoint(InteractivePoint const &point) { m_globalAttractionPoints.push_back(point); }
    void RemoveGlobalRepulsionPoint(InteractivePoint const &point) { m_globalRepulsionPoints.push_back(point); }

    void AddLocalAttractionPoint(InteractivePoint const &point) { m_localAttractionPoints.push_back(point); }
    void AddLocalRepulsionPoint(InteractivePoint const &point) { m_localRepulsionPoints.push_back(point); }
    void RemoveLocalAttractionPoint(InteractivePoint const &point) { m_localAttractionPoints.push_back(point); }
    void RemoveLocalRepulsionPoint(InteractivePoint const &point) { m_localRepulsionPoints.push_back(point); }

    float &GetSeeingDistanceRef() { return m_seeingDistance; }
    float &GetSeeingAngleRef() { return m_seeingAngle; }
    float &GetSeparationStrengthRef() { return m_separationStrength; }
    float &GetAlignmentStrengthRef() { return m_alignmentStrength; }
    float &GetCohesionStrengthRef() { return m_cohesionStrength; }
    float &GetSpeedRef() { return m_speed; }

    bool &GetDrawBoidsRef() { return m_drawBoids; }
    bool &GetDrawVisibleNeighborRef() { return m_drawVisibleNeighborLines; }
    bool &GetDrawVisionRef() { return m_drawVision; }
    bool &GetDrawFlocksRef() { return m_drawFlocks; }

    bool *GetNeutralMouseModePtr() { return &m_neutralMouseMode; }
    bool *GetAttractMouseModePtr() { return &m_attractMouseMode; }
    bool *GetRepelMouseModePtr() { return &m_repelMouseMode; }

private:
    void Flock();
    void ComputeBoidsNeighbors();
    void ComputeBoidsFlockMates();
    void IterativeFlockCheck(std::shared_ptr<Boid> const &boid, std::set<std::shared_ptr<Boid>> &currentFlock);

private:
    InputUtility &m_iu;

    std::vector<std::shared_ptr<Boid>> m_allBoids;
    std::vector<std::set<std::shared_ptr<Boid>>> m_allFlocks;
    sf::FloatRect m_simulationBox;

    std::vector<InteractivePoint> m_globalAttractionPoints;
    std::vector<InteractivePoint> m_globalRepulsionPoints;

    std::vector<InteractivePoint> m_localAttractionPoints;
    std::vector<InteractivePoint> m_localRepulsionPoints;

    float m_seeingDistance;
    float m_seeingAngle;
    float m_separationStrength;
    float m_alignmentStrength;
    float m_cohesionStrength;
    float m_speed;

    bool m_drawBoids;
    bool m_drawVisibleNeighborLines;
    bool m_drawVision;
    bool m_drawFlocks;

    bool m_neutralMouseMode;
    bool m_attractMouseMode;
    bool m_repelMouseMode;

    bool m_currentTimer;
};
