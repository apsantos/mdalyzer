import libmdalyzer

class trajectory(object):
    """Base class for Trajectory"""
    def __init__(self):
        self.cpp = None
    
    def analyze(self):
        """Perform Trajectory analysis"""
        self.cpp.analyze()
        
    def add(self, files):
        """Attach files"""
        if not isinstance(files, (list,tuple)):
            files = [files]
        
        for file in files:
            self.cpp.addFile(file)   
            
    def set_box(self, lx=0.0, ly=0.0, lz=0.0, xy=0.0, xz=0.0, yz=0.0):
        if self.cpp is None:
            raise RuntimeError('Box cannot be set without constructing a Trajectory')
        self.box = libmdalyzer.TriclinicBox(libmdalyzer.Vector3d(lx,ly,lz), libmdalyzer.Vector3d(xy,xz,yz))
        self.cpp.setBox(self.box)
#     def set_box(v1, v2, v3):
#         if len(v1) != 3 or len(v2) != 3 or len(v3) != 3:
#             raise RuntimeError('Box is defined by three 3d vectors')
#         
#         self.box = libmdalyzer.TriclinicBox(libmdalyzer.Vector3d(v1[0], v1[1], v1[2]),
#                                             libmdalyzer.Vector3d(v2[0], v2[1], v2[2]),
#                                             libmdalyzer.Vector3d(v3[0], v3[1], v3[2]))
#         self.cpp.setBox(self.box)
        

class hoomd(trajectory):
    """HOOMD XML trajectory"""
    def __init__(self):
        self.cpp = libmdalyzer.HOOMDXMLTrajectory()

class gro(trajectory):
    """GRO file trajectory"""
    def __init__(self,files=None,precision=3):
        self.cpp = libmdalyzer.GROTrajectory(precision)
        
        if files is not None:
            self.add(files)
            
class xyz(trajectory):
    """XYZ file trajectory"""
    def __init__(self, files=None):
        self.cpp = libmdalyzer.XYZTrajectory()
            
        if files is not None:
            self.add(files)

class pdb(trajectory):
    """PDB file trajectory"""
    def __init__(self, files=None, time_step=1.0):
        self.cpp = libmdalyzer.PDBTrajectory(time_step)
            
        if files is not None:
            self.add(files)
