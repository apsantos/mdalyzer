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
            
class dcd(trajectory):
    """DCD trajectory object
       Make sure that all other trajectory classes preceed the dcd class, 
       as it depends on all others
    """
    def __init__(self, dcd_file, i_file, i_type=None):
        self.traj_types = {
                'HOOMDXML':libmdalyzer.HOOMDXMLTrajectory(), 
                'XML':libmdalyzer.HOOMDXMLTrajectory(),
                'xml':libmdalyzer.HOOMDXMLTrajectory()}
        self.dcd_file = dcd_file
        self.i_file = i_file
        self.i_file_ptr = None
        self.i_type = i_type
        if (i_type != None):
            if ( self.i_type in self.traj_types ):
                # loop over the avilable formats
                for t_type in self.traj_types:
                    if ( t_type.lower() == self.i_type.lower() ):
                        # create pointer to that trajectory
                        self.i_file_ptr = self.traj_types[t_type]
                        # add the file frame to that pointer
                        self.i_file_ptr.addFile(i_file)

            # if type is incompatible   
            else:
                self._notAtype()

        else:
            extension = self.i_file.split('.')
            # loop over the avilable formats
            for t_type in self.traj_types:
                if ( t_type.lower() in extension ):
                    self.i_type = t_type.lower()
                    # create pointer to that trajectory
                    self.i_file_ptr = self.traj_types[t_type]
                    # add the file frame to that pointer
                    self.i_file_ptr.addFile(i_file)

                # if type is incompatible   
            if (self.i_type==None):
                    self._notAtype()

        self.cpp = libmdalyzer.DCDTrajectory(self.i_file_ptr, self.dcd_file)

    def _notAtype(self) :
        err_str = ('The file type \'%s\' does not exist. Maybe you meant one of these:\n' % self.i_type)
        for t_type in self.traj_types:
            err_str += (t_type + " ")
        raise RuntimeError(err_str)
        return
        
    def add(self, files):
        """Add frames by file name"""
        if not isinstance(files, list):
            files = [files]
            
        for file in files:            
            self.cpp.addFile(file)
            
