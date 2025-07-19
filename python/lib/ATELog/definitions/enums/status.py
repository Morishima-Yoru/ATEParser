from enum import IntEnum, IntFlag


class ATEBTestTestStatus(IntEnum):
  PASSED               = 0
  UNCATEGORIZED_FAIL   = 1
  PIN_TEST_FAIL        = 2
  LEARN_MODE_FAIL      = 3
  SHORTS_TEST_FAIL     = 4
  # 5 is RESERVED     
  ANALOG_TEST_FAIL     = 6
  POWER_SUPPLY_FAIL    = 7
  DIGITAL_SCAN_FAIL    = 8
  FUNCTIONAL_TEST_FAIL = 9
  PRE_SHORTS_FAIL      = 10
  BOARD_HANDLER_FAIL   = 11
  BARCODE_FAIL         = 12
  XED_OUT              = 13
  VTEP_TESTJET_FAIL    = 14
  POLARITY_FAIL        = 15
  CONNECTCHECK_FAIL    = 16
  ANALOG_CLUSTER_FAIL  = 17
  # 18-79 is RESERVED     
  RUNTIME_ERROR        = 80
  ABORTED_STOP         = 81
  ABORTED_BREAK        = 82
  # 83-89 is RESERVED     
  # 90-99 is USER_DEFINABLE     

class ATETestDigitalStatus(IntEnum):
  PASSED                  = 0
  FAILED                  = 1
  CRC_RELATED_FAILURE     = 5
  FATAL_ERROR             = 7
  CHAIN_INTEGRITY_FAILURE = 8

class ATETestDigitalSubStatus(IntFlag):
  FAIL              = 0b000001
  SAFEGUARD_TIMEOUT = 0b000010
  HARDWARE_ERROR    = 0b000100
  PAUSE             = 0b001000
  HALT              = 0b010000
  OVERVOLTAGE       = 0b100000

class ATEBoundaryScanStatus(IntEnum):
  PASS = 0
  FAIL = 1
  CHAIN_FAILURE = 7

class ATEAnalogRecordStatus(IntEnum):
  PASSED              = 0
  FAILED              = 1
  COMPLIANCE_LIMIT    = 2
  DETECTOR_TIMEOUT    = 3
  MEASUREMENT_TIMEOUT = 7
  ABORTED_BY_OPERATOR = 11

class ATETestShortsStatus(IntEnum):
  PASSED = 0
  FAILED = 1
  LEARNING_PASSED = 20

class ATEGeneralRecordStatus(IntEnum):
  PASS = 0
  FAIL = 1
  FATAL_ERROR = 7



