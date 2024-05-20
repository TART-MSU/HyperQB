# -*- coding: utf-8 -*-
# Copyright (C) by the Spot authors, see the AUTHORS file for details.
#
# This file is part of Spot, a model checking library.
#
# Spot is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# Spot is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
# License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""
Auxiliary functions for Spot's Python bindings.
"""

from IPython.display import display, HTML, DisplayObject

class SVG(DisplayObject):
    """Replacement for IPython.display.SVG that does not use
    minidom to extract the <svg> element.

    We need that because prior to Python 3.8, minidom used sort all
    attributes, and in Python 3.8 this was changed to keep the same
    order, causing test failures in our diff-based test suite.

    We do not need the <svg> extraction when processing
    GraphViz output.

    Also nowadays Jupyter Notebook 7 started to render <svg> as inline
    <img> instead of inlining the <svg> directly, breaking many useful
    usages of SVG in the name of easier copy/paste.

    https://github.com/jupyter/notebook/issues/7114
    https://github.com/jupyterlab/jupyterlab/issues/10464
    """
    def _repr_html_(self):
        return self.data

def display_inline(*args, per_row=None, show=None):
    """
    This is a wrapper around IPython's `display()` to display multiple
    elements in a row, without forced line break between them.

    If the `per_row` argument is given, at most `per_row` arguments are
    displayed on each row, each one taking 1/per_row of the line width.
    """
    w = res = ''
    if per_row:
        w = f'width:{100//per_row}%;'
    for arg in args:
        dpy = 'display:inline-block'
        if show is not None and hasattr(arg, 'show'):
            arg = arg.show(show)
        if hasattr(arg, '_repr_html_'):
            rep = arg._repr_html_()
        elif hasattr(arg, '_repr_svg_'):
            rep = arg._repr_svg_()
        elif hasattr(arg, '_repr_latex_'):
            rep = arg._repr_latex_()
            if not per_row:
                dpy = 'display:inline'
        else:
            rep = str(arg)
        res += f"<div style='vertical-align:text-top;{dpy};{w}'>{rep}</div>"
    display(HTML(res))
