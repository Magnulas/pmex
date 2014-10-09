#include <iostream>

#include <QtGui>
#include <QRectF>

#include "diagram.h"
#include <cmath>

//static const double ellipse_size        = 0.035;
static const double ellipse_size        = 3;

/* DgmViewer Implementation */
DgmViewer::DgmViewer(const PDiagram& dgm):
    min_x(0), min_y(0), max_x(0), max_y(0)
{
    points.reserve(dgm.size());
    
    for (PDiagram::const_iterator cur = dgm.begin(); cur != dgm.end(); ++cur)
    {
        min_x = std::min(min_x, cur->x());
        min_y = std::min(min_y, cur->y());
        max_x = std::max(max_x, cur->x());
        max_y = std::max(max_y, cur->y());

        points.push_back(new DgmPoint(*cur, ellipse_size));
    }
        
    addDgmPoints();
    setWindowTitle(QString("Persistence Diagram"));
}
DgmViewer::~DgmViewer()
{
    for (PointsVector::iterator cur = points.begin(); cur != points.end(); ++cur)
        delete *cur;
}


void DgmViewer::addDgmPoints()
{    
    RealType min = std::min(min_x, min_y);
    RealType max = std::max(max_x, max_y);

    QGraphicsLineItem* diagonal = new QGraphicsLineItem(QLineF(min, -min, max, -max));
    QGraphicsLineItem* y_axis = new QGraphicsLineItem(QLineF(0, -min_y, 0, -max_y));
    QGraphicsLineItem* x_axis = new QGraphicsLineItem(QLineF(min_x, 0, max_x, 0));

    scene.addItem(diagonal);
    scene.addItem(y_axis);
    scene.addItem(x_axis);

    for (PointsVector::const_iterator cur = points.begin(); cur != points.end(); ++cur)
        scene.addItem(*cur);

    //scale(100,100);
    setScene(&scene);
    setRenderHint(QPainter::Antialiasing);
    ensureVisible(scene.itemsBoundingRect());
    //setMinimumSize( (int)(maxX - minX)*100 + 100, (int) (maxY - minY)*100 + 100);
}


DgmPoint::DgmPoint(QGraphicsItem* parent): 
    QGraphicsItem(parent) 
{
}

DgmPoint::DgmPoint(const Parent& pt, qreal size, QGraphicsItem *parent):
    Parent(pt), ellipse_size(size), QGraphicsItem(parent)
{
    setToolTip(QString("(%1, %2)").arg(getX()).arg(getY()));
}

DgmPoint::DgmPoint(RealType b, RealType d, qreal size, QGraphicsItem *parent): 
    Parent(b, d), ellipse_size(size), QGraphicsItem(parent)
{
    setToolTip(QString("(%1, %2)").arg(getX()).arg(getY()));
}

void DgmPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    //QBrush solidFill(unselectColor);
    //QBRush selectSolidFill(selectColor);
    painter->setBrush(Qt::SolidPattern);
    //painter->setPen(selectColor);
    painter->drawEllipse(QRectF(getX() - ellipse_size, -getY() - ellipse_size, 2*ellipse_size, 2*ellipse_size));
}


QRectF DgmPoint::boundingRect() const
{
    return QRectF(getX() - ellipse_size, -getY() - ellipse_size, 2*ellipse_size, 2*ellipse_size);
}
