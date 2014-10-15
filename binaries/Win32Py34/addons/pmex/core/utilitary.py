"""This is part of the Periodic Motion Extractor plugin for Blender,
and is to be used for extracting periodic motions from motion capture data.
Copyright (C) 2014  Magnus Raunio

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import math
import os

from numpy import array, pi, sin, cos

from itertools import chain
import numpy.linalg as npl
import re
import numpy

import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection
from matplotlib.colors import ListedColormap, BoundaryNorm

def make_segments(x, y):
    '''
    Create list of line segments from x and y coordinates, in the correct format for LineCollection:
    an array of the form   numlines x (points per line) x 2 (x and y) array
    '''

    points = numpy.array([x, y]).T.reshape(-1, 1, 2)
    segments = numpy.concatenate([points[:-1], points[1:]], axis=1)
    
    return segments


# Interface to LineCollection:

def colorline(x, y, z=None, cmap=plt.get_cmap('copper'), norm=plt.Normalize(0.0, 1.0), linewidth=3, alpha=1.0):
    '''
    Plot a colored line with coordinates x and y
    Optionally specify colors in the array z
    Optionally specify a colormap, a norm function and a line width
    '''
    
    # Default colors equally spaced on [0,1]:
    if z is None:
        z = numpy.linspace(0.0, 1.0, len(x))
           
    # Special case if a single number:
    if not hasattr(z, "__iter__"):  # to check for numerical input -- this is a hack
        z = numpy.array([z])
        
    z = numpy.asarray(z)
    
    segments = make_segments(x, y)
    lc = LineCollection(segments, array=z, cmap=cmap, norm=norm, linewidth=linewidth, alpha=alpha)
    
    ax = plt.gca()
    ax.add_collection(lc)
    
    return lc

def PCAPlot(pointcloud,cyclemap,skeleton,folder,name):
    
    if not(os.path.isdir(folder)):
        os.makedirs(folder)
            
    if len(pointcloud) < 1 or len(pointcloud[0]) < 2:
        return
    
    from numpy import array
    from matplotlib.pylab import draw,plot,savefig,colorbar,scatter,axes,close,clf
    #from matplotlib.mlab import PCA
    from sklearn.decomposition import PCA
    
    np_points = array(pointcloud)
    pca = PCA(n_components = 2)
    pca.fit(np_points)
    data = pca.transform(np_points)

    #pca = PCA(np_points)
   
    xx = [pt[0] for pt in data]
    yy = [pt[1] for pt in data]
    values = numpy.array(cyclemap)
    values = numpy.mod(values - values[0],1.0)

    # hsv()
    scatter(xx,yy,c=values)
    colorbar()

    ax = axes()
    ax.set_axis_off()
    ax.set_aspect('equal', 'box')
    
    if len(skeleton) > 0 and len(skeleton[0])>1:
        np_skel = array(skeleton)

        data = pca.transform(np_skel.T)
        x = [d[0] for d in data]
        x.append(data[0][0])
        y = [d[1] for d in data]
        y.append(data[0][1])
        colorline(x,y,cmap=plt.get_cmap())
        #plot(x,y)

    draw()
    
    savefig(folder + "/" + name + ".png")
    close()
    clf()

def centralFiniteDifferenceArray(data,order=1,h=1,cyclic=False):
    
    if order==1:
        coefficients = [1/280, -4/105,    1/5,    -4/5,    0,    4/5,    -1/5,    4/105,    -1/280]
    else:
        coefficients = [-1/560,    8/315,    -1/5,    8/5,    -205/72,    8/5,    -1/5,    8/315,    -1/560]
    
    k = math.floor(len(coefficients)/2)
    if cyclic:
        d = 0
        result = len(data)*[0]
    else:
        d = k
        result = (len(data)-(2*k))*[0]
        
    for i in range(d,len(data)-d):
        finite_difference = array(len(data[0])*[0])
        for j in range(-k,k+1):
            index = i+j
            if index >= len(data):
                index = index - len(data) 
            finite_difference = finite_difference + coefficients[j+k]*array(data[index])

        result[i-d] = finite_difference/pow(h,order)

    return result#, cov

def timeSequenceError(seq1,seq2):

    short_seq = seq2
    long_seq = seq1
    if len(seq1) < len(seq2):
        short_seq = seq1
        long_seq = seq2
        
    M = len(long_seq)
    N = len(short_seq)

    min_err = float('inf')
    start_i = -1

    for i in range(0,M-N):
        err = numpy.sqrt(numpy.sum(numpy.power(array(long_seq[i:(i+N)]) - array(short_seq),2))/N)
        
        if err < min_err:
            min_err = err
            start_i = i
                        
    return min_err, min_err, start_i#vErr, vErr/(N-2*k), aErr, aErr/(N-2*k),start_i

def velocityAccelerationCoefficients(alphas):
    accuracy = len(alphas) - 1
               
    delta = generalFiniteDifference(3,accuracy,alphas,alphas[0])
    velocity_coefficients = delta[1,accuracy]
    acceleration_coefficients = delta[2,accuracy]
        
    return numpy.array(velocity_coefficients), numpy.array(acceleration_coefficients)

def mul(matrix,array):
    res_array = 0
    for i in range(0,len(array)):
        
        res_array = res_array + matrix[i]*array[i]
        
    return res_array

def velocityAcceleration(data, cycle_map, accuracy):
    central_velocity = (len(data)-2*accuracy)*[0]
    central_acceleration = (len(data)-2*accuracy)*[0]
    forward_velocity = (accuracy)*[0]
    forward_acceleration = (accuracy)*[0]
    backward_velocity = (accuracy)*[0]
    backward_acceleration = (accuracy)*[0]
    #dev_map = cycle_map[accuracy:-accuracy]
    
    #Forward/backward finite differences
    for i in range(0,accuracy):
        f_lb = i
        f_ub = i + accuracy
        b_lb = len(data)-(2*accuracy-1) + i
        b_ub = len(data)-(accuracy-1)+i
        
        fvc, fac = velocityAccelerationCoefficients(cycle_map[f_lb:f_ub])
        time_steps = cycle_map[b_lb:b_ub]
        time_steps = time_steps[::-1]
        
        bvc, bac = velocityAccelerationCoefficients(time_steps)
                
        forward_velocity[i] = mul(numpy.array(data[f_lb:f_ub]),fvc)
        forward_acceleration[i] = mul(numpy.array(data[f_lb:f_ub]),fac)
        backward_velocity[i] = mul(numpy.array(data[b_lb:b_ub]),bvc[::-1])
        backward_acceleration[i] = mul(numpy.array(data[b_lb:b_ub]),bac[::-1])
    #Central finite differences
    for i in range(accuracy,len(data)-accuracy):
        lb = i - accuracy
        ub = i + accuracy + 1
                
        alphas = cycle_map[lb:ub]
        alphas = centralCoefficientsOrder(alphas)
        #acc=4 alphas = [alphas[4],alphas[5],alphas[3],alphas[6],alphas[2],alphas[7],alphas[1],alphas[8],alphas[0]]
        #acc=3 alphas = [alphas[3],alphas[4],alphas[2],alphas[5],alphas[1],alphas[6],alphas[0]]
        #acc=2 alphas = [alphas[2],alphas[3],alphas[1],alphas[4],alphas[0]]
        #acc=1 alphas = [alphas[1],alphas[2],alphas[0]]

        dat = data[lb:ub]
        dat = numpy.array(centralCoefficientsOrder(dat))
        #dat = numpy.array([dat[4],dat[5],dat[3],dat[6],dat[2],dat[7],dat[1],dat[8],dat[0]])
        #dat = numpy.array([dat[3],dat[4],dat[2],dat[5],dat[1],dat[6],dat[0]])
        
        delta = generalFiniteDifference(3,accuracy*2,alphas,alphas[0])
        velocity_coefficients = delta[1,accuracy*2]
        acceleration_coefficients = delta[2,accuracy*2]
                
        central_velocity[i-accuracy] = mul(dat,velocity_coefficients)
        central_acceleration[i-accuracy] = mul(dat,acceleration_coefficients)

    central_velocity = numpy.array(central_velocity)
    central_acceleration = numpy.array(central_acceleration)
    forward_velocity = numpy.array(forward_velocity)
    forward_acceleration = numpy.array(forward_acceleration)
    backward_velocity = numpy.array(backward_velocity)
    backward_acceleration = numpy.array(backward_acceleration)
    
    vel = numpy.concatenate((forward_velocity,central_velocity,backward_velocity))
    acc = numpy.concatenate((forward_acceleration,central_acceleration,backward_acceleration))    
    
    return vel,acc

def centralCoefficientsOrder(dat):

    accuracy = int(len(dat)/2)
    tmp = numpy.array(len(dat)*[0])

    if len(dat) % 2 == 1:
        for i in range(0,len(dat)):
            sign = 1 if i % 2 == 1 else -1 
            tmp[i] = dat[accuracy+int((i+1)/2)*sign]
        return tmp
    else:
        
        for i in range(0,len(dat)):
            sign = 1 if i % 2 == 1 else -1 
            tmp[i] = dat[(accuracy-1)+int((i+1)/2)*sign]
            #3 4 2 5 1 6 0 7
        return tmp

def generalFiniteDifference(M,N, alpha, x0):
    
    delta = numpy.zeros((M+1,N+1,N+1))
    delta[0,0,0] = 1
    c1 = 1
    
    for n in range(1,N+1):
        c2 = 1
        for v in range(0,n):
            c3 = alpha[n] - alpha[v]
            c2 = c2 * c3
            
            for m in range(0,min(n,M)+1):
                delta[m,n,v] = (alpha[n]-x0)*delta[m,n-1,v] - m*delta[m-1,n-1,v]
                delta[m,n,v] = delta[m,n,v]/c3
        for m in range(0,min(n,M)+1):
            delta[m,n,n] = m*delta[m-1,n-1,n-1] - (alpha[n-1] - x0)*delta[m,n-1,n-1]
            delta[m,n,n] = c1*delta[m,n,n]/c2
               
        c1 = c2
    return delta
    

def centralFiniteDifference(data,order=1,h=1):
    
    if order==1:
        coefficients = [1/280, -4/105,    1/5,    -4/5,    0,    4/5,    -1/5,    4/105,    -1/280]
    else:
        coefficients = [-1/560,    8/315,    -1/5,    8/5,    -205/72,    8/5,    -1/5,    8/315,    -1/560]
    
    k = math.floor(len(coefficients)/2)
    result = (len(data)-(2*k))*[0]
    
    for i in range(0,len(result)):
        finite_difference = 0
        for j in range(-k,k+1):
            finite_difference = finite_difference + coefficients[j+k]*data[i+j+k]

        result[i] = finite_difference/pow(h,order)
        
    return result

def forwardFiniteDifference(data,h):
    
    coefficients = [-1, 1]
    data = numpy.array(data)
    k = 1
    result = (len(data)-k)*[0]
    
    for i in range(0,len(result)):
        finite_difference = 0
        for j in range(0,k):
            finite_difference = finite_difference + coefficients[j]*data[i+k]

        result[i] = finite_difference/(h[i+1]-h[i])
        
    return result

def printError(file,label, n_cycels, skeleton, points,rme):
    p = list(skeleton)
    for i in range(0, math.ceil(n_cycels)):
        p.extend(skeleton)
        
    esum, emean, ind = timeSequenceError(p, points)        
    #e1, e2, ve1, ve2, ae1, ae2, ind = timeSequenceError(p, points,1/fps)
    print(label, rme[0],rme[1],rme[2],esum, emean, ind) 
    file.write(label + " " + str(rme[0]) + " " +  str(rme[1]) + " " +  str(rme[2]) + " " + str(esum) + " " +  str(emean) + " " + str(ind) + '\n')
    return ind
    
def plotRMSE(rmse1,rmse2,rmse3,cloud_label,name):
    import matplotlib.pylab as plt
    import matplotlib
    font = {'size'   : 18 }
        
    matplotlib.rc('font', **font)
    nplots = 3
    plt.subplot(nplots, 1, 1)

    plt.hist(rmse1,100)
    plt.xlabel("Position error distribution")
    plt.ylabel("Count")
    plt.subplot(nplots, 1, 2)
    #plt.plot(range(0, len(points)), [c for c in V], 'ro')
    plt.hist(rmse2,100)
    plt.xlabel("Velocity error distribution")
    plt.ylabel("Count")
     
    plt.subplot(nplots, 1, 3)
    plt.hist(rmse3,100)
    plt.xlabel("Acceleration error distribution")
    plt.ylabel("Count")
     
    fig = plt.gcf()
    fig.set_size_inches(18.5, 10.5)
       
    plt.draw()
    plt.savefig("plots/" + cloud_label + "/" + name + "rmse.png")
    #plt.savefig("plots/" + cloud_label + "/" + prefix + "_" + dimension_labels[d] + '_regression.png')
    
    plt.close()
    plt.clf()

def create_plots(V, cycle_map, prefix, cloud_label):
    import matplotlib.pylab as plt
    import matplotlib
    font = {'size'   : 24 }
        
    matplotlib.rc('font', **font)
    nplots = 2
    plt.subplot(nplots, 1, 1)

    plt.plot(range(0, len(cycle_map)), [c for c in cycle_map], 'bo')
    plt.xlabel("Frame index")
    plt.ylabel("Circular parameterisation")
    plt.subplot(nplots, 1, 2)
    #plt.plot(range(0, len(points)), [c for c in V], 'ro')
    plt.plot(range(0, len(V)), [c for c in V], 'ro')
      
    fig = plt.gcf()
    fig.set_size_inches(18.5, 10.5)
       
    plt.draw()
    plt.xlabel("Frame index")
    plt.ylabel("Circular parameterisation")
    plt.savefig("plots/" + cloud_label + "/" + prefix + "cycle.png")
    #plt.savefig("plots/" + cloud_label + "/" + prefix + "_" + dimension_labels[d] + '_regression.png')
    
    plt.close()
    plt.clf()
