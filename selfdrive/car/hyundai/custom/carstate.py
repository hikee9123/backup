import copy

from cereal import car
from cereal import messaging
from panda import ALTERNATIVE_EXPERIENCE
from openpilot.common.params import Params
from openpilot.common.conversions import Conversions as CV
from openpilot.selfdrive.car.hyundai.values import CAR, Buttons

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

    self.cruise_buttons_old = 0
    self.control_mode = 0
    self.clu_Vanz = 0
    self.is_highway = False

    # cruise_speed_button
    self.old_acc_active = 0
    self.prev_acc_active = 0
    self.cruise_set_speed_kph = 0
    self.cruise_buttons_time = 0
    self.VSetDis = 0
    self.prev_cruise_btn = 0
    self.lead_distance = 0

    self.gapSet = 0

    self.cars = []
    self.get_type_of_car( CP )


  def get_type_of_car( self, CP ):
    cars = []
    for _, member in CAR.__members__.items():
      cars.append(member.value)
    #cars.sort()
    self.cars = cars


  def get_can_parser( self, messages, CP ):
    messages += [
      ("TPMS11", 5),
    ]


  @staticmethod
  def get_cam_can_parser( messages, CP ):
    messages += [
      ("LFAHDA_MFC", 20),          
    ]

  def cruise_control_mode( self ):
    cruise_buttons = self.CS.prev_cruise_buttons
    if cruise_buttons == self.cruise_buttons_old:
       return
      
    self.cruise_buttons_old = cruise_buttons
    if cruise_buttons == (Buttons.RES_ACCEL): 
      self.control_mode += 1
    elif cruise_buttons == (Buttons.SET_DECEL):
      self.control_mode -= 1

    if self.control_mode < 0:
      self.control_mode = 0
    elif self.control_mode > 5:
      self.control_mode = 0

  def cruise_speed_button( self ):
    if self.prev_acc_active != self.acc_active:
      self.old_acc_active = self.prev_acc_active
      self.prev_acc_active = self.acc_active
      self.cruise_set_speed_kph = self.VSetDis

    set_speed_kph = self.cruise_set_speed_kph
    if not self.acc_active:
      return self.cruise_set_speed_kph

    cruise_buttons = self.CS.prev_cruise_buttons   #cruise_buttons[-1]
    if cruise_buttons in (Buttons.RES_ACCEL, Buttons.SET_DECEL):
      self.cruise_buttons_time += 1
    else:
      self.cruise_buttons_time = 0

    # long press should set scc speed with cluster scc number
    if self.cruise_buttons_time >= 55:
      self.cruise_set_speed_kph = self.VSetDis
      return self.cruise_set_speed_kph


    if self.prev_cruise_btn == cruise_buttons:
      return self.cruise_set_speed_kph

    self.prev_cruise_btn = cruise_buttons

    if cruise_buttons == (Buttons.RES_ACCEL): 
      set_speed_kph = self.VSetDis + 1
    elif cruise_buttons == (Buttons.SET_DECEL):
      if self.CS.out.gasPressed or not self.old_acc_active:
        set_speed_kph = self.clu_Vanz
      else:
        set_speed_kph = self.VSetDis - 1

    if set_speed_kph < 30:
      set_speed_kph = 30

    self.cruise_set_speed_kph = set_speed_kph
    return  set_speed_kph
  
  def set_cruise_speed( self, set_speed ):
    self.cruise_set_speed_kph = set_speed  

  def get_tpms(self, ret, unit, fl, fr, rl, rr):
    factor = 0.72519 if unit == 1 else 0.1 if unit == 2 else 1 # 0:psi, 1:kpa, 2:bar
    ret.unit = unit
    ret.fl = fl * factor
    ret.fr = fr * factor
    ret.rl = rl * factor
    ret.rr = rr * factor


  def update(self, ret, CS,  cp, cp_cruise, cp_cam ):
    mainMode_ACC = cp_cruise.vl["SCC11"]["MainMode_ACC"] == 1

    if not mainMode_ACC:
      self.cruise_control_mode()

    # save the entire LFAHDA_MFC
    self.lfahda = copy.copy(cp_cam.vl["LFAHDA_MFC"])
    self.mdps12 = copy.copy(cp.vl["MDPS12"])
    if not self.CP.openpilotLongitudinalControl:
      self.acc_active = (cp_cruise.vl["SCC12"]['ACCMode'] != 0)
      ret.cruiseState.speed = self.cruise_speed_button() * CV.KPH_TO_MS

    ret.engineRpm = cp.vl["E_EMS11"]["N"] # opkr
    ret.brakeLightsDEPRECATED = bool( cp.vl["TCS13"]['BrakeLight'] )

    self.brakePos = cp.vl["E_EMS11"]["Brake_Pedal_Pos"] 
    self.is_highway = self.lfahda["HDA_Icon_State"] != 0.
    self.lead_distance = cp.vl["SCC11"]["ACC_ObjDist"]
    self.gapSet = cp.vl["SCC11"]['TauGapSet']
    self.VSetDis = cp_cruise.vl["SCC11"]["VSetDis"]   # kph   크루즈 설정 속도.    
    self.clu_Vanz = cp.vl["CLU11"]["CF_Clu_Vanz"]     # kph  현재 차량의 속도.
    
    if CS.customCS.control_mode == 4:
      ret.cruiseState.available = False
      ret.cruiseState.enabled = False
    elif not self.CP.openpilotLongitudinalControl:
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



    if self.frame % 20 == 0:
      dat = messaging.new_message('carStateCustom')
      carStatus = dat.carStateCustom
      self.get_tpms( carStatus.tpms,
        cp.vl["TPMS11"]["UNIT"],
        cp.vl["TPMS11"]["PRESSURE_FL"],
        cp.vl["TPMS11"]["PRESSURE_FR"],
        cp.vl["TPMS11"]["PRESSURE_RL"],
        cp.vl["TPMS11"]["PRESSURE_RR"],
      )

      carStatus.breakPos = self.brakePos
      carStatus.supportedCars = self.cars
      carStatus.electGearStep = cp.vl["ELECT_GEAR"]["Elect_Gear_Step"] # opkr

    
      global trace1
      carStatus.alertTextMsg1 = str(trace1.global_alertTextMsg1)
      carStatus.alertTextMsg2 = str(trace1.global_alertTextMsg2)
      carStatus.alertTextMsg3 = str(trace1.global_alertTextMsg3)
      self.pm.send('carStateCustom', dat )



      lkas11 = CS.lkas11

      trace1.printf1( 'MD={}'.format( self.control_mode ) )

      log1 = 'MDPS={:.0f}/{:.0f}'.format( self.mdps12["CF_Mdps_ToiActive"], self.mdps12["CF_Mdps_ToiUnavail"] )
      trace1.printf2( 'break={:.0f} HI={:.0f} HU={:.0f} {}'.format( self.brakePos, self.lfahda["HDA_Icon_State"], self.lfahda["HDA_USM"], log1  ) )
      trace1.printf3( 'LM={:.0f}/{:.0f}/{:.0f} LS={:.0f}/{:.0f} LA={:.0f}/{:.0f} '.format( 
        lkas11["CF_Lkas_LdwsActivemode"],lkas11["CF_Lkas_LdwsOpt_USM"],lkas11["CF_Lkas_FcwOpt_USM"], 
        lkas11["CF_Lkas_LdwsSysState"], lkas11["CF_Lkas_SysWarning"], 
        lkas11["CF_Lkas_ActToi"], lkas11["CF_Lkas_ToiFlt"] ) )

