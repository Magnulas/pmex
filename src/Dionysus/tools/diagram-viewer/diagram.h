#ifndef __DIAGRAM_H__
#define __DIAGRAM_H__

#include <QtGui>
#include <QObject>
#include <QColor>

#include <map>

#include <utilities/types.h>
#include <topology/persistence-diagram.h>

typedef         PersistenceDiagram<>                PDiagram;
typedef         std::map<Dimension, PDiagram>       Diagrams;

class DgmPoint;

class DgmViewer: public QGraphicsView
{
    Q_OBJECT

    public:
        typedef             std::vector<DgmPoint*>          PointsVector;

                            DgmViewer(const PDiagram& dgm);
                            ~DgmViewer();

        void                addDgmPoints();

    private:
        PointsVector        points;
        QGraphicsScene      scene;
        RealType            min_x, min_y, max_x, max_y;
};


class DgmPoint: public PDPoint<>, public QGraphicsItem
{
    public:
        typedef             PDPoint<>                                       Parent;

                            DgmPoint(QGraphicsItem* parent = 0);
                            DgmPoint(const Parent& pt, qreal size, QGraphicsItem *parent = 0); 
                            DgmPoint(RealType b, RealType d, qreal size, QGraphicsItem *parent = 0); 
        
        void                paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
        QRectF                 boundingRect() const;
        
        qreal               getX() const                    { return Parent::x(); }
        qreal               getY() const                    { return Parent::y(); }

        int                 type() const                                    { return QGraphicsItem::UserType + 1; }

    private:
        // size of rectangle containing ellipses
        qreal                 ellipse_size;
};


#endif // __DIAGRAM_H__
