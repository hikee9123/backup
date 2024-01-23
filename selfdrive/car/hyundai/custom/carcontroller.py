from cereal import car

from openpilot.selfdrive.car.hyundai.values import HyundaiFlags, Buttons
from openpilot.selfdrive.car.hyundai import hyundaicanfd, hyundaican
from openpilot.selfdrive.car.hyundai.custom.hyundaican import create_clu11, create_lkas11
from openpilot.selfdrive.car.hyundai.custom.navicontrol  import NaviControl



class CarControllerCustom:
  def __init__(self, CP):
    self.CP = CP
    self.NC = NaviControl( CP)

    self.resume_cnt = 0
    self.hyundai_lkass = 1


  def make_lkas11(self, packer, frame, car_fingerprint, apply_steer, steer_req,
                    torque_fault, CS, sys_warning, sys_state, CC,
                    hud_control,
                    left_lane_depart, right_lane_depart):
    
    left_lane = hud_control.leftLaneVisible, 
    right_lane = hud_control.rightLaneVisible

    control_mode = CS.customCS.control_mode

    if control_mode == 4:
      can_lkas = create_lkas11(packer, frame, car_fingerprint, apply_steer, steer_req,
                                      torque_fault, CS.lkas11, sys_warning, sys_state, CC.enabled,
                                      left_lane, right_lane,
                                      left_lane_depart, right_lane_depart)
    else:
      can_lkas = hyundaican.create_lkas11(packer, frame, car_fingerprint, apply_steer, steer_req,
                                      torque_fault, CS.lkas11, sys_warning, sys_state, CC.enabled,
                                      left_lane, right_lane,
                                      left_lane_depart, right_lane_depart)
    return can_lkas
  
  

  def create_button_messages(self, CC: car.CarControl, CS: car.CarState, use_clu11: bool):
    can_sends = []
    if CC.cruiseControl.cancel:
      can_sends.append(create_clu11(self.packer, self.frame, CS.clu11, Buttons.CANCEL, self.CP.carFingerprint))
    elif CS.customCS.acc_active:
      #custom 
      btn_signal = self.NC.update( CC, CS, CC.frame )
      if btn_signal != None:
        can_sends.extend( [create_clu11(CC.packer, self.resume_cnt, CS.clu11, btn_signal, self.CP.carFingerprint)] * 2 )
        self.resume_cnt += 1
      else:
        self.resume_cnt = 0


    return can_sends