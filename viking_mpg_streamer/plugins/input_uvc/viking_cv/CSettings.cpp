/*******************************************************************************************/
/* The MIT License (MIT)                                                                   */
/*                                                                                         */
/* Copyright (c) 2014 - Marina High School FIRST Robotics Team 4276 (Huntington Beach, CA) */
/*                                                                                         */
/* Permission is hereby granted, free of charge, to any person obtaining a copy            */
/* of this software and associated documentation files (the "Software"), to deal           */
/* in the Software without restriction, including without limitation the rights            */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell               */
/* copies of the Software, and to permit persons to whom the Software is                   */
/* furnished to do so, subject to the following conditions:                                */
/*                                                                                         */
/* The above copyright notice and this permission notice shall be included in              */
/* all copies or substantial portions of the Software.                                     */
/*                                                                                         */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR              */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,                */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE             */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER                  */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,           */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN               */
/* THE SOFTWARE.                                                                           */
/*******************************************************************************************/

/*******************************************************************************************/
/* We are a high school robotics team and always in need of financial support.             */
/* If you use this software for commercial purposes please return the favor and donate     */
/* (tax free) to "Marina High School Educational Foundation"  (Huntington Beach, CA)       */
/*******************************************************************************************/


#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "CSettings.h"

#define BASE_DIR "/home/ubuntu/"

CSettings g_settings;

CSettings::CSettings()
{
    init();
}

CSettings::CSettings(const CSettings& orig)
{
    init();
}

CSettings::~CSettings()
{
    init();
}

void CSettings::init()
{    
    char buf[128] = {0};
    const INIT_SETTINGS initSettings[] = {
        {SETTING_ENABLE_DYNAMIC_SETTINGS, "enableDynamicSettings.txt", 0, &m_enableDynamicSettings},
        {SETTING_EXPOSURE, "exposureZeroTo2047.txt", 260, &m_exposureZeroTo2047},
        {SETTING_ENABLE_STREAM_FILTER_IMAGE, "enableStreamFilterImage.txt", 0, &m_enableStreamFilterImage},
        {SETTING_FILTER_HUE_LOWER_BOUND, "filterHueLowerBoundZeroTo255.txt", 79, &m_filterHueLowerBoundZeroTo255},
        {SETTING_FILTER_HUE_UPPER_BOUND, "filterHueUpperBoundZeroTo255.txt", 90, &m_filterHueUpperBoundZeroTo255},
        {SETTING_TYPE_UNKNOWN, "", 0, 0},
    };
    s_initSettings = initSettings;
    
    const INIT_SETTINGS* pInit = initSettings;
    while(pInit->settingType != SETTING_TYPE_UNKNOWN)
    {
        *(pInit->pValue) = pInit->defaultValue;
        int iTemp = getValueFromFile(pInit->settingType);
        if(iTemp != -1)           
        {
            *(pInit->pValue) = iTemp;
        }
        if(!isSettingFileExist(pInit->settingType))
        {
            std::string sPath = BASE_DIR;
            sPath += pInit->name;
            FILE* fp = fopen(sPath.c_str(), "w");
            if (fp == NULL)
            {
               printf("?? Can't create file");
            }
            else
            {
                sprintf(buf, "%d", *(pInit->pValue));
                fwrite(buf, 1, strlen(buf), fp);
            }
        }
    }
}

bool CSettings::isDynamicSettingsEnabled() const
{
    static int iCount = 0;
    if(m_enableDynamicSettings != 0)
    {
        iCount++;
        if(iCount % 20)
        {            
            return true;
        }
    }
    return false;
}

std::string CSettings::getSettingText(SETTING_TYPE typ)
{
    char buf[128] = {0};
    sprintf(buf, "%s", getSetting(typ));
    return buf;
}

int CSettings::getSetting(SETTING_TYPE typ)
{      
    if(typ == SETTING_ENABLE_DYNAMIC_SETTINGS)
    {
        printf("?? SETTING_ENABLE_DYNAMIC_SETTINGS cannot be set dynamically");
    }
    else
    {
        *(s_initSettings[typ].pValue) = getValueFromFile(typ);
    }
    return *(s_initSettings[typ].pValue);
}

bool CSettings::isSettingFileExist(SETTING_TYPE typ) const
{
    std::string sPath = BASE_DIR;
    sPath += s_initSettings[typ].name;
    return (access(sPath.c_str(), F_OK) != -1);
}

int CSettings::getValueFromFile(SETTING_TYPE typ) const
{
    int iRet = -1;
    if(isSettingFileExist(typ))
    {
        std::string sPath = BASE_DIR;
        sPath += s_initSettings[typ].name;
        FILE* fp = fopen(sPath.c_str(), "r");
        if (fp == NULL)
        {
            printf("?? File exists but cannot open");
        }
        else
        {
            char buf[128] = {0};
            int nRead = fread(buf, 1, 128, fp);
            if(nRead > 0)
            {
                iRet = atoi(buf);
            }
            fclose(fp);
        }
    }
    return iRet;
}



