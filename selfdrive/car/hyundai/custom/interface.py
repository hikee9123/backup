from cereal import car
from panda import Panda
from openpilot.common.params import Params
from openpilot.selfdrive.car.hyundai.values import CANFD_CAR, CAMERA_SCC_CAR, LEGACY_SAFETY_MODE_CAR
from openpilot.selfdrive.car import get_safety_config





def get_params( ret, candidate ):
  params = Params()  
  disengage_on_accelerator = params.get_bool("DisengageOnAccelerator")
  

  # *** panda safety config ***
  if candidate in CANFD_CAR:
    pass
  else:
    if candidate in LEGACY_SAFETY_MODE_CAR:
      # these cars require a special panda safety mode due to missing counters and checksums in the messages
      ret.safetyConfigs = [get_safety_config(car.CarParams.SafetyModel.hyundaiLegacy)]
    elif not disengage_on_accelerator:
      ret.safetyConfigs = [get_safety_config(car.CarParams.SafetyModel.hyundaiCommunity)]
    else:
      ret.safetyConfigs = [get_safety_config(car.CarParams.SafetyModel.hyundai, 0)]

    if candidate in CAMERA_SCC_CAR:
      ret.safetyConfigs[0].safetyParam |= Panda.FLAG_HYUNDAI_CAMERA_SCC
    return ret
