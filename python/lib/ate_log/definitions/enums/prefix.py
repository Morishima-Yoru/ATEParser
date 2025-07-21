

from contextlib import suppress
from enum import Enum

class ATEPrefix(str, Enum):
  A_CAP   = '@A-CAP'
  A_DIO   = '@A-DIO'
  A_FUS   = '@A-FUS'
  A_IND   = '@A-IND'
  A_JUM   = '@A-JUM'
  A_MEA   = '@A-MEA'
  A_NFE   = '@A-NFE'
  A_NPN   = '@A-NPN'
  A_PFE   = '@A-PFE'
  A_PNP   = '@A-PNP'
  A_POT   = '@A-POT'
  A_RES   = '@A-RES'
  A_SWI   = '@A-SWI'
  A_ZEN   = '@A-ZEN'
  ALM     = '@ALM'
  AID     = '@AID'
  ARRAY   = '@ARRAY'
  BATCH   = '@BATCH'
  BLOCK   = '@BLOCK'
  BS_CON  = '@BS-CON'
  BS_O    = '@BS-O'
  BS_S    = '@BS-S'
  BTEST   = '@BTEST'
  DPIN    = '@DPIN'
  D_PLD   = '@D-PLD'
  D_T     = '@D-T'
  INDICT  = '@INDICT'
  LIM2    = '@LIM2'
  LIM3    = '@LIM3'
  NETV    = '@NETV'
  NODE    = '@NODE'
  PCHK    = '@PCHK'
  PIN     = '@PIN'
  PF      = '@PF'
  PRB     = '@PRB'
  RETEST  = '@RETEST'
  RPT     = '@RPT'
  TJET    = '@TJET'
  TS      = '@TS'
  TS_D    = '@TS-D'
  TS_O    = '@TS-O'
  TS_P    = '@TS-P'
  TS_S    = '@TS-S'
  #@EXPRT
  #@NOTE
  UNKNOWN = "@UNKNOWN"
  
  @property
  def prefix(self) -> str: return self.value

  @classmethod
  def by_prefix(cls, prefix: str) -> 'ATEPrefix':
    with suppress(ValueError): return cls(prefix)
    return cls.UNKNOWN

if __name__ == "__main__":
  print(ATEPrefix.by_prefix("@LIM"))
