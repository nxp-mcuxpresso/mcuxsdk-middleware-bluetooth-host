#------------------------------------------------------------------------------
# Copyright : Stichting imec Nederland (http://www.imec.nl)
#             *** IMEC CONFIDENTIAL ***
#------------------------------------------------------------------------------

from PyQt5.QtWidgets import*
from matplotlib.backends.backend_qt5agg import FigureCanvas
from matplotlib.figure import Figure
import numpy as np
from PyQt5 import QtGui

imec_bondi_blue = '#3F98BD'
imec_dark_gray = '#3C3C3B'
imec_light_gray = '#929497'
imec_midnight_blue = '#36337D'
imec_medium_blue = '#1582BE'
imec_turquoise = '#52BDC2'
imec_pink = '#C778AD'
imec_light_blue = '#99BDE4'
imec_purple = '#90298D'

class MplCanvas(FigureCanvas):
    def __init__(self):#(self, parent = None, width = 5, height = 5, dpi = 100):
        self.fig = Figure() #(figsize=(width, height), dpi=dpi)
        self.axes = self.fig.add_subplot(111)

        FigureCanvas.__init__(self, self.fig)
        #FigureCanvas.setSizePolicy(self, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        #self.setParent(parent)
        FigureCanvas.updateGeometry(self)

class MplWidget(QWidget):
    def __init__(self, parent = None):
        QWidget.__init__(self, parent)
        self.canvas = MplCanvas() #FigureCanvas(Figure())
        vertical_layout = QVBoxLayout()
        vertical_layout.addWidget(self.canvas)
        # self.canvas.axes = self.canvas.figure.add_subplot(111, projection='polar')

        self.canvas.axes.set_ylabel('y (m)', fontsize=15, fontweight='bold')
        self.canvas.axes.tick_params(labelsize=13)
        self.canvas.axes.set_xlabel('x (m)', fontsize=15, fontweight='bold')
        self.canvas.axes.grid()
        # self.canvas.axes.set_aspect('equal', 'box')

        #self.canvas.axes.set_title('Phase-Based Localization', fontsize=30, color=imec_purple)
        # self.canvas.axes.axis('equal')
        #self.canvas.axes.set_rticks(list(range(0, 11)))
        #self.canvas.axes.set_rlabel_position(-60)
        #self.canvas.axes.set_theta_zero_location("S")
        #self.canvas.axes.annotate('Distance [m]', xy=(np.pi/3.2, 5), rotation=-30, fontsize=13)

        #r = tuple(range(1, 11))
        #r_str = [tuple(map(str, tup)) for tup in [r]]
        #self.canvas.axes.set_rgrids(r, labels=r_str[0], angle=45, fmt='%d')
        #self.canvas.fig.subplots_adjust(left=-1.3, right=2.3, bottom=-0.3, top=1.3, hspace=0.01,wspace=0.01)
        #self.canvas.fig.patch.set_facecolor('r')
        self.canvas.fig.tight_layout(pad=0.00, h_pad=0.00, w_pad=0.00, rect=[-0.08, -0.05, 1, 1])
        self.setLayout(vertical_layout)