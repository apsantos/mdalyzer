import libmdalyzer

class analyzer(object):
    counter = 0
    def __init__(self, trajectory, file_name, name):
        self.trajectory = trajectory
        self.file_name = file_name
        
        if name is None:
            name = str(analyzer.counter)
            analyzer.counter += 1
        self.name = name

class density(analyzer):
    """Density profile analyzer"""
    
    def __init__(self, traj, file_name='density', nx=0, ny=0, nz=0, name=None, types=[], weight=True):
        analyzer.__init__(self, traj, file_name, name)
        self.bins = libmdalyzer.Vector3uint(nx,ny,nz)
        
        self.cpp = libmdalyzer.DensityProfile(self.trajectory.cpp, self.file_name, self.bins)
        self.trajectory.cpp.addCompute(self.cpp, self.name)
        
        self.types = []
        self.weight = True
        
        if not isinstance(types, (list,tuple)):
            types = [types]
        self.add_type(types)
        self.mass_weight(weight)
    
    def add_type(self, types):
        """Add types to calculate"""
        if not isinstance(types, (list,tuple)):
            types = [types]
            
        for t in types:
            if t not in self.types:
                self.types += [t]
                self.cpp.addType(t)
    
    def delete_type(self, types):
        """Remove types to calculate"""
        if not isinstance(types, (list,tuple)):
            types = [types]
            
        for t in types:
            self.types.remove(t)
            self.cpp.deleteType(t)
    
    def mass_weight(self, weight):
        """Set mass weighting on or off"""
        if weight is None:
            weight = False
        
        if not isinstance(weight, bool):
            raise RuntimeError('Mass weighting must be True or False')
        
        self.weight = weight
        self.cpp.useMassWeighting(self.weight)
    
class clustering(analyzer):
    """Clustering analysis"""
    def __init__(self, traj, file_name='clustering.dat', distance=1.0, name=None):
        analyzer.__init__(self, traj, file_name, name)
        
        self.distance = distance
        self.cpp = libmdalyzer.Clustering(self.trajectory.cpp, self.file_name, self.distance)
        self.trajectory.cpp.addCompute(self.cpp, self.name)
    
    def set_distance(self, distance):
        """Set interatomic distance defining particles in a cluster"""
        self.distance = distance
        self.cpp.setDistance(self.distance)

class meanSquaredDisplacement(analyzer):
    """Density profile analyzer"""
    
    def __init__(self, traj, file_name='msd', origins='1', name=None, types=[]):
        analyzer.__init__(self, traj, file_name, name)
        self.origins = origins
        
        self.cpp = libmdalyzer.MeanSquaredDisplacement(self.trajectory.cpp, self.file_name, self.origins)
        self.trajectory.cpp.addCompute(self.cpp, self.name)
        
        self.types = []
        
        if not isinstance(types, list):
            types = [types]
        self.add_type(types)
    
    def add_type(self, types):
        """Add types to calculate"""
        if not isinstance(types, list):
            types = [types]
            
        for t in types:
            if t not in self.types:
                self.types += [t]
                self.cpp.addType(t)
    
    def delete_type(self, types):
        """Remove types to calculate"""
        if not isinstance(types, list):
            types = [types]
            
        for t in types:
            self.types.remove(t)
            self.cpp.deleteType(t)
    
