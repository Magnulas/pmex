from    PyQt4       import QtGui, QtCore
from    dionysus    import Simplex

class ComplexViewer2D(QtGui.QGraphicsView):
    def __init__(self, points, complex = None, values = None, subcomplex = None):
        super(QtGui.QGraphicsView, self).__init__()
        self._pan = False

        self.points = points
        if complex:
            complex = [s for s in complex]
        else:
            # Create vertex simplices if no complex provided
            complex = [Simplex([i]) for i in xrange(len(self.points))]

        if not subcomplex:
            subcomplex = []

        if not values:
            values = [0]*len(self.points)
        self.values = values
        self.maxval, self.minval = max(values), min(values)

        self.setRenderHint(QtGui.QPainter.Antialiasing)
        self.scene = QtGui.QGraphicsScene(self)
        self.setScene(self.scene)

        minx = min(p[0] for p in points)
        miny = min(p[1] for p in points)
        maxx = max(p[0] for p in points)
        maxy = max(p[1] for p in points)

        radius = min(maxx - minx, maxy - miny)/100
        self.scene.setSceneRect(minx - 10*radius, miny - 10*radius, (maxx - minx) + 20*radius, (maxy - miny) + 20*radius)

        self.draw_complex(complex, radius, colormap = self.colormap)
        self.draw_complex(subcomplex, 3*radius, colormap = lambda v: QtCore.Qt.green, line_color = QtCore.Qt.green)

        # Flip y-axis
        self.scale(1,-1)

        # Set the correct view
        rect = self.scene.itemsBoundingRect()
        self.fitInView(rect, QtCore.Qt.KeepAspectRatio)

    def draw_complex(self, complex, radius, colormap, line_color = QtCore.Qt.black):
        complex.sort(lambda s1, s2: -cmp(s1.dimension(), s2.dimension()))
        for s in complex:
            vertices = [v for v in s.vertices]
            if s.dimension() == 0:              # point
                p = self.points[vertices[0]]
                v = self.values[vertices[0]]
                item = QtGui.QGraphicsEllipseItem(p[0] - radius/2,p[1] - radius/2,radius,radius)
                color = colormap(v)
                item.setBrush(QtGui.QBrush(color))
                item.setPen(QtGui.QPen(color))
            elif s.dimension() == 1:            # edge
                p0 = self.points[vertices[0]]
                p1 = self.points[vertices[1]]
                item = QtGui.QGraphicsLineItem(p0[0], p0[1], p1[0], p1[1])
                item.setPen(QtGui.QPen(line_color))
            else:                               # higher-d simplex
                pts = [QtCore.QPointF(self.points[v][0], self.points[v][1]) for v in vertices]
                item = QtGui.QGraphicsPolygonItem(QtGui.QPolygonF(pts))
                item.setBrush(QtCore.Qt.blue)

            self.scene.addItem(item)

    def colormap(self, v):
        if self.maxval <= self.minval:
            t = 0
        else:
            t = (v - self.minval)/(self.maxval - self.minval)
        c = QtGui.QColor()
        c.setHsv(int(t*255), 255, 255)
        return c

    def wheelEvent(self, event):
        delta = 1 + float(event.delta())/100
        if delta < 0:
            event.ignore()
            return
        self.scale(delta, delta)
        event.accept()

    def mousePressEvent(self, event):
        if event.button() == QtCore.Qt.RightButton:
            self._pan = True
            self._panStartX = event.x()
            self._panStartY = event.y()
            self.setCursor(QtCore.Qt.ClosedHandCursor)
            event.accept()

    def mouseReleaseEvent(self, event):
        if event.button() == QtCore.Qt.RightButton:
            self._pan = False
            self.setCursor(QtCore.Qt.ArrowCursor)
            event.accept()
            return
        event.ignore()

    def mouseMoveEvent(self, event):
        if self._pan:
            self.horizontalScrollBar().setValue(self.horizontalScrollBar().value() - (event.x() - self._panStartX))
            self.verticalScrollBar().setValue(self.verticalScrollBar().value() - (event.y() - self._panStartY))
            self._panStartX = event.x()
            self._panStartY = event.y()
            event.accept()
            return
        event.ignore()

def show_complex_2D(points, complex = None, values = None, subcomplex = None, app = None):
    #app = QtGui.QApplication([])
    view = ComplexViewer2D(points, complex, values, subcomplex)
    view.show()
    view.raise_()
    app.exec_()
