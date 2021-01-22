
#include "pch.h"
#include "cpuinfo.h"

const std::string CPUInfo::TEMPERATURE_QUERY_STRING = "wmic /namespace:\\\\root\\wmi PATH MSAcpi_ThermalZoneTemperature get CurrentTemperature";
const std::string CPUInfo::TEMPERATURE_ERROR_IDENTIFIER_STRING = "ERROR:";
const std::string CPUInfo::CLOCK_SPEED_QUERY_STRING = "wmic cpu get /format:list | findstr /R /C:CurrentClockSpeed=";
const std::string CPUInfo::CURRENT_CLOCK_SPEED_IDENTIFIER_STRING = "CurrentClockSpeed=";
const std::string CPUInfo::NAME_IDENTIFIER_STRING = "Name=";
const std::string CPUInfo::NUMBER_OF_CORES_IDENTIFIER_STRING = "NumberOfCores=";
const std::string CPUInfo::MANUFACTURER_IDENTIFIER_STRING = "Manufacturer=";
const std::string CPUInfo::ARCHITECTURE_IDENTIFIER_STRING = "DataWidth=";
const std::string CPUInfo::L2_CACHE_SIZE_IDENTIFIER_STRING = "L2CacheSize=";
const std::string CPUInfo::L3_CACHE_SIZE_IDENTIFIER_STRING = "L3CacheSize=";

CPUInfo::CPUInfo(const std::vector<std::string> &rawData, int cpuNumber) :
	_name{""},
	_manufacturer{""},
	_numberOfCores{""},
	_architecture{""},
	_L2CacheSizeStr{""},
	_L3CacheSizeStr{""},
	_cpuNumber{cpuNumber}
{
    for (auto iter = rawData.begin(); iter != rawData.end(); iter++) {
        
        //Name
        if ((iter->find(NAME_IDENTIFIER_STRING) != std::string::npos) && (iter->find(NAME_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(NAME_IDENTIFIER_STRING);
            this->_name = iter->substr(foundPosition+NAME_IDENTIFIER_STRING.length());
        }

        //Manufacturer
        if ((iter->find(MANUFACTURER_IDENTIFIER_STRING) != std::string::npos) && (iter->find(MANUFACTURER_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(MANUFACTURER_IDENTIFIER_STRING);
            this->_manufacturer = iter->substr(foundPosition+ MANUFACTURER_IDENTIFIER_STRING.length());
        }

        //Number Of Cores
        if ((iter->find(NUMBER_OF_CORES_IDENTIFIER_STRING) != std::string::npos) && (iter->find(NUMBER_OF_CORES_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(NUMBER_OF_CORES_IDENTIFIER_STRING);
            this->_numberOfCores = iter->substr(foundPosition+NUMBER_OF_CORES_IDENTIFIER_STRING.length());
        }

        //Architecture
        if ((iter->find(ARCHITECTURE_IDENTIFIER_STRING) != std::string::npos) && (iter->find(ARCHITECTURE_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(ARCHITECTURE_IDENTIFIER_STRING);
            std::string dataWidth = iter->substr(foundPosition+ARCHITECTURE_IDENTIFIER_STRING.length());
			this->_architecture = getArchitecture(dataWidth);
        }

        //L2 Cache Size
        if ((iter->find(L2_CACHE_SIZE_IDENTIFIER_STRING) != std::string::npos) && (iter->find(L2_CACHE_SIZE_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(L2_CACHE_SIZE_IDENTIFIER_STRING);
            auto L2SizeStr = iter->substr(foundPosition + L2_CACHE_SIZE_IDENTIFIER_STRING.length());
            this->_L2CacheSizeStr = L2SizeStr + "KB";
            if (this->_L2CacheSizeStr == "KB") {
                this->_L2CacheSizeStr = "";
            }
            else
            {
                _L2CacheSize = std::stof(L2SizeStr);
            }
        }

        //L3 Cache Size
        if ((iter->find(L3_CACHE_SIZE_IDENTIFIER_STRING) != std::string::npos) && (iter->find(L3_CACHE_SIZE_IDENTIFIER_STRING) == 0)) {
            size_t foundPosition = iter->find(L3_CACHE_SIZE_IDENTIFIER_STRING);
            auto L3Str = iter->substr(foundPosition + L3_CACHE_SIZE_IDENTIFIER_STRING.length());
            this->_L3CacheSizeStr = L3Str + "KB";
            if (this->_L3CacheSizeStr == "KB") {
                this->_L3CacheSizeStr = "";
            }
            else {
                _L3CacheSize = std::stof(L3Str);
            }
        }
    }
    //In case any of these values are missing or don't get assigned
    if (this->_name == "") {
        this->_name = "Unknown";
    }
    if (this->_manufacturer == "") {
        this->_manufacturer = "Unknown";
    }
    if (this->_numberOfCores == "") {
        this->_numberOfCores = "Unknown";
    }
    if (this->_architecture == "") {
        this->_architecture = "Unknown";
    }
    if (this->_L2CacheSizeStr == "") {
        this->_L2CacheSizeStr= "Unknown";
    }
    if (this->_L3CacheSizeStr == "") {
        this->_L3CacheSizeStr = "Unknown";
    }
}

std::string& CPUInfo::name()  
{
    return this->_name;
}

std::string CPUInfo::name() const
{
    return this->_name;
}

std::string& CPUInfo::manufacturer() 
{
    return this->_manufacturer;
}

std::string CPUInfo::manufacturer() const
{
    return this->_manufacturer;
}

std::string& CPUInfo::numberOfCores() 
{
    return this->_numberOfCores;
}

std::string CPUInfo::numberOfCores() const
{
    return this->_numberOfCores;
}

std::string& CPUInfo::architecture() 
{
	return this->_architecture;
}

std::string CPUInfo::architecture() const
{
    return this->_architecture;
}
std::string& CPUInfo::L2CacheSizeStr() 
{
	return this->_L2CacheSizeStr;
}

std::string CPUInfo::L2CacheSizeStr() const
{
    return this->_L2CacheSizeStr;
}

uint32_t CPUInfo::L2CacheSize() const
{
    return _L2CacheSize;
}

std::string& CPUInfo::L3CacheSizeStr() 
{
	return this->_L3CacheSizeStr;
}

std::string CPUInfo::L3CacheSizeStr() const
{
    return this->_L3CacheSizeStr;
}

uint32_t CPUInfo::L3CacheSize() const
{
    return _L3CacheSize;
}

int CPUInfo::cpuNumber() const
{
	return this->_cpuNumber;
}

std::string CPUInfo::currentClockSpeed() const
{
	std::string clockSpeed{ "" };
	SystemCommand systemCommand{ CLOCK_SPEED_QUERY_STRING };
	systemCommand.execute();
	if (!systemCommand.hasError()) {
		std::vector<std::string> raw{ systemCommand.outputAsVector() };
		if (raw.empty()) {
			clockSpeed = "Unknown";
		}
		int cpuInfoNumber = 0;
		for (std::vector<std::string>::const_iterator iter = raw.begin(); iter != raw.end(); iter++) {
			if (cpuInfoNumber == this->_cpuNumber) {
				if ((iter->find(CURRENT_CLOCK_SPEED_IDENTIFIER_STRING) != std::string::npos) && (iter->find(CURRENT_CLOCK_SPEED_IDENTIFIER_STRING) == 0)) {
					size_t foundPosition = iter->find(CURRENT_CLOCK_SPEED_IDENTIFIER_STRING);
					clockSpeed = iter->substr(foundPosition + CURRENT_CLOCK_SPEED_IDENTIFIER_STRING.length()) + "MHz";
				}
			}
			cpuInfoNumber++;
		}
	} else {
		clockSpeed = "Unknown";
	}
	if ((clockSpeed == "MHz") || (clockSpeed == "")) {
		clockSpeed = "Unknown";
	}
	return clockSpeed;
}

std::string CPUInfo::currentTemperature() const
{
	//NOTE: THIS IS NOT SUPPORTED BY ALL COMPUTERS!!!
	std::string temperature{ "" };
    SystemCommand systemCommand{ TEMPERATURE_QUERY_STRING };
    systemCommand.execute();
	if (!systemCommand.hasError()) {
		std::vector<std::string> raw{ systemCommand.outputAsVector() };
		for (auto iter = raw.begin(); iter != raw.end(); iter++) {
			if (iter->find(TEMPERATURE_ERROR_IDENTIFIER_STRING)) {
				temperature = "Unknown";
			} else {
				std::string rawTemp = *(raw.begin() + 1);
				try {
					int tempInKelvin = std::stoi(rawTemp);
					int tempInCelcius = kelvinToCelcius(tempInKelvin);
					temperature = toString(tempInCelcius) + "C";
				} catch (std::exception &e) {
					(void)e;
					temperature = "Unknown";
				}
			}
        }
    } else {
        temperature = "Unknown";
    }
    return temperature;
}


int CPUInfo::kelvinToCelcius(int tempInKelvin) const
{
    return tempInKelvin - 273;
}

std::string CPUInfo::getArchitecture(std::string &dataWidth) const
{
	try {
		int dataWidthInt = std::stoi(dataWidth);
		switch (dataWidthInt) {
			case 32: return "x86";
			case 64: return "x86_64";
			default: return "Unknown";
		}
	} catch (std::exception &e) {
		(void)e;
		return "Unknown";
	}
}