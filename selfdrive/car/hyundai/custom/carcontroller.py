from cereal import car
from openpilot.common.conversions import Conversions as CV
from openpilot.selfdrive.car.hyundai import hyundaican
from openpilot.selfdrive.car.hyundai.values import Buttons
from openpilot.selfdrive.car.hyundai.custom.navicontrol  import NaviControl

import openpilot.selfdrive.custom.loger as  trace1

class CarControllerCustom:
  def __init__(self, CP):
    self.CP = CP
    self.NC = NaviControl( CP)

  def create_button_messages(self, CC, CS, can_sends):
    # pass
    btn_signal = self.NC.update( CC, CS, CC.frame )
    if btn_signal != None:
      can_sends.append(hyundaican.create_clu11( CC.packer, CC.frame, CS.clu11, btn_signal, self.CP.carFingerprint))

    str_log1 = 'bs={}  seq={}'.format(  btn_signal, self.NC.seq_command )
    trace1.printf1( '{}'.format( str_log1 ) )