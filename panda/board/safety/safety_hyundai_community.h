#include "safety_hyundai_common.h"

#define    LKAS11         0x340
#define    CLU11          0x4F1
#define    LFAHDA_MFC     0x485
#define    MDPS12         0x251

#define    EMS16          0x260
#define    E_EMS11        0x371
#define    WHL_SPD11      0x386
#define    TCS13          0x394

#define    SCC11          0x420
#define    SCC12          0x421
#define    SCC13          0x50A  
#define    SCC14          0x389  

#define HYUNDAI_COMMUNITY_LIMITS(steer, rate_up, rate_down) { \
  .max_steer = (steer), \
  .max_rate_up = (rate_up), \
  .max_rate_down = (rate_down), \
  .max_rt_delta = 112, \
  .max_rt_interval = 250000, \
  .driver_torque_allowance = 50, \
  .driver_torque_factor = 2, \
  .type = TorqueDriverLimited, \
   /* the EPS faults when the steering angle is above a certain threshold for too long. to prevent this, */ \
   /* we allow setting CF_Lkas_ActToi bit to 0 while maintaining the requested torque value for two consecutive frames */ \
  .min_valid_request_frames = 89, \
  .max_invalid_request_frames = 2, \
  .min_valid_request_rt_interval = 810000,  /* 810ms; a ~10% buffer on cutting every 90 frames */ \
  .has_steer_req_tolerance = true, \
}

const SteeringLimits HYUNDAI_COMMUNITY_STEERING_LIMITS = HYUNDAI_COMMUNITY_LIMITS(384, 3, 7);
const SteeringLimits HYUNDAI_COMMUNITY_STEERING_LIMITS_ALT = HYUNDAI_COMMUNITY_LIMITS(270, 2, 3);

const LongitudinalLimits HYUNDAI_COMMUNITY_LONG_LIMITS = {
  .max_accel = 200,   // 1/100 m/s2
  .min_accel = -350,  // 1/100 m/s2
};

const CanMsg HYUNDAI_COMMUNITY_TX_MSGS[] = {
  {LKAS11,      0, 8}, // LKAS11 Bus 0
  {CLU11,       0, 4}, // CLU11 Bus 0
  {LFAHDA_MFC,  0, 4}, // LFAHDA_MFC Bus 0
  {MDPS12,      2, 8},   // MDPS12, Bus 2
};

const CanMsg HYUNDAI_COMMUNITY_LONG_TX_MSGS[] = {
  {LKAS11,      0, 8}, // LKAS11 Bus 0
  {CLU11,       0, 4}, // CLU11 Bus 0
  {LFAHDA_MFC,  0, 4}, // LFAHDA_MFC Bus 0
  {SCC11,       0, 8}, // SCC11 Bus 0
  {SCC12,       0, 8}, // SCC12 Bus 0
  {SCC13,       0, 8}, // SCC13 Bus 0
  {SCC14,       0, 8}, // SCC14 Bus 0
  {0x4A2,       0, 2}, // FRT_RADAR11 Bus 0
  {0x38D,       0, 8}, // FCA11 Bus 0
  {0x483,       0, 8}, // FCA12 Bus 0
  {0x7D0,       0, 8}, // radar UDS TX addr Bus 0 (for radar disable)
};

const CanMsg HYUNDAI_COMMUNITY_CAMERA_SCC_TX_MSGS[] = {
  {LKAS11,      0, 8}, // LKAS11 Bus 0
  {CLU11,       2, 4}, // CLU11 Bus 2
  {LFAHDA_MFC,  0, 4}, // LFAHDA_MFC Bus 0
};

#define HYUNDAI_COMMUNITY_COMMON_RX_CHECKS(legacy)                                                                                              \
  {.msg = {{EMS16, 0, 8, .check_checksum = true, .max_counter = 3U, .frequency = 100U},                                       \
           {E_EMS11, 0, 8, .frequency = 100U}, { 0 }}},                                                                         \
  {.msg = {{WHL_SPD11, 0, 8, .check_checksum = !(legacy), .max_counter = (legacy) ? 0U : 15U, .frequency = 100U}, { 0 }, { 0 }}}, \
  {.msg = {{TCS13, 0, 8, .check_checksum = !(legacy), .max_counter = (legacy) ? 0U : 7U, .frequency = 100U}, { 0 }, { 0 }}},  \

#define HYUNDAI_COMMUNITY_SCC12_ADDR_CHECK(scc_bus)                                                                                  \
  {.msg = {{SCC12, (scc_bus), 8, .check_checksum = true, .max_counter = 15U, .frequency = 50U}, { 0 }, { 0 }}}, \

RxCheck hyundai_community_rx_checks[] = {
   HYUNDAI_COMMUNITY_COMMON_RX_CHECKS(false)
   HYUNDAI_COMMUNITY_SCC12_ADDR_CHECK(0)
};

RxCheck hyundai_community_cam_scc_rx_checks[] = {
  HYUNDAI_COMMUNITY_COMMON_RX_CHECKS(false)
  HYUNDAI_COMMUNITY_SCC12_ADDR_CHECK(2)
};

RxCheck hyundai_community_long_rx_checks[] = {
  HYUNDAI_COMMUNITY_COMMON_RX_CHECKS(false)
  // Use CLU11 (buttons) to manage controls allowed instead of SCC cruise state
  {.msg = {{CLU11, 0, 4, .check_checksum = false, .max_counter = 15U, .frequency = 50U}, { 0 }, { 0 }}},
};

int main_button =0;

static uint8_t hyundai_community_get_counter(const CANPacket_t *to_push) {
  int addr = GET_ADDR(to_push);

  uint8_t cnt = 0;
  if (addr == EMS16) {
    cnt = (GET_BYTE(to_push, 7) >> 4) & 0x3U;
  } else if (addr == WHL_SPD11) {
    cnt = ((GET_BYTE(to_push, 3) >> 6) << 2) | (GET_BYTE(to_push, 1) >> 6);
  } else if (addr == TCS13) {
    cnt = (GET_BYTE(to_push, 1) >> 5) & 0x7U;
  } else if (addr == SCC12) {
    cnt = GET_BYTE(to_push, 7) & 0xFU;
  } else if (addr == CLU11) {
    cnt = (GET_BYTE(to_push, 3) >> 4) & 0xFU;
  } else {
  }
  return cnt;
}

static uint32_t hyundai_community_get_checksum(const CANPacket_t *to_push) {
  int addr = GET_ADDR(to_push);

  uint8_t chksum = 0;
  if (addr == EMS16) {
    chksum = GET_BYTE(to_push, 7) & 0xFU;
  } else if (addr == WHL_SPD11) {
    chksum = ((GET_BYTE(to_push, 7) >> 6) << 2) | (GET_BYTE(to_push, 5) >> 6);
  } else if (addr == TCS13) {
    chksum = GET_BYTE(to_push, 6) & 0xFU;
  } else if (addr == SCC12) {
    chksum = GET_BYTE(to_push, 7) >> 4;
  } else {
  }
  return chksum;
}

static uint32_t hyundai_community_compute_checksum(const CANPacket_t *to_push) {
  int addr = GET_ADDR(to_push);

  uint8_t chksum = 0;
  if (addr == WHL_SPD11) {
    // count the bits
    for (int i = 0; i < 8; i++) {
      uint8_t b = GET_BYTE(to_push, i);
      for (int j = 0; j < 8; j++) {
        uint8_t bit = 0;
        // exclude checksum and counter
        if (((i != 1) || (j < 6)) && ((i != 3) || (j < 6)) && ((i != 5) || (j < 6)) && ((i != 7) || (j < 6))) {
          bit = (b >> (uint8_t)j) & 1U;
        }
        chksum += bit;
      }
    }
    chksum = (chksum ^ 9U) & 15U;
  } else {
    // sum of nibbles
    for (int i = 0; i < 8; i++) {
      if ((addr == TCS13) && (i == 7)) {
        continue; // exclude
      }
      uint8_t b = GET_BYTE(to_push, i);
      if (((addr == EMS16) && (i == 7)) || ((addr == TCS13) && (i == 6)) || ((addr == SCC12) && (i == 7))) {
        b &= (addr == SCC12) ? 0x0FU : 0xF0U; // remove checksum
      }
      chksum += (b % 16U) + (b / 16U);
    }
    chksum = (16U - (chksum %  16U)) % 16U;
  }

  return chksum;
}


static void generic_community_rx_checks(bool stock_ecu_detected) {
  // exit controls on rising edge of gas press
  if (gas_pressed && !gas_pressed_prev && !(alternative_experience & ALT_EXP_DISABLE_DISENGAGE_ON_GAS)) {
    controls_allowed = false;
  }
  gas_pressed_prev = gas_pressed;

  // exit controls on rising edge of brake press
  if (brake_pressed && (!brake_pressed_prev || vehicle_moving)  && !(alternative_experience & ALT_EXP_DISABLE_DISENGAGE_ON_GAS)) {
    controls_allowed = false;
  }
  brake_pressed_prev = brake_pressed;

  // exit controls on rising edge of regen paddle
  if (regen_braking && (!regen_braking_prev || vehicle_moving)) {
    controls_allowed = false;
  }
  regen_braking_prev = regen_braking;

  // check if stock ECU is on bus broken by car harness
  if ((safety_mode_cnt > RELAY_TRNS_TIMEOUT) && stock_ecu_detected) {
    relay_malfunction_set();
  }
}

static void hyundai_community_rx_hook( const CANPacket_t *to_push) {
  int bus = GET_BUS(to_push);
  int addr = GET_ADDR(to_push);

  if( hyundai_longitudinal || !(alternative_experience & ALT_EXP_DISABLE_DISENGAGE_ON_GAS))
  {
    // SCC12 is on bus 2 for camera-based SCC cars, bus 0 on all others
    if ((addr == SCC12) && (((bus == 0) && !hyundai_camera_scc) || ((bus == 2) && hyundai_camera_scc))) {
      // 2 bits: 13-14
      int cruise_engaged = (GET_BYTES(to_push, 0, 4) >> 13) & 0x3U;
      hyundai_common_cruise_state_check(cruise_engaged);
    }
  }
  else
  {
    if ((addr == SCC11) && (((bus == 0) && !hyundai_camera_scc) || ((bus == 2) && hyundai_camera_scc))) {
      // 0 bits
      int cruise_engaged = GET_BYTES(to_push, 0, 4) & 0x1U; // ACC main_on signal
      if( cruise_engaged )
      {
        if( heartbeat_engaged_mismatches ) {}
        else if( !controls_allowed )
        {
          if( !heartbeat_engaged ) cruise_engaged = 0;
          else if( cruise_engaged_prev )  cruise_engaged_prev = 0;
          else  hyundai_last_button_interaction = 0U;
        } 
      }

      hyundai_common_cruise_state_check(cruise_engaged);
    }

  }

  if (bus == 0) {
    if (addr == MDPS12) {
      int torque_driver_new = (GET_BYTES(to_push, 0, 2) & 0x7ffU) - 1024U;
      // update array of samples
      update_sample(&torque_driver, torque_driver_new);
    }

    // ACC steering wheel buttons
    if (addr == CLU11) {
      int cruise_button = GET_BYTE(to_push, 0) & 0x7U;
      main_button = GET_BIT(to_push, 3U);
      hyundai_common_cruise_buttons_check(cruise_button, main_button);
    }

    // gas press, different for EV, hybrid, and ICE models
    if ((addr == E_EMS11) && hyundai_ev_gas_signal) {
      gas_pressed = (((GET_BYTE(to_push, 4) & 0x7FU) << 1) | GET_BYTE(to_push, 3) >> 7) != 0U;
    } else if ((addr == E_EMS11) && hyundai_hybrid_gas_signal) {
      gas_pressed = GET_BYTE(to_push, 7) != 0U;
    } else if ((addr == EMS16) && !hyundai_ev_gas_signal && !hyundai_hybrid_gas_signal) {
      gas_pressed = (GET_BYTE(to_push, 7) >> 6) != 0U;
    } else {
    }

    // sample wheel speed, averaging opposite corners
    if (addr == WHL_SPD11) {
      uint32_t front_left_speed = GET_BYTES(to_push, 0, 2) & 0x3FFFU;
      uint32_t rear_right_speed = GET_BYTES(to_push, 6, 2) & 0x3FFFU;
      vehicle_moving = (front_left_speed > HYUNDAI_STANDSTILL_THRSLD) || (rear_right_speed > HYUNDAI_STANDSTILL_THRSLD);
    }

    if (addr == TCS13) {
      brake_pressed = ((GET_BYTE(to_push, 5) >> 5U) & 0x3U) == 0x2U;
    }

    bool stock_ecu_detected = (addr == LKAS11);

    // If openpilot is controlling longitudinal we need to ensure the radar is turned off
    // Enforce by checking we don't see SCC12
    if (hyundai_longitudinal && (addr == SCC12)) {
      stock_ecu_detected = true;
    }
    generic_community_rx_checks(stock_ecu_detected);
  }
}

static bool hyundai_community_tx_hook( const CANPacket_t *to_send) {
  bool tx = true;
  int addr = GET_ADDR(to_send);

  // FCA11: Block any potential actuation
  if (addr == 0x38D) {
    int CR_VSM_DecCmd = GET_BYTE(to_send, 1);
    int FCA_CmdAct = GET_BIT(to_send, 20U);
    int CF_VSM_DecCmdAct = GET_BIT(to_send, 31U);

    if ((CR_VSM_DecCmd != 0) || (FCA_CmdAct != 0) || (CF_VSM_DecCmdAct != 0)) {
      tx = false;
    }
  }

  // ACCEL: safety check
  if (addr == SCC12) {
    int desired_accel_raw = (((GET_BYTE(to_send, 4) & 0x7U) << 8) | GET_BYTE(to_send, 3)) - 1023U;
    int desired_accel_val = ((GET_BYTE(to_send, 5) << 3) | (GET_BYTE(to_send, 4) >> 5)) - 1023U;

    int aeb_decel_cmd = GET_BYTE(to_send, 2);
    int aeb_req = GET_BIT(to_send, 54U);

    bool violation = false;

    violation |= longitudinal_accel_checks(desired_accel_raw, HYUNDAI_COMMUNITY_LONG_LIMITS);
    violation |= longitudinal_accel_checks(desired_accel_val, HYUNDAI_COMMUNITY_LONG_LIMITS);
    violation |= (aeb_decel_cmd != 0);
    violation |= (aeb_req != 0);

    if (violation) {
      tx = false;
    }
  }

  // LKA STEER: safety check
  if (addr == LKAS11) {
    int desired_torque = ((GET_BYTES(to_send, 0, 4) >> 16) & 0x7ffU) - 1024U;
    bool steer_req = GET_BIT(to_send, 27U) != 0U;

    const SteeringLimits limits = hyundai_alt_limits ? HYUNDAI_COMMUNITY_STEERING_LIMITS_ALT : HYUNDAI_COMMUNITY_STEERING_LIMITS;
    if (steer_torque_cmd_checks(desired_torque, steer_req, limits)) {
      tx = false;
    }
  }

  // UDS: Only tester present ("\x02\x3E\x80\x00\x00\x00\x00\x00") allowed on diagnostics address
  if (addr == 0x7D0) {
    if ((GET_BYTES(to_send, 0, 4) != 0x00803E02U) || (GET_BYTES(to_send, 4, 4) != 0x0U)) {
      tx = false;
    }
  }

  // BUTTONS: used for resume spamming and cruise cancellation
  if ((addr == CLU11) && !hyundai_longitudinal) {
    int button = GET_BYTE(to_send, 0) & 0x7U;

    bool allowed_resume = (button == 1) && controls_allowed; // acc
    bool allowed_set = (button == 2) && controls_allowed;    // dec
    bool allowed_cancel = (button == 4) && cruise_engaged_prev;
    if (!(allowed_resume || allowed_set ||  allowed_cancel)) {
      tx = false;
    }
  }
  
  return tx;
}

static int hyundai_community_fwd_hook(int bus_num, int addr) {

  int bus_fwd = -1;
  
  // forward cam to ccan and viceversa, except lkas cmd
  if (bus_num == 0) {
    //if( addr != MDPS12 ) {
        bus_fwd = 2;
   // }
  }
  if ((bus_num == 2) && (addr != LKAS11) && (addr != LFAHDA_MFC)) {
    bus_fwd = 0;
  }

  return bus_fwd;
}

static safety_config hyundai_community_init(uint16_t param) {
  hyundai_common_init(param);


  if (hyundai_camera_scc) {
    hyundai_longitudinal = false;
  }

  safety_config ret;
  if (hyundai_longitudinal) {
    ret = BUILD_SAFETY_CFG(hyundai_community_long_rx_checks, HYUNDAI_COMMUNITY_LONG_TX_MSGS);
  } else if (hyundai_camera_scc) {
    ret = BUILD_SAFETY_CFG(hyundai_community_cam_scc_rx_checks, HYUNDAI_COMMUNITY_CAMERA_SCC_TX_MSGS);
  } else {
    ret = BUILD_SAFETY_CFG(hyundai_community_rx_checks, HYUNDAI_COMMUNITY_TX_MSGS);
  }
  return ret;
}


const safety_hooks hyundai_community_hooks = {
  .init = hyundai_community_init,
  .rx = hyundai_community_rx_hook,
  .tx = hyundai_community_tx_hook,
  .fwd = hyundai_community_fwd_hook,
  .get_counter = hyundai_community_get_counter,
  .get_checksum = hyundai_community_get_checksum,
  .compute_checksum = hyundai_community_compute_checksum,
};
