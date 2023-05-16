# =========================================================================
#
#  File Name:         client_gui.py
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
#
# Main program calling hierarchy
#
#   run()                          -- Public run method. Create frame, add an image, frames and a text box, the run mainloop()
#     __genconnfrm()               -- Add socket connection sub-frame and populate with widgets
#     __gentransfrm()              -- Add Transaction Generation sub-frame and populate with widgets
#     __genscriptfrm()             -- Add Script File sub-frame and populate with widgets
#
# Button Callbacks:
#     __connectBtn()               -- On Connect button press, open a TCP/IP socket and update state
#         __opToTextWin()
#     __disconnectBtn()            -- On Disconnect button press, close socket and update state
#         __opToTextWin()
#         __sendmsg()
#             __chksum()
#             __getmsg()
#     __sendBtn()                  -- On Send button press send packet over socket to specification
#         __opToTextWin()
#         __chksum()
#         __sendmsg()
#             __chksum()
#             getmsg()
#         __procReadResp()
#     __scriptBrowseBtn()          -- On Browse button press, open file selector dialog to choose script file
#     __scriptExecBtn()            -- On Exectute button press, start processing script in a new thread, updating text box
#         __scriptExecBtnThread()  -- Thread method invoked after an Execute press
#             __opToTextWin()
#             __chksum()
#             __sendmsg()
#                 __chksum()
#                 __getmsg()
#             __procReadResp()
#
# Variable update callbacks:
#
#     __rnwUpdate()                -- Callback when read/write radio button state changed
#     __portUpdate()               -- Callback whe Port Number entry changed
#
# =========================================================================

import argparse
import time
import socket
import platform
import os
import sys

from tkinter            import *
from tkinter.ttk        import *
from tkinter.filedialog import askopenfilename, askdirectory
from threading          import Thread

class client_gui :

  # -----------------------------------------------------------------
  # __init__
  #
  # Constructor for client_gui class
  #
  def __init__(self) :

    # Define some 'constants'
    self.__CONNECTED_COLOUR    = '#20c060'
    self.__DISCONNECTED_COLOUR = '#ff4040'
    self.__ERROR_COLOUR        = '#ff0000'
    self.__DEFAULT_TXT_WIDTH   = 76
    self.__TXT_LINUX_PADDING   = 16
    self.__DEFAULT_PADDING     = 36

    # Create a Tk object
    self.__root = Tk()
    self.__root.title('OSVVM Co-simulation TCP/IP client GUI. ')

    # Define widget variables
    self.__hostName          = StringVar()
    self.__hostName.set('localhost')

    self.__portNumber        = StringVar()
    self.__portNumber.set('49152')
    self.__portNumber.trace ('w', self.__portUpdated)

    self.__portNumberHex     = StringVar()
    self.__portNumberHex.set(hex(int(self.__portNumber.get())))

    self.__connStatus        = StringVar()
    self.__connStatus.set('Not connected')

    self.__transAddr         = StringVar()
    self.__addrWidth         = IntVar()
    self.__addrWidth.set(4)

    self.__transData         = StringVar()
    self.__dataWidth         = IntVar()
    self.__dataWidth.set(4)

    self.__transRnw          = BooleanVar()
    self.__transRnw.set(False)
    self.__transRnw.trace ('w', self.__rnwUpdated)

    self.__scriptFile        = StringVar()
    self.__scriptFile.set('sktscript.txt')

    # Create a flag to indicate whether on Linux or not
    self.__isLinux           = platform.system().lower() == 'linux'

    self.__runDir            = StringVar()
    self.__runDir.set(os.getcwd())
    
    self.__batchMode         = False

  # -----------------------------------------------------------------
  # __opToTextWin()
  #
  # Method to output a string to the text window, with re-enabling
  # and immediate disabling
  #
  def __opToTextWin (self, hdl, str) :

    if not self.__batchMode :
      hdl.config(state = NORMAL)
      hdl.insert(INSERT, str);
      hdl.see('end');
      hdl.config(state = DISABLED)
    else :
      print(str, end = "")

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
  # __procReadResp()
  #
  #
  #
  def __procReadResp(self, response, datawidth) :

    # Get the hex number part of the response string
    hexstr = response[2:].split('#')[0]

    # Add any requred leading zeroes
    hexstr = '0' * (8 - len(hexstr)) + hexstr

    # Update the data entry box with the response value
    oldstate = self.__dataEntry['state']
    self.__dataEntry.config(state = NORMAL)
    self.__dataEntry.delete(0, END)
    self.__dataEntry.insert(0, hexstr)
    self.__dataEntry.config(state = oldstate)

    # Return number as an integer value
    return int('0x' + hexstr, 16)


  # -----------------------------------------------------------------
  #  __connectBtn()
  #
  #  Callback method for 'Connect' button activation
  #
  def __connectBtn(self) :
  
    if self.__connStatus.get() != 'Connected' or self.__batchMode :
        try :
          # Open up a socket
          self.__skt               = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
          # Open connection with configuired host TCP/IP address or name, and port number
          self.__skt.connect((self.__hostName.get(), int(self.__portNumber.get())))
        
          # Update widget states on successful connection
          if not self.__batchMode :
            self.__connStatus.set('Connected')
            self.__opToTextWin(self.__txt, 'Connected to port ' + self.__portNumber.get() + '\n')
            self.__scriptExecBtn.config(state = NORMAL)
            self.__sndBtn.config(state = NORMAL)
            self.__connStatusLbl.config(background=self.__CONNECTED_COLOUR)
            self.__hostnameEntry.config(state = DISABLED)
            self.__portNumEntry.config(state = DISABLED)
            self.__disconnBtnEntry.config(state = NORMAL)
            self.__connBtnEntry.config(state = DISABLED)
        
        # Update widget states on unsuccessful connection
        except:
          if not self.__batchMode :
            self.__connStatus.set('Connection Error')
            self.__connStatusLbl.config(background=self.__ERROR_COLOUR)
            self.__hostnameEntry.config(state = NORMAL)
            self.__portNumEntry.config(state = NORMAL)
            self.__disconnBtnEntry.config(state = DISABLED)
            self.__connBtnEntry.config(state = NORMAL)
        
          # Shutdown and close the connection
          self.__skt.close()

  # -----------------------------------------------------------------
  # __disconnectBtn()
  #
  # Callback method for 'Disconnect' button activation
  #
  def __disconnectBtn(self) :

    # If actually connected (else ignore)...
    if self.__connStatus.get() == 'Connected' or self.__batchMode:

      # Update widget state when disconnected
      if not self.__batchMode :
        self.__connStatus.set('Not Connected')
        self.__opToTextWin(self.__txt, 'Disconnected from port ' + self.__portNumber.get() + '\n')
        self.__scriptExecBtn.config(state = DISABLED)
        self.__sndBtn.config(state = DISABLED)
        self.__portNumEntry.config(state = NORMAL)
        self.__hostnameEntry.config(state = NORMAL)
        self.__connStatusLbl.config(background=self.__DISCONNECTED_COLOUR)
        
      response = self.__sendmsg('D', self.__skt)
      
      if not self.__batchMode :
        self.__disconnBtnEntry.config(state = DISABLED)
        self.__connBtnEntry.config(state = NORMAL)

      # Shutdown and close the connection
      self.__skt.shutdown(socket.SHUT_RDWR)
      self.__skt.close()

      return response

  # -----------------------------------------------------------------
  # __sendBtn()
  #
  # Callback method for 'Send' button activation
  #
  def __sendBtn(self) :

      # Choose command based on read-not-write state
      if self.__transRnw.get() :
        cmd = 'm'
      else :
        cmd = 'M'
        # On writes, also fetch the data
        data = self.__transData.get()

      # Calculate the address and data width part of the commands
      addr         = self.__transAddr.get()
      datawidth    = str(self.__dataWidth.get())

      # Construct the main part of the command message
      msg          = cmd + addr + ',' + datawidth

      # On writes, append the write data to the message
      if not self.__transRnw.get() :
        msg        += ':' + data

      # Calculate the padding between command and response for alignment
      padding = ''
      if len(msg) < self.__DEFAULT_PADDING :
        padding = ' ' * (self.__DEFAULT_PADDING - len(msg))

      # Update the text window with the command being sent
      self.__opToTextWin(self.__txt, 'SENT: $' + msg + '#' + self.__chksum(msg))

      # Send the message and get the returned response
      response     = self.__sendmsg(msg, self.__skt)

      # If a read, process the response value and update the data window
      if self.__transRnw.get() :
        rdata = self.__procReadResp(response, datawidth)

      # Send the response to the text window
      self.__opToTextWin(self.__txt, padding + 'RESPONSE: ' + response +  '\n')

  # -----------------------------------------------------------------
  # __scriptBrowseBtn()
  #
  # Callback method on pressing of the 'Browse' button
  #
  def __scriptBrowseBtn(self) :

    # Open a file select dialog box
    fname = askopenfilename(filetypes = (('Script files','*.txt'), ('all files','*.*')))

    # Only update the entry if the returned value not an empty string
    if fname != '' :
      self.__scriptFile.set(os.path.relpath(fname, self.__runDir.get()))

  # -----------------------------------------------------------------
  # __scriptExecBtnThread()
  #
  # Thread method invoked from callback method on 'Execute' button
  # press
  #
  def __scriptExecBtnThread(self) :

    # Open the file
    script = open(self.__scriptFile.get(), 'r')

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

          # Calculate the padding between command and response for alignment
          padding = ''
          if len(msg) < self.__DEFAULT_PADDING :
            padding = ' ' * (self.__DEFAULT_PADDING - len(msg))

          if msg[0] == 'D' :
            self.__disconnectBtn()
          else :
            # Update the text window with the command being sent
            self.__opToTextWin(self.__txt, 'SENT: $' + msg + '#' + self.__chksum(msg))

            # Send and get response
            response     = self.__sendmsg(msg, self.__skt)

            # Send the response to the text window
            self.__opToTextWin(self.__txt, padding + 'RESPONSE: ' + response +  '\n')

    # Close the script file
    script.close()

    # If still in connected state, re-enable the script execution button
    if self.__connStatus.get() == 'Connected' :
      self.__scriptExecBtn.config(state = NORMAL)

  # -----------------------------------------------------------------
  # __scriptExec()
  #
  # Callback method on pressing of the 'Execute' button
  #
  def __scriptExec(self) :

    # Start a new thread to update the text box during script execution
    thread = Thread(target = self.__scriptExecBtnThread)
    thread.start()

    # Disable the script file execution button
    self.__scriptExecBtn.config(state = DISABLED)

  # -----------------------------------------------------------------
  # __rnwUpdated()
  #
  # Callback method on update to the transRnw variable
  #
  def __rnwUpdated(self, object, lstidx, mode) :

    # When read/write radio buttons are for read, disable the
    # data entry box, else enable it
    if self.__transRnw.get() :
      self.__dataEntry.config (state = DISABLED)
    else :
      self.__dataEntry.config (state = NORMAL)

  # -----------------------------------------------------------------
  # __portUpdated()
  #
  # Callback method on update to the portNumber variable
  #
  def __portUpdated(self, object, lstidx, mode) :
    self.__portNumberHex.set(hex(int(self.__portNumber.get())))

  # -----------------------------------------------------------------
  # __genconnfrm()
  #
  # Method to generate the connection widgets in a new frame
  #
  def __genconnfrm(self, master, row=0, col=0) :

      # Initialise some row/col indexes for this frame
      rowidx = 0
      colidx = 0

      # Create a new lable frame to hold connection widgets
      frm = LabelFrame(master=master, text='Socket connection')
      frm.grid(row=row, column=col, padx=10, pady=10, sticky=W)

      # Label and entry box for hostname or TBP/IP address on first row
      hdl = Label(frm, text='IP addr/hostname')
      hdl.grid(row=rowidx, column=colidx, padx=5, pady=5, sticky=E)

      colidx += 1
      self.__hostnameEntry = Entry(frm, textvariable=self.__hostName, width=20)
      self.__hostnameEntry.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      # Label and entry box for port number on a new row
      colidx = 0
      rowidx+=1
      hdl = Label(frm, text='Port Number')
      hdl.grid(row=rowidx, column=colidx, padx=5, pady=5, sticky=E)

      colidx += 1
      self.__portNumEntry = Entry(frm, textvariable=self.__portNumber, width=20)
      self.__portNumEntry.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      # Label for a hex representation of the selected port number in next column
      colidx +=1
      hdl = Label(frm, textvariable=self.__portNumberHex, width=20)
      hdl.grid(row=rowidx, column=colidx, padx=5, pady=5, sticky=W)
      colidx +=1

      # In a new row, add a connect button
      colidx = 0
      rowidx += 1
      self.__connBtnEntry = Button(frm, text='Connect', command=self.__connectBtn, width = 15)
      self.__connBtnEntry.grid(row=rowidx, column=colidx, padx=10, pady=5)

      # In the next column add a disconnect button
      colidx +=1
      self.__disconnBtnEntry = Button(frm, text='Disconnect', command=self.__disconnectBtn, width=15)
      self.__disconnBtnEntry.grid(row=rowidx, column=colidx, padx=10, pady=10)
      self.__disconnBtnEntry.config(state = DISABLED)

      # In the next column add a label to display the socket connection status
      colidx +=1
      self.__connStatusLbl = Label(frm, textvariable=self.__connStatus, width=20, anchor=CENTER)
      self.__connStatusLbl.grid(row=rowidx, column=colidx, padx=5, pady=5, sticky=W)
      self.__connStatusLbl.config(background ='#ff4040', foreground='white', font='Arial')

  # -----------------------------------------------------------------
  # __gentransfrm()
  #
  # Method to generate the transaction widgets in a new frame
  #
  def __gentransfrm(self, master, row=0, col=0) :

      # Initialise some row/col indexes for this frame
      rowidx = 0
      colidx = 0

      # Create a new label frame for the transaction widgets, spanning
      # two columns of the parent frame
      frm = LabelFrame(master=master, text='Transaction generation')
      frm.grid(row=row, column=col, padx=10, pady=10, columnspan=2, sticky=W+E)

      # Add a label and entry box for the address
      hdl = Label(frm, text='Address (hex)')
      hdl.grid(row=rowidx, column=colidx, padx=5, pady=5, sticky=E)

      colidx += 1
      hdl = Entry(frm, textvariable=self.__transAddr, width=20)
      hdl.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      # In the next column, add a radio button for read
      colidx += 1
      hdl = Radiobutton(master = frm, text = 'read', variable = self.__transRnw, value = True)
      hdl.grid(row=rowidx, column=colidx, padx=30, pady=5, sticky=W)

      # In the next two columns add radio buttons to select between 64 and 32 bit wide address
      # The assigned value is the number of bytes value.
      colidx += 1
      hdl = Radiobutton(master = frm, text = '64', variable = self.__addrWidth, value = '8')
      hdl.config(state = DISABLED)
      hdl.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      colidx += 1
      hdl = Radiobutton(master = frm, text = '32', variable = self.__addrWidth, value = '4')
      hdl.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      # In a new row, add a label and entry box for the data
      colidx = 0
      rowidx+=1
      hdl = Label(frm, text='Data (hex)')
      hdl.grid(row=rowidx, column=colidx, padx=5, pady=5, sticky=E)

      colidx += 1
      self.__dataEntry = Entry(frm, textvariable=self.__transData, width=20)
      self.__dataEntry.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      # In the next four columns, add radio buttons to select between 16, 32, 16, or 8
      # bit wide data accesses. The values are in bytes
      colidx += 1
      hdl = Radiobutton(master = frm, text = 'write', variable = self.__transRnw, value = False)
      hdl.grid(row=rowidx, column=colidx, padx=30, pady=5, sticky=W)

      colidx += 1
      hdl = Radiobutton(master = frm, text = '64', variable = self.__dataWidth, value = '8')
      hdl.config(state = DISABLED)
      hdl.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      colidx += 1
      hdl = Radiobutton(master = frm, text = '32', variable = self.__dataWidth, value = '4')
      hdl.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      colidx +=1
      hdl = Radiobutton(master = frm, text = '16', variable = self.__dataWidth, value = '2')
      hdl.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      colidx += 1
      hdl = Radiobutton(master = frm, text = '8', variable = self.__dataWidth, value = '1')
      hdl.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      # Remember the number of columns for the last row
      numcols = colidx + 1

      # In a new row, add a 'Send' button, spanning all the columns
      colidx  = 0
      rowidx += 1
      self.__sndBtn = Button(frm, text='Send', command=self.__sendBtn, width=15)
      self.__sndBtn.config(state = DISABLED)
      self.__sndBtn.grid(row=rowidx, column=colidx, padx=10, pady=10, columnspan=numcols)

  # -----------------------------------------------------------------
  # __genscriptfrm()
  #
  # Method to generate the script section widgets in a new frame
  #
  def __genscriptfrm(self, master, row=0, col=0) :

      # Initialise some row/col indexes for this frame
      rowidx = 0
      colidx = 0

      # Create a new label frame for the scipt widgets, spanning
      # two columns of the parent frame
      frm = LabelFrame(master=master, text='Script File')
      frm.grid(row=row, column=col, padx=10, pady=10, columnspan=2, sticky=W+E)

      # Add a label and entry box for the script file
      hdl = Label(frm, text='Script file')
      hdl.grid(row=rowidx, column=colidx, padx=5, pady=5, sticky=E)

      colidx += 1
      hdl = Entry(frm, textvariable=self.__scriptFile, width=60)
      hdl.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      # In the next column, add a 'Browse' button
      colidx += 1
      hdl = Button(frm, text='Browse', command=self.__scriptBrowseBtn, width=15)
      hdl.grid(row=rowidx, column=colidx, padx=10, pady=10)

      # Remember the number of columns for the last row
      numcols = colidx + 1

      # In a new row, add an 'Execute' button, spanning all the columns
      colidx = 0
      rowidx += 1
      self.__scriptExecBtn = Button(frm, text='Execute', command=self.__scriptExec, width=15)
      self.__scriptExecBtn.config(state = DISABLED)
      self.__scriptExecBtn.grid(row=rowidx, column=colidx, padx=10, pady=10, columnspan=numcols)
      colidx +=1

  # -----------------------------------------------------------------
  # run()
  #
  # Top level public calling method to activate the GUI
  #
  def run(self) :

    # Initialise some row/col indexes for the top level frame
    rowidx = 0;
    colidx = 0;

    # Create a frame using the grid method.
    frm = Frame(master=self.__root)
    frm.grid(row=0, padx=10, pady=10)

    # Add a frame popluated by connection widgets
    self.__genconnfrm(master=frm, row=rowidx, col=colidx)

    # Add an image in the next column
    colidx += 1
    path = os.path.dirname(os.path.realpath(sys.argv[0]))
    img = PhotoImage(file = path + '/' + 'icon.png')
    panel = Label(master = frm, image = img)
    panel.grid(row = rowidx, column = colidx, pady = 10, padx = 5, sticky = W+E)

    # In a new row, add a frame populated with transaction widgets.
    # Spans two columns
    rowidx += 1
    colidx = 0
    self.__gentransfrm(master=frm, row=rowidx, col=colidx)

    # In a new row, add a frame populated with script widgets.
    # Spans two columns
    rowidx += 1
    colidx = 0
    self.__genscriptfrm(master=frm, row=rowidx, col=colidx)

    # In a new row, add a text box for logging data.
    # Spans two columns
    rowidx += 1
    colidx = 0
    txtBoxWidth = self.__DEFAULT_TXT_WIDTH
    if self.__isLinux :
      txtBoxWidth += self.__TXT_LINUX_PADDING
    self.__txt = Text(frm, height=20, width=txtBoxWidth);
    self.__txt.config(state = DISABLED)
    self.__txt.grid(row=rowidx, column=colidx, columnspan=2, padx=10, pady=10)
    rowidx += 1

    mainloop()

  # -----------------------------------------------------------------
  # runBatch()
  #
  # Top level public calling method to activate a batch run
  #
  def runBatch(self, portNum, script) :

    self.__txt = None
    self.__batchMode = True
    self.__portNumber.set(str(portNum))
    self.__scriptFile.set(script)
    self.__connectBtn();
    self.__scriptExecBtnThread()
    self.__disconnectBtn()

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
      parser.add_argument('-b', '--batch', dest='batch', default=False, action='store_true',
                          help='Run in batch mode')
      parser.add_argument('-s', '--script', dest='script', default='sktscript.txt', action='store',
                          help='Specify a script to run in batch mode')
      parser.add_argument('-w', '--wait', dest='wait', default='1', action='store',
                          help='Specify wait period (secs) before running batch script')

      return parser.parse_args()

# ###############################################################
# Only run if not imported
#
if __name__ == '__main__' :

  gui = client_gui()

  # Process the command line options
  cmdArgs = gui.processCmdLine()

  if cmdArgs.batch :
    time.sleep(int(cmdArgs.wait))
    gui.runBatch(cmdArgs.portNum, cmdArgs.script)
  else :
    gui.run()