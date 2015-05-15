#include "IKWalk/IKWalk.hpp"
#include "Spline/CubicSpline.hpp"
#include "Spline/SmoothSpline.hpp"

namespace Leph {

bool IKWalk::walk(HumanoidModel& model, 
    const Parameters& params, 
    double& phase, double dt)
{
    //Compute phase for left and right leg
    double phaseLeft = phase;
    double phaseRight = phase + 0.5;
    boundPhase(phaseLeft);
    boundPhase(phaseRight);

    //Compute the length of a step 
    //(from ground touch to take off) in phase time
    double stepLength = 0.5*params.supportPhaseRatio + 0.5;

    //Build X foot step spline
    //The foot goes backward between t=0 and t=stepLength and
    //then goes forward. Custom velovity (tangents) are applied at
    //foot take off and landing.
    //During foot backward movement, constant velocity is
    //apply because both foot must have the same velocity 
    //during double support phase.
    CubicSpline stepSpline;
    stepSpline.addPoint(0.0, 0.5, -1.0/stepLength);
    stepSpline.addPoint(stepLength, -0.5, -1.0/stepLength);
    stepSpline.addPoint(stepLength, -0.5, params.stepUpVel);
    stepSpline.addPoint(1.0, 0.5, -params.stepDownVel);

    //Build Y trunk swing spline.
    //The trunk lateral oscillation goes from right to left, 
    //wait a bit (swingPause) on left side then goes to the
    //right and pause as well.
    //Trajectory "smoothness" can be tunned with
    //swingVel updating splines tangents. 
    CubicSpline swingSpline;
    swingSpline.addPoint(0.0, -1.0);
    swingSpline.addPoint(params.swingPause/2.0, -1.0);
    swingSpline.addPoint(params.swingPause/2.0, -1.0, params.swingVel);
    swingSpline.addPoint(0.5-params.swingPause/2.0, 1.0, params.swingVel);
    swingSpline.addPoint(0.5-params.swingPause/2.0, 1.0);
    swingSpline.addPoint(0.5+params.swingPause/2.0, 1.0);
    swingSpline.addPoint(0.5+params.swingPause/2.0, 1.0, -params.swingVel);
    swingSpline.addPoint(1.0-params.swingPause/2.0, -1.0, -params.swingVel);
    swingSpline.addPoint(1.0-params.swingPause/2.0, -1.0);
    swingSpline.addPoint(1.0, -1.0, 0.0);

    //Build Z foot rise spline.
    //The foot stays on the ground during backward step and then
    //moves up and down.
    //Custom velocities (tangents) can be tunned to achieve
    //specific trajectory at foot take off and landing.
    CubicSpline riseSpline;
    riseSpline.addPoint(0.0, 0.0);
    riseSpline.addPoint(stepLength, 0.0);
    riseSpline.addPoint(stepLength, 0.0, params.riseUpVel);
    riseSpline.addPoint((1.0+stepLength)/2.0, 1.0);
    riseSpline.addPoint(1.0, 0.0, -params.riseDownVel);
    
    //Build Yaw foot turn spline.
    //This is the same as stepSpline but movement occurs 
    //only during single support phase as robot degrees of freedom
    //could not achieve rotation during double support phase.
    SmoothSpline turnSpline;
    turnSpline.addPoint(0.0, 0.0);
    turnSpline.addPoint(stepLength-0.5, 0.0);
    turnSpline.addPoint(0.5, 1.0);
    turnSpline.addPoint(stepLength, 1.0);
    turnSpline.addPoint(1.0, 0.0);

    //Compute swing value
    double swingVal = params.swingGain
        * swingSpline.posMod(0.5 + phaseLeft + params.swingPhase);

    //Compute feet forward (step) oscillation
    double leftX = params.stepGain*stepSpline.pos(phaseLeft);
    double rightX = params.stepGain*stepSpline.pos(phaseRight);
    
    //Compute feet swing oscillation
    double leftY = swingVal;
    double rightY = swingVal;
    //Compute feet lateral movement oscillation
    leftY += params.lateralGain*(stepSpline.pos(phaseLeft) + 0.5);
    rightY += params.lateralGain*(stepSpline.pos(phaseRight) + 0.5);
    //Set feet lateral offset (feet distance from trunk center)
    leftY += params.footYOffset;
    rightY += -params.footYOffset;
    
    //Compute feet vertical (rise) oscillation and offset
    double leftZ = params.riseGain*riseSpline.pos(phaseLeft);
    double rightZ = params.riseGain*riseSpline.pos(phaseRight);
    //Set trunk to foot distance height offset
    leftZ += params.trunkZOffset;
    rightZ += params.trunkZOffset;
    
    //Compute feet rotation (turn) oscillation
    double leftYaw = params.turnGain*turnSpline.pos(phaseLeft);
    double rightYaw = params.turnGain*turnSpline.pos(phaseRight);
    
    //Set feet orientation
    double leftPitch = params.trunkPitch;
    double leftRoll = params.trunkRoll;
    double rightPitch = params.trunkPitch;
    double rightRoll = params.trunkRoll;
    
    //Add custom extra foot offset on both feet
    leftX += params.extraLeftX;
    leftY += params.extraLeftY;
    leftZ += params.extraLeftZ;
    leftYaw += params.extraLeftYaw;
    rightX += params.extraRightX;
    rightY += params.extraRightY;
    rightZ += params.extraRightZ;
    rightYaw += params.extraRightYaw;

    //Build rotation matrix for trunk pitch and roll
    //orientation
    Eigen::AngleAxisd pitchRot(-params.trunkPitch, Eigen::Vector3d::UnitY());
    Eigen::AngleAxisd rollRot(-params.trunkRoll, Eigen::Vector3d::UnitX());
    Eigen::Quaternion<double> quat(pitchRot * rollRot);
    Eigen::Matrix3d rotation = quat.matrix();

    //Build target vector.
    //Used Euler angles orders is Pitch Roll Yaw because
    //Yaw has to be applied last, after the foot get the good
    //ground orientation. Roll has to be applied after Pitch.
    Eigen::Vector3d posLeft(leftX, leftY, leftZ);
    Eigen::Vector3d angleLeft(leftPitch, leftRoll, leftYaw);
    Eigen::Vector3d posRight(rightX, rightY, rightZ);
    Eigen::Vector3d angleRight(rightPitch, rightRoll, rightYaw);
    
    //Rotate built feet trajectory to
    //meet asked trunk Pitch and Roll new
    //ground orientation
    posLeft = rotation*posLeft;
    posRight = rotation*posRight;
    
    //Apply trunk X-Y offset
    posLeft(0) -= params.trunkXOffset;
    posRight(0) -= params.trunkXOffset;
    posLeft(1) -= params.trunkYOffset;
    posRight(1) -= params.trunkYOffset;

    //In case of trunk Roll rotation, an height (Z) 
    //positive offset have to be applied on external foot to
    //set both feet on same level
    double deltaLen = model.feetDistance()*tan(params.trunkRoll);
    if (params.trunkRoll > 0.0) {
        posRight(2) += deltaLen;
    } else if (params.trunkRoll < 0.0) {
        posLeft(2) -= deltaLen;
    }
    
    //Trunk X and Y offset is applied to compensate
    //Pitch and Roll rotation. It is better for tunning if
    //trunk pitch or roll rotation do not apply offset on
    //trunk position.
    posLeft(0) += (model.legsLength())*tan(params.trunkPitch);
    posRight(0) += (model.legsLength())*tan(params.trunkPitch);
    posLeft(1) -= (model.legsLength())*tan(params.trunkRoll);
    posRight(1) -= (model.legsLength())*tan(params.trunkRoll);

    //Run inverse invert kinematics on both legs
    //using Pitch-Roll-Yaw convention
    bool successLeft = model.legIkLeft("foot tip init", 
        posLeft, angleLeft, EulerPitchRollYaw);
    bool successRight = model.legIkRight("foot tip init", 
        posRight, angleRight, EulerPitchRollYaw);

    //Check inverse kinematics success
    if (!successLeft || !successRight) {
        return false;
    }

    //Increment given phase
    phase += dt*params.freq;
    //Cycling between  and 1
    boundPhase(phase);

    return true;
}
        
void IKWalk::boundPhase(double& phase)
{
    while (phase >= 1.0) {
        phase -= 1.0;
        //Bound to zero in case 
        //of floating point error
        if (phase < 0.0) {
            phase = 0.0;
        }
    }
}

}
