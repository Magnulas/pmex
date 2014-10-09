from    PyQt4       import QtGui, QtCore
from    math        import fabs

class DiagramPoint(QtGui.QGraphicsEllipseItem):
    def __init__(self,x,y, p, infty = False, color = 0):
        super(QtGui.QGraphicsEllipseItem, self).__init__()
        c = self.color(color)
        self.setBrush(QtGui.QBrush(c[0]))
        self.setPen(QtGui.QPen(c[1]))
        self.radius = .075
        if infty:
            self.radius *= 2
        self.x, self.y = x,y
        self.scale(1)
        self.p = p

    def scale(self, delta):
        self.radius *= delta
        self.setRect(self.x - self.radius, self.y - self.radius, 2*self.radius, 2*self.radius)

    def color(self, i):
        return self._colors[i % len(self._colors)]

    # (fill, border) pairs
    _colors = [(QtCore.Qt.red,   QtGui.QColor(225, 0, 0)),
               (QtCore.Qt.blue,  QtGui.QColor(0, 0, 225)),
               (QtCore.Qt.green, QtGui.QColor(0, 225, 0)),
              ]

class DiagramViewer(QtGui.QGraphicsView):
    def __init__(self, dgm, noise):
        super(QtGui.QGraphicsView, self).__init__()

        self.selection = None
        self._pan = False

        self.setRenderHint(QtGui.QPainter.Antialiasing)
        self.scene = QtGui.QGraphicsScene(self)
        self.setScene(self.scene)

        if not isinstance(dgm, list):
            # Assume it's just a single diagram
            dgms = [dgm]
        else:
            dgms = dgm

        inf = float('inf')
        xs = [p[0] for d in dgms for p in d]
        ys = [p[1] for d in dgms for p in d]
        minx = min(0, min(xs) if xs else 0)
        miny = min(0, min(ys) if ys else 0)
        xs = [x for x in xs if x != inf]
        ys = [y for y in ys if y != inf]
        maxx = max(0, max(xs) if xs else 0)
        maxy = max(0, max(ys) if ys else 0)

        self.draw_axes(minx,miny,maxx,maxy)

        for i, dgm in enumerate(dgms):
            for p in dgm:
                x,y = p[0],p[1]
                if fabs(y - x) < noise:
                    continue
                if fabs(x) == inf or fabs(y) == inf:
                    if x == inf: x = maxx + 2
                    if y == inf: y = maxy + 2
                    if x == -inf: x = minx - 2
                    if y == -inf: y = miny - 2
                    item = DiagramPoint(x,y,p, infty = True, color = i)
                else:
                    item = DiagramPoint(x,y,p, color = i)
                self.scene.addItem(item)

        # Flip y-axis
        self.scale(1, -1)

        # Set the correct view
        rect = self.scene.itemsBoundingRect()
        self.fitInView(rect, QtCore.Qt.KeepAspectRatio)

    def mousePressEvent(self, event):
        if event.button() == QtCore.Qt.RightButton:
            self._pan = True
            self._panStartX = event.x()
            self._panStartY = event.y()
            self.setCursor(QtCore.Qt.ClosedHandCursor)
            event.accept()
        else:
            p = self.mapToScene(event.pos())
            item = self.scene.itemAt(p)
            if isinstance(item, DiagramPoint):
                self.selection = item.p
                self.close()

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

    def wheelEvent(self, event):
        delta = 1 + float(event.delta())/100
        if delta < 0:
            event.ignore()
            return
        self.scale(delta, delta)
        for item in self.scene.items():
            if isinstance(item, DiagramPoint):
                item.scale(1/delta)
        event.accept()

    def draw_axes(self, minx, miny, maxx, maxy):
        # Draw axes and diagonal
        if maxx > 0:
            self.scene.addItem(QtGui.QGraphicsLineItem(0,0, maxx, 0))
        if minx < 0:
            self.scene.addItem(QtGui.QGraphicsLineItem(minx,0, 0, 0))
        if maxy > 0:
            self.scene.addItem(QtGui.QGraphicsLineItem(0,0, 0, maxy))
        if miny < 0:
            self.scene.addItem(QtGui.QGraphicsLineItem(0,miny, 0, 0))
        self.scene.addItem(QtGui.QGraphicsLineItem(0,0, min(maxx, maxy), min(maxx, maxy)))
        self.scene.addItem(QtGui.QGraphicsLineItem(max(minx,miny), max(minx,miny), 0,0))

        # Dashed, gray integer lattice
        pen = QtGui.QPen(QtCore.Qt.DashLine)
        pen.setColor(QtCore.Qt.gray)
        for i in xrange(min(0, int(minx)) + 1, max(0,int(maxx)) + 1):
            line = QtGui.QGraphicsLineItem(i,0, i, maxy)
            line.setPen(pen)
            self.scene.addItem(line)
        for i in xrange(min(0, int(miny)) + 1, max(0, int(maxy)) + 1):
            line = QtGui.QGraphicsLineItem(0,i, maxx, i)
            line.setPen(pen)
            self.scene.addItem(line)


def show_diagram(dgm, noise, app):
    #app = QtGui.QApplication([])
    view = DiagramViewer(dgm, noise)
    view.show()
    view.raise_()
    app.exec_()
    return view.selection
