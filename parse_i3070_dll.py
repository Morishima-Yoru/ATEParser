import ctypes
import os
from typing import Optional

DLL_PATH = r"D:\moris\!workspace\ATEParser\build\output_dll\ATEParser.dll"

# 載入 DLL
_ateparser = ctypes.cdll.LoadLibrary(DLL_PATH)

# 設定函數原型
_ateparser.parse_i3070_log.restype = ctypes.c_void_p
_ateparser.parse_i3070_log.argtypes = [
    ctypes.c_char_p,  # log_filepath
    ctypes.c_char_p,  # dst_filepath
    ctypes.c_int,     # indent
    ctypes.c_bool     # keep_raw
]

_ateparser.free_i3070_result.restype = None
_ateparser.free_i3070_result.argtypes = [ctypes.c_void_p]

def parse_i3070_log(log_filepath: str, dst_filepath: Optional[str] = None, indent: int = 2, keep_raw: bool = False) -> str:
    """
    解析 i3070 log 檔案，回傳 JSON 字串。
    :param log_filepath: log 檔案路徑
    :param dst_filepath: 輸出 JSON 檔案路徑（可為 None）
    :param indent: JSON 縮排
    :param keep_raw: 是否保留 raw 欄位
    :return: JSON 字串
    """
    log_path_bytes = log_filepath.encode('utf-8')
    dst_path_bytes = dst_filepath.encode('utf-8') if dst_filepath else b''
    result_ptr = _ateparser.parse_i3070_log(log_path_bytes, dst_path_bytes, indent, keep_raw)
    if not result_ptr:
        return ''
    result = ctypes.string_at(result_ptr).decode('utf-8')
    # print(result)
    _ateparser.free_i3070_result(result_ptr)
    print(result)
    return result

# 測試用
if __name__ == '__main__':
    import sys
    if len(sys.argv) < 2:
        print('Usage: python parse_i3070_dll.py <logfile> [outfile]')
        exit(1)
    logf = sys.argv[1]
    outf = sys.argv[2] if len(sys.argv) > 2 else None
    dat_ = parse_i3070_log(logf, outf, indent=2, keep_raw=False)
    print("a")
    print(dat_)