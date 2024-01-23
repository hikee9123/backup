from cereal import car

from openpilot.selfdrive.car.hyundai.values import HyundaiFlags, Buttons
from openpilot.selfdrive.car.hyundai.hyundaican import create_lkas11
from openpilot.selfdrive.car.hyundai.custom.hyundaican import hyundai_lkas11, create_clu11, create_hda_mfc, create_mdps12
from openpilot.selfdrive.car.hyundai.custom.navicontrol  import NaviControl



class CarControllerCustom:
  def __init__(self, CP):
    self.CP = CP
    self.NC = NaviControl( CP)

    self.resume_cnt = 0
    self.hyundai_lkass = 1


  def custom_lkas11(self, packer, frame, car_fingerprint, apply_steer, steer_req,
                    torque_fault, CS, sys_warning, sys_state, CC,
                    hud_control,
                    left_lane_depart, right_lane_depart):
    
    left_lane = hud_control.leftLaneVisible, 
    right_lane = hud_control.rightLaneVisible

    control_mode = CS.customCS.control_mode

    if control_mode == 4:
      can_lkas = hyundai_lkas11(packer, frame, car_fingerprint, apply_steer, steer_req,
                                      torque_fault, CS.lkas11, sys_warning, sys_state, CC.enabled,
                                      left_lane, right_lane,
                                      left_lane_depart, right_lane_depart)
    else:
      can_lkas = create_lkas11(packer, frame, car_fingerprint, apply_steer, steer_req,
                                      torque_fault, CS.lkas11, sys_warning, sys_state, CC.enabled,
                                      left_lane, right_lane,
                                      left_lane_depart, right_lane_depart)
    return can_lkas
  

  def custom_sends(self, can_sends, packer, frame, CS,  CC ):
    if CS.customCS.control_mode == 4:
      return

    # 50 Hz
    if frame % 2 == 0:   # send mdps12 to LKAS to prevent LKAS error
       can_sends.append( create_mdps12( packer, int(frame/2), CS.customCS.mdps12 ) )

    # 20 Hz LFA MFA message
    if frame % 5 == 0 and self.CP.flags & HyundaiFlags.SEND_LFA.value:
       can_sends.append( create_hda_mfc( packer, CS, CC ) )
      #can_sends.append(hyundaican.create_lfahda_mfc(self.packer, CC.enabled))


  def create_button_messages(self, CC: car.CarControl, CS: car.CarState, frame: int):
    can_sends = []    
    if CS.customCS.control_mode == 4:
      return  can_sends

    if CC.cruiseControl.cancel:
      can_sends.append(create_clu11(self.packer, frame, CS.clu11, Buttons.CANCEL, self.CP.carFingerprint))
    elif CS.customCS.acc_active:
      #custom 
      btn_signal = self.NC.update( CC, CS, frame )
      if btn_signal != None:
        can_sends.extend( [create_clu11(CC.packer, self.resume_cnt, CS.clu11, btn_signal, self.CP.carFingerprint)] * 2 )
        self.resume_cnt += 1
      else:
        self.resume_cnt = 0


    return can_sends