#!/usr/bin/env python
# -*- coding: utf-8 -*-
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *

import yaml

from mainwindow import *

dImodel = QStandardItemModel (0, 3)
dQmodel = QStandardItemModel (0, 3)
aImodel = QStandardItemModel (0, 5)
aQmodel = QStandardItemModel (0, 5)
mRmodel = QStandardItemModel (0, 3)
mVmodel = QStandardItemModel (0, 3)
Tmodel = QStandardItemModel (0, 4)
Smodel = QStandardItemModel (0, 4)

commands = ['NONE',
    'START',
    'STOP',
    'HELP',
    'FORCE',
    'UNFORCE',
    'EDIT',
    'LOAD',
    'SAVE',
    'QUIT']

def on_edited_di(item):
    #labels = ['INDEX', 'ID', 'VALUE']

    r = item.index().row()
    c = item.index().column()
    
    v = item.text()
    if(c == 1):
        comm = {'COMMAND': 6, 'DI': [{ 'INDEX' : r, 'ID' : v }]}
        print(yaml.dump(comm))
    bv = 0
    if(c == 2):
        if(v==''):
            co = 'UNFORCE'

        else:
            co = 'FORCE'
            if(int(v)>0):
                bv = 1
          
        comm = {'COMMAND': 4, 'DI': [{ 'INDEX' : r, co : bv }]}
        print(yaml.dump(comm))    
        
def on_edited_dq(item):
    #labels = ['INDEX', 'ID', 'VALUE']

    r = item.index().row()
    c = item.index().column()
    
    v = item.text()
    if(c == 1):
        comm = {'COMMAND': 6, 'DQ': [{ 'INDEX' : r, 'ID' : v }]}
        print(yaml.dump(comm))
    bv = 0
    if(c == 2):
        if(v==''):
            co = 'UNFORCE'

        else:
            co = 'FORCE'
            if(int(v)>0):
                bv = 1
          
        comm = {'COMMAND': 4, 'DQ': [{ 'INDEX' : r, co : bv }]}
        print(yaml.dump(comm))            

def on_edited_ai(item):
    #labels = ['INDEX', 'ID', 'VALUE', 'MIN', 'MAX']

    r = item.index().row()
    c = item.index().column()
    
    v = item.text()
    if(c == 1):
        comm = {'COMMAND': 6, 'AI': [{ 'INDEX' : r, 'ID' : v }]}
        print(yaml.dump(comm))

    if(c == 2):
        bv = 0
        if(v==''):
            co = 'UNFORCE'

        else:
            co = 'FORCE'
            bv = float(v)
          
        comm = {'COMMAND': 4, 'AI': [{ 'INDEX' : r, co : bv }]}
        print(yaml.dump(comm))    

    if(c == 3):
        comm = {'COMMAND': 6, 'AI': [{ 'INDEX' : r, 'MIN' : float(v) }]}
        print(yaml.dump(comm))  

    if(c == 4):
        comm = {'COMMAND': 6, 'AI': [{ 'INDEX' : r, 'MAX' : float(v) }]}
        print(yaml.dump(comm))        
        
        
def on_edited_aq(item):
    #labels = ['INDEX', 'ID', 'VALUE', 'MIN', 'MAX']

    r = item.index().row()
    c = item.index().column()
    
    v = item.text()
    if(c == 1):
        comm = {'COMMAND': 6, 'AQ': [{ 'INDEX' : r, 'ID' : v }]}
        print(yaml.dump(comm))

    if(c == 2):
        bv = 0
        if(v==''):
            co = 'UNFORCE'

        else:
            co = 'FORCE'
            bv = float(v)
          
        comm = {'COMMAND': 4, 'AQ': [{ 'INDEX' : r, co : bv }]}
        print(yaml.dump(comm))    

    if(c == 3):
        comm = {'COMMAND': 6, 'AQ': [{ 'INDEX' : r, 'MIN' : float(v) }]}
        print(yaml.dump(comm))  

    if(c == 4):
        comm = {'COMMAND': 6, 'AQ': [{ 'INDEX' : r, 'MAX' : float(v) }]}
        print(yaml.dump(comm))      

def on_edited_t(item):
    #labels = ['INDEX', 'ID', 'VALUE', 'RESOLUTION', 'PRESET']

    r = item.index().row()
    c = item.index().column()
    
    v = item.text()
    if(c == 1):
        comm = {'COMMAND': 6, 'TIMERS': [{ 'INDEX' : r, 'ID' : v }]}
        print(yaml.dump(comm))
    
    if(c == 3):
        comm = {'COMMAND': 6, 'TIMERS': [{ 'INDEX' : r, 'RESOLUTION' : int(v) }]}
        print(yaml.dump(comm))  

    if(c == 4):
        comm = {'COMMAND': 6, 'TIMERS': [{ 'INDEX' : r, 'PRESET' : int(v) }]}
        print(yaml.dump(comm))      

def on_edited_s(item):
    #labels = ['INDEX', 'ID', 'VALUE', 'RESOLUTION']

    r = item.index().row()
    c = item.index().column()
    
    v = item.text()
    if(c == 1):
        comm = {'COMMAND': 6, 'PULSES': [{ 'INDEX' : r, 'ID' : v }]}
        print(yaml.dump(comm))

    if(c == 3):
        comm = {'COMMAND': 6, 'PULSES': [{ 'INDEX' : r, 'RESOLUTION' : int(v) }]}
        print(yaml.dump(comm)) 


def on_edited_mr(item):
    #labels = ['INDEX', 'ID', 'VALUE']

    r = item.index().row()
    c = item.index().column()
    
    v = item.text()
    if(c == 1):
        comm = {'COMMAND': 6, 'MREG': [{ 'INDEX' : r, 'ID' : v }]}
        print(yaml.dump(comm))

    if(c == 2):
        bv = 0
        if(v==''):
            co = 'UNFORCE'

        else:
            co = 'FORCE'
            bv = int(v)
          
        comm = {'COMMAND': 4, 'MREG': [{ 'INDEX' : r, co : bv }]}
        print(yaml.dump(comm))    
        
        
def on_edited_mv(item):
    #labels = ['INDEX', 'ID', 'VALUE']

    r = item.index().row()
    c = item.index().column()
    
    v = item.text()
    if(c == 1):
        comm = {'COMMAND': 6, 'MVAR': [{ 'INDEX' : r, 'ID' : v }]}
        print(yaml.dump(comm))

    if(c == 2):
        bv = 0
        if(v==''):
            co = 'UNFORCE'

        else:
            co = 'FORCE'
            bv = float(v)
          
        comm = {'COMMAND': 4, 'MVAR': [{ 'INDEX' : r, co : bv }]}
        print(yaml.dump(comm))    
        

def on_action_connect():

    alert = QMessageBox()
    alert.setText('wanna connect?')
    alert.exec_()

def on_action_load():
    fileName = QFileDialog.getOpenFileName(None,
    "Open PLC configuration", ".", "YML Files (*.yml)")

    with open(fileName[0], 'r') as stream:
        try:
            data = yaml.safe_load(stream)
            di = data.get('DI')
            dq = data.get('DQ')
            ai = data.get('AI')
            aq = data.get('AQ')
            mr = data.get('MREG')
            mv = data.get('MVAR')
            tt = data.get('TIMERS')
            ps = data.get('PULSES')

            ui.diView.hide()
            ui.dqView.hide()
            ui.aiView.hide()
            ui.aqView.hide()
            ui.mView.hide()
            ui.rView.hide()
            ui.tView.hide()
            ui.sView.hide()
            
            populate_di(di)
            populate_dq(dq)
            populate_ai(ai)
            populate_aq(aq)
            populate_mreg(mr)
            populate_mvar(mv)
            populate_timer(tt)
            populate_pulse(ps)

        except yaml.YAMLError as exc:
            print(exc)

def update_di(data):
    if(data!=None):
        
        for i in data:
            keyItem = QStandardItem("I %d" % i['INDEX'])
            dImodel.setItem(i['INDEX'], 0, keyItem)

            nameItem = QStandardItem(i.get('ID'))
            if(nameItem!=None):
                dImodel.setItem(i['INDEX'], 1, nameItem)
            
            valItem = QStandardItem(i.get('VALUE'))
            if(valItem!=None):
                dImodel.setItem(i['INDEX'], 2, valItem)

def populate_di(data):
    if(data!=None):
        dImodel.clear()
        dImodel.setHorizontalHeaderLabels(["Digital Inputs", "Name", "VALUE"]);
        if(isinstance(data[0],(int))):
            num, *items = data
            for i in range(0,num):
                keyItem = QStandardItem("I %d" % i)
                keyItem.setEditable(False) #FIXME
                dImodel.appendRow([keyItem])
            update_di(items)
        
        ui.diView.setModel(dImodel)
        ui.diView.show()


def update_dq(data):
    if(data!=None):
        
        for i in data:
            keyItem = QStandardItem("Q %d" % i['INDEX'])
            dQmodel.setItem(i['INDEX'], 0, keyItem)

            nameItem = QStandardItem(i.get('ID'))
            if(nameItem!=None):
                dQmodel.setItem(i['INDEX'], 1, nameItem)
            
            valItem = QStandardItem( i.get('VALUE'))
            if(valItem!=None):
                dQmodel.setItem(i['INDEX'], 2, valItem)
            
def populate_dq(data):  
    if(data!=None):
        dQmodel.clear()
        dQmodel.setHorizontalHeaderLabels(["Digital Outputs", "Name", "VALUE"]);
        if(isinstance(data[0],(int))):
            num, *items = data
            for i in range(0,num):
                keyItem = QStandardItem("Q %d" % i)
                keyItem.setEditable(False) #FIXME
                dQmodel.appendRow([keyItem])
            update_dq(items)
        
        ui.dqView.setModel(dQmodel)
        ui.dqView.show()    

def update_ai(data):
    if(data!=None):
        
        for i in data:
            keyItem = QStandardItem("AI %d" % i['INDEX'])
            aImodel.setItem(i['INDEX'], 0, keyItem)
            
            nameItem = QStandardItem(i.get('ID'))
            if(nameItem!=None):
                aImodel.setItem(i['INDEX'], 1, nameItem)
            
            if(i.get('VALUE')!=None):
                valItem = QStandardItem(str(i.get('VALUE')))
                aImodel.setItem(i['INDEX'], 2, valItem)

            if(i.get('MIN')!=None):
                minItem = QStandardItem(str(i['MIN']))
                aImodel.setItem(i['INDEX'], 3, minItem)
            
            if(i.get('MAX')!=None):
                maxItem = QStandardItem(str(i['MAX']))
                aImodel.setItem(i['INDEX'], 4, maxItem)
            
        
def populate_ai(data):
    if(data!=None):
        aImodel.clear()
        aImodel.setHorizontalHeaderLabels(["Analog Inputs", "Name", "VALUE", "MIN", "MAX"]);
        if(isinstance(data[0],(int))):
            num, *items = data
            for i in range(0,num):
                keyItem = QStandardItem("AI %d" % i)
                keyItem.setEditable(False) #FIXME
                aImodel.appendRow([keyItem])
            update_ai(items)
        
        ui.aiView.setModel(aImodel)
        ui.aiView.show()  

def update_aq(data):
    if(data!=None):
        
        for i in data:
            keyItem = QStandardItem("AQ %d" % i['INDEX'])
            aQmodel.setItem(i['INDEX'], 0, keyItem)

            nameItem = QStandardItem(i.get('ID'))
            if(nameItem!=None):
                aQmodel.setItem(i['INDEX'], 1, nameItem)

            if(i.get('VALUE')!=None):
                valItem = QStandardItem(str(i.get('VALUE')))
                aQmodel.setItem(i['INDEX'], 2, valItem)

            if(i.get('MIN')!=None):    
                minItem = QStandardItem(str(i['MIN']))
                aQmodel.setItem(i['INDEX'], 3, minItem)

            if(i.get('MAX')!=None):
                maxItem = QStandardItem(str(i['MAX']))
                aQmodel.setItem(i['INDEX'], 4, maxItem)
            

def populate_aq(data):    
    if(data!=None):
        aQmodel.clear()
        aQmodel.setHorizontalHeaderLabels(["Analog Outputs", "Name", "VALUE", "MIN", "MAX"]);
        if(isinstance(data[0],(int))):
            num, *items = data
            for i in range(0,num):
                keyItem = QStandardItem("AQ %d" % i)
                keyItem.setEditable(False) #FIXME
                aQmodel.appendRow([keyItem])
            update_aq(items)
        
        ui.aqView.setModel(aQmodel)
        ui.aqView.show()   

def update_mreg(data):
    if(data!=None):
        
        for i in data:
            keyItem = QStandardItem("MW %d" % i['INDEX'])
            mRmodel.setItem(i['INDEX'], 0, keyItem)
            
            nameItem = QStandardItem(i.get('ID'))
            if(nameItem!=None):
                mRmodel.setItem(i['INDEX'], 1, nameItem)

            if(i.get('VALUE')!=None):
                valItem = QStandardItem(str(i.get('VALUE')))
                mRmodel.setItem(i['INDEX'], 2, valItem)
    
def populate_mreg(data):    
    if(data!=None):
        mRmodel.clear()
        mRmodel.setHorizontalHeaderLabels(["Memory Registers", "Name", "VALUE"]);
        if(isinstance(data[0],(int))):
            num, *items = data
            for i in range(0,num):
                keyItem = QStandardItem("MW %d" % i)
                keyItem.setEditable(False) #FIXME
                mRmodel.appendRow([keyItem])
            update_mreg(items)
        
        ui.mView.setModel(mRmodel)
        ui.mView.show()      

def update_mvar(data):
    if(data!=None):
        
        for i in data:
            keyItem = QStandardItem("MF %d" % i['INDEX'])
            mVmodel.setItem(i['INDEX'], 0, keyItem)
            
            nameItem = QStandardItem(i.get('ID'))
            if(nameItem!=None):
                mVmodel.setItem(i['INDEX'], 1, nameItem)

            if(i.get('VALUE')!=None):
                valItem = QStandardItem(str(i.get('VALUE')))   
                mVmodel.setItem(i['INDEX'], 2, valItem)
            
def populate_mvar(data):   
    if(data!=None):
        mVmodel.clear()
        mVmodel.setHorizontalHeaderLabels(["Memory Reals", "Name", "VALUE"]);
        if(isinstance(data[0],(int))):
            num, *items = data
            for i in range(0,num):
                keyItem = QStandardItem("MF %d" % i)
                keyItem.setEditable(False) #FIXME
                mVmodel.appendRow([keyItem])
            update_mvar(items)
        
        ui.rView.setModel(mVmodel)
        ui.rView.show()      

def update_timer(data):
    if(data!=None):
        
        for i in data:
            keyItem = QStandardItem("T %d" % i['INDEX'])
            Tmodel.setItem(i['INDEX'], 0, keyItem)
            nameItem = QStandardItem(i.get('ID'))
            if(nameItem!=None):
                Tmodel.setItem(i['INDEX'], 1, nameItem)

            if(i.get('VALUE')!=None):
                valItem = QStandardItem(str(i.get('VALUE')))   
                Tmodel.setItem(i['INDEX'], 2, valItem)

            if(i.get('RESOLUTION')!=None):
                resItem = QStandardItem(str(i['RESOLUTION']))
                Tmodel.setItem(i['INDEX'], 3, resItem)
            
            if(i.get('PRESET')!=None):    
                presItem = QStandardItem(str(i['PRESET']))
                Tmodel.setItem(i['INDEX'], 4, presItem)
            
def populate_timer(data):
    if(data!=None):
        Tmodel.clear()
        Tmodel.setHorizontalHeaderLabels(["Timers", "Name", "VALUE", "RESOLUTION", "PRESET"]);
        if(isinstance(data[0],(int))):
            num, *items = data
            for i in range(0,num):
                keyItem = QStandardItem("T %d" % i)
                keyItem.setEditable(False) #FIXME
                Tmodel.appendRow([keyItem])
            update_timer(items)
        
        ui.tView.setModel(Tmodel)
        ui.tView.show()  

def update_pulse(data):
    if(data!=None):
        
        for i in data:
            keyItem = QStandardItem("S %d" % i['INDEX'])
            Smodel.setItem(i['INDEX'], 0, keyItem)

            nameItem = QStandardItem(i.get('ID'))
            if(nameItem!=None):
                Smodel.setItem(i['INDEX'], 1, nameItem)
            
            if(i.get('VALUE')!=None):
                valItem = QStandardItem(str(i.get('VALUE')))
                Smodel.setItem(i['INDEX'], 2, valItem)

            if(i.get('RESOLUTION')!=None):
                resItem = QStandardItem(str(i['RESOLUTION']))
                Smodel.setItem(i['INDEX'], 3, resItem)

    
def populate_pulse(data):       
    if(data!=None):
        Smodel.clear()
        Smodel.setHorizontalHeaderLabels(["Pulses", "Name", "VALUE", "RESOLUTION"]);
        if(isinstance(data[0],(int))):
            num, *items = data
            for i in range(0,num):
                keyItem = QStandardItem("S %d" % i)
                keyItem.setEditable(False) #FIXME
                Smodel.appendRow([keyItem])
            update_timer(items)
        
        ui.sView.setModel(Smodel)
        ui.sView.show()

if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)

    ui.actionConnect.triggered.connect(on_action_connect)
    ui.actionLoad.triggered.connect(on_action_load)
    dImodel.itemChanged.connect(on_edited_di)
    dQmodel.itemChanged.connect(on_edited_dq)
    aImodel.itemChanged.connect(on_edited_ai)
    aQmodel.itemChanged.connect(on_edited_aq)
    mVmodel.itemChanged.connect(on_edited_mv)
    mRmodel.itemChanged.connect(on_edited_mr)
    Tmodel.itemChanged.connect(on_edited_t)
    Smodel.itemChanged.connect(on_edited_s)

    MainWindow.show()

    sys.exit(app.exec_())



