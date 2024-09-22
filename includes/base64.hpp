#ifndef __BASE64_HPP__
# define __BASE64_HPP__

# include <iostream>
# include <vector>
# include <string>
# include <map>
# include <stdexcept>
# include <stdint.h>


std::string base64_decode(const std::string& encoded) ;

class base64
{
    private:

    public:
        base64();
        ~base64();
};

#endif