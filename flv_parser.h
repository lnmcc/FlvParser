/*
* flvParser.h
*
*  Created on:  Nov 7, 2013
*  Author: sijiewang
*/

#ifndef _FLV_PARSER_H
#define _FLV_PARSER_H

#include <iostream>
#include <stdio.h>
#include <string>

#define FLV_HEAD_LEN 9
#define TAG_HEAD_LEN 11

class FlvHeader {

public:
    FlvHeader(unsigned char *header, unsigned int length);
    ~FlvHeader();

private:
    unsigned char *m_header;
    unsigned int m_length;
};


class FlvMetaData {

public:
    FlvMetaData(unsigned char *meta, unsigned int length);
    ~FlvMetaData();

    FlvMetaData(const FlvMetaData&);
    FlvMetaData& operator=(const FlvMetaData&);

    double getDuration();
    double getWidth();
    double getHeight();
    double getFramerate();
    double getVideoDatarate();
    double getAudioDatarate();
    double getVideoCodecId();
    double getAudioCodecId();
    double getAudioSamplerate();
    double getAudioSamplesize();
    bool getStereo();

private:
    //convert HEX to double
    double hexStr2double(const unsigned char* hex, const unsigned int length);
    void parseMeta();

private:
    unsigned char *m_meta;
    unsigned int m_length;

    double m_duration;
    double m_width;
    double m_height;
    double m_framerate;
    double m_videodatarate;
    double m_audiodatarate;
    double m_videocodecid;
    double m_audiocodecid;
    double m_audiosamplerate;
    double m_audiosamplesize;

    bool m_stereo;
};

class FlvReader {

public:
    FlvReader();
    ~FlvReader();

    bool init(std::string path);
    FlvHeader* readHeader();
    FlvMetaData* readMeta();

private:
    std::string m_file;
    FILE *m_fp;
};

#endif
