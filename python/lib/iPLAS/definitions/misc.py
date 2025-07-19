from typing import TYPE_CHECKING, Dict, List, Tuple, Union
from typing_extensions import TypeAlias, TypeAliasType

# Directly declare the TypeAlias about JSONObject type and assign it to `BaseModel` will cause `RecursionError`
# Refer: https://github.com/pydantic/pydantic/issues/8346 for more details
if TYPE_CHECKING:
  _JSONValueT: TypeAlias = Union[str, int, float, bool, None, List[Union[str, int, float]], Tuple[Union[str, int, float], ...]]
  JSONObjectT: TypeAlias = Dict[str, Union[_JSONValueT, 'JSONObjectT']]
else:
  _JSONValueT = TypeAliasType("_JSONValueT", 
                              "Union[str, int, float, bool, None, List[Union[str, int, float]], Tuple[Union[str, int, float], ...]]")
  JSONObjectT = TypeAliasType("JSONObjectT", 
                              "Dict[str, Union[_JSONValueT, JSONObjectT]]")
