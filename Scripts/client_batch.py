# =========================================================================
#
#  File Name:         client_batch.py
#  Design Unit Name:
#  Revision:          OSVVM MODELS STANDARD VERSION
#
#  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
#  Contributor(s):
#     Simon Southwell      simon.southwell@gmail.com
#
#
#  Description:
#      Client test script for OSVVM TCP/IP socket features
#
#  Revision History:
#    Date      Version    Description
#    11/2022   2023.01    Initial revision
#
#
#  This file is part of OSVVM.
#
#  Copyright (c) 2022 by [OSVVM Authors](../AUTHORS.md)
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
# =========================================================================

import argparse
import time
import socket
import platform
import os

class client_batch :

  # -----------------------------------------------------------------
  # __init__
  #
  # Constructor for client_gui class
  #
  def __init__(self) :
  
      # Define widget variables
    self.__hostName          = 'localhost'
    self.__portNumber        = '49152'

  # -----------------------------------------------------------------
  # __chksum()
  #
  # Method to calculate a byte checksum over a string and return
  # an ASCII HEX byte value string
  #
  @staticmethod
  def __chksum(msg) :

    checksum = 0

    for c in range(0, len(msg)) :
      checksum = (checksum + ord(msg[c])) % 256

    if checksum < 16 :
      return '0' + hex(checksum)[2:]
    else :
      return hex(checksum)[2:]

  # -----------------------------------------------------------------
  # __sendmsg()
  #
  # Method to retrieve a gdb remote protocol message over a socket
  #
  def __sendmsg (self, str, skt) :
    msg = '$' + str + '#' + self.__chksum(str)
    skt.sendall (msg.encode())

    try:
      response = self.__getmsg(skt)
    except :
      response = ''

    return response

  # -----------------------------------------------------------------
  # __getmsg()
  #
  # Method to retrieve a gdb remote protocol message over a socket
  #
  @staticmethod
  def __getmsg(skt) :

    msgstr = ''
    while True :
       msg = skt.recv(1)
       msgstr += msg.decode()
       if msg.decode() == '#' :
         break

    msg = skt.recv(1)
    msgstr += msg.decode()
    msg = skt.recv(1)
    msgstr += msg.decode()

    return msgstr

  # -----------------------------------------------------------------
  #  __connectSkt()
  #
  #  Method to connect to socket
  #
  def __connectSkt(self) :

    try :
      # Open up a socket
      self.__skt               = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

      # Open connection with configuired host TCP/IP address or name, and port number
      self.__skt.connect((self.__hostName, int(self.__portNumber)))

    # Update widget states on unsuccessful connection
    except:

      # Shutdown and close the connection
      self.__skt.close()

  # -----------------------------------------------------------------
  # __disconnectSkt()
  #
  # Method to disconnect from socket
  # 
  def __disconnectSkt(self) :

    response = self.__sendmsg('D', self.__skt)

    # Shutdown and close the connection
    self.__skt.shutdown(socket.SHUT_RDWR)
    self.__skt.close()

    return response


  # -----------------------------------------------------------------
  # __scriptExec()
  #
  # Method to process scrpt and send commands
  #
  def __scriptExec(self) :

    # Open the file
    script = open(self.__scriptFile, 'r')

    # for all readlines
    for line in script :

      # Remove leading and trailing whitespace
      line.strip()

      # Process the line if not a comment
      if line[0] != '#' :

        # Check a valid command
        if line[0] in 'MmDk' :

          # Strip newlines
          msg = line.rstrip()
          
          if msg[0] == 'D' :
            self.__disconnectSkt()
          else :

            # Send and get response
            response     = self.__sendmsg(msg, self.__skt)

    # Close the script file
    script.close()

  # -----------------------------------------------------------------
  # runBatch()
  #
  # Top level public calling method to activate a batch run
  #
  def runBatch(self, portNum, script) :

    self.__txt             = None
    self.__batchMode       = True
    self.__portNumber      = portNum
    self.__scriptFile      = script
    self.__connectSkt();
    self.__scriptExec()
    self.__disconnectSkt()

  # --------------------------------------------------------------
  # Parse the command line arguments specific to the package
  # generator
  #
  @staticmethod
  def processCmdLine() :

      # Create a parser object
      parser = argparse.ArgumentParser(description='Process command line options.')

      # Command line options added here
      parser.add_argument('-p', '--portnum', dest='portNum', default='49152', action='store',
                          help='Set a TCP/IP port number')
      parser.add_argument('-s', '--script', dest='script', default='sktscript.txt', action='store',
                          help='Specify a script to run in batch mode')
      parser.add_argument('-w', '--wait', dest='wait', default='1', action='store',
                          help='Specify wait period (secs) before running batch script')

      return parser.parse_args()

# ###############################################################
# Only run if not imported
#
if __name__ == '__main__' :

  client= client_batch()

  # Process the command line options
  cmdArgs = client.processCmdLine()

  time.sleep(int(cmdArgs.wait))
  client.runBatch(cmdArgs.portNum, cmdArgs.script)
