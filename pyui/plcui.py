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

def on_edited_di(item):
    
    r = item.index().row()
    c = item.index().column()
    v = item.text()

    print(r,c,v)

def on_action_connect():

    alert = QMessageBox()
    alert.setText('wanna connect?')
    alert.exec_()


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

#

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

    with open("program.yml", 'r') as stream:
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

            #print(ai)
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

    ui.actionConnect.triggered.connect(on_action_connect)
    dImodel.itemChanged.connect(on_edited_di)

    MainWindow.show()

    sys.exit(app.exec_())



