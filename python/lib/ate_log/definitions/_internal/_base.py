
from abc import ABC
from datetime import datetime
from pydantic import BaseModel, ConfigDict, field_validator

class _ATELogPolicy(BaseModel, ABC):
  model_config = ConfigDict(
    use_enum_values=False,)

class ATELogRecordPolicy(_ATELogPolicy):
  @field_validator(
    'end_datetime', 
    'start_datetime', 
    'datetime', 
    mode='before', check_fields=False)
  def datetime_validate(cls, var: int) -> datetime:
    var_ = str(var)
    return datetime.strptime(var_, "%y%m%d%H%M%S")