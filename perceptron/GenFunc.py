# -*- coding: utf-8 -*-

try:
    from PySide import QtWidgets
except:
    from PyQt5 import QtWidgets

class Example(QWidget):
    def __init__(self):
        super().__init__()

        self.initUI()

    def initUI(self):
        self.setGeometry(300, 300, 300, 220)
        self.setWindowTitle('Icon')
        self.setWindowIcon(QIcon('web.png'))

        self.show()

class GenFunc:
    def __init__(self):


#app = QApplication(sys.argv)
#ex = Example()
#sys.exit(app.exec_())
