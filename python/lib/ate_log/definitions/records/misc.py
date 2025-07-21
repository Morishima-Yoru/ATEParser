from lib.ATELog.definitions._internal._base import ATELogRecordPolicy
from lib.ATELog.definitions.enums.prefix import ATEPrefix


from typing_extensions import Literal


class ATELogReportRecord(ATELogRecordPolicy):
  prefix  : Literal[ATEPrefix.RPT]
  message : str


class ATELogArbitraryRecord(ATELogRecordPolicy):
  prefix  : Literal[ATEPrefix.UNKNOWN]
  raw     : str