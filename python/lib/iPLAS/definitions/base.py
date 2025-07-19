from typing import Dict, Union, Optional, ClassVar, Tuple, List
from typing_extensions import Literal

import csv
import shutil
from datetime import datetime, timedelta
from io import StringIO
from pathlib import Path
from pydantic import BaseModel, field_serializer, Field, field_validator

from .alias import IPLASOnlineT, IPLASStatusT
from .misc import JSONObjectT


class IPLASLogContext(BaseModel):
  TS_FMT           : ClassVar[str] = r"%Y-%m-%d %H:%M:%S"
  TITLE            : ClassVar[Tuple[str, str, str, str, str, str]] = (r"TEST", r"STATUS", r"VALUE", r"UCL", r"LCL", r"CYCLE")
  isn              : str = Field(serialization_alias='ISN')
  project          : str = Field(serialization_alias='Project')
  tsp              : str = Field(serialization_alias='TSP')
  test_status      : IPLASStatusT = Field(serialization_alias='Test Status')
  error_code       : Optional[str] = Field(serialization_alias='ErrorCode')
  online_type      : IPLASOnlineT = Field(serialization_alias='Type')
  test_start_time  : datetime = Field(serialization_alias='Test Start Time')
  test_end_time    : datetime = Field(serialization_alias='Test end Time')
  device_id        : str = Field(serialization_alias='DeviceId')
  model            : Optional[str] = Field(serialization_alias='Model', default=None)
  line             : Optional[Union[str, Literal["NULL"]]] = Field(serialization_alias='Line', default="NULL")
  lib_ver          : Optional[str] = Field(serialization_alias='ProducerVersion', default=None)
  send_time        : Optional[datetime] = Field(serialization_alias='CallerSendTime', default=None)
  sfis_message     : Optional[str] = Field(serialization_alias='SFIS message', default=None)
  slot             : Optional[str] = Field(serialization_alias='Slot', default=None)
  mo               : Optional[str] = Field(serialization_alias='MO', default=None)
  error_message    : Optional[Union[str, Literal['PASS']]] = Field(serialization_alias='Error Message', default=None)
  ats_ver          : Optional[str] = Field(serialization_alias='ATS_Ver', default=None)
  sn               : Optional[str] = Field(serialization_alias='SN', default=None)
  error_test_item  : Optional[str] = Field(serialization_alias='ErrorName', default=None)
  elapsed_time     : Optional[timedelta] = Field(serialization_alias='Total Testing Time', default=None)
  grp              : Optional[str] = Field(serialization_alias='GRP', default=None)

  @field_serializer("error_code", when_used='always')
  def serialize_error_code(self, var: Optional[str], _) -> Union[str, Literal['']]: return '' if var is None else var

  @field_serializer("test_start_time", "test_end_time", "send_time", when_used='always')
  def serialize_ts(self, ts: datetime, _info) -> str: return ts.strftime(self.TS_FMT)

  @field_validator("test_start_time", "test_end_time", "send_time", mode='before')
  def deserialize_ts(cls, v, _info) -> datetime:
    if isinstance(v, datetime): return v
    return datetime.strptime(v, cls.TS_FMT)
  
  @field_serializer("elapsed_time", when_used='always')
  def serialize_dt(self, dt: timedelta, _info) -> str: return str(dt.seconds)

  @field_validator("elapsed_time", mode="before")
  def deserialize_dt(cls, dt: Union[timedelta, str], _) -> timedelta:
    if isinstance(dt, timedelta): return dt
    return timedelta(seconds=float(dt))
  


class IPLASTestItem(BaseModel):
  name:   str
  status: bool
  value:  Union[float, str]
  ucl:    Optional[Union[float, str]]
  lcl:    Optional[Union[float, str]]
  cycle:  Optional[float]

  @field_serializer("status", when_used='always')
  def serialize_status(self, var: bool, _) -> Literal['PASS', 'FAIL']: 
    return 'PASS' if var else 'FAIL'

  @field_validator("status", mode="before")
  def deserialize_status(cls, stat, _) -> bool: 
    return (str(stat).lower() in {"true", "pass", "passed", "1", "1.0"})
  
  @field_validator("cycle", mode='before')
  def deserialize_cycle(cls, var: Optional[str], _) -> Union[None, float]:
    if (var is None): return None
    return float(var) 
  
class IPLASLog(IPLASLogContext):
  test_items: Optional[List[IPLASTestItem]]


  def export(
      self,
      *,
      exclude_none: bool = True,
      save_filepath: Optional[Union[str, Path]]=None) -> str:
    """ Export/Serialize a CSV formatted string which is suitable to upload to iPLAS service

    >>> from handler.ATE import ATEComposer
    >>> composer = ATEComposer()
    >>> log = composer.parse("test.txt")
    >>> log = composer.to_iplas(log)
    >>> _csv = log.export(exclude_none=True, save_filepath=None)
    >>> print(_csv)

    :param exclude_none: Ignore ``None`` value or specified with ``''``. Defaults: ``True``
    :type exclude_none: bool
    :param save_filepath: Save exported log as specified filepath. Defaults: ``None``
    :type save_filepath: str | Path | None
    :return: A CSV formatted string which is suitable to upload to iPLAS service
    :rtype: str
    :raise OSError: Pending ``save_filepath`` unable to write results in.
    """

    serialized_: JSONObjectT = self.model_dump(mode='json', by_alias=True, exclude_none=exclude_none)
    _io = StringIO()
    _writer = csv.writer(_io, delimiter=',', lineterminator='\n')
    _writer.writerow(self.TITLE)
    for k, vs in serialized_.items():
      if (isinstance(vs, str) or vs is None):
        _writer.writerow(self.__construct_value_row(k, vs or ''))

    test_items_: Optional[List[Dict[str, Optional[Union[str, bool, float]]]]] = serialized_.get('test_items') # type: ignore
    assert isinstance(test_items_, list)
    _writer.writerows((
      itr["name"],
      itr["status"],
      itr["value"],
      itr.get("ucl", None),
      itr.get("lcl", None),
      itr.get("cycle", None))
    for itr in test_items_)

    if (save_filepath):
      fpth = Path(save_filepath).absolute()
      with open(fpth, 'w') as f:
        _io.seek(0)
        shutil.copyfileobj(_io, f, -1)
    return _io.getvalue()
  
  @staticmethod
  def __construct_value_row(k: str, v: str) -> Tuple[str, str, str, str, str, str]:
    return (k, '', v, '', '', '')
