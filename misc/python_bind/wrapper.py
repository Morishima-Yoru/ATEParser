from pathlib import Path
from typing import TYPE_CHECKING, Callable, Final, no_type_check
import os.path as osp
import sys
import ctypes as ct
from enum import IntEnum

class ParseResult(IntEnum):
    SUCCESS = 0
    INPUT_ERROR = 1
    PARSE_ERROR = 2
    JSON_CONVERSION_ERROR = 3
    OUTPUT_ERROR = 4
    FAILED_INTEGRITY = 5

class ATELogParseWrapper:
    LIBRARY_FNAMES: Final[tuple[str, ...]] = (
        r"ATEParser.dll",
        r"ATEParser.so",
    )
    def __init__(self, lib_filepath: str | Path | None=None) -> None:
        """
        Initialize the wrapper with the path to the ATEParser library.
        If no path is provided, it will try to load the library from the default location
        The default is the `bin/` directory as this module.

        Args:
            lib_filepath (str | Path | None, optional): The path to the ATEParser library. Defaults to None.
        
        Raises:
            FileNotFoundError: If the library cannot be found.
        """
        self._handler: ct.CDLL
        if lib_filepath is not None:
            self._handler = ct.cdll.LoadLibrary(str(lib_filepath))
        else:
            lib_dpth_ = Path(osp.abspath(sys.modules[ATELogParseWrapper.__module__].__file__ or '')) / 'bin'
            self.__try_load_lib(lib_dpth_)

        self.__construct()

    def __try_load_lib(self, lib_dirpath: Path) -> None:
        for lib_fname in self.LIBRARY_FNAMES:
            lib_fpth_ = lib_dirpath / lib_fname
            if lib_fpth_.is_file():
                self._handler = ct.cdll.LoadLibrary(str(lib_fpth_))
                return
        raise FileNotFoundError(f"Failed to find any of the following libraries: {self.LIBRARY_FNAMES}")

    def parse_log_string(
        self, 
        content: str | bytes, 
        dst_fpth: str | None = None,
        indent: int = 2,
        keep_raw: bool = False
    ) -> tuple[ParseResult, str]:
        """
        Parse a log string and return the result as a JSON string.

        Args:
            content (str | bytes): The log string to parse.
            dst_fpth (str | None, optional): The path to the output JSON file. Defaults to None.
            indent (int, optional): The indentation level for the JSON output. Defaults to 2.
            keep_raw (bool, optional): Whether to keep the raw log lines in the JSON output. Defaults to False.

        Returns:
            tuple[ParseResult, str]: A tuple containing the parse result and the JSON string.
        """
        if isinstance(content, str): content = str(content).encode('utf-8')
        dst_fpth_ = str(dst_fpth).encode('utf-8') if dst_fpth else b''
        out_json_str = ct.c_char_p()
        ret_code = self.__parse_log_string(
            ct.c_char_p(content), 
            ct.c_char_p(dst_fpth_), 
            ct.c_int(indent), 
            ct.c_bool(keep_raw),
            ct.byref(out_json_str) # type: ignore
        )
        if ret_code != ParseResult.SUCCESS:
            raise RuntimeError(f"Failed to parse log string, error code: {ret_code}")
        if not out_json_str.value:
            return ParseResult.OUTPUT_ERROR, ""
        result = out_json_str.value.decode('utf-8')
        self.__free_mem(ct.cast(out_json_str, ct.c_void_p))
        return ParseResult.SUCCESS, result

    def parse_file(
        self, 
        log_fpth: str, 
        dst_fpth: str | None = None,
        indent: int = 2,
        keep_raw: bool = False
    ) -> tuple[ParseResult, str]:
        """
        Parse a log file and return the result as a JSON string.

        Args:
            log_fpth (str): The path to the log file to parse.
            dst_fpth (str | None, optional): The path to the output JSON file. Defaults to None.
            indent (int, optional): The indentation level for the JSON output. Defaults to 2.
            keep_raw (bool, optional): Whether to keep the raw log lines in the JSON output. Defaults to False.

        Returns:
            tuple[ParseResult, str]: A tuple containing the parse result and the JSON string.
        """
        log_path_bytes = str(log_fpth).encode('utf-8')
        dst_path_bytes = str(dst_fpth).encode('utf-8') if dst_fpth else b''
        out_json_str = ct.c_char_p()
        ret_code = self.__parse_file(
            ct.c_char_p(log_path_bytes), 
            ct.c_char_p(dst_path_bytes), 
            ct.c_int(indent), 
            ct.c_bool(keep_raw),
            ct.byref(out_json_str) # type: ignore
        )
        ret_ = (out_json_str.value or b'').decode('utf-8')
        self.__free_mem(ct.cast(out_json_str, ct.c_void_p))
        return ParseResult(ret_code), ret_

    def __construct(self) -> None:
        self._handler.parse_file.restype = ct.c_int
        self._handler.parse_file.argtypes = [
            ct.c_char_p, 
            ct.c_char_p, 
            ct.c_int,    
            ct.c_bool,
            ct.POINTER(ct.c_char_p)
        ]

        self._handler.parse_log_string.restype = ct.c_int
        self._handler.parse_log_string.argtypes = [
            ct.c_char_p, 
            ct.c_char_p, 
            ct.c_int,    
            ct.c_bool,
            ct.POINTER(ct.c_char_p)
        ]

        self._handler.free_mem.restype = None
        self._handler.free_mem.argtypes = [ct.c_void_p]
        self.__parse_file = self._handler.parse_file
        self.__parse_log_string = self._handler.parse_log_string
        self.__free_mem = self._handler.free_mem

    if TYPE_CHECKING:
        @no_type_check
        def __type_hintting(self) -> None:
            self.__parse_file: Callable[[
                ct.c_char_p, 
                ct.c_char_p, 
                ct.c_int,    
                ct.c_bool,
                ct._Pointer[ct.c_char_p]], int
            ]
            self.__parse_log_string: Callable[[
                ct.c_char_p, 
                ct.c_char_p, 
                ct.c_int,    
                ct.c_bool,
                ct._Pointer[ct.c_char_p]], int
            ]
            self.__free_mem: Callable[[ct.c_void_p], None]
