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
    def __init__(self, dt=1.0):
        if dt <= 0.0:
            raise RuntimeError("HOOMDXMLTrajectory timestep must be positive")
        self.cpp = libmdalyzer.HOOMDXMLTrajectory(dt)

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
            
class dcd(trajectory):
    """DCD trajectory object
       Make sure that all other trajectory classes preceed the dcd class, 
       as it depends on all others
    """
    def __init__(self, dcd_file, i_file, i_type=None, precision=3, time_step=0, freq=0):
        self.dcd_file = dcd_file
        self.i_file = i_file
        self.i_file_ptr = None
        if (i_type == None):
            i_type = self.parseFileName()
        self.i_type = i_type
        # create pointer to that trajectory
        self.i_file_ptr = self.getTraj(i_type, time_step, precision)
        # add the file frame to that pointer
        self.i_file_ptr.addFile(i_file)

        self.cpp = libmdalyzer.DCDTrajectory(self.i_file_ptr, self.dcd_file, time_step, freq)

    def _notAtype(self):
        err_str = ('The file type \'%s\' does not exist. Maybe you meant one of these:\n' % self.i_type)
        for t_type in self.traj_types:
            err_str += (t_type + " ")
        raise RuntimeError(err_str)

    def parseFileName(self):
        self.traj_types = ['HOOMDXML','XML', 'xml',
                           'GRO', 'gro',
                           'PDB', 'pdb',
                           'XYZ', 'xyz']
        extension = self.i_file.split('.')
        for t_type in self.traj_types:
            if ( t_type in extension ):
                return t_type
        self._notAtype
        return None

    def getTraj(self, t_type, time_step, precision):
        if ( t_type in ['HOOMDXML', 'XML', 'xml'] ):
            return libmdalyzer.HOOMDXMLTrajectory(time_step)

        elif ( t_type in ['GRO', 'gro'] ):
            return libmdalyzer.GROTrajectory(precision)

        elif ( t_type in ['PDB', 'pdb'] ):
            return libmdalyzer.PDBTrajectory(time_step)

        elif ( t_type in ['XYZ', 'xyz'] ):
            return libmdalyzer.XYZTrajectory()
        
        else :
            self._notAtype()

