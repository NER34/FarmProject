//----------------------------------------------------------------------------------------
/**
 * \file       data_parser.h
 * \author     Plotnikau Pavel
 * \date       2021/05/20
 * \brief      Contains functions for working with configure file data
*/
//----------------------------------------------------------------------------------------

#ifndef DATA_PARSER
#define DATA_PARSER

#include <iostream>
#include <fstream>
#include <sstream> 
#include <vector>
#include <string>

#include "pgr.h"
/// <summary>
/// Reads data from file and returns buffer with lines
/// </summary>
/// <param name="path">Path to file</param>
/// <param name="data">Returnew buffer</param>
/// <returns>Returns true if reading was succesful. Otherwis returns false</returns>
bool ReadDataFromFile(const std::string& path, std::vector<std::string>& data);
/// <summary>
/// Replace data from one buffer to another. Data from "from" buffer will be deleted
/// </summary>
/// <param name="size">Size of replaced data</param>
/// <param name="from">Buffer with data</param>
/// <param name="to">Returned buffer of replaced data</param>
/// <returns>Returns true if replacing was succesful</returns>
bool ReplaceData(unsigned int size, std::vector<std::string>& from, std::vector<std::string>& to);
/// <summary>
/// Reads header(int value) from buffer and returns it
/// </summary>
/// <param name="data">Data buffer</param>
/// <returns>Returns -1 if reading was failed</returns>
int ReadHeader(std::vector<std::string>& data);
/// <summary>
/// Converts data-buffer to vector of 3 floats and returns it
/// </summary>
/// <param name="data">Data buffer</param>
/// <param name="vector">Returned vector of 3 floats</param>
/// <returns>Returns true if converting was succesfull</returns>
bool GetVector3f(std::string data, glm::vec3& vector);
/// <summary>
/// Converts data-buffer to vector of 4 floats and returns it
/// </summary>
/// <param name="data">Data buffer</param>
/// <param name="vector">Returned vector of 4 floats</param>
/// <returns>Returns true if converting was succesfull</returns>
bool GetVector4f(std::string data, glm::vec4& vector);

#endif // !DATA_PARSER