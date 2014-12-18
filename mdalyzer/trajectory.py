import libmdalyzer

class trajectory(object):
    """Base class for Trajectory"""
    def __init__(self):
        self.cpp = None
    
    def analyze(self):
        """Perform Trajectory analysis"""
        self.cpp.analyze()
        

class hoomd(trajectory):
    """HOOMD XML trajectory object"""
    def __init__(self):
        self.cpp = libmdalyzer.HOOMDXMLTrajectory()
        
    def add(self, files):
        """Add frames by file name"""
        if not isinstance(files, list):
            files = [files]
            
        for file in files:            
            self.cpp.addFile(file)
            
