#pragma once

#include "mbed.h"
#include "lidar.hpp"        // Ton capteur Lidar
#include "Differentiel.hpp"

class LidarAnalyzer {
public:
    LidarAnalyzer(Lidar* lidar, differentiel* robot, DigitalOut* status_led);

    void update();

    bool isObstacleDetected() const;
    float getObstacleDistance() const;
    float getObstacleAngle() const;
    float getObstacleX() const;
    float getObstacleY() const;

private:
    Lidar* lidar_;
    differentiel* robot_;
    DigitalOut* led_;

    bool stop_;
    int NbDetecLidarPack;
    int NbNoDetecLidarPack;
    float DistanceLidar;
    float AngleLidar;
    float PointLidarX;
    float PointLidarY;

    bool isAngleInRange(float angle, float min, float max) const;
};