
from cereal import car
from openpilot.common.conversions import Conversions as CV


class CarStateCustom():
  def __init__(self, CP):
    self.oldCruiseStateEnabled = False    


  def update(self, ret, cs):
      if ret.doorOpen:
        self.oldCruiseStateEnabled = False
      elif ret.seatbeltUnlatched:
        self.oldCruiseStateEnabled = False
      elif ret.gearShifter != car.CarState.GearShifter.drive:
        self.oldCruiseStateEnabled = False
      elif not ret.cruiseState.available:
        self.oldCruiseStateEnabled = True
      elif self.oldCruiseStateEnabled:
        ret.cruiseState.enabled = True    