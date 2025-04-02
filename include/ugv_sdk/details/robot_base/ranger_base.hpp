/**
 * @Kit       : Qt-Creator: Desktop
 * @Author    : Wang Zhe
 * @Date      : 2021-04-19  19:12:52
 * @FileName  : ranger_base.hpp
 * @Mail      : wangzheqie@qq.com
 * Copyright  : AgileX Robotics
 **/

 #ifndef RANGER_BASE_HPP
 #define RANGER_BASE_HPP
 
 #include <cstdint>
 #include <mutex>
 #include <string>
 #include <thread>
 
 #include "ugv_sdk/details/interface/ranger_interface.hpp"
 #include "ugv_sdk/details/robot_base/agilex_base.hpp"
 
 #include "ugv_sdk/details/protocol_v2/protocol_v2_parser.hpp"
 
 namespace westonrobot {
 class RangerBase : public AgilexBase<ProtocolV2Parser>, public RangerInterface {
  public:
   RangerBase() : AgilexBase<ProtocolV2Parser>() {};
   virtual ~RangerBase() = default;
 
   // set up connection
   bool Connect(std::string can_name) override {
     return AgilexBase<ProtocolV2Parser>::Connect(can_name);
   }
 
   // robot control
   void SetMotionMode(uint8_t mode) { AgilexBase::SetMotionMode(mode); }
 
   void SetMotionCommand(double linear_vel, double steer_angle,
                         double angular_vel = 0.0) override {
     AgilexBase<ProtocolV2Parser>::SendMotionCommand(linear_vel, angular_vel,
                                                     0.0, steer_angle);
   }
 
   void SetLightCommand(AgxLightMode f_mode, uint8_t f_value,
                        AgxLightMode r_mode, uint8_t r_value) override {
     AgilexBase<ProtocolV2Parser>::SendLightCommand(f_mode, f_value, r_mode,
                                                    r_value);
   }
 
   void DisableLightControl() override {
     AgilexBase<ProtocolV2Parser>::DisableLightControl();
   }
 
   // get robot state
   RangerCoreState GetRobotState() override {
     auto state = AgilexBase<ProtocolV2Parser>::GetRobotCoreStateMsgGroup();
 
     RangerCoreState ranger_state;
     ranger_state.time_stamp = state.time_stamp;
     ranger_state.system_state = state.system_state;
     ranger_state.motion_state = state.motion_state;
     ranger_state.light_state = state.light_state;
     ranger_state.rc_state = state.rc_state;
     ranger_state.motion_mode_state = state.motion_mode_state;
 
     return ranger_state;
   }
 
   RangerActuatorState GetActuatorState() override {
     auto actuator = AgilexBase<ProtocolV2Parser>::GetActuatorStateMsgGroup();
 
     RangerActuatorState ranger_actuator;
     ranger_actuator.time_stamp = actuator.time_stamp;
 
     ranger_actuator.motor_speeds.speed_1 = actuator.motor_speeds.speed_1;
     ranger_actuator.motor_speeds.speed_2 = actuator.motor_speeds.speed_2;
     ranger_actuator.motor_speeds.speed_3 = actuator.motor_speeds.speed_3;
     ranger_actuator.motor_speeds.speed_4 = actuator.motor_speeds.speed_4;
     ranger_actuator.motor_angles.angle_5 = actuator.motor_angles.angle_5;
     ranger_actuator.motor_angles.angle_6 = actuator.motor_angles.angle_6;
     ranger_actuator.motor_angles.angle_7 = actuator.motor_angles.angle_7;
     ranger_actuator.motor_angles.angle_8 = actuator.motor_angles.angle_8;
 
     for (int i = 0; i < 8; ++i) {
       ranger_actuator.actuator_hs_state[i] = actuator.actuator_hs_state[i];
       ranger_actuator.actuator_ls_state[i] = actuator.actuator_ls_state[i];
     }
     return ranger_actuator;
   }
 
   RangerCommonSensorState GetCommonSensorState() override {
     auto common_sensor =
         AgilexBase<ProtocolV2Parser>::GetCommonSensorStateMsgGroup();
 
     RangerCommonSensorState ranger_bms;
 
     ranger_bms.time_stamp = common_sensor.time_stamp;
 
     ranger_bms.bms_basic_state.current = common_sensor.bms_basic_state.current;
     ranger_bms.bms_basic_state.voltage =
         common_sensor.bms_basic_state.voltage;
     ranger_bms.bms_basic_state.battery_soc =
         common_sensor.bms_basic_state.battery_soc;
     ranger_bms.bms_basic_state.battery_soh =
         common_sensor.bms_basic_state.battery_soh;
     ranger_bms.bms_basic_state.temperature =
         common_sensor.bms_basic_state.temperature;
 
     return ranger_bms;
   }
 };
 
 using RangerMiniV3Base = RangerBase;
 class RangerMiniV2Base : public RangerBase {
   RangerCommonSensorState GetCommonSensorState() override {
     auto common_sensor =
         AgilexBase<ProtocolV2Parser>::GetCommonSensorStateMsgGroup();
 
     RangerCommonSensorState ranger_bms;
 
     ranger_bms.time_stamp = common_sensor.time_stamp;
 
     ranger_bms.bms_basic_state.current = common_sensor.bms_basic_state.current;
     // Note: BMS CAN message definition is not consistent across AgileX robots.
     // RM2 BMS voltage data follows unit: 0.01V
     ranger_bms.bms_basic_state.voltage =
         common_sensor.bms_basic_state.voltage * 0.1f;
     ranger_bms.bms_basic_state.battery_soc =
         common_sensor.bms_basic_state.battery_soc;
     ranger_bms.bms_basic_state.battery_soh =
         common_sensor.bms_basic_state.battery_soh;
     ranger_bms.bms_basic_state.temperature =
         common_sensor.bms_basic_state.temperature;
 
     return ranger_bms;
   }
 };
 
 // Note: Ranger Mini V1 uses a modified AgileX V2 protocol
 // Here we provide a work-around fix as no new firmware will be provided from
 // AgileX to properly fix the issue.
 class RangerMiniV1Base : public RangerBase {
  public:
   RangerMiniV1Base() : RangerBase() {};
   ~RangerMiniV1Base() = default;
 
   // robot control
   void SetMotionMode(uint8_t mode) override {
     if (mode == RangerInterface::MotionMode::kPark) {
       return;
     } else if (mode == RangerInterface::MotionMode::kSideSlip) {
       mode = 3;
     }
     AgilexBase::SetMotionMode(mode);
   }
 
   void SetMotionCommand(double linear_vel, double steer_angle,
                         double angular_vel) override {
     auto state = GetRobotState();
     if (state.motion_mode_state.motion_mode ==
         RangerInterface::MotionMode::kSpinning) {
       angular_vel *= 0.254558;
     }
     AgilexBase<ProtocolV2Parser>::SendMotionCommand(
         linear_vel, 0.0, -angular_vel, -steer_angle / 10.0 / 3.14 * 180);
   }
 
   RangerCoreState GetRobotState() override {
     auto state = AgilexBase<ProtocolV2Parser>::GetRobotCoreStateMsgGroup();
 
     RangerCoreState ranger_state;
     ranger_state.time_stamp = state.time_stamp;
     ranger_state.system_state = state.system_state;
 
     ranger_state.light_state = state.light_state;
     ranger_state.rc_state = state.rc_state;
     ranger_state.motion_mode_state = state.motion_mode_state;
 
     if (ranger_state.motion_mode_state.motion_mode ==
         RangerInterface::MotionMode::kSpinning) {
       ranger_state.motion_state.linear_velocity = 0;
       ranger_state.motion_state.angular_velocity =
           -state.motion_state.linear_velocity / 0.254558;
       ranger_state.motion_state.lateral_velocity =
           state.motion_state.lateral_velocity;
       ranger_state.motion_state.steering_angle =
           -state.motion_state.steering_angle * 10 / 180.0 * 3.14;
     } else if (ranger_state.motion_mode_state.motion_mode ==
                RangerInterface::MotionMode::kSideSlip) {
       ranger_state.motion_state.linear_velocity =
           -state.motion_state.linear_velocity;
       state.motion_state.angular_velocity;
       ranger_state.motion_state.lateral_velocity =
           state.motion_state.lateral_velocity;
       ranger_state.motion_state.steering_angle =
           -state.motion_state.steering_angle * 10 / 180.0 * 3.14;
     } else {
       ranger_state.motion_state.linear_velocity =
           state.motion_state.linear_velocity;
       ranger_state.motion_state.angular_velocity =
           -state.motion_state.angular_velocity;
       ranger_state.motion_state.lateral_velocity =
           state.motion_state.lateral_velocity;
       ranger_state.motion_state.steering_angle =
           -state.motion_state.steering_angle * 10 / 180.0 * 3.14;
     }
 
     return ranger_state;
   }
 
   RangerActuatorState GetActuatorState() override {
     auto actuator = AgilexBase<ProtocolV2Parser>::GetActuatorStateMsgGroup();
 
     RangerActuatorState ranger_actuator;
     ranger_actuator.time_stamp = actuator.time_stamp;
 
     ranger_actuator.motor_speeds.speed_1 = actuator.motor_speeds.speed_1;
     ranger_actuator.motor_speeds.speed_2 = actuator.motor_speeds.speed_2;
     ranger_actuator.motor_speeds.speed_3 = actuator.motor_speeds.speed_3;
     ranger_actuator.motor_speeds.speed_4 = actuator.motor_speeds.speed_4;
     ranger_actuator.motor_angles.angle_5 =
         -actuator.motor_angles.angle_5 / 18.0 * M_PI;
     ranger_actuator.motor_angles.angle_6 =
         -actuator.motor_angles.angle_6 / 18.0 * M_PI;
     ranger_actuator.motor_angles.angle_7 =
         -actuator.motor_angles.angle_7 / 18.0 * M_PI;
     ranger_actuator.motor_angles.angle_8 =
         -actuator.motor_angles.angle_8 / 18.0 * M_PI;
 
     for (int i = 0; i < 8; ++i) {
       ranger_actuator.actuator_hs_state[i] = actuator.actuator_hs_state[i];
       ranger_actuator.actuator_ls_state[i] = actuator.actuator_ls_state[i];
     }
     return ranger_actuator;
   }
 };
 }  // namespace westonrobot
 #endif /* RANGER_BASE_HPP */
 