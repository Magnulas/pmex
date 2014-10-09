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

import bpy
import bpy.utils
import bpy.props

from pmex.model.extractionproperties import ExtractionProperties 
from pmex.operator.extractionoperator import ExtractionOperator 
from pmex.view.extractionpanel import ObjectExtractionPanel
from pmex.view.extractionpanel import PoseExtractionPanel

bl_info = {
    "name": "Periodic Motion Extraction",
    "author": "Magnus Raunio",
    "version": (1, 0),
    "blender": (2, 7, 2),
    "location": "View3D > Tools ",
    "description": "A tool for extracting periodic motions from motion capture data, ex. a walk cycle.",
    "warning": "",
    'wiki_url': "",
    "category": "Object",}

def register():
    bpy.utils.register_class(ExtractionProperties)
    bpy.utils.register_class(ExtractionOperator)
    bpy.utils.register_class(ObjectExtractionPanel)
    bpy.utils.register_class(PoseExtractionPanel)
    #Blender does currently not support properties for all types or custom types so have to store properties in scene
    bpy.types.Scene.extractionProperties = bpy.props.PointerProperty(type=ExtractionProperties)

def unregister():
    bpy.utils.unregister_class(ExtractionProperties)
    bpy.utils.unregister_class(ExtractionOperator)
    bpy.utils.unregister_class(ObjectExtractionPanel)
    bpy.utils.unregister_class(PoseExtractionPanel)


# This allows you to run the script directly from blenders text editor
# to test the addon without having to install it.
if __name__ == "__main__":
    register()
