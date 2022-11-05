from tkinter import *
from tkinter.ttk  import *
from tkinter.filedialog import askopenfilename, askdirectory
from threading import Thread
import socket
import platform
import os
import time

class client_gui :

  # -----------------------------------------------------------------
  # __init__
  #
  # Constructor for client_gui class
  #
  def __init__(self) :

    # Define some 'constants'
    self.CONNECTED_COLOUR    = '#20c060'
    self.DISCONNECTED_COLOUR = '#ff4040'
    self.ERROR_COLOUR        = '#ff0000'
    self.DEFAULT_TXT_WIDTH   = 76
    self.TXT_LINUX_PADDING   = 16

    self.DEFAULT_PADDING     = 36

    # Create a Tk object
    self.root = Tk()
    self.root.title('TCP/IP client GUI. ')

    # Define widget variables
    self.hostName            = StringVar()
    self.hostName.set('localhost')

    self.portNumber          = StringVar()
    self.portNumber.set('49152')
    self.portNumber.trace ('w', self.__portUpdated)

    self.portNumberHex       = StringVar()
    self.portNumberHex.set(hex(int(self.portNumber.get())))

    self.connStatus          = StringVar()
    self.connStatus.set('Not connected')

    self.transAddr           = StringVar()
    self.addrWidth           = IntVar()
    self.addrWidth.set(4)

    self.transData           = StringVar()
    self.dataWidth           = IntVar()
    self.dataWidth.set(4)

    self.transRnw            = IntVar()
    self.transRnw.set(0)
    self.transRnw.trace ('w', self.__rnwUpdated)

    self.scriptFile          = StringVar()
    self.scriptFile.set('sktscript.txt')

    # Create a flag to indicate whether on Linux or not
    self.__isLinux           = platform.system().lower() == 'linux'

    self.runDir              = StringVar()
    self.runDir.set(os.getcwd())

    # Open up a socket
    self.skt = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

  # -----------------------------------------------------------------
  # __opToTextWin()
  #
  # Method to output a string to the text window, with re-enabling
  # and immediate disabling
  #
  @staticmethod
  def __opToTextWin (hdl, str) :
    hdl.config(state = NORMAL)
    hdl.insert(INSERT, str);
    hdl.see("end");
    hdl.config(state = DISABLED)

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
      return "0" + hex(checksum)[2:]
    else :
      return hex(checksum)[2:]

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
    oldstate = self.dataEntry['state']
    self.dataEntry.config(state = NORMAL)
    self.dataEntry.delete(0, END)
    self.dataEntry.insert(0, hexstr)
    self.dataEntry.config(state = oldstate)

    # Return number as an integer value
    return int('0x' + hexstr, 16)


  # -----------------------------------------------------------------
  #  __connectBtn()
  #
  #  Callback method for 'Connect' button activation
  #
  def __connectBtn(self) :
    if self.connStatus.get() != 'Connected' :
      try :
        # Open connection with configuired host TCP/IP address or name, and port number
        self.skt.connect((self.hostName.get(), int(self.portNumber.get())))

        # Update widget states on successful connection
        self.connStatus.set('Connected')
        self.__opToTextWin(self.__txt, "Connected to port " + self.portNumber.get() + "\n")
        self.execBtn.config(state = NORMAL)
        self.sndBtn.config(state = NORMAL)
        self.__connStatusLbl.config(background=self.CONNECTED_COLOUR)
        self.__portNumEntry.config(state = DISABLED)

      # Update widget states on unsuccessful connection
      except:
        self.connStatus.set('Connection Error')
        self.__connStatusLbl.config(background=self.ERROR_COLOUR)
        self.__portNumEntry.config(state = NORMAL)

  # -----------------------------------------------------------------
  # __disconnectBtn()
  #
  # Callback method for 'Disconnect' button activation
  #
  def __disconnectBtn(self) :

    # If actually connected (else ignore)...
    if self.connStatus.get() == 'Connected' :

      # Update widget state when disconnected
      self.connStatus.set('Not Connected')
      self.__opToTextWin(self.__txt, "Disconnected from port " + self.portNumber.get() + "\n")
      self.execBtn.config(state = DISABLED)
      self.sndBtn.config(state = DISABLED)
      self.__portNumEntry.config(state = NORMAL)
      self.__connStatusLbl.config(background=self.DISCONNECTED_COLOUR)
      response = self.__sendmsg('D', self.skt)

      return response

  # -----------------------------------------------------------------
  # __sendBtn()
  #
  # Callback method for 'Send' button activation
  #
  def __sendBtn(self) :

      # Choose command based on read-not-write state
      if self.transRnw.get() :
        cmd = 'm'
      else :
        cmd = 'M'
        # On writes, also fetch the data
        data = self.transData.get()

      # Calculate the address and data width part of the commands
      addr         = self.transAddr.get()
      datawidth    = str(self.dataWidth.get())

      # Construct the main part of the command message
      msg          = cmd + addr + ',' + datawidth

      # On writes, append the write data to the message
      if not self.transRnw.get() :
        msg        += ':' + data

      # Calculate the padding between command and response for alignment
      padding = ''
      if len(msg) < self.DEFAULT_PADDING :
        padding = ' ' * (self.DEFAULT_PADDING - len(msg))

      # Update the text window with the command being sent
      self.__opToTextWin(self.__txt, "SENT: $" + msg + "#" + self.__chksum(msg))

      # Send the message and get the returned response
      response     = self.__sendmsg(msg, self.skt)

      # If a read, process the response value and update the data window
      if self.transRnw.get() :
        rdata = self.__procReadResp(response, datawidth)
        #print(rdata, hex(rdata))

      # Send the response to the text window
      self.__opToTextWin(self.__txt, padding + "RESPONSE: " + response +  "\n")

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
      self.scriptFile.set(os.path.relpath(fname, self.runDir.get()))

  # -----------------------------------------------------------------
  # __scriptExecBtnThread()
  #
  # Thread method invoked from callback method on 'Execute' button
  # press
  #
  def __scriptExecBtnThread(self) :

    # Open the file
    script = open(self.scriptFile.get(), 'r')

    # for all readlines
    for line in script :

      #print(line)

      # Remove leading and trailing whitespace
      line.strip()

      # Process the line if not a comment
      if line[0] != '#' :

        # Check a valid command
        if line[0] in "MmDk" :

          # Strip newlines
          msg = line.rstrip()

          # Calculate the padding between command and response for alignment
          padding = ''
          if len(msg) < self.DEFAULT_PADDING :
            padding = ' ' * (self.DEFAULT_PADDING - len(msg))

          if msg[0] == 'D' :
            self.__disconnectBtn()
          else :
            # Update the text window with the command being sent
            self.__opToTextWin(self.__txt, "SENT: $" + msg + "#" + self.__chksum(msg))

            # Send and get response
            response     = self.__sendmsg(msg, self.skt)

            # If a read, process the response value and update the data window
            if self.transRnw.get() :
              rdata = self.__procReadResp(response, datawidth)

            # Send the response to the text window
            self.__opToTextWin(self.__txt, padding + "RESPONSE: " + response +  "\n")

          #time.sleep(1)

    script.close()
    
    if self.connStatus.get() == 'Connected' :
      self.execBtn.config(state = NORMAL)

  # -----------------------------------------------------------------
  # __scriptExecBtn()
  #
  # Callback method on pressing of the 'Execute' button
  #
  def __scriptExecBtn(self) :

    thread = Thread(target = self.__scriptExecBtnThread)
    thread.start()

    self.execBtn.config(state = DISABLED)

  # -----------------------------------------------------------------
  # __rnwUpdated()
  #
  # Callback method on update to the transRnw variable
  #
  def __rnwUpdated(self, object, lstidx, mode) :

    # When read/write radio buttons are for read, disable the
    # data entry box, else enable it
    if self.transRnw.get() :
      self.dataEntry.config (state = DISABLED)
    else :
      self.dataEntry.config (state = NORMAL)
  
  # -----------------------------------------------------------------
  # __portUpdated()
  #
  # Callback method on update to the portNumber variable
  #  
  def __portUpdated(self, object, lstidx, mode) :
    self.portNumberHex.set(hex(int(self.portNumber.get())))
    

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
      hdl = Label(frm, text="IP addr/hostname")
      hdl.grid(row=rowidx, column=colidx, padx=5, pady=5, sticky=E)
      colidx += 1

      hdl = Entry(frm, textvariable=self.hostName, width=20)
      hdl.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      # Label and entry box for port number on a new row
      colidx = 0
      rowidx+=1
      hdl = Label(frm, text="Port Number")
      hdl.grid(row=rowidx, column=colidx, padx=5, pady=5, sticky=E)

      colidx += 1
      self.__portNumEntry = Entry(frm, textvariable=self.portNumber, width=20)
      self.__portNumEntry.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      # Label for a hex representation of the selected port number in next column
      colidx +=1
      hdl = Label(frm, textvariable=self.portNumberHex, width=20)
      hdl.grid(row=rowidx, column=colidx, padx=5, pady=5, sticky=W)
      colidx +=1

      # In a new row, add a connect button
      colidx = 0
      rowidx += 1
      hdl = Button(frm, text='Connect', command=self.__connectBtn, width = 15)
      hdl.grid(row=rowidx, column=colidx, padx=10, pady=5)

      # In the next column add a disconnect button
      colidx +=1
      hdl = Button(frm, text='Disconnect', command=self.__disconnectBtn, width=15)
      hdl.grid(row=rowidx, column=colidx, padx=10, pady=10)

      # In the next column add a label to display the socket connection status
      colidx +=1
      self.__connStatusLbl = Label(frm, textvariable=self.connStatus, width=20, anchor=CENTER)
      self.__connStatusLbl.grid(row=rowidx, column=colidx, padx=5, pady=5, sticky=W)
      self.__connStatusLbl.config(background ='#ff4040', foreground='white', font='Arial')
      colidx +=1

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
      hdl = Label(frm, text="Address (hex)")
      hdl.grid(row=rowidx, column=colidx, padx=5, pady=5, sticky=E)

      colidx += 1
      hdl = Entry(frm, textvariable=self.transAddr, width=20)
      hdl.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      # In the next column, add a radio button for read
      colidx += 1
      hdl = Radiobutton(master = frm, text = 'read', variable = self.transRnw, value = 1)
      hdl.grid(row=rowidx, column=colidx, padx=30, pady=5, sticky=W)

      # In the next two columns add radio buttons to select between 64 and 32 bit wide address
      # The assigned value is the number of bytes value.
      colidx += 1
      hdl = Radiobutton(master = frm, text = '64', variable = self.addrWidth, value = '8')
      hdl.config(state = DISABLED)
      hdl.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      colidx += 1
      hdl = Radiobutton(master = frm, text = '32', variable = self.addrWidth, value = '4')
      hdl.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      # In a new row, add a label and entry box for the data
      colidx = 0
      rowidx+=1
      hdl = Label(frm, text="Data (hex)")
      hdl.grid(row=rowidx, column=colidx, padx=5, pady=5, sticky=E)

      colidx += 1
      self.dataEntry = Entry(frm, textvariable=self.transData, width=20)
      self.dataEntry.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      # In the next four columns, add radio buttons to select between 16, 32, 16, or 8
      # bit wide data accesses. The values are in bytes
      colidx += 1
      hdl = Radiobutton(master = frm, text = 'write', variable = self.transRnw, value = 0)
      hdl.grid(row=rowidx, column=colidx, padx=30, pady=5, sticky=W)

      colidx += 1
      hdl = Radiobutton(master = frm, text = '64', variable = self.dataWidth, value = '8')
      hdl.config(state = DISABLED)
      hdl.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      colidx += 1
      hdl = Radiobutton(master = frm, text = '32', variable = self.dataWidth, value = '4')
      hdl.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      colidx +=1
      hdl = Radiobutton(master = frm, text = '16', variable = self.dataWidth, value = '2')
      hdl.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      colidx += 1
      hdl = Radiobutton(master = frm, text = '8', variable = self.dataWidth, value = '1')
      hdl.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      # Remember the number of columns for the last row
      numcols = colidx + 1

      # In a new row, add a 'Send' button, spanning all the columns
      colidx = 0
      rowidx+=1
      self.sndBtn = Button(frm, text='Send', command=self.__sendBtn, width=15)
      self.sndBtn.config(state = DISABLED)
      self.sndBtn.grid(row=rowidx, column=colidx, padx=10, pady=10, columnspan=numcols)

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
      hdl = Label(frm, text="Script file")
      hdl.grid(row=rowidx, column=colidx, padx=5, pady=5, sticky=E)

      colidx += 1
      self.scriptFileEntry = Entry(frm, textvariable=self.scriptFile, width=60)
      self.scriptFileEntry.grid(row=rowidx, column=colidx, padx=10, pady=5, sticky=W)

      # In the next column, add a 'Browse' button
      colidx += 1
      hdl = Button(frm, text='Browse', command=self.__scriptBrowseBtn, width=15)
      hdl.grid(row=rowidx, column=colidx, padx=10, pady=10)

      # Remember the number of columns for the last row
      numcols = colidx + 1

      # In a new row, add an 'Execute' button, spanning all the columns
      colidx = 0
      rowidx += 1
      self.execBtn = Button(frm, text='Execute', command=self.__scriptExecBtn, width=15)
      self.execBtn.config(state = DISABLED)
      self.execBtn.grid(row=rowidx, column=colidx, padx=10, pady=10, columnspan=numcols)
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
    frm = Frame(master=self.root)
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
    txtBoxWidth = self.DEFAULT_TXT_WIDTH
    if self.__isLinux :
      txtBoxWidth += self.TXT_LINUX_PADDING
    self.__txt = Text(frm, height=20, width=txtBoxWidth);
    self.__txt.config(state = DISABLED)
    self.__txt.grid(row=rowidx, column=colidx, columnspan=2, padx=10, pady=10)
    rowidx += 1

    mainloop()

# ###############################################################
# Only run if not imported
#
if __name__ == '__main__' :

  gui = client_gui()
  gui.run()