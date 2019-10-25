#!/usr/bin/env python
# -*- coding: utf-8 -*-
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
import json

from mainwindow import *

def on_action_connect():

    alert = QMessageBox()
    alert.setText('wanna connect?')
    alert.exec_()

if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)

    model = QStandardItemModel (0, 3)
    model.setHorizontalHeaderLabels(["Inputs", "Name", "Value"]);
    parentItem = model.invisibleRootItem()

    for r in range(4):
        rowItem = QStandardItem("I %d" % r)
        parentItem.appendRow(rowItem)
        for k in range(8):
            keyItem = QStandardItem("Bit %d" % k)
            nameItem = QStandardItem("Name")
            valItem = QStandardItem("true")
            rowItem.appendRow([keyItem, nameItem, valItem])



    ui.aiView.setModel(model)
    ui.actionConnect.triggered.connect(on_action_connect)

    MainWindow.show()

    sys.exit(app.exec_())



