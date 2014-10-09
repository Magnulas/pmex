from    PyQt4       import QtGui, QtCore
from    PyGLWidget  import PyGLWidget
from    OpenGL.GL   import *
from    dionysus    import Simplex
from    math        import sqrt

class ComplexViewer3D(PyGLWidget):
    def __init__(self, points, complex = None, values = None, subcomplex = None, point_size = 3.):
        self.display_list = None
        PyGLWidget.__init__(self)

        #glEnable( GL_BLEND )
        #glEnable( GL_LINE_SMOOTH )
        #glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

        self.point_size = point_size
        self.points = points
        if complex:
            self.complex = [s for s in complex]
        else:
            # Create vertex simplices if no complex provided
            self.complex = [Simplex([i]) for i in xrange(len(self.points))]

        if subcomplex:
            self.subcomplex = subcomplex
        else:
            self.subcomplex = []

        self.values = values
        if not values:
            self.values = [0]*len(self.points)
        self.maxval, self.minval = max(self.values), min(self.values)

        center, radius = self.center_radius()
        self.set_radius(radius)
        self.set_center(center)

        self.make_display_list()

    def center_radius(self):
        c = [0,0,0]
        for p in self.points:
            for i in xrange(3): c[i] += p[i]
        for i in xrange(3): c[i] /= len(self.points)

        r = 0
        for p in self.points:
            d = sqrt((p[0] - c[0])**2 + (p[1] - c[1])**2 + (p[2] - c[2])**2)
            if d > r: r = d
        return c,r

    def paintGL(self):
        PyGLWidget.paintGL(self)
        if self.display_list:
            glCallList(self.display_list)

    def make_display_list(self):
        self.display_list = glGenLists(1)
        glNewList(self.display_list, GL_COMPILE)
        self.draw_complex(self.complex,    self.point_size,   2., self.colormap)
        self.draw_complex(self.subcomplex, 2*self.point_size, 4., colormap = lambda v: (0,1.,0), line_color = (0,1.,0))
        glEndList()


    def draw_complex(self, complex, point_size, line_size, colormap, line_color = (0,0,1.)):
        glPointSize(point_size)
        glLineWidth(line_size)
        complex.sort(lambda s1, s2: -cmp(s1.dimension(), s2.dimension()))
        for s in complex:
            vertices = [v for v in s.vertices]
            if s.dimension() == 0:              # point
                p = self.points[vertices[0]]
                v = self.values[vertices[0]]

                c = self.colormap(v)
                glColor3f(*c)
                glBegin(GL_POINTS)
                glVertex3f(p[0],p[1],p[2])
                glEnd()
            if s.dimension() == 1:            # edge
                p0 = self.points[vertices[0]]
                p1 = self.points[vertices[1]]

                glColor3f(*line_color)
                glBegin(GL_LINES)
                glVertex3f(p0[0],p0[1],p0[2])
                glVertex3f(p1[0],p1[1],p1[2])
                glEnd()
            elif s.dimension() == 2:
                p0 = self.points[vertices[0]]
                p1 = self.points[vertices[1]]
                p2 = self.points[vertices[2]]

                glColor3f(1,1,0)
                glBegin(GL_TRIANGLES)
                glVertex3f(p0[0],p0[1],p0[2])
                glVertex3f(p1[0],p1[1],p1[2])
                glVertex3f(p2[0],p2[1],p2[2])
                glEnd()

    def colormap(self, v):
        if self.maxval <= self.minval:
            t = 0
        else:
            t = (v - self.minval)/(self.maxval - self.minval)
        c = QtGui.QColor()
        c.setHsv(int(t*255), 255, 255)
        cr = float(c.red())/255
        cg = float(c.green())/255
        cb = float(c.blue())/255
        return (cr,cg,cb)

def show_complex_3D(points, complex = None, values = None, subcomplex = None, app = None, point_size = 3.):
    #app = QtGui.QApplication([])
    view = ComplexViewer3D(points, complex, values, subcomplex, point_size)
    view.show()
    view.raise_()
    app.exec_()
