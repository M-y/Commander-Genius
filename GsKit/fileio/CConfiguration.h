/*
 * CConfiguration.h
 *
 *  Created on: 16.06.2010
 *      Author: gerstrong
 */

#ifndef CCONFIGURATION_H
#define CCONFIGURATION_H

#include "IniReader.h"
#include <string>

class CConfiguration : public IniReader
{
public:
    CConfiguration();

    virtual ~CConfiguration() override;

	bool saveCfgFile();

    /*
    bool OnNewSection (const std::string& section) override;
    bool OnEntry (const std::string& section, const std::string& propname, const std::string& value) override;
    */

	void WriteString(const std::string& section, const std::string& key, const std::string& string);
	void SetKeyword(const std::string& section, const std::string& keyword, const bool value);
	void WriteInt(const std::string &section, const std::string &keyword, const int value);        

    static const std::string CONFIGFILENAME;
};

#endif /* CCONFIGURATION_H */
