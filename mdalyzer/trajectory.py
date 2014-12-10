import libmdalyzer

from mdalyzer import compute

class trajectory(object):
    """Base class for Trajectory"""
    def __init__(self):
        self.cpp = libmdalyzer.Trajectory()
        self.attached_computes = []
    
    def analyze(self):
        """Perform Trajectory analysis"""
        self.cpp.analyze()
        

class hoomd(trajectory):
    """HOOMD XML trajectory object"""
    def __init(self):
        trajectory.__init__(self)
        
    def add(self, files):
        """Add frames by file name"""
        if not isinstance(files, list):
            files = [files]
            
        for file in files:            
            frame = libmdalyzer.HOOMDXMLFrame(file)
            self.cpp.addFrame(frame)
            