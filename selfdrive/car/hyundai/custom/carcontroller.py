from cereal import car
from openpilot.common.conversions import Conversions as CV
from openpilot.selfdrive.car.hyundai import hyundaican
from openpilot.selfdrive.car.hyundai.values import Buttons
from openpilot.selfdrive.car.hyundai.custom.navicontrol  import NaviControl


class CarControllerCustom:
  def __init__(self, CP):
    self.CP = CP
    self.resume_cnt = 0    
    self.NC = NaviControl(self.params, CP)

  def create_button_messages(self, CC, CS, can_sends):
    # pass
    btn_signal = self.NC.update( CC, CS, self.frame )
    if btn_signal != None:
      can_sends.append(hyundaican.create_clu11( CC.packer, CC.resume_cnt, CS.clu11, btn_signal, self.CP.carFingerprint))
      self.resume_cnt += 1
    else:
      self.resume_cnt = 0

