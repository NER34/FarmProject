#include "data_parser.h"

bool ReadDataFromFile(const std::string& path, std::vector<std::string>& data) 
{
    std::ifstream ifs;
    ifs.exceptions(std::ifstream::badbit | std::ifstream::failbit);

    try
    {
        ifs.open(path);
        while (!ifs.eof()) {
            std::string buf;
            std::getline(ifs, buf);
            data.push_back(buf);
        }
        ifs.close();
    }
    catch (const std::exception& ex)
    {
        std::cout << "failed to read data from file: " << ex.what() << std::endl;
        return false;
    }

    return true;
}

bool ReplaceData(unsigned int size, std::vector<std::string>& from, std::vector<std::string>& to)
{
    try
    {
        to.insert(to.begin(), from.begin(), from.begin() + size);
        from.erase(from.begin(), from.begin() + size);
    }
    catch (const std::exception& ex)
    {
        std::cout << "failed to replace data: " << ex.what() << std::endl;
        return false;
    }

    return true;
}

int ReadHeader(std::vector<std::string>& data) 
{
    int res;
    try
    {
        res = std::stoi(data[0]);
        data.erase(data.begin());
    }
    catch (const std::exception& ex)
    {
        std::cout << "failed to read header: " << ex.what() << std::endl;
        return -1;
    }
    return res;
}

bool GetFloatsFromString(std::string data, std::vector<float>& numbers) 
{
    std::istringstream iss(data);
    try
    {
        while (!iss.eof()) {
            float tmp;
            iss >> tmp;
            numbers.push_back(tmp);
        }
    }
    catch (const std::exception& ex)
    {
        std::cout << "failed to convert data to floats: " << data << ": " << ex.what() << std::endl;
        return false;
    }
    return true;
}

bool GetVector3f(std::string data, glm::vec3& vector)
{
    std::vector<float> nums;
    if (!GetFloatsFromString(data, nums)) return false;
    vector = glm::vec3(nums[0], nums[1], nums[2]);
    return true;
}

bool GetVector4f(std::string data, glm::vec4& vector)
{
    std::vector<float> nums;
    if(!GetFloatsFromString(data, nums)) return false;
    vector = glm::vec4(nums[0], nums[1], nums[2], nums[3]);
    return true;
}


