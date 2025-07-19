

from datetime import datetime
from typing import Any
from pydantic import ConfigDict, Field
from typing_extensions import Literal, TypeAlias

from .records.misc import ATELogArbitraryRecord, ATELogReportRecord

from .records.test_shorts import ATELogTestShortsRecord

from ._internal._base import ATELogRecordPolicy
from .enums.prefix import ATEPrefix
from .enums.status import ATEAnalogRecordStatus, ATEBTestTestStatus, ATEBoundaryScanStatus, ATEGeneralRecordStatus, ATETestDigitalStatus
from .records.general import (
  ATELogBoundaryScanRecord,
  ATELogPinsFailedRecord, 
  ATELogResultsOfProbingRecord, 
  ATELogTestDigitalRecord, 
  PossibleATEBlockSubRecordT)


class ATETodo(ATELogRecordPolicy):
  model_config = ConfigDict(extra='allow')
  prefix: Any
  foo: None = Field(default=None)

class ATELogBlockRecord(ATELogRecordPolicy):
  prefix           : Literal[ATEPrefix.BLOCK]
  block_designator : str
  block_status     : (ATEAnalogRecordStatus 
                      | ATETestDigitalStatus 
                      | ATEBoundaryScanStatus 
                      | ATEGeneralRecordStatus 
                      | int)
  sub              : list[PossibleATEBlockSubRecordT]
  
  
PossibleATEBtestSubRecordT: TypeAlias = (
  ATELogBlockRecord               
  | ATELogBoundaryScanRecord                
  | ATELogTestDigitalRecord               
  | ATELogPinsFailedRecord                
  | ATELogResultsOfProbingRecord                 
  | ATELogReportRecord
  | ATELogTestShortsRecord
  | ATELogArbitraryRecord) # The log and log using statements can be used to generate other subrecords
  # @D-LOG
  # @RETEST|
  # @BLINE


class ATELogBtestRecord(ATELogRecordPolicy):
  prefix           : Literal[ATEPrefix.BTEST]
  board_id         : str
  test_status      : ATEBTestTestStatus | int
  start_datetime   : datetime
  duration         : int
  multiple_test    : bool
  log_level        : str
  log_set          : int | None
  learning         : bool
  known_good       : bool
  end_datetime     : datetime
  status_qualifier : str
  board_number     : int
  parent_panel_id  : str
  sub : list[PossibleATEBtestSubRecordT]


class ATELogBatchRecord(ATELogRecordPolicy):
  prefix                : Literal[ATEPrefix.BATCH]
  uut_type              : str
  uut_type_rev          : str
  fixture_id            : int
  testhead_number       : int
  testhead_type         : Literal[''] | str # "This is currently not used"
  process_step          : str
  batch_id              : str
  operator_id           : str
  controller            : str
  testplan_id           : str
  testplan_rev          : str
  parent_panel_type     : str
  parent_panel_type_rev : str
  version_label         : str
  sub                   : list[ATELogBtestRecord]

