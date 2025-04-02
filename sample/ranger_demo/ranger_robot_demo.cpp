/**
 * @Kit       : Qt-Creator: Desktop
 * @Author    : Wang Zhe
 * @Date      : 2021-04-19  19:41:52
 * @FileName  : ranger_demo.cpp
 * @Mail      : wangzheqie@qq.com
 * Copyright  : AgileX Robotics
 **/
#include <string>

#include <iomanip>

#include "ugv_sdk/mobile_robot/ranger_robot.hpp"

using namespace westonrobot;

int main(int argc, char *argv[]) {
  std::string device_name;
  RangerRobot::Variant variant = RangerRobot::Variant::kRanger;

  if (argc == 2) {
    device_name = {argv[1]};
    std::cout << "Specified CAN: " << device_name << std::endl;
  } else if (argc == 3) {
    device_name = {argv[1]};
    std::string check = argv[2];
    if (check == "mini_v1") {
      variant = RangerRobot::Variant::kRangerMiniV1;
      std::cout << "Specified mini v1" << std::endl;
    } else if (check == "mini_v2") {
      variant = RangerRobot::Variant::kRangerMiniV2;
      std::cout << "Specified mini v2" << std::endl;
    } else if (check == "mini_v3") {
      variant = RangerRobot::Variant::kRangerMiniV3;
      std::cout << "Specified mini v3" << std::endl;
    } else {
      std::cout << "Default to ranger base" << std::endl;
    }
    std::cout << "Specified CAN: " << device_name << std::endl;
  } else {
    std::cout << "Usage: app_ranger_demo <interface> <ranger_model>"
              << std::endl
              << "Example 1: ./app_ranger_demo can0 <mini_v1, mini_v2, mini_v3>"
              << std::endl;
    return -1;
  }

  // RangerMiniV1Robot ranger;
  auto ranger = std::make_shared<RangerRobot>(variant);
  ranger->Connect(device_name);
  ranger->EnableCommandedMode();

  int count = 0;
  while (true) {
    if (count < 2000) {
      ranger->SetMotionMode(RangerInterface::MotionMode::kDualAckerman);
      ranger->SetMotionCommand(0.0, 0.4);
    }

    auto state = ranger->GetRobotState();
    auto motion = ranger->GetActuatorState();
    auto sensor = ranger->GetCommonSensorState();

    std::cout << "-------------------------------" << std::endl;
    std::cout << "count: " << count << std::endl;
    std::cout << "control mode: "
              << static_cast<int>(state.system_state.control_mode)
              << " , vehicle state: "
              << static_cast<int>(state.system_state.vehicle_state)
              << std::endl;
    std::cout << "battery voltage: " << state.system_state.battery_voltage
              << "battery voltage (BMS): " << sensor.bms_basic_state.voltage
              << ", battery current: " << sensor.bms_basic_state.current
              << ", SOC: "
              << static_cast<int>(sensor.bms_basic_state.battery_soc)
              << std::endl;
    std::cout << "velocity (linear, angular, lateral, steering): "
              << std::setw(6) << state.motion_state.linear_velocity << ", "
              << std::setw(6) << state.motion_state.angular_velocity << ", "
              << std::setw(6) << state.motion_state.lateral_velocity << ", "
              << std::setw(6) << state.motion_state.steering_angle << std::endl;

    std::cout << "Wheel angles: " << std::setw(6) << motion.motor_angles.angle_5
              << ", " << std::setw(6) << motion.motor_angles.angle_6 << ", "
              << std::setw(6) << motion.motor_angles.angle_7 << ", "
              << std::setw(6) << motion.motor_angles.angle_8 << std::endl;

    std::cout << "Wheel speeds: " << std::setw(6) << motion.motor_speeds.speed_1
              << ", " << std::setw(6) << motion.motor_speeds.speed_2 << ", "
              << std::setw(6) << motion.motor_speeds.speed_3 << ", "
              << std::setw(6) << motion.motor_speeds.speed_4 << std::endl;
    //        printf("velocity (linear, angular, lateral, steering): %.2f, %.2f,
    //        %.2f, "
    //               "%.2f\n",
    //               state.motion_state.linear_velocity,
    //               state.motion_state.angular_velocity,
    //               state.motion_state.lateral_velocity,
    //               state.motion_state.steering_angle);
    //        std::cout << "-------------------------------" << std::endl;

    usleep(20000);
    ++count;
  }
  return 0;
}
