#------------------------------------------------------------------------------
# Copyright : Stichting imec Nederland (http://www.imec.nl)
#             *** IMEC CONFIDENTIAL ***
#------------------------------------------------------------------------------

from PyQt5.QtWidgets import*
from matplotlib.backends.backend_qt5agg import FigureCanvas
from matplotlib.figure import Figure
import numpy as np
from PyQt5 import QtGui

class MplCanvas(FigureCanvas):
    def __init__(self):
        self.fig = Figure()
        self.axes = self.fig.add_axes([-0.7, -0.3, 2.4, 1.6], projection='polar')

        #self.axes = self.fig.add_subplot(projection='polar')
        FigureCanvas.__init__(self, self.fig)
        #FigureCanvas.setSizePolicy(self, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        FigureCanvas.updateGeometry(self)

class MplWidget(QWidget):
    def __init__(self, parent = None):
        QWidget.__init__(self, parent)
        self.canvas = MplCanvas() #FigureCanvas(Figure())
        vertical_layout = QVBoxLayout()
        vertical_layout.addWidget(self.canvas)
        # self.canvas.axes = self.canvas.figure.add_subplot(111, projection='polar')

        # Initial plot conditions

        # self.canvas.axes.tick_params(labelsize=13)
        #self.canvas.axes.bar(0, 10, width=2*(np.pi/3), bottom=2.0, color=(1, 0.1764, 0.1764, 0.1255)) #red
        #self.canvas.axes.bar(0, 2, width=2*(np.pi/3), bottom=0.0, color=(0.1333, 0.6941, 0.298, 0.1255)) #green
        # self.canvas.axes.set_rmax(10)
        # self.canvas.axes.set_thetamin(-60)
        # self.canvas.axes.set_thetamax(60)
        # self.canvas.axes.set_rticks(list(range(0, 11)))
        # self.canvas.axes.set_rlabel_position(-60)
        self.canvas.axes.set_theta_zero_location("S")
        # self.canvas.axes.annotate('Distance [m]', xy=(np.pi/3.2, 5), rotation=-30, fontsize=13)

        #r = tuple(range(1, 11))
        #r_str = [tuple(map(str, tup)) for tup in [r]]
        #self.canvas.axes.set_rgrids(r, labels=r_str[0], angle=45, fmt='%d')
        #self.canvas.fig.subplots_adjust(left=-1.3, right=2.3, bottom=-0.3, top=1.3, hspace=0.01,wspace=0.01)
        #self.canvas.fig.patch.set_facecolor('r')
        #self.canvas.fig.tight_layout(pad=0.001, h_pad=0.001, w_pad=0.001)

        self.setLayout(vertical_layout)