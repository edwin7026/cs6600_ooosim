/**
 * @file base.h
 * @details a base file for all classes in this project
 * @author Edwin Joy <edwin7026@gmail.com>
 */

#ifndef BASE_H
#define BASE_H

// standard imports
#include <string>

// local imports
#include <common.h>

/**
 * @details base class for all classes
 */
class base
{
    protected:
        std::string _name;
    public:
        base(const std::string& name) : _name(name) {}

        const std::string& get_name(){
            return _name;
        }
};

#endif // BASE_H