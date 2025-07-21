from typing_extensions import Literal, TypeAlias
from datetime import datetime
from pydantic import Field


from .._internal._base import ATELogRecordPolicy
from ..records.boundary_scan import ATELogBoundaryScanRecord
from ..alias import ATEAnalogRecordGenericPrefixesT
from ..records.misc import ATELogReportRecord
from ..enums.status import ATEAnalogRecordStatus, ATEGeneralRecordStatus, ATETestDigitalStatus, ATETestDigitalSubStatus
from ..enums.prefix import ATEPrefix
  

class ATELogDevicePinsRecord(ATELogRecordPolicy):
  prefix: Literal[ATEPrefix.DPIN]
  device_name: str
  node_pin_list: list[str | list[str]]

class ATELogIndictRecord(ATELogRecordPolicy):
  prefix          : Literal[ATEPrefix.INDICT]
  technique       : str 
  device_list     : list[str]
  est_capacitance : None | float
  est_inductance  : None | float
  est_model       : None | float
  est_resistance  : str = Field(default="")

class ATELogResultsOfProbingRecord(ATELogRecordPolicy):
  prefix          : Literal[ATEPrefix.PRB]
  test_status     : ATEGeneralRecordStatus
  pin_count       : int
  test_designator : str
  sub             : list[ATELogDevicePinsRecord]

class ATERetestRecord(ATELogRecordPolicy):
  prefix: Literal[ATEPrefix.RETEST]
  datetime: datetime 

class ATELogTestJetRecord(ATELogRecordPolicy):
  prefix          : Literal[ATEPrefix.TJET]
  test_status     : ATEGeneralRecordStatus
  pin_count       : int
  test_designator : str

class ATELogTestDigitalRecord(ATELogRecordPolicy):
  prefix                : Literal[ATEPrefix.D_T]
  test_status           : ATETestDigitalStatus 
  substatus             : ATETestDigitalSubStatus
  failing_vector_number : int | None = Field(default=None)
  pin_count             : int
  test_designator       : str

class ATELogPinsFailedRecord(ATELogRecordPolicy):
  prefix: Literal[ATEPrefix.PF]
  designator  : str
  test_status : ATEGeneralRecordStatus 
  total_pins  : int

class ATELogLimit3Record(ATELogRecordPolicy):
  prefix : Literal[ATEPrefix.LIM3]
  high_limit    : float
  low_limit     : float
  nominal_value : float

class ATELogLimit2Record(ATELogRecordPolicy):
  prefix : Literal[ATEPrefix.LIM2]
  high_limit : float
  low_limit  : float

class ATELogAnalogRecordGeneric(ATELogRecordPolicy):
  prefix             : ATEAnalogRecordGenericPrefixesT
  test_status        : ATEAnalogRecordStatus
  measured_value     : float
  subtest_designator : str | None = Field(default=None)
  sub : tuple[ATELogLimit2Record | ATELogLimit3Record] | None = Field(default=None)

PossibleATEBlockSubRecordT: TypeAlias = (
  ATELogAnalogRecordGeneric        
  | ATELogBoundaryScanRecord         
  | ATELogTestJetRecord        
  | ATELogTestDigitalRecord        
  | ATELogReportRecord)
