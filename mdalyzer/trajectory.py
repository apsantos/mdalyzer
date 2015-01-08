##
# \file trajectory.py
# \author Michael P. Howard
# \author Andrew P. Santos
# \date 6 Jaunary 2015
# \brief Implementation of Python wrappers to the Trajectory methods
import libmdalyzer

##
# \package mdalyzer.trajectory
# \brief Defines Python wrappers to the Trajectory methods
#
# Trajectories read and store simulation data in memory. Each implementation of trajectory contains methods
# to parse a specific data format.

##
# \brief Abstract definition of a Trajectory
#
# All trajectories contain a C++ shared pointer with the Trajectory object.
class trajectory(object):
    ## \internal
    # \brief Null initialize an object
    def __init__(self):
        self.cpp = None
    
    ##
    # \brief Initiate analysis on a fully configured trajectory and analyzer
    #
    # An exception is caught and raised on error.
    #
    # \b Example:
    # \code
    # # simple density calculation using the HOOMD XML trajectory
    # t = trajectory.hoomd()
    # t.add(files=['foo.xml','bar.xml'])
    # analyzer.density(traj=t, nx=5)
    # t.analyze()
    # \endcode
    def analyze(self):
        self.cpp.analyze()

    ##
    # \brief Attach files to a trajectory for parsing
    # \param files string or Python list of files to parse
    #
    # \b Example:
    # \code
    # t = trajectory.hoomd()
    # t.add(files='fizz.xml')
    # t.add(files=['bizz.xml','buzz.xml'])
    # \endcode
    def add(self, files):
        if not isinstance(files, (list,tuple)):
            files = [files]
        
        for file in files:
            self.cpp.addFile(file)   

    ##
    # \brief Override the trajectory triclinic simulation box to a constant value.
    # \param lx \f$ x \f$ edge length
    # \param ly \f$ y \f$ edge length
    # \param lz \f$ z \f$ edge length
    # \param xy \f$ xy \f$ tilt factor
    # \param xz \f$ xz \f$ tilt factor
    # \param yz \f$ yz \f$ tilt factor
    #
    # A triclinic simulation box is defined by three edge lengths \f$(L_x, L_y, L_z)\f$ and three tilt factors
    # \f$(xy, xz, yz)\f$. The tilt factors are dimensionless and define how far an orthorhombic box must be distorted
    # in order to arrive at the triclinic box, and are named based on the angle between vectors. A value of zero means
    # a \f$ 90^\circ \f$ angle.
    #
    # \b Example:
    # \code
    # # set an orthorhombic box for XYZ trajectory
    # t = trajectory.xyz(files='foo.xyz')
    # t.set_box(lx=10.0, ly=5.0, lz=22.0)
    #
    # # set an 2D rectangular box
    # t.set_box( lx=10.0, ly=10.0)
    #
    # # set an Box with a tilt
    # t.set_box( lx=10.0, ly=10.0, lz=5.0, xy=0.5, yz=0.2)
    # \endcode
    def set_box(self, lx, ly, lz, xy=0.0, xz=0.0, yz=0.0):
        if self.cpp is None:
            raise RuntimeError('Box cannot be set without constructing a Trajectory')
        self.box = libmdalyzer.TriclinicBox(libmdalyzer.Vector3d(lx,ly,lz), libmdalyzer.Vector3d(xy,xz,yz))
        self.cpp.setBox(self.box)
        
##
# \brief HOOMD XML reader
#
# Reads HOOMD XML 1.5 file format.
#
# HOOMD XML consists of several required sections, which are defined in the
# <a href="https://codeblue.umich.edu/hoomd-blue/doc/page_xml_file_format.html">HOOMD documentation</a>.
# The first line of the file identifies as XML
# \code{.xml}
# <?xml version="1.0" encoding="UTF-8"?>
# \endcode
# The root XML nodes are
# \code{.xml}
# <hoomd_xml version="1.5">
# <configuration time_step="0">
# <!-- frame data -->
# </configuration>
# </hoomd_xml>
# \endcode
# The configuration node \b must contain the time step, which HOOMD defines to be the integer id of the frame.
# The simulation box is then defined in the configuration
# \code{.xml}
# <box lx="11" ly="5" lz="8" xy="0" xz="0" yz="0" />
# \endcode
# Support is provided for triclinic boxes through the appropriate tilt factors.
# Particle data is then contained in the appropriate nodes. Currently, the following are supported:
# \code{.xml}
# <position>
# <image>
# <velocity>
# <type>
# <diameter>
# <mass>
# \endcode
# Images are used to unwrap positions if supplied. HOOMD types are interpreted as string names in the mdalyzer
# nomenclature, which are then paired with a numerical type id.
# \sa trajectory.set_box
class hoomd(trajectory):
    ## Initialize a HOOMD XML trajectory
    # \param dt Simulation time step to use to scale frame id
    #
    # HOOMD-blue outputs the frame id as the time step instead of current simulation time. For some calculations, it
    # is important to use the actual time instead of frame id. The time step will be scaled by dt.
    #
    # \b Example:
    # \code
    # # create a trajectory with simulation timestep dt = 0.005
    # t = trajectory.hoomd(dt=0.005)
    # # attach files to the trajectory
    # t.add(['frame.0.xml','frame.1.xml'])
    # \endcode
    def __init__(self, dt=1.0):
        if dt <= 0.0:
            raise RuntimeError("HOOMDXMLTrajectory timestep must be positive")
        self.cpp = libmdalyzer.HOOMDXMLTrajectory(dt)

##
# \brief GRO file reader
#
# Reads extended GROMACS GRO files.
#
# GRO is a fixed column file format inherited in GROMACS. It contains atom names, numbers, positions, and velocities,
# as well as a definition of a triclinic simulation box and the simulation time.
#
# \b Example:
# \code{.xml}
# A comment line ending in the *required* time, t=2.5
#   2
#     1RESID    AA    1   1.000   2.000   3.000  3.0000 -2.0000  1.0000
#     1RESID   BBB    2  -3.000  -2.000  -1.000 -1.0000  2.0000 -3.0000
#    11.0   5.0   8.0
# \endcode
# The timestep \b must be specified in the comment line using t=. The next line sets the number of particles \f$N\f$,
# and is followed by \f$N\f$ lines of particle data. Particle numbering ranges from 1 to \f$ N \f$,
# and is strictly column formatted as follows:
# -# Columns   1-5: residue id (ignored)
# -# Columns  6-10: residue name (ignored)
# -# Columns 11-15: atom name
# -# Columns 16-20: atom number
#
# If atom numbers are not specified, the atoms will be automatically numbered in the order they are defined.
# The particle positions and velocities then follow. If there are \f$ n \f$ fixed decimals of precision for the
# positions, then the positions and velocities are each given \f$ n + 5 \f$ columns, including sign and decimal.
# Velocities must have \f$n+1\f$ decimal places.
# The default precision is \f$n=3\f$ since this is the default GROMACS output.
#
# The final line defines the simulation box with no strict formatting (other than white space between entries).
# If only three entries are present, an orthorhombic box is defined with edges \f$L_x\f$, \f$L_y\f$, and \f$L_z\f$.
# However, a triclinic box may be defined by adding entries to define three arbitrary lattice vectors. The order for
# these entries is:
# \f[ v_1(x) ~v_2(y) ~v_3(z) ~v_1(y) ~v_1(z) ~v_2(x) ~v_2(z) ~v_3(x) ~v_3(y) \f]
# These three lattice vectors are then converted to edge lengths and tilt factors.
#
# A single gro file may contain multiple frames. These frames may be separated by an arbitrary amount of empty lines.
# However, no empty lines are permitted once a comment line begins.
class gro(trajectory):
    ## Initialize a GRO trajectory
    # \param files File or files to attach
    # \param precision Number of decimal places for position entries
    #
    # \b Example:
    # \code
    # # create a trajectory from a single file
    # t = trajectory.gro(files='trajectory.gro')
    # 
    # # create a trajectory from multiple files with 5 decimals of precision
    # t = trajectory.gro(files=['frame1.gro','frame2.gro'], precision=5)
    # \endcode
    def __init__(self,files=None,precision=3):
        self.cpp = libmdalyzer.GROTrajectory(precision)
        
        if files is not None:
            self.add(files)
            
##
# \brief XYZ file reader
#
# Reads XYZ files with types.
#
# XYZ is a tabular file format. It contains atom names and positions, and an optional definition of the simulation
# timestep. A single xyz file may contain multiple simulation frames, which can be separated by an arbitrary number
# of empty lines. A single frame contains the following:
# -# A line containing the number of particles \f$N\f$
# -# A comment line, optionally containing the simulation timestep using t=
# -# \f$N\f$ lines containing the particle name and position separated by white space
#
# \b Example:
# \code{.xml}
# 2
# A comment optionally including t=2.5
# AA 1.000 2.0 3.0
# BBB -3.0 -2.000  -1.0
# \endcode
#
# XYZ does not store the simulation box, so it should be fixed to a constant value using xyz.set_box. Consequently,
# it is not advised to use XYZ to store and analyze data from simulations where the box dimension fluctuates.
class xyz(trajectory):
    ## Initialize a XYZ trajectory
    # \param files File or files to attach
    #
    # \b Example:
    # \code
    # # create a trajectory from a single file
    # t = trajectory.xyz(files='trajectory.xyz')
    # t.set_box(lx=11.0, ly=5.0, lz=8.0)
    # 
    # # create a trajectory from multiple files
    # t = trajectory.xyz(files=['frame1.xyz','frame2.xyz'])
    # \endcode
    def __init__(self, files=None):
        self.cpp = libmdalyzer.XYZTrajectory()
            
        if files is not None:
            self.add(files)

##
# \brief PDB file reader
#
# Reads PDB files with types.
#
# PDB is a fixed column file format commonly found in biological science to study proteins. It is also an output of the
# GROMACS simulation package. Among other pieces of information, it contains the particle name, number, and position,
# as well as a definition of a triclinic simulation box. No information about the simulation timestep is stored.
# A PDB file contains a single CRYST1 record (in columns 1-6), which defines the simulation box. The relevant entries
# are:
# -# 7-15: \a a lattice constant (3 decimals)
# -# 16-24: \a b lattice constant (3 decimals)
# -# 25-33: \a c lattice constant (3 decimals)
# -# 34-40: \f$\alpha\f$ angle (2 decimals)
# -# 41-47: \f$\beta\f$ angle (2 decimals)
# -# 48-54: \f$\gamma\f$ angle (2 decimals)
#
# Multiple frames may then be contained in a single file (using this same CRYST1 box). Each frame must be initiated
# with a MODEL record. The MODEL serial number is then stored in columns 11-14.
# Immediately following the MODEL record, ATOM or HETATM records contain the particle data.
# The relevant entries of the particle data are:
# -# 7-11: atom number
# -# 13-16: atom name
# -# 31-38: \f$x\f$ coordinate (3 decimals)
# -# 39-46: \f$y\f$ coordinate (3 decimals)
# -# 47-54: \f$z\f$ coordinate (3 decimals)
#
# Each frame is then ended with a ENDMDL record. Because PDB files do not contain information about the timestep,
# the simulation time will be extracted from the MODEL number by multiplying against a set timestep between frames.
# Other records in the PDB file will be ignored. Currently, TER is not supported for separating chain topology, and
# will not be recognized as a frame separator.
#
# \b Example:
# \code{.xml}
# TITLE     PDB test
# REMARK    THIS IS A SIMULATION BOX
# CRYST1   11.000    5.000    8.000  90.00  90.00  90.00 P 1           1
# MODEL        0
# ATOM      1   A          1       0.000   0.000   0.000  1.00  0.00
# ATOM      2   B          1       1.500   0.000   0.000  1.00  0.00
# ENDMDL
# MODEL        1
# ATOM      1   A          1       0.000   0.000   0.000  1.00  0.00
# ATOM      2   B          1      -2.500   0.000   0.000  1.00  0.00
# ENDMDL
# \endcode
class pdb(trajectory):
    ## Initialize a PDB trajectory
    # \param files File or files to attach
    # \param time_step Time between frames to scale MODEL
    #
    # \b Example:
    # \code
    # # create a trajectory from a single file with time spacing 0.005
    # t = trajectory.pdb(files='trajectory.pdb', time_step=0.005)
    # 
    # # create a trajectory from multiple files
    # t = trajectory.pdb(files=['frame1.xyz','frame2.xyz'])
    # \endcode
    def __init__(self, files=None, time_step=1.0):
        self.cpp = libmdalyzer.PDBTrajectory(time_step)

        if files is not None:
            self.add(files)
            
## Wraps to a DCD file trajectory
# 
#  Binary file with multiple timestep configurations
#  
class dcd(trajectory):
    """DCD trajectory object
       Make sure that all other trajectory classes preceed the dcd class, 
       as it depends on all others
    """
    ## \internal
    # \brief Initialize dcd trajectory
    # \param dcd_file trajectory file
    # \param i_file initial trajectory/Frame file name needed for DCD
    # \param i_type initial trajectory/Frame file type, defalts to None, and tries to find from the file name extensions
    # \param precision Precision of the data needed if it is a GRO file, defalts to 3
    # \param time_step trajectory timestep, defalts to 0 and will be determined by the DCD reader
    # \param freq Number of timesteps between each DCD configuration, defalts to 0 and will be determined by the DCD reader
    #
    # \b Examples:
    # \code
    # # use the DCD file's output frequency and time step, with an xml initial frame
    # t = trajectory.dcd( dcd_file='frames.dcd', i_file='frame1.xml')
    #
    # # use a GRO initial configuration with 4 Precision
    # t = trajectory.dcd( dcd_file='frames.dcd', i_file='frame1.gro', precision=4)
    #
    # # Define the output frequency output every 1000 timesteps, with a 0.001 (unit) timestep
    # t = trajectory.dcd( dcd_file='frames.dcd', i_file='frame1.xyz', time_step=0.001, freq=1000)
    #
    # # Define the ininitail file type
    # t = trajectory.dcd( dcd_file='frames.dcd', i_file='frame1.pdb', i_type='pdb')
    # \endcode
    #
    def __init__(self, dcd_file, i_file, i_type=None, precision=3, time_step=0, freq=0):
        self.dcd_file = dcd_file
        self.i_file = i_file
        self.i_file_ptr = None
        self.traj_types = ['HOOMDXML','XML', 'xml',
                           'GRO', 'gro',
                           'PDB', 'pdb',
                           'XYZ', 'xyz']
        if (i_type == None):
            i_type = self._parseFileName()
        self.i_type = i_type
        # create pointer to that trajectory
        self.i_file_ptr = self._getTraj(i_type, time_step, precision)
        # add the file frame to that pointer
        self.i_file_ptr.addFile(i_file)

        self.cpp = libmdalyzer.DCDTrajectory(self.i_file_ptr, self.dcd_file, time_step, freq)
    ## \var trajectory.dcd.i_file_ptr
    # \internal
    # \brief boost shared_ptr to a trajectory object of the initial trajectory
    #  Needed for the DCDTrajectory constructor
            

    ## \internal
    # \brief Raises an error the initial file type is incorrect
    #
    def _notAtype(self):
        err_str = ('The file type \'%s\' does not exist. Maybe you meant one of these:\n' % self.i_type)
        for t_type in self.traj_types:
            err_str += (t_type + " ")
        raise RuntimeError(err_str)

    ## \internal
    # \brief Parses the initial file name to get the file type
    #
    def _parseFileName(self):
        extension = self.i_file.split('.')
        for t_type in self.traj_types:
            if ( t_type in extension ):
                return t_type
        self._notAtype()
        return None

    ## \internal
    # \brief Parses the initial file name to get the file type
    # \param t_type initial trajectory/Frame file type
    # \param time_step trajectory timestep, for the initial trajectory pointer initialization
    # \param precision Data precision, for the initial trajectory pointer initialization
    #
    def _getTraj(self, t_type, time_step, precision):
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
            
