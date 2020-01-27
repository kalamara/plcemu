#!/usr/bin/env python
# -*- coding: utf-8 -*-
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *

import yaml
import zmq
import threading

from mainwindow import *

context = zmq.Context()
requester = context.socket(zmq.REQ)
subscriber = context.socket(zmq.SUB)

dImodel = QStandardItemModel (0, 3)
dQmodel = QStandardItemModel (0, 3)
aImodel = QStandardItemModel (0, 5)
aQmodel = QStandardItemModel (0, 5)
mRmodel = QStandardItemModel (0, 3)
mVmodel = QStandardItemModel (0, 3)
Tmodel = QStandardItemModel (0, 4)
Smodel = QStandardItemModel (0, 4)
Pmodel = QStandardItemModel(0, 1)

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

def req_resp(command):
    if(ui.actionConnect.isChecked()):    
        request = bytes(yaml.dump(command), 'utf-8')  
        try:  
            requester.send(request)
            message = requester.recv()#zmq.NOBLOCK)
            print("Received reply %s [%s] " %  (message, request))

        except Exception as e:
            print(e)    
 
def pub_sub():
    while(True):
        print("waiting for updates from PLC EMU...")
        message = subscriber.recv()
        print("Received update %s " % message)

sub_thread = threading.Thread(target=pub_sub, args=(), daemon=True)

def dump_di():
    count = dImodel.rowCount()
    payload = [count]
    for r in range(0, count):
        id = dImodel.item(r,1)
        if(id!=None):
            payload.append({ 'INDEX' : r, 'ID' : id.text() })

    return payload

def dump_dq():
    count = dQmodel.rowCount()
    payload = [count]
    for r in range(0, count):
        id = dQmodel.item(r,1)
        if(id!=None):
            payload.append({ 'INDEX' : r, 'ID' : id.text() })
   
    return payload

def dump_ai():
    count = aImodel.rowCount()
    payload = [count]
    for r in range(0, count):
        id = aImodel.item(r,1)
        min = aImodel.item(r,3)
        max = aImodel.item(r,4)
        line = {}
        if(id!=None):
            line.update({'INDEX' : r})
            line.update({'ID' : id.text()})
            if(min!=None):
                line.update({'MIN' : min.text()})
            if(max!=None):
                line.update({'MAX' : max.text()})    
            payload.append(line)
  
    return payload

def dump_aq():
    count = aQmodel.rowCount()
    payload = [count]
    for r in range(0, count):
        id = aQmodel.item(r,1)
        min = aQmodel.item(r,3)
        max = aQmodel.item(r,4)
        line = {}
        if(id!=None):
            line.update({'INDEX' : r})
            line.update({'ID' : id.text()})
            if(min!=None):
                line.update({'MIN' : min.text()})
            if(max!=None):
                line.update({'MAX' : max.text()})    
            payload.append(line)
 
    return payload

def dump_mr():
    count = mRmodel.rowCount()
    payload = [count]
    for r in range(0, count):
        id = mRmodel.item(r,1)
        if(id!=None):
            payload.append({ 'INDEX' : r, 'ID' : id.text() })

    return payload

def dump_mv():
    count = mVmodel.rowCount()
    payload = [count]
    for r in range(0, count):
        id = mVmodel.item(r,1)
        if(id!=None):
            payload.append({ 'INDEX' : r, 'ID' : id.text() })
     
    return payload

def dump_t():
    count = Tmodel.rowCount()
    payload = [count]
    for r in range(0, count):
        id = Tmodel.item(r,1)
        res = Tmodel.item(r,3)
        pres = Tmodel.item(r,4)
        line = {}
        if(id!=None):
            line.update({'INDEX' : r})
            line.update({'ID' : id.text()})
            if(res!=None):
                line.update({'RESOLUTION' : res.text()})
            if(pres!=None):
                line.update({'PRESET' : pres.text()})    
            payload.append(line)
    
    return payload

def dump_s():
    count = Smodel.rowCount()
    payload = [count]
    for r in range(0, count):
        id = Smodel.item(r,1)
        res = Smodel.item(r,3)
        line = {}
        if(id!=None):
            line.update({'INDEX' : r})
            line.update({'ID' : id.text()})
            if(res!=None):
                line.update({'RESOLUTION' : res.text()})
            payload.append(line)
         
    return payload

def dump_prog():
    count = Pmodel.rowCount()
    payload = [count]
    for r in range(0, count):
        id = Pmodel.item(r)
        payload.append({ 'INDEX' : r, 'ID' : id.text() })
    
    return payload    

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

def update_programs(data):
    if(data!=None):
        
        for i in data:
            
            nameItem = QStandardItem(i.get('ID'))
            if(nameItem!=None):
                Pmodel.setItem(i['INDEX'], 0, nameItem)
            
    
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

def populate_programs(data):       
    if(data!=None):
        Pmodel.clear()
        Pmodel.setHorizontalHeaderLabels(["Program rungs"]);
        if(isinstance(data[0],(int))):
            num, *items = data
            for i in range(0,num):
                keyItem = QStandardItem(i)
                Pmodel.appendRow([])
            update_programs(items)
        
        ui.listView.setModel(Pmodel)
        ui.listView.show()        

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
        #print(yaml.dump(comm))    
        
def on_action_start(state):
    if(state):
        comm = {'COMMAND': 1}
    else:    
        comm = {'COMMAND': 2}
    req_resp(comm)
    ui.programEdit.setEnabled(not(state))

def on_action_connect(state):
    if(state):
        print("Connecting to PLC EMU...")
        requester.connect("tcp://localhost:5555")
        subscriber.connect("tcp://localhost:5556")
        subscriber.setsockopt(zmq.SUBSCRIBE, b"---")
        sub_thread.start()

    else:
        print("Disconnecting...")
        requester.disconnect("tcp://localhost:5555")
        subscriber.disconnect("tcp://localhost:5556")
        sub_thread.join()

def save_config(file, config):
    try:
        f = open(file, 'w')      
        f.write(yaml.dump(config))

    except Exception as e:
        print(e)

def load_config(file):
    with open(file, 'r') as stream:
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
            prog = data.get('PROGRAM')

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
            populate_programs(prog)

        except yaml.YAMLError as exc:
            print(exc)

def on_action_load():
    fileName = QFileDialog.getOpenFileName(None,
    "Open PLC configuration", ".", "YML Files (*.yml)")
    load_config(fileName[0])
    
def on_action_save():
    payload = {}
    payload.update({"DI":dump_di()})
    payload.update({"DQ":dump_dq()})
    payload.update({"AI":dump_ai()})
    payload.update({"AQ":dump_aq()})
    payload.update({"MREG":dump_mr()})
    payload.update({"MVAR":dump_mv()})
    payload.update({"TIMERS":dump_t()})
    payload.update({"PULSES":dump_s()})
    payload.update({"PROGRAM":dump_prog()})
    #print(yaml.dump(payload))
    fileName = QFileDialog.getOpenFileName(None,
    "Save PLC configuration", ".", "YML Files (*.yml)")
    save_config(fileName[0], payload)


def load_program(fileName):
    
    try:
        with open(fileName, 'r') as stream:              
            ui.programEdit.setPlainText(stream.read())

    except Exception as e:
        print(e)


def on_program_selected(curr):
    fn = Pmodel.itemFromIndex(curr).text()
    #print(fn)
    load_program(fn)

def on_program_save():
    selection = ui.listView.selectedIndexes()
    try:
        f = open(Pmodel.itemFromIndex(selection[0]).text(), 'w')      
        f.write(ui.programEdit.toPlainText())

    except Exception as e:
        print(e)

def on_program_add():
    fileName = QFileDialog.getOpenFileName(None,
    "Open Rung program", ".", "IL or LD Program files  (*.il *.ld)")
    Pmodel.appendRow([QStandardItem(fileName[0])])    

def on_program_remove():
    selection = ui.listView.selectedIndexes()
    for s in selection:
        Pmodel.removeRows(s.row(), 1)
        ui.programEdit.clear()

if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)

    ui.actionConnect.triggered.connect(on_action_connect)
    ui.actionStart_stop.triggered.connect(on_action_start)
    ui.actionLoad.triggered.connect(on_action_load)
    ui.actionSave.triggered.connect(on_action_save)
    ui.listView.clicked.connect(on_program_selected)
    ui.pushButtonAdd.clicked.connect(on_program_add)
    ui.pushButtonRemove.clicked.connect(on_program_remove)
    ui.programEdit.textChanged.connect(on_program_save)

    load_config("program.yml") #FOR DEBUG

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



