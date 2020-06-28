import sys
import PySide2.QtCore as QtCore
from PySide2.QtWidgets import QApplication
from mainWindow import MainWindow
app = QApplication(sys.argv)


w = MainWindow()
w.show()
if __name__ == '__main__':
    import sys
    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QApplication.instance().exec_()