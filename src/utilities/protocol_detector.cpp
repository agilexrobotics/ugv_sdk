/*
 * protocol_detector.cpp
 *
 * Created on: Jul 15, 2021 14:05
 * Description:
 *
 * Copyright (c) 2021 Weston Robot Pte. Ltd.
 */

#include "ugv_sdk/utilities/protocol_detector.hpp"
#include "ugv_sdk/utilities/stopwatch.hpp"

namespace westonrobot {
void ProtocolDectctor::Connect(std::string can_name) {
  can_ = std::make_shared<AsyncCAN>(can_name);
  can_->SetReceiveCallback(
      std::bind(&ProtocolDectctor::ParseCANFrame, this, std::placeholders::_1));
  can_->StartListening();
}
void ProtocolDectctor::Connect(std::string uart_name, uint32_t baudrate) {}

ProtocolVersion ProtocolDectctor::DetectProtocolVersion(uint32_t timeout_sec) {
  msg_v1_detected_ = false;
  msg_v2_detected_ = false;

  StopWatch sw;
  Timer timer;
  while (sw.stoc() < timeout_sec) {
    timer.reset();
    if (msg_v1_detected_ || msg_v2_detected_) break;
    timer.sleep_until_ms(50);
  }

  // make sure only one version is detected
  if (msg_v1_detected_ && msg_v2_detected_) return ProtocolVersion::UNKONWN;

  if (msg_v1_detected_)
    return ProtocolVersion::AGX_V1;
  else if (msg_v2_detected_)
    return ProtocolVersion::AGX_V2;

  return ProtocolVersion::UNKONWN;
};

void ProtocolDectctor::ParseCANFrame(can_frame *rx_frame) {
  // state feedback frame with id 0x151 is unique to V1 protocol
  if (rx_frame->can_id == 0x151) {
    msg_v1_detected_ = true;
  }
  // rc state frame with id 0x241 is unique to V2 protocol
  else if (rx_frame->can_id == 0x241) {
    msg_v2_detected_ = true;
  }
}
}  // namespace westonrobot