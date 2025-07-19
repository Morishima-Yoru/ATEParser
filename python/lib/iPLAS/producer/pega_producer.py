from enum import IntEnum

class ProducerStatus(IntEnum):
  OK                = 0
  GENERAL_ERR       = FAIL              = 10000
  NOT_INITIALIZED   = ERR_NOT_INIT      = 10001
  ILLEGAL_PARAMETER = ERR_ILLEGAL_PARAM = 10002
  BUSY              = ERR_BUSY          = 10003
  DISK_ISFULL       = ERR_DISKFULL      = 10004
  FILE_NOTFOUND     = ERR_FILE          = 10005
  DAEMON_DESTROYED  = ERR_EXIT          = 10006
  SIZE_EXCEED       = ERR_SIZE          = 10007
  UNKNOWN_TOKEN     = ERR_PRJ_TOKEN     = 10008
  TOPIC_NOTFOUND    = ERR_TOPIC         = 10009
  DEPRECATED        = ERR_UPDATE        = 33832

  @property
  def is_failed(self) -> bool: return self != ProducerStatus.OK

class ProducerOpMode(IntEnum):
  """ Enumerates the operation modes of the Producer.

  :cvar ONLINE:  Online mode. Messages will be sent.
  :cvar DEBUG:   DEBUG mode.  All messages and files will be sent. Furthermore, the name of msg/file will be stored to the local
  :cvar OFFLINE: Offline mode. Only name of the msg/file will be stored to the local.
  """
  ONLINE  = 0
  DEBUG   = 1
  OFFLINE = 2
