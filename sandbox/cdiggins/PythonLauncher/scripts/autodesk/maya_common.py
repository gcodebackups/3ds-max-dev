import pymel.core as native
import base
import maya
import maya.utils

class Application(base.Application):
    def __init__(self):
        pass
        
    def write_line(self, text):
        print text

    @property
    def roots(self):
        for x in native.ls(tr=True):
            if x and not _parents(x):
                yield Node(x)
        
    def load_file(self, fname):        
        return native.importFile(fname)
        
    def save_file(self, fname):
        return native.saveAs(fname)
    
    @property
    def product(self):
        return "Autodesk Maya"    

    def add_geometry(self, vertices, indices, name):
        mesh = maya.OpenMaya.MFnMesh()
        mvertices = maya.OpenMaya.MFloatPointArray()
        for v in vertices:
            mv = maya.OpenMaya.MFloatPoint(v[0], v[1], v[2]) 
            mvertices.append(mv)
        mcounts = maya.OpenMaya.MIntArray()
        for i in xrange(len(indices) / 3):
            mcounts.append(3)
        mindices = maya.OpenMaya.MIntArray()
        for i in indices:
            mindices.append(i)
        node = mesh.create(mvertices.length(), mcounts.length(), mvertices, mcounts, mindices)
        return Node(node)
        
class Node(base.Node):
    def __init__(self, node):
        self._node = node        
    
    @property
    def children(self):
        for x in native.listRelatives(self._node, type='transform'):
            if x:
                yield Node(x)

    @property
    def name(self):
        return self._node.nodeName()
    
    @name.setter  
    def set_name(self, value):
        native.rename(self._node, value)
    
    @property
    def parent(self):
        xs = native.listRelatives(self._node, parent=True)
        if len(xs) < 1: return None
        if not xs[0]: return None
        return Node(xs[0])
    
    @property
    def transform(self):
        return _mat_to_tuple(self._node.transformationMatrix())
                
    @property
    def element(self):
        shp = self._node.getShape()
        if not shp: return None
        if not shp.type() == 'mesh': return None
        return GeometricObject(shp)

    @property
    def selected(self):
        return self._node in native.selected()

    @selected.setter
    def selected(self, value):
        if value:
            native.select(self._node, add=True)
        else:
            native.select(self._node, deselect=True)
                    
class GeometricObject(base.GeometricObject):
    def __init__(self, shape):        
        self._shape = shape
        
    @property 
    def mesh(self):
        return Mesh(self._shape)
        
    @property
    def name(self):
        return self._shape.nodeName()
    
    @name.setter  
    def set_name(self, value):
        native.rename(self._shape, value)

class Camera(base.Camera):
    def __init__(self, cam):
        self._cam = cam

    @property
    def is_ortho(self):
        return False

    @property
    def field_of_view(self):
        return self._cam.getHorizontalFieldOfView()

    @property
    def vertical_field_of_view(self):
        return self._cam.getVerticalFieldOfView()

    @property
    def aspect_ratio(self):
        return self._cam.getAspectRatio()

    @property 
    def near_clip(self):
        return self._cam.getNearClipPlane()

    @property
    def far_clip(self):
        return self._cam.getFarClipPlane()

class Mesh(base.Mesh):              
    def __init__(self, mesh):
        # HACK: I should really do the triangulation in memory rather than changing the scene.
        # However, this is fine for now. 
        native.polyTriangulate(mesh)
        self.vertices = tuple(tuple(v) for v in mesh.getPoints())
        self.indices = tuple(self._compute_indices(mesh))
        self.uvs = tuple(self._compute_uvs(mesh))
        self.normals = tuple(self._compute_normals(mesh))
        
    def _compute_indices(self, mesh):
        for f in mesh.faces:
            vxs = f.getVertices()
            for i in xrange(3):
                yield vxs[i]            
            
    def _compute_uvs(self, mesh):
        for f in mesh.faces:
            for i in xrange(3):
                yield tuple(f.getUV(i))
                
    def _compute_normals(self, mesh):
        for f in mesh.faces:
            normals = f.getNormals()
            for i in xrange(3):
                yield tuple(normals[i])

def _mat_to_tuple(m):
    return tuple(tuple(r) for r in m)

def _parents(x):
    return native.listRelatives(x, parent=True)

''' This is a singleton representing the application '''
app = Application()

def _do_nothing():
    pass
