import copy

from cereal import car
from cereal import messaging
from panda import ALTERNATIVE_EXPERIENCE
from openpilot.common.params import Params
from openpilot.common.conversions import Conversions as CV
from openpilot.selfdrive.car.hyundai.values import CAMERA_SCC_CAR

import openpilot.selfdrive.custom.loger as  trace1


class CarStateCustom():
  def __init__(self, CP, CS):
    self.CS = CS
    self.CP = CP
    self.params = Params()
    self.oldCruiseStateEnabled = False
    self.pm = messaging.PubMaster(['carStateCustom'])
    self.frame = 0
    self.acc_active = 0
    self.cruise_set_mode = 0

    self.is_highway = False
     

  def get_can_parser( self, messages, CP ):
    messages += [
      ("TPMS11", 5),   
    ]


  @staticmethod
  def get_cam_can_parser( CP):
    messages = [
      ("LFAHDA_MFC", 20),          
    ]
    return messages


  def get_tpms(self, ret, unit, fl, fr, rl, rr):
    factor = 0.72519 if unit == 1 else 0.1 if unit == 2 else 1 # 0:psi, 1:kpa, 2:bar
    ret.unit = unit
    ret.fl = fl * factor
    ret.fr = fr * factor
    ret.rl = rl * factor
    ret.rr = rr * factor


  def update(self, ret, CS,  cp, cp_cruise ):
    self.clu_Vanz = cp.vl["CLU11"]["CF_Clu_Vanz"]  #kph  현재 차량의 속도.
    # save the entire LFAHDA_MFC
    # self.lfahda = copy.copy(cp_cruise.vl["LFAHDA_MFC"])
    if not self.CP.openpilotLongitudinalControl and self.CP.carFingerprint in CAMERA_SCC_CAR:
      self.acc_active = (cp_cruise.vl["SCC12"]['ACCMode'] != 0)

    ret.engineRpm = cp.vl["E_EMS11"]["N"] # opkr
    #self.is_highway = False # (cp_cruise.vl["LFAHDA_MFC"]["HDA_Icon_State"] != 0)     
    #self.is_highway = self.lfahda["HDA_Icon_State"] != 0.

    if not self.CP.openpilotLongitudinalControl:
      if not (CS.CP.alternativeExperience & ALTERNATIVE_EXPERIENCE.DISABLE_DISENGAGE_ON_GAS):
        pass
      elif ret.doorOpen:
        self.oldCruiseStateEnabled = False
      elif ret.seatbeltUnlatched:
        self.oldCruiseStateEnabled = False
      elif ret.gearShifter != car.CarState.GearShifter.drive:
        self.oldCruiseStateEnabled = False
      elif not ret.cruiseState.available:
        self.oldCruiseStateEnabled = True
      elif self.oldCruiseStateEnabled:
        ret.cruiseState.enabled = True

    self.frame += 1
       

    if self.frame % 10 == 0:
      dat = messaging.new_message('carStateCustom')
      carStatus = dat.carStateCustom
      self.get_tpms( carStatus.tpms,
        cp.vl["TPMS11"]["UNIT"],
        cp.vl["TPMS11"]["PRESSURE_FL"],
        cp.vl["TPMS11"]["PRESSURE_FR"],
        cp.vl["TPMS11"]["PRESSURE_RL"],
        cp.vl["TPMS11"]["PRESSURE_RR"],
      )

      carStatus.electGearStep = cp.vl["ELECT_GEAR"]["Elect_Gear_Step"] # opkr
      global trace1
      carStatus.alertTextMsg1 = str(trace1.global_alertTextMsg1)
      carStatus.alertTextMsg2 = str(trace1.global_alertTextMsg2)
      carStatus.alertTextMsg3 = str(trace1.global_alertTextMsg3)       
      self.pm.send('carStateCustom', dat )          