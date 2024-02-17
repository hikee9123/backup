from cereal import car
from panda import Panda
from openpilot.common.params import Params
from openpilot.selfdrive.car.hyundai.values import CANFD_CAR, CAMERA_SCC_CAR, LEGACY_SAFETY_MODE_CAR
from openpilot.selfdrive.car import get_safety_config





def get_params( ret, candidate ):
  params = Params()  
  disengage_on_accelerator = params.get_bool("DisengageOnAccelerator")

  if not disengage_on_accelerator:
    ret.safetyConfigs = [get_safety_config(car.CarParams.SafetyModel.hyundaiCommunity)]
    return True

  return False
