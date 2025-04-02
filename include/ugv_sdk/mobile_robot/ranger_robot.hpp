/*
 * ranger_robot.hpp
 *
 * Created on: Jul 14, 2021 21:45
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#ifndef RANGER_ROBOT_HPP
#define RANGER_ROBOT_HPP

#include <cmath>

#include "ugv_sdk/details/robot_base/ranger_base.hpp"

namespace westonrobot {
class RangerRobot : public RobotCommonInterface, public RangerInterface {
 public:
 enum class Variant {
  kRangerMiniV1 = 0,
  kRangerMiniV2,
  kRangerMiniV3,
  kRanger,
};

  RangerRobot(Variant variant);
  ~RangerRobot();

  bool Connect(std::string can_name) override;

  void EnableCommandedMode() override;
  std::string RequestVersion(int timeout_sec) override;

  // functions to be implemented by each robot class
  void ResetRobotState() override;

  void DisableLightControl() {
    // do nothing if no light on robot
  }

  ProtocolVersion GetParserProtocolVersion() override;

  // robot control
  void SetMotionMode(uint8_t mode) override;
  void SetMotionCommand(double linear_vel, double steer_angle,
                        double angular_vel = 0.0) override;
  void SetLightCommand(AgxLightMode f_mode, uint8_t f_value,
                       AgxLightMode r_mode, uint8_t r_value) override;

  // get robot state
  RangerCoreState GetRobotState() override;
  RangerActuatorState GetActuatorState() override;
  RangerCommonSensorState GetCommonSensorState() override;

 private:
  RobotCommonInterface* robot_;
};
}  // namespace westonrobot

#endif /* RANGER_ROBOT_HPP */
