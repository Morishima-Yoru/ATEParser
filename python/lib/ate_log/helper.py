
from collections.abc import Iterator
from typing import overload
from .definitions.base import ATELogBatchRecord, ATELogBlockRecord, ATELogBtestRecord
from .definitions.enums.prefix import ATEPrefix
from .definitions.records.general import ATELogReportRecord
from .definitions.records.boundary_scan import ATELogBoundaryScanRecord


class ATELogHelper:
  def __init__(self, log: ATELogBatchRecord):
    self.log: ATELogBatchRecord = log

  def get_blocks(self) -> Iterator[ATELogBlockRecord]:
    all_btest = (_ for _ in self.log.sub)
    for btest_ in all_btest:
      for record_ in btest_.sub:
        if (record_.prefix == ATEPrefix.BLOCK):
          yield record_
  
  @overload
  @classmethod
  def get_report(cls, var: ATELogBlockRecord) -> tuple[ATELogReportRecord, ...]: ...
  
  @overload
  @classmethod
  def get_report(cls, var: ATELogBtestRecord) -> tuple[ATELogReportRecord, ...]: ...
  
  @classmethod
  def get_report(cls, var: ATELogBlockRecord | ATELogBtestRecord) -> tuple[ATELogReportRecord, ...]: 
    if isinstance(var, ATELogBtestRecord): return cls._get_btest_report(var)
    else: return cls._get_block_report(var)
      
      
  @staticmethod
  def _get_btest_report(btest: ATELogBtestRecord) -> tuple[ATELogReportRecord, ...]:
    memo_: list[ATELogReportRecord] = list()
    for itr in btest.sub:
      if isinstance(itr, ATELogReportRecord):
        memo_.append(itr)
    return tuple(memo_)
    
  @staticmethod
  def _get_block_report(block: ATELogBlockRecord) -> tuple[ATELogReportRecord, ...]:
    memo_: list[ATELogReportRecord] = list()
    for itr in block.sub:
      if isinstance(itr, ATELogReportRecord):
        memo_.append(itr)
      if isinstance(itr, (
        ATELogBoundaryScanRecord, 
        # TODO
      )):
        if (itr.sub is None): continue
        for itr2 in itr.sub:
          if isinstance(itr2, ATELogReportRecord):
            memo_.append(itr2)
    return tuple(memo_)
          
      


