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
  lfahda = CS.customCS.lfahda
  values = {s: lfahda[s] for s in [
    "HDA_USM",
    "HDA_Active",
    "HDA_Icon_State",
    "HDA_LdwSysState",
    "HDA_Icon_Wheel",
    "HDA_VSetReq",
  ]}  
  enabled = CC.enabled

  ldwSysState = 0
  if CC.hudControl.leftLaneVisible:
     ldwSysState += 1
  if CC.hudControl.rightLaneVisible:
     ldwSysState += 2

  values["HDA_LdwSysState"] = ldwSysState
  values["HDA_Icon_Wheel"] = 1 if enabled else 0
  return packer.make_can_msg("LFAHDA_MFC", 0, values)

# 2 Hz
def create_avm( packer, CS, CC ):
  values = CS.customCS.avm
  trace1.printf1( 'bt.{:.0f}/{:.0f}/{:.0f} rdy.{:.0f} op.{:.0f}/{:.0f}/{:.0f}/{:.0f} c.{:.0f}/{:.0f}/{:.0f}'.format( values["AVM_ParkingAssist_BtnSts"], values["AVM_FrontBtn_Type"], values["AVM_ParkingAssist_Step"], 
                                                                                                                     values["AVM_Ready"],
                                                                                                                     values["AVM_Option"],values["AVM_HU_FrontViewPointOpt"],values["AVM_HU_RearView_Option"],values["AVM_HU_FrontView_Option"], 
                                                                                                                     values["AVM_View"], values["AVM_Display_Message"], values["AVM_Popup_Msg"]  ) )


  leftBlinker = CS.out.leftBlinker 
  rightBlinker = CS.out.rightBlinker
  clu_Vanz = CS.customCS.clu_Vanz  #kph  현재 차량의 속도.



  if leftBlinker:
      values["AVM_View"] = 5       #1:off 2:rear 3:front  5:left  7:right  9:front   23:rear
      values["AVM_Display_Message"] = 61   # 1:off 61:disp
      values["AVM_Popup_Msg"] = 1   #0:non 1:disp
      values["AVM_HU_FrontViewPointOpt"] = 3

  """
  gearShifter = CS.out.gearShifter
  if gearShifter == GearShifter.drive:
    if clu_Vanz > 30:
      pass
    else:
      values["AVM_View"] = 3       #1:off 2:rear 3:front
      values["AVM_Display_Message"] = 61   # 1:off 61:disp
      values["AVM_Popup_Msg"] = 1   #0:non 1:disp
  elif gearShifter == GearShifter.reverse:
      values["AVM_View"] = 2       #1:off 2:rear 3:front
      values["AVM_Display_Message"] = 61   # 1:off 61:disp
      values["AVM_Popup_Msg"] = 1   #0:non 1:disp

  
  values["AVM_ParkingAssist_BtnSts"] =  # 7
  values["AVM_Ready"] =       #2 
  values["AVM_ParkingAssist_Step"] = #15   
  values["AVM_FrontBtn_Type"] =     #15
  values["AVM_Option"] =            # 3
  values["AVM_HU_FrontViewPointOpt"] =  #3  1(front), 2(rear), 3(off)
  values["AVM_HU_RearView_Option"] =    #1
  values["AVM_HU_FrontView_Option"] =   #1
  values["AVM_Version"] =    {:.0f}               #260

  1 rear view
    - AVM_HU_FrontViewPointOpt 2(stop) , 1: 주행중(53)
    - AVM_View  23
    - AVM_Display_Message 60
    - AVM_Popup_Msg 1

  2 stop
     (rear view)  
    - 7/15/15  rdy.2  op.3/2/1/1  c.23/60/1
     (front view)  
    - 7/15/15  rdy.2  op.3/3/1/1  c.9/61/1
     (front araund view)  
    - 7/15/15  rdy.2  op.3/3/1/1  c.3/61/1
     (left araund view)
    - 7/15/15  rdy.2  op.3/3/1/1  c.5/61/1  
     (right araund view)
    - 7/15/15  rdy.2  op.3/3/1/1  c.7/61/1  
     (no view)
    - 7/15/15  rdy.2  op.3/3/1/1  c.1/1/0  

    
  3 non
    
  """  
  trace1.printf3( 'bt.{:.0f}/{:.0f}/{:.0f} rdy.{:.0f} op.{:.0f}/{:.0f}/{:.0f}/{:.0f} c.{:.0f}/{:.0f}/{:.0f}'.format( values["AVM_ParkingAssist_BtnSts"], values["AVM_FrontBtn_Type"], values["AVM_ParkingAssist_Step"], 
                                                                                                                     values["AVM_Ready"],
                                                                                                                     values["AVM_Option"],values["AVM_HU_FrontViewPointOpt"],values["AVM_HU_RearView_Option"],values["AVM_HU_FrontView_Option"], 
                                                                                                                     values["AVM_View"], values["AVM_Display_Message"], values["AVM_Popup_Msg"]  ) )
  # 0.2,128
  return packer.make_can_msg("AVM_HU_PE_00", 0, values)  
