from cereal import car
#from openpilot.selfdrive.car.hyundai import hyundaican
from openpilot.selfdrive.car.hyundai.custom import hyundaican
from openpilot.selfdrive.car.hyundai.custom.navicontrol  import NaviControl



class CarControllerCustom:
  def __init__(self, CP):
    self.CP = CP
    self.NC = NaviControl( CP)

    self.resume_cnt = 0


  def create_button_messages( self, can_sends, CC, CS ):
    btn_signal = self.NC.update( CC, CS, CC.frame )
    if btn_signal != None:
      can_sends.extend( [hyundaican.create_clu11(CC.packer, self.resume_cnt, CS.clu11, btn_signal, self.CP.carFingerprint)] * 2 )
      self.resume_cnt += 1
    else:
      self.resume_cnt = 0
    
    return can_sends
