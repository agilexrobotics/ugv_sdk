/*
 * protocol_detector.hpp
 *
 * Created on: Jul 15, 2021 14:03
 * Description:
 *
 * Copyright (c) 2021 Weston Robot Pte. Ltd.
 */

#ifndef PROTOCOL_DETECTOR_HPP
#define PROTOCOL_DETECTOR_HPP

#include <atomic>

#include "ugv_sdk/details/async_port/async_can.hpp"
#include "ugv_sdk/details/interface/parser_interface.hpp"

namespace westonrobot {
class ProtocolDectctor {
 public:
  ~ProtocolDectctor();

  bool Connect(std::string can_name);

  ProtocolVersion DetectProtocolVersion(uint32_t timeout_sec);

 private:
  std::shared_ptr<AsyncCAN> can_;
  void ParseCANFrame(can_frame *rx_frame);

  std::atomic<bool> msg_v1_detected_;
  std::atomic<bool> msg_v2_detected_;
};
}  // namespace westonrobot

#endif /* PROTOCOL_DETECTOR_HPP */
