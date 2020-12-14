//
// Copyright (c) 2016-2020 CNRS INRIA
//

#include "pinocchio/utils/file-explorer.hpp"

namespace pinocchio
{

  void extractPathFromEnvVar(const std::string & env_var_name,
                             std::vector<std::string> & list_of_paths,
                             const std::string & delimiter)
  {
    const char * env_var_value = std::getenv(env_var_name.c_str());
    
    if(env_var_value != NULL)
    {
      std::string policyStr(env_var_value);
      // Add a separator at the end so that last path is also retrieved
      policyStr += std::string(":");
      size_t lastOffset = 0;
      
      while(true)
      {
        size_t offset = policyStr.find_first_of(delimiter, lastOffset);
        if (offset < policyStr.size())
          list_of_paths.push_back(policyStr.substr(lastOffset, offset - lastOffset));
        if (offset == std::string::npos)
          break;
        else
          lastOffset = offset + 1; // add one to skip the delimiter
      }
    }
  }

  std::vector<std::string> extractPathFromEnvVar(const std::string & env_var_name,
                                                 const std::string & delimiter)
  {
    std::vector<std::string> list_of_paths;
    extractPathFromEnvVar(env_var_name,list_of_paths,delimiter);
    return list_of_paths;
  }

  std::vector<std::string> rosPaths()
  {
    std::vector<std::string> list_of_paths;
    extractPathFromEnvVar("ROS_PACKAGE_PATH",list_of_paths);
    extractPathFromEnvVar("AMENT_PREFIX_PATH",list_of_paths);
    
    return list_of_paths;
  }

}
