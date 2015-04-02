/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * MessageHandlerTest.cpp
 * Tests for the MessageHandler code.
 * Copyright (C) 2015 Simon Newton
 */

#include <gtest/gtest.h>

#include "AppMock.h"
#include "Array.h"
#include "FlagsMock.h"
#include "LoggerMock.h"
#include "Matchers.h"
#include "TransceiverMock.h"
#include "TransportMock.h"
#include "constants.h"
#include "message_handler.h"

using ::testing::Args;
using ::testing::Return;
using ::testing::_;

class MessageHandlerTest : public testing::Test {
 public:
  void TearDown() {
    Transceiver_SetMock(nullptr);
    Flags_SetMock(nullptr);
    Logger_SetMock(nullptr);
    Transport_SetMock(nullptr);
  }
};

TEST_F(MessageHandlerTest, testEcho) {
  MockTransport transport_mock;
  Transport_SetMock(&transport_mock);

  const uint8_t echo_payload[] = {1, 2, 3, 4};

  EXPECT_CALL(transport_mock, Send(ECHO, RC_OK, _, 1))
      .With(Args<2, 3>(PayloadIs(echo_payload, arraysize(echo_payload))))
      .WillOnce(Return(true));

  MessageHandler_Initialize(Transport_Send);

  Message message = { ECHO, arraysize(echo_payload), &echo_payload[0] };
  MessageHandler_HandleMessage(&message);
}

TEST_F(MessageHandlerTest, testDMX) {
  MockTransport transport_mock;
  Transport_SetMock(&transport_mock);
  MockTransceiver transceiver_mock;
  Transceiver_SetMock(&transceiver_mock);

  const uint8_t dmx_data[] = {1, 2, 3, 4};

  testing::InSequence seq;
  EXPECT_CALL(transceiver_mock, QueueDMX(_, _, arraysize(dmx_data)))
      .WillOnce(Return(true));
  EXPECT_CALL(transceiver_mock, QueueDMX(_, _, arraysize(dmx_data)))
      .WillOnce(Return(false));
  EXPECT_CALL(transport_mock, Send(TX_DMX, RC_BUFFER_FULL, NULL, 0))
      .WillOnce(Return(true));

  MessageHandler_Initialize(Transport_Send);

  Message message = { TX_DMX, arraysize(dmx_data), &dmx_data[0] };
  MessageHandler_HandleMessage(&message);
  MessageHandler_HandleMessage(&message);
}

TEST_F(MessageHandlerTest, testLogger) {
  MockLogger logger_mock;
  Logger_SetMock(&logger_mock);

  EXPECT_CALL(logger_mock, SendResponse());

  MessageHandler_Initialize(Transport_Send);

  Message message = { GET_LOG, 0, NULL };
  MessageHandler_HandleMessage(&message);
}

TEST_F(MessageHandlerTest, testFlags) {
  MockFlags flags_mock;
  Flags_SetMock(&flags_mock);

  EXPECT_CALL(flags_mock, SendResponse());

  MessageHandler_Initialize(Transport_Send);

  Message message = { GET_FLAGS, 0, NULL };
  MessageHandler_HandleMessage(&message);
}

TEST_F(MessageHandlerTest, testReset) {
  MockApp app_mock;
  APP_SetMock(&app_mock);
  MockTransport transport_mock;
  Transport_SetMock(&transport_mock);

  EXPECT_CALL(app_mock, Reset());
  EXPECT_CALL(transport_mock, Send(COMMAND_RESET_DEVICE, RC_OK, NULL, 0))
      .WillOnce(Return(true));

  MessageHandler_Initialize(Transport_Send);
  Message message = { COMMAND_RESET_DEVICE, 0, NULL };
  MessageHandler_HandleMessage(&message);
}

TEST_F(MessageHandlerTest, testUnknownMessage) {
  MockTransport transport_mock;
  Transport_SetMock(&transport_mock);

  EXPECT_CALL(transport_mock, Send((Command) 0, RC_UNKNOWN, NULL, 0))
      .WillOnce(Return(true));

  MessageHandler_Initialize(Transport_Send);

  Message message = { (Command) 0, 0, NULL };
  MessageHandler_HandleMessage(&message);
}

TEST_F(MessageHandlerTest, transceiverDMXEvent) {
  MockTransport transport_mock;
  Transport_SetMock(&transport_mock);

  const uint8_t frame_reply1[] = {0};
  const uint8_t frame_reply2[] = {1};

  EXPECT_CALL(transport_mock, Send(TX_DMX, RC_OK, _, _))
      .With(Args<2, 3>(PayloadIs(
              reinterpret_cast<const uint8_t*>(&frame_reply1),
              arraysize(frame_reply1))))
      .WillOnce(Return(true));
  EXPECT_CALL(transport_mock, Send(TX_DMX, RC_TX_ERROR, _, _))
      .With(Args<2, 3>(PayloadIs(
          reinterpret_cast<const uint8_t*>(&frame_reply2),
          arraysize(frame_reply2))))
      .WillOnce(Return(true));

  MessageHandler_Initialize(Transport_Send);
  MessageHandler_TransceiverEvent(0, T_OP_TRANSCEIVER_NO_RESPONSE,
                                  T_RC_COMPLETED_OK, NULL, 0);
  MessageHandler_TransceiverEvent(1, T_OP_TRANSCEIVER_NO_RESPONSE,
                                  T_RC_TX_ERROR, NULL, 0);
}

TEST_F(MessageHandlerTest, transceiverRDMEvent) {
  MockTransport transport_mock;
  Transport_SetMock(&transport_mock);

  // Any data, doesn't have to be valid RDM
  const uint8_t rdm_reply[] = {1, 2, 3, 4, 5};

  const uint8_t frame_reply1[] = {0, 1, 2, 3, 4, 5};
  const uint8_t frame_reply2[] = {1};
  const uint8_t frame_reply3[] = {2};

  EXPECT_CALL(transport_mock, Send(COMMAND_RDM_REQUEST, RC_OK, _, _))
      .With(Args<2, 3>(PayloadIs(
              reinterpret_cast<const uint8_t*>(&frame_reply1),
              arraysize(frame_reply1))))
      .WillOnce(Return(true));
  EXPECT_CALL(transport_mock, Send(COMMAND_RDM_REQUEST, RC_TX_ERROR, _, _))
      .With(Args<2, 3>(PayloadIs(
          reinterpret_cast<const uint8_t*>(&frame_reply2),
          arraysize(frame_reply2))))
      .WillOnce(Return(true));
  EXPECT_CALL(transport_mock, Send(COMMAND_RDM_REQUEST, RC_RX_TIMEOUT, _, _))
      .With(Args<2, 3>(PayloadIs(
          reinterpret_cast<const uint8_t*>(&frame_reply3),
          arraysize(frame_reply3))))
      .WillOnce(Return(true));

  MessageHandler_Initialize(Transport_Send);
  MessageHandler_TransceiverEvent(0, T_OP_RDM_WITH_RESPONSE, T_RC_COMPLETED_OK,
                                  static_cast<const uint8_t*>(rdm_reply),
                                  arraysize(rdm_reply));
  MessageHandler_TransceiverEvent(1, T_OP_RDM_WITH_RESPONSE, T_RC_TX_ERROR,
                                  NULL, 0);
  MessageHandler_TransceiverEvent(2, T_OP_RDM_WITH_RESPONSE, T_RC_RX_TIMEOUT,
                                  NULL, 0);
}