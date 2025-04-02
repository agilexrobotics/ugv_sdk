/*
 * ranger_robot.cpp
 *
 * Created on: Feb 23, 2023 17:10
 * Description:
 *
 * Copyright (c) 2023 Weston Robot Pte. Ltd.
 */
#include "ugv_sdk/mobile_robot/ranger_robot.hpp"
#include "ugv_sdk/details/robot_base/ranger_base.hpp"

namespace westonrobot {
  RangerRobot::RangerRobot(Variant variant) {
    if (variant == Variant::kRangerMiniV1) {
      robot_ = new RangerMiniV1Base();
    } else if (variant == Variant::kRangerMiniV2) {
      robot_ = new RangerMiniV2Base();
    } else if (variant == Variant::kRangerMiniV3) {
      robot_ = new RangerMiniV3Base();
    } else {
      robot_ = new RangerBase();
    }
  }

RangerRobot::~RangerRobot() {
  if (robot_) delete robot_;
}

bool RangerRobot::Connect(std::string can_name) {
  return robot_->Connect(can_name);
}

void RangerRobot::EnableCommandedMode() { robot_->EnableCommandedMode(); }

std::string RangerRobot::RequestVersion(int timeout_sec) {
  return robot_->RequestVersion(timeout_sec);
}

// functions to be implemented by each robot class
void RangerRobot::ResetRobotState() { robot_->ResetRobotState(); }

ProtocolVersion RangerRobot::GetParserProtocolVersion() {
  return robot_->GetParserProtocolVersion();
}

// robot control
void RangerRobot::SetMotionMode(uint8_t mode) {
  auto ranger = dynamic_cast<RangerInterface*>(robot_);
  return ranger->SetMotionMode(mode);
}

void RangerRobot::SetMotionCommand(double linear_vel, double steer_angle,
                                   double angular_vel) {
  auto ranger = dynamic_cast<RangerInterface*>(robot_);
  return ranger->SetMotionCommand(linear_vel, steer_angle, angular_vel);
}

void RangerRobot::SetLightCommand(AgxLightMode f_mode, uint8_t f_value,
                                  AgxLightMode r_mode, uint8_t r_value) {
  auto ranger = dynamic_cast<RangerInterface*>(robot_);
  return ranger->SetLightCommand(f_mode, f_value, r_mode, r_value);
}

// get robot state
RangerCoreState RangerRobot::GetRobotState() {
  auto ranger = dynamic_cast<RangerInterface*>(robot_);
  return ranger->GetRobotState();
}

RangerActuatorState RangerRobot::GetActuatorState() {
  auto ranger = dynamic_cast<RangerInterface*>(robot_);
  return ranger->GetActuatorState();
}
RangerCommonSensorState RangerRobot::GetCommonSensorState() {
  auto ranger = dynamic_cast<RangerInterface*>(robot_);
  return ranger->GetCommonSensorState();
}
}  // namespace westonrobot