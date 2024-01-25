from cereal import car

from openpilot.selfdrive.car.hyundai.values import HyundaiFlags, Buttons, CAR
from openpilot.selfdrive.car.hyundai.hyundaican import create_lkas11
from openpilot.selfdrive.car.hyundai.custom.hyundaican import hyundai_lkas11, create_clu11, create_hda_mfc, create_mdps12
from openpilot.selfdrive.car.hyundai.custom.navicontrol  import NaviControl

VisualAlert = car.CarControl.HUDControl.VisualAlert

class CarControllerCustom:
  def __init__(self, CP):
    self.CP = CP
    self.car_fingerprint = CP.carFingerprint    
    self.NC = NaviControl( CP)
    self.resume_cnt = 0



  def process_hud_alert(self, enabled, hud_control):
    sys_warning = (hud_control.visualAlert in (VisualAlert.steerRequired, VisualAlert.ldw))

    # initialize to no line visible
    # TODO: this is not accurate for all cars
    sys_state = 1
    if hud_control.leftLaneVisible and hud_control.rightLaneVisible or sys_warning:  # HUD alert only display when LKAS status is active
      sys_state = 3 if enabled or sys_warning else 4
    elif hud_control.leftLaneVisible:
      sys_state = 5
    elif hud_control.rightLaneVisible:
      sys_state = 6

    # initialize to no warnings
    left_lane_warning = 0
    right_lane_warning = 0
    if hud_control.leftLaneDepart:
      left_lane_warning = 1 if self.car_fingerprint in (CAR.GENESIS_G90, CAR.GENESIS_G80) else 2
    if hud_control.rightLaneDepart:
      right_lane_warning = 1 if self.car_fingerprint in (CAR.GENESIS_G90, CAR.GENESIS_G80) else 2

    return sys_warning, sys_state, left_lane_warning, right_lane_warning



  def custom_lkas11(self, can_sends, packer, frame, apply_steer, steer_req, CS, CC ):
    hud_control = CC.hudControl
    left_lane = hud_control.leftLaneVisible, 
    right_lane = hud_control.rightLaneVisible
    control_mode = CS.customCS.control_mode
    enable = CC.enabled

    # Hold torque with induced temporary fault when cutting the actuation bit
    torque_fault = CC.latActive and not steer_req

     # HUD messages
    sys_warning, sys_state, left_lane_depart, right_lane_depart = self.process_hud_alert( enable,  hud_control )

    if control_mode == 4:
      can_sends.append( hyundai_lkas11(packer, frame, self.car_fingerprint, apply_steer, steer_req,
                                      torque_fault, CS.lkas11, sys_warning, sys_state, enable,
                                      left_lane, right_lane,
                                      left_lane_depart, right_lane_depart) )
    else:
      can_sends.append( create_lkas11(packer, frame, self.car_fingerprint, apply_steer, steer_req,
                                      torque_fault, CS.lkas11, sys_warning, sys_state, enable,
                                      left_lane, right_lane,
                                      left_lane_depart, right_lane_depart) )

      # 100 Hz
      #can_sends.append( create_mdps12( packer, frame, CS.customCS.mdps12 ) )  # send mdps12 to LKAS to prevent LKAS error    

      if not self.CP.openpilotLongitudinalControl:
        can_sends.extend( self.create_button_messages( packer, CC, CS, frame ) ) #custom

  




  # 20 Hz LFA MFA message
  def custom_hda_mfc(self, can_sends, packer, CS,  CC ):
    if CS.customCS.control_mode == 4:
      return
    can_sends.append( create_hda_mfc( packer, CS, CC ) )



  def create_button_messages(self, packer, CC: car.CarControl, CS: car.CarState, frame: int):
    can_sends = []    
    if CS.customCS.control_mode == 4:
      return  can_sends

    if CC.cruiseControl.cancel:
      can_sends.append(create_clu11( packer, frame, CS.clu11, Buttons.CANCEL, self.CP.carFingerprint))
    elif CS.customCS.acc_active:
      #custom 
      btn_signal = self.NC.update( CC, CS, frame )
      if btn_signal != None:
        can_sends.extend( [create_clu11( packer, self.resume_cnt, CS.clu11, btn_signal, self.CP.carFingerprint)] * 2 )
        self.resume_cnt += 1
      else:
        self.resume_cnt = 0

    return can_sends
