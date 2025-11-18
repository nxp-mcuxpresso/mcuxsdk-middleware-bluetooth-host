# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------


#  Content   : PopUpDialog provides easy to use functions for basic user
#              interaction. (e.g. yes/no, file selection, radio-button selection)
import tkinter as Tkinter
from tkinter import messagebox as tkMessageBox
from tkinter import filedialog as tkFileDialog
import configparser as ConfigParser
import os
import logging
import sys


class PopUpDialog(object):

    def __init__(self):
        self.logger = logging.getLogger(__name__)
        self.config = ConfigParser.RawConfigParser()

    def ok(self, message="Ready?"):
        self.logger.info("popupdialog:ok:%s", message)
        root = Tkinter.Tk()
        self.set_top_most(root)
        root.withdraw()
        tkMessageBox.showinfo("popupdialog", message)
        root.destroy()

    def askIfTrueCfg(self, cfgItem, cfgName='popupdialog.cfg', cfgSection='askIfTrueCfg'):
        if not self.__has_config_item(cfgItem, cfgName, cfgSection):
            cfgValue = self.askIfTrue("Is %s True?" % cfgItem)
            self.__create_config_item(cfgItem, cfgName, cfgSection, cfgValue)
        return (self.__get_config_item(cfgItem, cfgName, cfgSection) == 'True')

    def askIfTrue(self, question="Okay?"):
        root = Tkinter.Tk()
        self.set_top_most(root)
        root.withdraw()
        answer = tkMessageBox.askquestion("popupdialog", question)
        self.logger.info("askIfTrue:%s:%s", question, answer)
        root.destroy()
        retval = False
        if answer=="yes":
            retval = True
        return retval

    def askYesNo(self, question="Okay?"):
        root = Tkinter.Tk()
        root.withdraw()
        self.set_top_most(root)
        answer = tkMessageBox.askyesno("popupdialog", question)
        self.logger.info("askYesNo:%s:%s" % (question, answer))
        root.destroy()
        return answer

    def selectDirectoryCfg(self, cfgItem, cfgName='popupdialog.cfg', cfgSection='selectDirectoryCfg', description="Please select a directory", startDir="."):
        if not self.__has_config_item(cfgItem, cfgName, cfgSection):
            cfgValue = self.selectDirectory(description=description, startDir=startDir)
            self.__create_config_item(cfgItem, cfgName, cfgSection, cfgValue)
        return self.__get_config_item(cfgItem, cfgName, cfgSection)

    def selectDirectory(self, description="Please select a directory", startDir="."):
        root = Tkinter.Tk()
        self.set_top_most(root)
        root.withdraw()
        retval = tkFileDialog.askdirectory(initialdir=startDir,title=description)
        self.logger.info("selectDirectory:%s:%s", description, retval)
        root.destroy()
        return retval

    def selectFileCfg(self, cfgItem, cfgName='popupdialog.cfg', cfgSection='selectFileCfg', description="Please select a file", startDir=".", filetypes=[("All files","*.*")]):
        if not self.__has_config_item(cfgItem, cfgName, cfgSection):
            cfgValue = self.selectFile(description=description, startDir=startDir, filetypes=filetypes)
            self.__create_config_item(cfgItem, cfgName, cfgSection, cfgValue)
        return self.__get_config_item(cfgItem, cfgName, cfgSection)

    def selectFile(self, description="Please select a file", startDir=".", filetypes=[("All files","*.*")]):
        root = Tkinter.Tk()
        self.set_top_most(root)
        root.withdraw()
        retval = tkFileDialog.askopenfilename(initialdir=startDir,title=description, filetypes=filetypes)
        self.logger.info("selectFile:%s:%s", description, retval)
        root.destroy()
        return retval

    def radioSelectCfg(self, cfgItem, cfgName='popupdialog.cfg', cfgSection='radioSelectCfg',
                       description="Please make a choice", choices=("first","second")):
        if not self.__has_config_item(cfgItem, cfgName, cfgSection):
            cfgValue = self.radioSelect(description=description, choices=choices)
            self.__create_config_item(cfgItem, cfgName, cfgSection, cfgValue)
        return self.__get_config_item(cfgItem, cfgName, cfgSection)

    def radioSelect(self, description="Please make a choice", choices=("first", "second")):
        assert(len(choices) >= 2), "Minimally 2 choices expected"
        self.retval = ""
        root = Tkinter.Tk()
        self.set_top_most(root)
        root.title("popupdialog")
        frame = Tkinter.Frame(root)
        frame.pack()
        label = Tkinter.Label(frame, text=description)
        label.grid(row=0,column=0)
        selectVar = Tkinter.StringVar()
        rblist = [None] * len(choices)
        for i, rb in enumerate(rblist):
            rb = Tkinter.Radiobutton(root, text=choices[i], variable=selectVar, value=choices[i])
            rb.pack(anchor=Tkinter.W)
        button = Tkinter.Button(root, text="Select", command=lambda: self.__buttonRadioSelectClicked(root, selectVar))
        button.pack()
        self.__center_window(root)
        selectVar.set(choices[0])
        root.mainloop()
        self.logger.info("radioSelect:%s:%s", description, self.retval)
        return self.retval

    def userInputCfg(self, cfgItem, cfgName='popupdialog.cfg', cfgSection='userInputCfg', description=None):
        if not self.__has_config_item(cfgItem, cfgName, cfgSection):
            if description is None:
                description = "Please enter " + str(cfgItem)
            cfgValue = self.userInput(description)
            self.__create_config_item(cfgItem, cfgName, cfgSection, cfgValue)
        return self.__get_config_item(cfgItem, cfgName, cfgSection)

    def userInput(self, description="Value"):
        self.retval = ""
        root = Tkinter.Tk()
        self.set_top_most(root)
        root.title("popupdialog")
        frame = Tkinter.Frame(root)
        frame.pack()
        label = Tkinter.Label(frame, text=description)
        label.grid(row=0,column=0)
        entry = Tkinter.Entry(frame)
        entry.delete(0,Tkinter.END)
        entry.grid(row=1,column=0)
        button = Tkinter.Button(frame, text="Enter", command=lambda: self.__buttonUserInputClicked(root, entry))
        button.grid(row=1,column=1)
        self.__center_window(root)
        root.mainloop()
        self.logger.info("userInput:%s:%s", description, self.retval)
        return self.retval

    def __has_config_item(self, cfgItem, cfgName, cfgSection):
        retval = False
        if os.path.isfile(cfgName):
            self.config.read(cfgName)
            try:
                hasCfgItem = self.config.has_option(cfgSection, cfgItem)
                if hasCfgItem:
                    retval=True
            except:
                pass
        return retval

    def __create_config_item(self, cfgItem, cfgName, cfgSection, cfgValue):
        retval = True
        try:
            if not self.config.has_section(cfgSection):
                self.config.add_section(cfgSection)
            self.config.set(cfgSection, cfgItem, cfgValue)
            with open(cfgName, 'wb') as configfile:
                self.config.write(configfile)
        except:
            self.logger.error('create_config_item failed')
            retval = False
        return retval

    def __get_config_item(self, cfgItem, cfgName, cfgSection):
        retval = ''
        if os.path.isfile(cfgName):
            self.config.read(cfgName)
            try:
                hasCfgItem = self.config.has_option(cfgSection, cfgItem)
                if hasCfgItem:
                    retval=self.config.get(cfgSection, cfgItem)
            except:
                pass
        return retval

    def __buttonRadioSelectClicked(self, root, selectVar):
        self.retval = selectVar.get()
        root.destroy()

    def __buttonUserInputClicked(self, root, entry):
        self.retval = entry.get()
        root.destroy()

    def __center_window(self, root):
        # get screen width and height
        ws = root.winfo_screenwidth()
        hs = root.winfo_screenheight()
        # get windows current width and height
        root.update_idletasks()
        w = root.winfo_reqwidth()
        h = root.winfo_reqheight()
        # calculate position x, y
        x = (ws/2) - (w/2)
        y = (hs/2) - (h/2)
        root.geometry('%dx%d+%d+%d' % (w, h, x, y))     

    @staticmethod
    def set_top_most(root):
        root.lift()
        if sys.platform == 'win32':
            root.wm_attributes('-topmost',1)

if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG)
    dialog = PopUpDialog()
    itHappenedBool = dialog.askIfTrue("Did it really happen?")
    dialog.ok("Continue after this or that...")
    musicDir = dialog.selectDirectory(description="Find me the directory which has music in it.", startDir="/")
    lastName = dialog.userInput("Enter your last name:")
    colourOfChoice = dialog.radioSelect(description="Pick your least favorite colour", choices=("red", "blue", "black", "yellow"))
