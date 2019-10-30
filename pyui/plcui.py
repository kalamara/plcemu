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
Tmodel = QStandardItemModel (0, 3)
Smodel = QStandardItemModel (0, 3)

def on_action_connect():

    alert = QMessageBox()
    alert.setText('wanna connect?')
    alert.exec_()

def val_or_null(data, val):
    if data.has_value(val):
        return data[val]
    else:
        return ""

def populate_di(data):

    dImodel.setHorizontalHeaderLabels(["Digital Inputs", "Name", "Value"]);
    for i in data:
        keyItem = QStandardItem("I %d" % i['Index'])
        nameItem = QStandardItem( i['Identifier'])
        dImodel.appendRow([keyItem, nameItem])
    ui.diView.setModel(dImodel)

def populate_dq(data):

    dQmodel.setHorizontalHeaderLabels(["Digital Outputs", "Name", "Value"]);
    for i in data:
        keyItem = QStandardItem("Q %d" % i['Index'])
        nameItem = QStandardItem( i['Identifier'])
        dQmodel.appendRow([keyItem, nameItem])
    ui.dqView.setModel(dQmodel)

def populate_ai(data):

    aImodel.setHorizontalHeaderLabels(["Analog Inputs", "Name", "Value", "Min", "Max"]);
    for i in data:
        keyItem = QStandardItem("AI %d" % i['Index'])
        nameItem = QStandardItem( i['Identifier'])
        #valItem = QStandardItem(i['Value'])
        minItem = QStandardItem(str(i['Min']))
        maxItem = QStandardItem(str(i['Max']))
        aImodel.appendRow([keyItem, nameItem, QStandardItem(""), minItem, maxItem])
    ui.aiView.setModel(aImodel)

def populate_aq(data):

    aQmodel.setHorizontalHeaderLabels(["Analog Outputs", "Name", "Value"]);
    for i in data:
        keyItem = QStandardItem("AQ %d" % i['Index'])
        nameItem = QStandardItem( i['Identifier'])
        minItem = QStandardItem(str(i['Min']))
        maxItem = QStandardItem(str(i['Max']))
        aQmodel.appendRow([keyItem, nameItem, QStandardItem(""), minItem, maxItem])
    ui.aqView.setModel(aQmodel)

def populate_mreg(data):

    mRmodel.setHorizontalHeaderLabels(["Memory Registers", "Name", "Value"]);
    for i in data:
        keyItem = QStandardItem("MW %d" % i['Index'])
        nameItem = QStandardItem( i['Identifier'])
        mRmodel.appendRow([keyItem, nameItem])
    ui.mView.setModel(mRmodel)

def populate_mvar(data):

    mVmodel.setHorizontalHeaderLabels(["Memory Reals", "Name", "Value"]);
    for i in data:
        keyItem = QStandardItem("MF %d" % i['Index'])
        nameItem = QStandardItem( i['Identifier'])
        mVmodel.appendRow([keyItem, nameItem])
    ui.rView.setModel(mVmodel)

def populate_timer(data):

     Tmodel.setHorizontalHeaderLabels(["Timers", "Name", "Value", "Resolution", "Preset"]);
     for i in data:
         keyItem = QStandardItem("T %d" % i['Index'])
         #nameItem = QStandardItem( i['Identifier'])
         #valItem = QStandardItem(i['Value'])
         resItem = QStandardItem(str(i['Resolution']))
         presItem = QStandardItem(str(i['Preset']))
         Tmodel.appendRow([keyItem, QStandardItem(""), QStandardItem(""), resItem, presItem])
     ui.tView.setModel(Tmodel)

def populate_pulse(data):

    Smodel.setHorizontalHeaderLabels(["Pulses", "Name", "Value", "Resolution"]);
    for i in data:
        keyItem = QStandardItem("S %d" % i['Index'])
        #nameItem = QStandardItem( i['Identifier'])
        #valItem = QStandardItem(str(i['Value']))
        resItem = QStandardItem(str(i['Resolution']))
        Smodel.appendRow([keyItem, QStandardItem(""), QStandardItem(""), resItem, QStandardItem("")])
    ui.sView.setModel(Smodel)

if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)

    with open("program.yml", 'r') as stream:
        try:
            data = yaml.safe_load(stream)
            di = data['DI']
            dq = data['DQ']
            ai = data['AI']
            aq = data['AQ']
            mr = data['MREG']
            mv = data['MVAR']
            tt = data['TIMER']
            ps = data['PULSE']

            print(ai)
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

#    for r in range(4):
#        rowItem = QStandardItem("I %d" % r)
#        parentItem.appendRow(rowItem)
#        for k in range(8):
#            keyItem = QStandardItem("Bit %d" % k)
#            nameItem = QStandardItem("Name")
#            valItem = QStandardItem("true")
#            rowItem.appendRow([keyItem, nameItem, valItem])




    ui.actionConnect.triggered.connect(on_action_connect)


    MainWindow.show()

    sys.exit(app.exec_())



