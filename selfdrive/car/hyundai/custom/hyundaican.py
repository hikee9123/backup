import crcmod

from cereal import car
from openpilot.selfdrive.car.hyundai.values import CAMERA_SCC_CAR

import openpilot.selfdrive.custom.loger as  trace1

GearShifter = car.CarState.GearShifter

def create_clu11(packer, frame, clu11, button, car_fingerprint):
  values = clu11
  frame = (values["CF_Clu_AliveCnt1"] + 1)
  
  values["CF_Clu_CruiseSwState"] = button
  values["CF_Clu_AliveCnt1"] = frame % 0x10
  # send buttons to camera on camera-scc based cars
  bus = 2 if car_fingerprint in CAMERA_SCC_CAR else 0
  return packer.make_can_msg("CLU11", bus, values)



# 20 Hz 
def create_hda_mfc( packer, CS, CC ):
  values = CS.customCS.lfahda
  enabled = CC.enabled

  ldwSysState = 0
  if CC.hudControl.leftLaneVisible:
     ldwSysState += 1
  if CC.hudControl.rightLaneVisible:
     ldwSysState += 2

  # HDA_USM  2 normal   3 이상동작.
  # LFA_Icon_State   0 no_hda  1 white_hda  2 green_hda
  values["HDA_LdwSysState"] = ldwSysState
  values["HDA_Icon_Wheel"] = 1 if enabled else 0
  return packer.make_can_msg("LFAHDA_MFC", 0, values)
