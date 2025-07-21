from typing_extensions import Literal
from pydantic import Field

from .._internal._base import ATELogRecordPolicy
from ..enums.prefix import ATEPrefix
from ..enums.status import ATEBoundaryScanStatus
from .misc import ATELogReportRecord

class ATELogBsOpenRecord(ATELogRecordPolicy):
  prefix       			  : Literal[ATEPrefix.BS_O]
  first_device_name		: str
  first_device_pin 		: str
  second_device_name	: str | None = Field(default=None)
  second_device_pin 	: str | None = Field(default=None)

class ATELogBsShortRecord(ATELogRecordPolicy):
  prefix       : Literal[ATEPrefix.BS_S]
  cause        : Literal['S', 'U', '0', '1'] # TODO
  node_list    : list[str]
  shorts_count : int

class ATELogBoundaryScanRecord(ATELogRecordPolicy):
  prefix				  : Literal[ATEPrefix.BS_CON]
  test_designator : str
  status          : ATEBoundaryScanStatus
  shorts_count    : int
  opens_count     : int
  sub             : list[ATELogBsShortRecord | ATELogBsOpenRecord | ATELogReportRecord] | None = Field(default=None)
  
  
