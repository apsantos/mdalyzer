/*!
 * \file test_Analyzer.h
 * \author Michael P. Howard
 * \date 9 January 2015
 * \brief Declaration of useful Analyzer methods
 */
 
#ifndef TEST_UNIT_TEST_ANALYZER_H_
#define TEST_UNIT_TEST_ANALYZER_H_

#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>
#include <fstream>

//! a fixture to cleanup files that might be produced during testing
struct FileFixture
    {
    FileFixture()
        {
        if (boost::unit_test::framework::master_test_suite().argc > 0)
            {
            tmp = std::string(boost::unit_test::framework::master_test_suite().argv[1]);
            }
        }
    ~FileFixture()
        {
        for (unsigned int cur_file = 0; cur_file < files.size(); ++cur_file)
            {
            if (files[cur_file].length() > 0)
                {
//                 remove(files[cur_file].c_str());
                }
            }
        }
    void addFile(const std::string& file)
        {
        files.push_back(file);
        }
    std::vector<std::string> files;
    std::string tmp;
    };

//! compare float entries in a two column file    
inline void checkTwoColumn(const std::string& file,
                    const double *col1,
                    const double *col2,
                    unsigned int n_comment,
                    unsigned int n_entries,
                    double tol)
    {
    std::ifstream inf(file.c_str());
    BOOST_REQUIRE(inf.good());
    
    for (unsigned int skip=0; skip < n_comment; ++skip)
        {
        inf.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    
    double val1(0.), val2(0.);
    for (unsigned int line=0; line < n_entries; ++line)
        {
        BOOST_REQUIRE( inf >> val1 >> val2 );
            
        if (col1 != NULL)
            {
            BOOST_CHECK_CLOSE(val1, col1[line], tol);
            }
        if (col2 != NULL)
            {
            BOOST_CHECK_CLOSE(val2, col2[line], tol);
            }
        }
    }

#endif // TEST_UNIT_TEST_ANALYZER_H_