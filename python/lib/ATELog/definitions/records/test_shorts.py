

from typing import Literal

from pydantic import Field


from ..enums.prefix import ATEPrefix
from ..enums.status import ATETestShortsStatus
from .misc import ATELogReportRecord
from .general import ATELogRecordPolicy

class ATELogTsPhantomRecord(ATELogRecordPolicy):
  prefix: Literal[ATEPrefix.TS_P]
  deviation         : float

class ATELogTsOpenRecord(ATELogRecordPolicy):
  prefix: Literal[ATEPrefix.TS_O]
  source_node       : str
  destination_node  : str
  deviation         : float

class ATELogTsDestinationRecord(ATELogRecordPolicy):
  prefix: Literal[ATEPrefix.TS_D]
  destinations_list: list[tuple[str, float]]

class ATELogTsSourceRecord(ATELogRecordPolicy):
  prefix: Literal[ATEPrefix.TS_S]
  shorts_count   : int
  phantoms_count : int
  source_node    : str
  sub            : list[ATELogTsDestinationRecord | ATELogTsPhantomRecord | ATELogReportRecord] | None = Field(default=None)

class ATELogTestShortsRecord(ATELogRecordPolicy):
  prefix: Literal[ATEPrefix.TS]
  test_status    : ATETestShortsStatus 
  shorts_count   : int
  opens_count    : int
  phantoms_count : int
  designator     : str | None = Field(default=None)
  sub            : list[ATELogTsOpenRecord | ATELogTsSourceRecord | ATELogReportRecord] | None = Field(default=None)
