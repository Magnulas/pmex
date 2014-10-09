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

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Button, LassoSelector
from matplotlib.path import Path

class PersistenDiagramWidget():

    window = None
    ax = None
    selector = None
    button = None
    
    def __init__(self, births,deaths):
        self.window, self.ax = plt.subplots()
        #plt.subplots_adjust(left=0.25, bottom=0.25)

        #plot barcodes
        #lines = plt.hlines(range(0,len(births)),births,deaths)
        #plt.axis([0, max(deaths), -1, len(births)+1])
        #print(lines)
        pts = self.ax.scatter(births,deaths)

        mx = max(births)*1.1
        my = max(deaths)*1.1
        ma = max(mx,my)
        self.ax.axis([0, ma, 0, ma])  
        
        self.ax.plot([0,ma],[0,ma],'k')
        self.ax.set_title('Pick the cocycles you wish to perform motion extraction for')
        self.ax.set_ylabel('Death')
        self.ax.set_xlabel('Birth')

        self.selector = SelectFromCollection(self.ax, pts)
        resetax = plt.axes([0.8, 0.025, 0.1, 0.04])
        self.button = Button(resetax, 'Done', color='lightgoldenrodyellow', hovercolor='0.975')
        
        self.button.on_clicked(self.close)
        
    def execute(self):
        plt.show()
        indices = self.selector.ind
        self.selector.disconnect()
        return indices
        
    def close(self,event):
        plt.close(self.window)

class SelectFromCollection(object):
    """Select indices from a matplotlib collection using `LassoSelector`.

    Selected indices are saved in the `ind` attribute. This tool highlights
    selected points by fading them out (i.e., reducing their alpha values).
    If your collection has alpha < 1, this tool will permanently alter them.

    Note that this tool selects collection objects based on their *origins*
    (i.e., `offsets`).

    Parameters
    ----------
    ax : :class:`~matplotlib.axes.Axes`
        Axes to interact with.

    collection : :class:`matplotlib.collections.Collection` subclass
        Collection you want to select from.

    alpha_other : 0 <= float <= 1
        To highlight a selection, this tool sets all selected points to an
        alpha value of 1 and non-selected points to `alpha_other`.
    """
       
    def __init__(self, ax, collection,selected_color=(0,0,1,1),unselected_color=(1,0,0,1)):
        self.canvas = ax.figure.canvas
        self.collection = collection
        self.unselected_color = unselected_color
        self.selected_color = selected_color
        self.shift_is_held = False
        self.xys = collection.get_offsets()
        self.Npts = len(self.xys)

        self.canvas.mpl_connect('key_press_event', self.on_key_press)
        self.canvas.mpl_connect('key_release_event', self.on_key_release)


        # Ensure that we have separate colors for each object
        self.fc = collection.get_facecolors()
        if len(self.fc) == 0:
            raise ValueError('Collection must have a facecolor')
        elif len(self.fc) == 1:
            self.fc = np.tile(self.fc, self.Npts).reshape(self.Npts, -1)

        lineprops = dict(color='black', linestyle='-',linewidth = 2, alpha=1)
        self.lasso = LassoSelector(ax, onselect=self.onselect,lineprops=lineprops)
        self.ind = []

    def on_key_press(self, event):
        if event.key == 'shift':
            self.shift_is_held = True

    def on_key_release(self, event):
        if event.key == 'shift':
            self.shift_is_held = False

    def onselect(self, verts):
        path = Path(verts)
        
        old_ind = self.ind
        
        self.ind = np.nonzero([path.contains_point(xy) for xy in self.xys])[0]
        
        if self.shift_is_held:
            self.ind = np.unique(np.append(self.ind, old_ind))
        
        self.fc[:, :] = self.unselected_color
        self.fc[self.ind, :] = self.selected_color
        self.collection.set_facecolors(self.fc)
        self.canvas.draw_idle()

    def disconnect(self):
        self.lasso.disconnect_events()
        self.fc[:, -1] = 1
        self.collection.set_facecolors(self.fc)
        self.canvas.draw_idle()        
