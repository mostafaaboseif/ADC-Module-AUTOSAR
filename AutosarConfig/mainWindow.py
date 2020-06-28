from PySide2.QtWidgets import(
    QMainWindow,
    QTableWidget,
    QPushButton,
    QCheckBox,
    QComboBox,
    QHBoxLayout,
    QGroupBox,
    QLineEdit,
    QTableWidgetItem,
    QWidget,
    QHeaderView,
    QVBoxLayout,
    QLabel,
    QTabWidget,
    QTabBar,
)
from PySide2.QtGui import QIcon
import json

TEMP_FILE_PATH = "header.h.temp"
PRE_COMPILE_FILE_PATH = "preCompile.gen"
POST_BUILD_FILE_PATH = "postBuild.gen"

class MainWindow(QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()
        self.__channels = {"number":0}
        self.__preBuildTemp = self.__loadGenFile(PRE_COMPILE_FILE_PATH)
        self.__preCompile = self.__loadOutput(TEMP_FILE_PATH)
        self.__postBuildTemp = self.__loadGenFile(POST_BUILD_FILE_PATH)
        self.postBuildSettingsTab = QTabWidget()
        self.genBTN = QPushButton("Generate")
        self.__draw()

        self.genBTN.clicked.connect(self.generate)

    def __draw(self):
        mainW = QWidget()
        mainW.setLayout(QVBoxLayout())

        settingsW = QWidget()
        settingsW.setLayout(QHBoxLayout())

        preCompileSettingsTable = QTableWidget()
        preCompileSettingsGPX = QGroupBox("Pre-Compile Settings")
        preCompileSettingsGPX.setLayout(QHBoxLayout())
        preCompileSettingsGPX.layout().addWidget(preCompileSettingsTable)
        self.loadPreBuildSettings(preCompileSettingsTable)

        postBuildSettingsGPX = QGroupBox("Post Build Settings")
        postBuildSettingsGPX.setLayout(QHBoxLayout())
        postBuildSettingsGPX.layout().addWidget(self.postBuildSettingsTab)
        self.postBuildSettingsTab.setTabsClosable(True)
        self.postBuildSettingsTab.tabBar().addTab(QIcon(), "+")
        self.postBuildSettingsTab.tabBar().setTabButton(0,QTabBar.RightSide,None)


        preCompileSettingsTable.verticalHeader().hide()
        preCompileSettingsTable.horizontalHeader().hide()
        header = preCompileSettingsTable.horizontalHeader()
        header.setSectionResizeMode(0, QHeaderView.Stretch)
        header.setSectionResizeMode(1, QHeaderView.Stretch)

        def handler(index:int):
                self.__currentTabChangedHandler(index)
        self.postBuildSettingsTab.currentChanged.connect(handler)

        handler(0)


        settingsW.layout().addWidget(preCompileSettingsGPX)
        settingsW.layout().addWidget(postBuildSettingsGPX)

        btnW = QWidget()
        btnW.setLayout(QHBoxLayout())
        btnW.layout().addWidget(QLabel())
        btnW.layout().addWidget(QPushButton("Reset"))
        btnW.layout().addWidget(self.genBTN)

        mainW.layout().addWidget(settingsW)
        mainW.layout().addWidget(btnW)
        self.setCentralWidget(mainW)

    def __loadGenFile(self, path:str)->dict:
        f = open(path)
        out = json.loads(f.readline())
        f.close()
        return out

    def __loadOutput(self, path:str)->str:
        f = open(path)
        out = "".join(f.readlines())
        f.close()
        return out

    def loadPreBuildSettings(self, widget:QTableWidget):
        rows = len(self.__preBuildTemp)
        widget.setRowCount(rows)
        widget.setColumnCount(2)
        row = 0
        for k in self.__preBuildTemp:
            view = self.__preBuildTemp[k]["view"]
            if self.__preBuildTemp[k]["type"] == "NUMBER":
                self.__preBuildTemp[k]["widget"] = QLineEdit()
                widget.setItem(row, 0, QTableWidgetItem(view))
                widget.setCellWidget(row, 1, self.__preBuildTemp[k]["widget"])
            elif self.__preBuildTemp[k]["type"] == "BOOL":
                self.__preBuildTemp[k]["widget"] = QCheckBox(view)
                widget.setCellWidget(row, 0, self.__preBuildTemp[k]["widget"])
            elif self.__preBuildTemp[k]["type"] == "SELECT":
                self.__preBuildTemp[k]["widget"] = QComboBox()
                print(self.__preBuildTemp[k])
                self.__preBuildTemp[k]["widget"].addItems(self.__preBuildTemp[k]["options"])
                widget.setItem(row, 0, QTableWidgetItem(view))
                widget.setCellWidget(row, 1, self.__preBuildTemp[k]["widget"])
            row += 1

    def __currentTabChangedHandler(self, index:int):
        print("called")
        if index == self.postBuildSettingsTab.count()-1:
            self.__channels["number"] += 1
            n = self.__channels["number"]
            self.postBuildSettingsTab.addTab(self.loadPostBuildSettings(), "Channel "+str(n))
            self.postBuildSettingsTab.setCurrentIndex(self.postBuildSettingsTab.count() - 2)

    def loadPostBuildSettings(self):
        widget = QTableWidget()

        widget.setRowCount(len(self.__postBuildTemp.keys()))
        widget.setColumnCount(2)
        widget.verticalHeader().hide()
        widget.horizontalHeader().hide()
        header = widget.horizontalHeader()
        header.setSectionResizeMode(0, QHeaderView.Stretch)
        header.setSectionResizeMode(1, QHeaderView.Stretch)

        row = 0
        for k in self.__postBuildTemp:
            if self.__postBuildTemp[k]["type"] == "NUMBER":
                self.__postBuildTemp[k]["widget"] = QLineEdit()
                widget.setItem(row, 0, QTableWidgetItem(k))
                widget.setCellWidget(row, 1, self.__postBuildTemp[k]["widget"])
                row += 1
            elif self.__postBuildTemp[k]["type"] == "BOOL":
                self.__postBuildTemp[k]["widget"] = QCheckBox(k)
                widget.setCellWidget(row, 0, self.__postBuildTemp[k]["widget"])
                row += 1
            elif self.__postBuildTemp[k]["type"] == "SELECT":
                self.__postBuildTemp[k]["widget"] = QComboBox()
                self.__postBuildTemp[k]["widget"].addItems(self.__postBuildTemp[k]["options"])
                widget.setItem(row, 0, QTableWidgetItem(k))
                widget.setCellWidget(row, 1, self.__postBuildTemp[k]["widget"])
                row += 1
            elif self.__postBuildTemp[k]["type"] == "ID_number":
                self.__postBuildTemp[k]["lastVal"] = 0
        return widget

    def generate(self):
        for k in self.__preBuildTemp:
            value = ""
            if self.__preBuildTemp[k]["type"] == "NUMBER":
                value = self.__preBuildTemp[k]["widget"].text()
            elif self.__preBuildTemp[k]["type"] == "BOOL":
                i = 0 if self.__preBuildTemp[k]["widget"].isChecked() else 1
                value = self.__preBuildTemp[k]["values"][i]
            elif self.__preBuildTemp[k]["type"] == "SELECT":
                i = self.__preBuildTemp[k]["widget"].currentIndex()
                value = self.__preBuildTemp[k]["values"][i]
            self.__preCompile = self.__preCompile.replace("%"+k+"%", value, 1)
        f = open("preCompile.h", "w")
        f.write(self.__preCompile)
        f.close()