from    diagram     import show_diagram as _show_diagram
from    complex2d   import show_complex_2D as _show_complex_2D
from    complex3d   import show_complex_3D as _show_complex_3D

from    PyQt4       import QtGui

_app = QtGui.QApplication([])

def show_complex(points, complex = None, values = None, subcomplex = None, **kwargs):
    if len(points[0]) == 2:
        _show_complex_2D(points, complex, values, subcomplex, app = _app, **kwargs)
    if len(points[0]) == 3:
        _show_complex_3D(points, complex, values, subcomplex, app = _app, **kwargs)

def show_diagram(dgm, noise = 0):
    return _show_diagram(dgm, noise, _app)
