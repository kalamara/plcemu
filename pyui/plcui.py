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

def on_action_connect():

    alert = QMessageBox()
    alert.setText('wanna connect?')
    alert.exec_()

def populate_di(data):
    if(data!=None):
        dImodel.setHorizontalHeaderLabels(["Digital Inputs", "Name", "Value"]);
        for i in data:
            keyItem = QStandardItem("I %d" % i['Index'])
            dImodel.setItem(i['Index'], 0, keyItem)

            nameItem = QStandardItem(i.get('Identifier'))
            if(nameItem!=None):
                dImodel.setItem(i['Index'], 1, nameItem)
            
            valItem = QStandardItem(i.get('Value'))
            if(valItem!=None):
                dImodel.setItem(i['Index'], 2, valItem)
            
        ui.diView.setModel(dImodel)
        ui.diView.show()

def populate_dq(data):
    if(data!=None):
        dQmodel.setHorizontalHeaderLabels(["Digital Outputs", "Name", "Value"]);
        for i in data:
            keyItem = QStandardItem("Q %d" % i['Index'])
            dQmodel.setItem(i['Index'], 0, keyItem)

            nameItem = QStandardItem(i.get('Identifier'))
            if(nameItem!=None):
                dQmodel.setItem(i['Index'], 1, nameItem)
            
            valItem = QStandardItem( i.get('Value'))
            if(valItem!=None):
                dQmodel.setItem(i['Index'], 2, valItem)
            
        ui.dqView.setModel(dQmodel)
        ui.dqView.show()

def populate_ai(data):
    if(data!=None):
        aImodel.setHorizontalHeaderLabels(["Analog Inputs", "Name", "Value", "Min", "Max"]);
        for i in data:
            keyItem = QStandardItem("AI %d" % i['Index'])
            aImodel.setItem(i['Index'], 0, keyItem)
            
            nameItem = QStandardItem(i.get('Identifier'))
            if(nameItem!=None):
                aImodel.setItem(i['Index'], 1, nameItem)
            
            if(i.get('Value')!=None):
                valItem = QStandardItem(str(i.get('Value')))
                aImodel.setItem(i['Index'], 2, valItem)

            if(i.get('Min')!=None):
                minItem = QStandardItem(str(i['Min']))
                aImodel.setItem(i['Index'], 3, minItem)
            
            if(i.get('Max')!=None):
                maxItem = QStandardItem(str(i['Max']))
                aImodel.setItem(i['Index'], 4, maxItem)
            
        ui.aiView.setModel(aImodel)
        ui.aiView.show()

def populate_aq(data):
    if(data!=None):
        aQmodel.setHorizontalHeaderLabels(["Analog Outputs", "Name", "Value"]);
        for i in data:
            keyItem = QStandardItem("AQ %d" % i['Index'])
            aQmodel.setItem(i['Index'], 0, keyItem)

            nameItem = QStandardItem(i.get('Identifier'))
            if(nameItem!=None):
                aQmodel.setItem(i['Index'], 1, nameItem)

            if(i.get('Value')!=None):
                valItem = QStandardItem(str(i.get('Value')))
                aQmodel.setItem(i['Index'], 2, valItem)

            if(i.get('Min')!=None):    
                minItem = QStandardItem(str(i['Min']))
                aQmodel.setItem(i['Index'], 3, minItem)

            if(i.get('Max')!=None):
                maxItem = QStandardItem(str(i['Max']))
                aQmodel.setItem(i['Index'], 4, maxItem)
            
        ui.aqView.setModel(aQmodel)
        ui.aqView.show()

def populate_mreg(data):
    if(data!=None):
        mRmodel.setHorizontalHeaderLabels(["Memory Registers", "Name", "Value"]);
        for i in data:
            keyItem = QStandardItem("MW %d" % i['Index'])
            mRmodel.setItem(i['Index'], 0, keyItem)
            
            nameItem = QStandardItem(i.get('Identifier'))
            if(nameItem!=None):
                mRmodel.setItem(i['Index'], 1, nameItem)

            if(i.get('Value')!=None):
                valItem = QStandardItem(str(i.get('Value')))
                mRmodel.setItem(i['Index'], 2, valItem)
    
        ui.mView.setModel(mRmodel)
        ui.mView.show()

def populate_mvar(data):
    if(data!=None):
        mVmodel.setHorizontalHeaderLabels(["Memory Reals", "Name", "Value"]);
        for i in data:
            keyItem = QStandardItem("MF %d" % i['Index'])
            mVmodel.setItem(i['Index'], 0, keyItem)
            
            nameItem = QStandardItem(i.get('Identifier'))
            if(nameItem!=None):
                mVmodel.setItem(i['Index'], 1, nameItem)

            if(i.get('Value')!=None):
                valItem = QStandardItem(str(i.get('Value')))   
                mVmodel.setItem(i['Index'], 2, valItem)
            
        ui.rView.setModel(mVmodel)
        ui.rView.show()

def populate_timer(data):
    if(data!=None):
        Tmodel.setHorizontalHeaderLabels(["Timers", "Name", "Value", "Resolution", "Preset"]);
        for i in data:
            keyItem = QStandardItem("T %d" % i['Index'])
            Tmodel.setItem(i['Index'], 0, keyItem)
            nameItem = QStandardItem(i.get('Identifier'))
            if(nameItem!=None):
                Tmodel.setItem(i['Index'], 1, nameItem)

            if(i.get('Value')!=None):
                valItem = QStandardItem(str(i.get('Value')))   
                Tmodel.setItem(i['Index'], 2, valItem)

            if(i.get('Resolution')!=None):
                resItem = QStandardItem(str(i['Resolution']))
                Tmodel.setItem(i['Index'], 3, resItem)
            
            if(i.get('Preset')!=None):    
                presItem = QStandardItem(str(i['Preset']))
                Tmodel.setItem(i['Index'], 4, presItem)
            
        ui.tView.setModel(Tmodel)
        ui.tView.show()

def populate_pulse(data):
    if(data!=None):
        Smodel.setHorizontalHeaderLabels(["Pulses", "Name", "Value", "Resolution"]);
        for i in data:
            keyItem = QStandardItem("S %d" % i['Index'])
            Smodel.setItem(i['Index'], 0, keyItem)

            nameItem = QStandardItem(i.get('Identifier'))
            if(nameItem!=None):
                Smodel.setItem(i['Index'], 1, nameItem)
            
            if(i.get('Value')!=None):
                valItem = QStandardItem(str(i.get('Value')))
                Smodel.setItem(i['Index'], 2, valItem)

            if(i.get('Resolution')!=None):
                resItem = QStandardItem(str(i['Resolution']))
                Smodel.setItem(i['Index'], 3, resItem)

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
            tt = data.get('TIMER')
            ps = data.get('PULSE')

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


    MainWindow.show()

    sys.exit(app.exec_())



