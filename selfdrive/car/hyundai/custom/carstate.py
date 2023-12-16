import copy

from cereal import car
from cereal import messaging
from panda import ALTERNATIVE_EXPERIENCE
from openpilot.common.params import Params
from openpilot.common.conversions import Conversions as CV
from openpilot.selfdrive.car.hyundai.values import CAMERA_SCC_CAR


class CarStateCustom():
  def __init__(self, CP, CS):
    self.CS = CS
    self.CP = CP
    self.params = Params()
    self.oldCruiseStateEnabled = False
    self.pm = messaging.PubMaster(['carStateCustom'])
    self.msg = messaging.new_message('carStateCustom')
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
    carStatus = self.msg.carStateCustom
    self.get_tpms( carStatus.tpms,
      cp.vl["TPMS11"]["UNIT"],
      cp.vl["TPMS11"]["PRESSURE_FL"],
      cp.vl["TPMS11"]["PRESSURE_FR"],
      cp.vl["TPMS11"]["PRESSURE_RL"],
      cp.vl["TPMS11"]["PRESSURE_RR"],
    )


    if self.frame % 100 == 0:
      self.pm.send('carStateCustom', self.msg )   


    # save the entire LFAHDA_MFC
    self.lfahda = copy.copy(cp_cruise.vl["LFAHDA_MFC"])
    if not self.CP.openpilotLongitudinalControl and self.CP.carFingerprint in CAMERA_SCC_CAR:
      self.acc_active = (cp_cruise.vl["SCC12"]['ACCMode'] != 0)

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
       