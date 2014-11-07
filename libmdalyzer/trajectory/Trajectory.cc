#include "Trajectory.h"

Trajectory::Trajectory()
    : m_read_from_file(false), m_sorted(false)
    {
    }

/*!
 * \param compute Shared pointer to new Compute
 * \param name String key for the Compute, generated on Python level
 *
 * As each frame in the Trajectory is read, each Compute is queried to determine if it needs to calculate on a Frame.
 * We store Computes in a map to make it easier to iterate on them as boost:shared_ptrs.
 */    
void Trajectory::addCompute(boost::shared_ptr<Compute> compute, const std::string& name)
    {
    std::map< std::string, boost::shared_ptr<Compute> >::iterator compute_i = m_computes.find(name);
    if (compute_i == m_computes.end())
        {
        m_computes[name] = compute;
        }
    else
        {
        // Do some type of error handling, like throw an exception
        }
    }

/*!
 * \param name String key for the Compute to remove, as assigned when added
 *
 * The Compute is erased from the map, so this is a harsh removal. All associated data for the Compute is destroyed.
 */
void Trajectory::removeCompute(const std::string& name)
    {
    std::map< std::string, boost::shared_ptr<Compute> >::iterator compute_i = m_computes.find(name);
    if (compute_i != m_computes.end())
        {
        m_computes.erase(compute_i);
        }
    else
        {
        // Doesn't exist, can't delete
        }
    }

/*!
 * \param name String key for the Compute to return, as assigned when added
 *
 * \returns A shared pointer to the Compute
 */
boost::shared_ptr<Compute> Trajectory::getCompute(const std::string& name)
    {
    std::map< std::string, boost::shared_ptr<Compute> >::iterator compute_i = m_computes.find(name);
    if (compute_i != m_computes.end())
        {
        return m_computes[name];
        }
    else
        {
        // Doesn't exist, raise hell and return a null pointer
        return boost::shared_ptr<Compute>(); // null ptr
        }
    }

void Trajectory::analyze()
    {
    boost::shared_ptr<Frame> cur_frame;
    std::map< std::string, boost::shared_ptr<Compute> >::iterator cur_compute;
    
    // main execution loop, so we want to catch exceptions as they are thrown and abort
    try
        {
        while( (cur_frame = getNextFrame()) )
            {
            for (cur_compute = m_computes.begin(); cur_compute != m_computes.end(); ++cur_compute)
                {
                if (cur_compute->second->shouldCompute())
                    {
                    cur_compute->second->run();
                    }
                }
            }
        }
    catch (std::exception const & e)
        {
        throw e;
        }
    }
