from pathlib import Path
from typing import TYPE_CHECKING, Any, Callable, Final, Union
import os.path as osp
import sys
import ctypes as ct
from enum import IntEnum


class ParseResult(IntEnum):
    SUCCESS               = 0
    INPUT_ERROR           = 1
    PARSE_ERROR           = 2
    JSON_CONVERSION_ERROR = 3
    OUTPUT_ERROR          = 4
    FAILED_INTEGRITY      = 5


class ATELogParseWrapper:
  DLL_RELFPTH: Final[Path] = Path(r"bin\ATEParser.dll")
  def __init__(self, dll_fpth: str | None=None) -> None:
    self.dll_fpth: Path
    if (dll_fpth is None):
      dll_fpth = osp.abspath(sys.modules[ATELogParseWrapper.__module__].__file__ or '')
    self.dll_fpth = Path(dll_fpth).parent / self.DLL_RELFPTH
    self._handler: ct.CDLL = ct.cdll.LoadLibrary(str(self.dll_fpth))
    
    self._handler.parse_file.restype = ct.c_int
    self._handler.parse_file.argtypes = [
        ct.c_char_p, 
        ct.c_char_p, 
        ct.c_int,    
        ct.c_bool,
        ct.POINTER(ct.c_char_p)]
    
    self._handler.parse_log_string.restype = ct.c_int
    self._handler.parse_log_string.argtypes = [
        ct.c_char_p, 
        ct.c_char_p, 
        ct.c_int,    
        ct.c_bool,
        ct.POINTER(ct.c_char_p)]

    self._handler.free_mem.restype = None
    self._handler.free_mem.argtypes = [ct.c_void_p]
  
    self.__construct()
    self.__type_hintting()
  
  def parse_log_string(
      self, 
      content  : str | bytes, 
      dst_fpth : str | None=None,
      indent   : int=2,
      keep_raw : bool=False) -> tuple[ParseResult, str]:
    if isinstance(content, str): content = str(content).encode('utf-8')
    dst_fpth_ = str(dst_fpth).encode('utf-8') if dst_fpth else b''
    out_json_str = ct.c_char_p()
    ret_code = self.__parse_log_string(
      ct.c_char_p(content), 
      ct.c_char_p(dst_fpth_), 
      ct.c_int(indent), 
      ct.c_bool(keep_raw),
      ct.byref(out_json_str)) # type: ignore
    if ret_code != ParseResult.SUCCESS:
        raise RuntimeError(f"Failed to parse log string, error code: {ret_code}")
    if not out_json_str.value:
        return ParseResult.OUTPUT_ERROR, ""
    result = out_json_str.value.decode('utf-8')
    self.__free_mem(ct.cast(out_json_str, ct.c_void_p))
    return ParseResult.SUCCESS, result
    
  
  def parse_file(
      self, 
      log_fpth : str, 
      dst_fpth : str | None=None,
      indent   : int=2,
      keep_raw : bool=False) -> tuple[ParseResult, str]:
    log_path_bytes = str(log_fpth).encode('utf-8')
    dst_path_bytes = str(dst_fpth).encode('utf-8') if dst_fpth else b''
    out_json_str = ct.c_char_p()
    ret_code = self.__parse_file(
      ct.c_char_p(log_path_bytes), 
      ct.c_char_p(dst_path_bytes), 
      ct.c_int(indent), 
      ct.c_bool(keep_raw),
      ct.byref(out_json_str)) # type: ignore
    ret_ = (out_json_str.value or b'').decode('utf-8')
    self.__free_mem(ct.cast(out_json_str, ct.c_void_p))
    return ParseResult(ret_code), ret_
    
  def __construct(self) -> None:
    self.__parse_file       = self._handler.parse_file
    self.__parse_log_string = self._handler.parse_log_string
    self.__free_mem         = self._handler.free_mem
  
  def __type_hintting(self) -> None:
    if TYPE_CHECKING:
      self.__parse_file: Callable[[
        ct.c_char_p, 
        ct.c_char_p, 
        ct.c_int,    
        ct.c_bool,
        ct._Pointer[ct.c_char_p]], int]
      self.__parse_log_string: Callable[[
        ct.c_char_p, 
        ct.c_char_p, 
        ct.c_int,    
        ct.c_bool,
        ct._Pointer[ct.c_char_p]], int]
      self.__free_mem: Callable[[ct.c_void_p], None]
  