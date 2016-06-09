#include <iostream>
#include "Viewer/ModelViewer.hpp"
#include "Viewer/ModelDraw.hpp"
#include "Model/HumanoidFixedModel.hpp"

int main()
{
    //Initialize the Model
    Leph::HumanoidFixedModel model(Leph::SigmabanModel);
    Leph::ModelViewer viewer(1200, 900);

    //Camera parameters in width and height
    Leph::CameraParameters camParams = {80*M_PI/180.0, 50*M_PI/180.0};

    double t = 0.0;
    while (viewer.update()) {
        t += 0.01;
        //Random trunk position and orientation
        model.get().setDOF("base_x", 0.15*sin(t));
        model.get().setDOF("base_y", 0.1*sin(2*t));
        model.get().setDOF("base_yaw", -0.5*sin(3*t));
        model.get().setDOF("left_hip_pitch", -0.5 + 0.5*sin(t));
        model.get().setDOF("left_hip_yaw", 0.2 + 0.2*sin(2*t));
        model.get().setDOF("left_hip_roll", 0.1 + 0.4*sin(2*t));
        
        //Camera world target
        Eigen::Vector3d target = model.get().selfInFrame("origin", Eigen::Vector3d(0.5, 0.5, 0.0));

        //Height pixel in camera target
        double pixelTarget = 0.5;

        //Compute head Inverse Kinematics
        model.get().cameraLookAt(camParams, target, pixelTarget);
        //Draw target
        viewer.drawFrame(target, Eigen::Matrix3d::Identity());
        //Draw view line on pixel target
        Eigen::Vector3d groundPos;
        model.get().cameraPixelToWorld(camParams, Eigen::Vector2d(0.0, pixelTarget), groundPos);
        viewer.drawLink(model.get().position("camera", "origin"), groundPos);
        
        //Display model and view box
        Leph::ModelDraw(model.get(), viewer);
        Leph::CameraDraw(camParams, model.get(), viewer);

        //Pixel to World and World to Pixel computation checks
        Eigen::Vector2d pixel(-1.0, 0.4);
        Eigen::Vector3d ground;
        Eigen::Vector2d pixelCheck;
        bool success1 = model.get().cameraPixelToWorld(camParams, pixel, ground);
        std::cout << "Pixel: " << pixel.transpose() << std::endl;
        std::cout << "Ground: " << ground.transpose() << std::endl;
        bool success2 = model.get().cameraWorldToPixel(camParams, ground, pixelCheck);
        std::cout << "PixelCheck: " << pixelCheck.transpose() << std::endl;
        if (!success1 || !success2 || (pixel-pixelCheck).norm() > 0.0001) {
            std::cout << "ASSERT ERROR" << std::endl;
            return 1;
        }

        //Pixel to PanTilt and PanTilt to Pixel computation check
        Eigen::Vector2d pixelTarget2(0.4, 0.5);
        Eigen::Vector2d angles = model.get().cameraPixelToPanTilt(
            camParams, pixelTarget2);
        std::cout << "Angles: " << angles(0)*180.0/M_PI << " " << angles(1)*180.0/M_PI << std::endl;
        Eigen::Vector2d pixelCheck2;
        model.get().cameraPanTiltToPixel(camParams, angles, pixelCheck2);
        std::cout << "Pixel2: " << pixelCheck2.transpose() << std::endl;
        if ((pixelCheck2 - pixelTarget2).norm() > 0.0001) {
            std::cout << "ASSERT ERROR" << std::endl;
            return 1;
        }
    }

    return 0;
}
