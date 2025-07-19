from pathlib import Path
import os, sys
sys.path.append(os.getcwd())
from typing import Final

from lib.ATELog.wrapper import ATELogParseWrapper


LOG_FPTH: Final[str] = r"D:\moris\@workspace\ATEUploader\@misc\example_log\1-1.txt"

def main() -> None:
  parser = ATELogParseWrapper()
  ret_code, json_str = parser.parse_file(
    log_fpth=LOG_FPTH,
    dst_fpth=str(Path(LOG_FPTH).with_suffix(".json")))
  print(ret_code, json_str)

if __name__ == "__main__":
  main()