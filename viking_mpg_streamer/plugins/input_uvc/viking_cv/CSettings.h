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


#ifndef CSETTINGS_H
#define	CSETTINGS_H

class CSettings
{
public:
    typedef enum 
    {
        SETTING_TYPE_UNKNOWN = -1,
        SETTING_ENABLE_DYNAMIC_SETTINGS = 0,  // ==0 init at startup only
        SETTING_EXPOSURE,
        SETTING_ENABLE_STREAM_FILTER_IMAGE,   // != 0 show filtered blobs instead of annotated original
        SETTING_FILTER_HUE_LOWER_BOUND,
        SETTING_FILTER_HUE_UPPER_BOUND,
        NUMBER_OF_SETTINGS,
    } SETTING_TYPE;
        
    typedef struct {
        SETTING_TYPE settingType;
        const char* name;
        int defaultValue;
        int* pValue;
    } INIT_SETTINGS;

    CSettings();
    CSettings(const CSettings& orig);
    virtual ~CSettings();

    void init();
    bool isDynamicSettingsEnabled() const;
    std::string getSettingText(SETTING_TYPE typ);
    int getSetting(SETTING_TYPE typ);
    bool isSettingFileExist(SETTING_TYPE typ) const;
    int getValueFromFile(SETTING_TYPE typ) const;

private:
    const INIT_SETTINGS* s_initSettings;
    
    int m_enableDynamicSettings;
    int m_exposureZeroTo2047;
    int m_enableStreamFilterImage;
    int m_filterHueLowerBoundZeroTo255;
    int m_filterHueUpperBoundZeroTo255;
 };

#endif	/* CSETTINGS_H */

