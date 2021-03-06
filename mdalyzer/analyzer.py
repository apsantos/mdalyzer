##
# \file analyzer.py
# \author Michael P. Howard
# \author Andrew P. Santos
# \date 8 Jaunary 2015
# \brief Implementation of Python wrappers to the Analyzer methods

import libmdalyzer

##
# \package mdalyzer.analyzer
# \brief Defines Python wrappers to the Analyzer methods
#
# Analyzers compute physical properties of simulations. They only have meaning when constructed attached to a
# trajectory, which contains the simulation data.

## \internal
# \brief Abstract definition of an Analyzer
# 
# All analyzers are assigned a string name by which they are saved
class analyzer(object):
    ##
    # \internal
    # \brief A counter to track the autonaming of added analyzer if a name is not supplied
    counter = 0
    
    ## \internal
    # \brief initializes an analyzer
    # \param trajectory Python trajectory object to which analyzer is attached
    # \param file_name the output file for the analyzer
    # \param name (if set) the string identifier for the file
    #
    # If a name is not set, one will automatically be generated
    def __init__(self, trajectory, file_name, name):
        self.trajectory = trajectory
        self.file_name = file_name
        
        if name is None:
            name = str(analyzer.counter)
            analyzer.counter += 1
        self.name = name
    
    ## \var trajectory
    # \internal
    # \brief Python object that analyzer is attached to
    # \sa trajectory.trajectory
    
    ## \var file_name
    # \internal
    # \brief output file as a string
    
    ## \var name
    # \internal
    # \brief the string key for the analyzer

## Computes the time-averaged %density profile
# 
# The time averaged %density profile \f$ \rho \f$ is computed by binning particles by type
# after projection into an orthorhombic box. The simulation box is divided into a certain number of bins along each
# coordinate axis, and the number or mass %density is computed.
class density(analyzer):
    ## Initialize a %density profile %analyzer
    # \param traj Python trajectory to attach to
    # \param file_name the stub output file name (will have extension auto-attached)
    # \param nx Number of bins along \f$ x \f$
    # \param ny Number of bins along \f$ y \f$
    # \param nz Number of bins along \f$ z \f$
    # \param name (if set) unique string name for the analyzer
    # \param types (if set) list of particle names to compute density for, average of all particles is used if unset
    # \param weight if true, compute the mass density, otherwise the number density
    #
    # \b Example:
    # \code
    # # compute average mass density along y using 12 bins
    # analyzer.density(traj=my_traj, ny=12)
    #
    # # compute number density of type Q along x and z
    # analyzer.density(traj=my_traj, file_name='num_dens', nx=5, nz=7, types='Q', weight=False)
    # \endcode
    def __init__(self, traj, file_name='density', nx=0, ny=0, nz=0, name=None, types=[], weight=True):
        analyzer.__init__(self, traj, file_name, name)
        
        self.bins = libmdalyzer.Vector3uint(nx,ny,nz)
        
        ## \internal
        # \brief C++ shared pointer to DensityProfile
        self.cpp = libmdalyzer.DensityProfile(self.trajectory.cpp, self.file_name, self.bins)
        self.trajectory.cpp.addCompute(self.cpp, self.name)
        
        ## \internal
        # \brief Python list of types attached to analyzer
        self.types = []
        
        ## \internal
        # \brief Flag to use mass weighting for the calculation
        self.weight = True
        
        self.add_type(types)
        self.mass_weight(weight)
    
    ## \var bins
    # \internal
    # \brief Vector3<unsigned int> setting the number of bins
    
    ## Add a type to the calculation
    # \param types String or Python list of types to attach
    #
    # \b Examples:
    # \code
    # dprof = analyzer.density(traj=my_traj, nx=5)
    #
    # # add a single type
    # dprof.add_type('A')
    #
    # # add multiple types
    # dprof.add_type(['B','C'])
    # \endcode
    def add_type(self, types):
        if not isinstance(types, (list,tuple)):
            types = [types]
            
        for t in types:
            if t not in self.types:
                self.types += [t]
                self.cpp.addType(t)
    
    ## Remove a type from the calculation
    # \param types String or Python list of types to remove
    #
    # \b Examples:
    # \code
    # dprof = analyzer.density(traj=my_traj, nx=5, types=['A','B','C'])
    #
    # # remove a single type
    # dprof.delete_type('A')
    #
    # # remove multiple types
    # dprof.delete_type(['B','C'])
    # \endcode
    def delete_type(self, types):
        if not isinstance(types, (list,tuple)):
            types = [types]
            
        for t in types:
            self.types.remove(t)
            self.cpp.deleteType(t)
    
    ## Toggle between mass and number density
    # \param weight if True, mass density is calculated
    #
    # \b Example:
    # \code
    # dprof = analyzer.density(traj=my_traj, nx=5, weight=False)
    #
    # # switch to mass weighting
    # dprof.mass_weight(True)
    # \endcode
    def mass_weight(self, weight):
        if weight is None:
            weight = False
        
        if not isinstance(weight, bool):
            raise RuntimeError('Mass weighting must be True or False')
        
        self.weight = weight
        self.cpp.useMassWeighting(self.weight)

## Computes the time-averaged %temperature profile
# 
# The time averaged %temperature profile is computed by binning particles by type
# after projection into an orthorhombic box. The simulation box is divided into a certain number of bins along each
# coordinate axis, and the temperature within each bin is computed from equipartition
# \f[ T = \frac{1}{3(N-1)}\sum m v^2 \f]
# where \f$ N \f$ is the number of particles in the bin and the sum is taken over particle masses and velocities.
class temperature(analyzer):
    ## Initialize a %temperature profile %analyzer
    # \param traj Python trajectory to attach to
    # \param file_name the stub output file name (will have extension auto-attached)
    # \param nx Number of bins along \f$ x \f$
    # \param ny Number of bins along \f$ y \f$
    # \param nz Number of bins along \f$ z \f$
    # \param name (if set) unique string name for the analyzer
    # \param types (if set) list of particle names to compute density for, average of all particles is used if unset
    #
    # \b Example:
    # \code
    # # compute average mass density along y using 12 bins
    # analyzer.temperature(traj=my_traj, ny=12)
    #
    # # compute number density of type Q along x and z
    # analyzer.temperature(traj=my_traj, file_name='temp', nx=5, nz=7, types='Q')
    # \endcode    
    def __init__(self, traj, file_name='temperature', nx=0, ny=0, nz=0, name=None, types=[]):
        analyzer.__init__(self, traj, file_name, name)
        
        ## \internal
        # \brief Vector3<unsigned int> setting the number of bins
        self.bins = libmdalyzer.Vector3uint(nx,ny,nz)
        
        ## \internal
        # \brief C++ shared pointer to TemperatureProfile
        self.cpp = libmdalyzer.TemperatureProfile(self.trajectory.cpp, self.file_name, self.bins)
        self.trajectory.cpp.addCompute(self.cpp, self.name)
        
        ## \internal
        # \brief Python list of types attached to analyzer
        self.types = []
        self.add_type(types)
    
    ## Add a type to the calculation
    # \param types String or Python list of types to attach
    #
    # \b Examples:
    # \code
    # tprof = analyzer.temperature(traj=my_traj, nx=5)
    #
    # # add a single type
    # tprof.add_type('A')
    #
    # # add multiple types
    # tprof.add_type(['B','C'])
    # \endcode
    def add_type(self, types):
        if not isinstance(types, (list,tuple)):
            types = [types]
            
        for t in types:
            if t not in self.types:
                self.types += [t]
                self.cpp.addType(t)
                
    ## Remove a type from the calculation
    # \param types String or Python list of types to remove
    #
    # \b Examples:
    # \code
    # tprof = analyzer.temperature(traj=my_traj, nx=5, types=['A','B','C'])
    #
    # # remove a single type
    # tprof.delete_type('A')
    #
    # # remove multiple types
    # tprof.delete_type(['B','C'])
    # \endcode
    def delete_type(self, types):
        if not isinstance(types, (list,tuple)):
            types = [types]
            
        for t in types:
            self.types.remove(t)
            self.cpp.deleteType(t)   

## Computes particle %clustering information
# 
# Determines the number of particle clusters and maximum cluster size in each frame.
class clustering(analyzer):
    ## Initialize a cluster %analyzer
    # \param traj Python trajectory to attach to
    # \param file_name the full file name to output
    # \param distance the interatomic distance to include two particles in the same cluster
    # \param name (if set) unique string name for the analyzer
    #
    # \b Example:
    # \code
    # analyzer.clustering(traj=my_traj, file_name='clustersize.dat', distance=1.2)
    # \endcode
    def __init__(self, traj, file_name='clustering.dat', distance=1.0, name=None):
        analyzer.__init__(self, traj, file_name, name)
        
        ## \internal
        # \brief the interatomic distance defining a cluster
        self.distance = distance
        
        ## \internal
        # \brief C++ shared pointer to Clustering
        self.cpp = libmdalyzer.Clustering(self.trajectory.cpp, self.file_name, self.distance)
        self.trajectory.cpp.addCompute(self.cpp, self.name)
    
    ## Set the distance defining particles in a cluster
    # \param distance the interatomic distance to include two particles in the same cluster
    #
    # \b Example:
    # \code
    # cf = analyzer.clustering(traj=my_traj)
    #
    # # change the cluster distance
    # cf.set_distance(1.5)
    # \endcode
    def set_distance(self, distance):
        self.distance = distance
        self.cpp.setDistance(self.distance)

## Computes the averaged mean-squared displacement
# 
# Mean-squared displacement \f$ \mathrm{r}(t)^2 \f$ is useful for calculating the diffusion coefficient of a particle.
# In three dimensions, for long times
# \f[ \langle \mathrm{r}(t)^2 \rangle \sim 6Dt\f]
# where \f$ D \f$ is the diffusion coefficient.
#
# analyzer.msd calculates the mean-squared displacement for specified particle types, averaging over particles and multiple
# time origins.
#
# \warning The mean-squared displacement must be calculated from the \a true positions of particles, \b not their
# periodic images. The data supplied must be unwrapped, or in a format that can be unwrapped (\a e.g. trajectory.hoomd).
class msd(analyzer):
    ## Initialize a mean-squared displacement calculator
    # \param traj Python trajectory to attach to
    # \param file_name the full file name to output
    # \param origins the skip between frames for time origins (1 = every frame)
    # \param name (if set) unique string name for the analyzer
    # \param types the particle types to analyze
    #
    # \b Examples:
    # \code
    # # use every frame as a time origin for type A
    # analyzer.msd(traj=my_traj, types='A')
    #
    # # use every fifth frame as an origin for types A and C
    # analyzer.msd(traj=my_traj, types=['A','C'], origins=5)
    # \endcode
    #
    # \note At least one type must be set when mdalyzer.trajectory.analyze() is called
    def __init__(self, traj, file_name='msd', origins=1, name=None, types=[]):
        analyzer.__init__(self, traj, file_name, name)
        self.origins = origins
        
        self.cpp = libmdalyzer.MeanSquaredDisplacement(self.trajectory.cpp, self.file_name, self.origins)
        self.trajectory.cpp.addCompute(self.cpp, self.name)
        
        self.types = []
        
        self.add_type(types)
        
    ## Add a type to the calculation
    # \param types String or Python list of types to attach
    #
    # \b Examples:
    # \code
    # msd = analyzer.msd(traj=my_traj)
    #
    # # add a single type
    # msd.add_type('A')
    #
    # # add multiple types
    # msd.add_type(['B','C'])
    # \endcode    
    def add_type(self, types):
        if not isinstance(types, (list,tuple)):
            types = [types]
            
        for t in types:
            if t not in self.types:
                self.types += [t]
                self.cpp.addType(t)
                
    ## Remove a type from the calculation
    # \param types String or Python list of types to remove
    #
    # \b Examples:
    # \code
    # msd = analyzer.msd(traj=my_traj, types=['A','B','C'])
    #
    # # remove a single type
    # msd.delete_type('A')
    #
    # # remove multiple types
    # msd.delete_type(['B','C'])
    # \endcode    
    def delete_type(self, types):
        if not isinstance(types, (list,tuple)):
            types = [types]
            
        for t in types:
            self.types.remove(t)
            self.cpp.deleteType(t)
    
## Computes the radial distribution function
# 
# The radial distribution function \f$ g(r) \f$ is calculated averaging over all particles and frames.
# It is calculated using particle binning
# \f[ g(r_i < r \le r_o) = \frac{\langle \sum \delta(r_i < r \le r_o) \rangle}{V(r_i, r_o)\langle\rho\rangle} \f]
# where \f$ r_i \f$ and \f$ r_o \f$ are the inner and outer radial distances, \f$ V(r_i, r_o) \f$ is the
# volume defined by the spherical shell with those radii, \f$ \langle \rho \rangle \f$ is the average
# number density, \f$ \delta \f$ is a function counting one if a particle lies within the radial distance and zero
# otherwise, and the sum and average are taken over all particles and set frames.
class rdf(analyzer):
    ## Initialize a radial distribution function calculator
    # \param traj Python trajectory to attach to
    # \param file_name the full file name to output
    # \param bin_size radial distance to use to bin particles
    # \param max_radius (if set) how far out to compute g(r), defaults to maximum allowed in first frame
    # \param origins the skip between frames for time origins (1 = every frame)
    # \param name (if set) unique string name for the analyzer
    #
    # \b Examples:
    # \code
    # # use every frame as a time origin, using a finer bin size but all particles
    # analyzer.rdf(traj=my_traj, bin_size=0.1)
    #
    # # use every fifth frame as an origin with max radius of 5
    # analyzer.rdf(traj=my_traj, origins=5, bin_size=0.5, max_radius=5.0)
    # \endcode
    #
    # \note Due to periodicity, max_radius cannot exceed twice the shortest box edge length during the simulation
    def __init__(self, traj, file_name='rdf.dat', origins=1, bin_size=1.0, max_radius=0.0, name=None):
        analyzer.__init__(self, traj, file_name, name)
        self.bin_size = bin_size
        self.max_radius = max_radius
        self.origins = origins
        
        self.cpp = libmdalyzer.RadialDistributionFunction(self.trajectory.cpp, self.file_name, self.bin_size, self.max_radius, self.origins)
        self.trajectory.cpp.addCompute(self.cpp, self.name)

## Computes the velocity autocorrelation function
#
# The velocity autocorrelation function is computed averaging over particles of a given type and multiple time origins
# \f[ \langle \mathbf{v}(t)\cdot\mathbf{v}(0) \rangle\f]    
#
# Currently, the autocorrelation function is computed for all possible lag times in the trajectory.
class vacf(analyzer):
    ## Initialize a velocity autocorrelation calculator
    # \param traj Python trajectory to attach to
    # \param file_name the full file name to output
    # \param origins the skip between frames for time origins (1 = every frame)
    # \param name (if set) unique string name for the analyzer
    # \param types the particle types to analyze
    #
    # \b Examples:
    # \code
    # # use every frame as a time origin for type A
    # analyzer.vacf(traj=my_traj, types='A')
    #
    # # use every fifth frame as an origin for types A and C
    # analyzer.vacf(traj=my_traj, types=['A','C'], origins=5)
    # \endcode
    def __init__(self, traj, file_name='vacf', origins=1, name=None, types=[]):
        analyzer.__init__(self, traj, file_name, name)
        self.origins = origins
        
        self.cpp = libmdalyzer.VelocityAutocorrelation(self.trajectory.cpp, self.file_name, self.origins)
        self.trajectory.cpp.addCompute(self.cpp, self.name)
        
        self.types = []
        
        self.add_type(types)
        
    ## Add a type to the calculation
    # \param types String or Python list of types to attach
    #
    # \b Examples:
    # \code
    # vacf = analyzer.vacf(traj=my_traj)
    #
    # # add a single type
    # vacf.add_type('A')
    #
    # # add multiple types
    # vacf.add_type(['B','C'])
    # \endcode      
    def add_type(self, types):
        if not isinstance(types, (list,tuple)):
            types = [types]
            
        for t in types:
            if t not in self.types:
                self.types += [t]
                self.cpp.addType(t)
   
    ## Remove a type from the calculation
    # \param types String or Python list of types to remove
    #
    # \b Examples:
    # \code
    # vacf = analyzer.vacf(traj=my_traj, types=['A','B','C'])
    #
    # # remove a single type
    # vacf.delete_type('A')
    #
    # # remove multiple types
    # vacf.delete_type(['B','C'])
    # \endcode  
    def delete_type(self, types):
        if not isinstance(types, (list,tuple)):
            types = [types]
            
        for t in types:
            self.types.remove(t)
            self.cpp.deleteType(t)
    
