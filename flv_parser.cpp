/*
* flvParser.cpp
*
*  Created on:  Nov 7, 2013
*  Author: sijiewang
*/

#include "flv_parser.h"
#include <string.h>

using namespace std;

FlvHeader::FlvHeader(unsigned char *header, unsigned int length) {

    m_header = header;
    m_length = length;
}

FlvHeader::~FlvHeader() {

    if(m_header != NULL) {
        delete m_header;
        m_header = NULL;
    }
}

FlvMetaData::FlvMetaData(unsigned char *meta, unsigned int length) {

    m_meta = meta;
    m_length = length;

    m_duration = 0;
    m_width = 0;
    m_height = 0;
    m_framerate = 0;
    m_videodatarate = 0;
    m_audiodatarate = 0;
    m_videocodecid = 0;
    m_audiosamplerate = 0;
    m_audiosamplesize = 0;
    m_stereo = false;

    parseMeta();
}

FlvMetaData::FlvMetaData(const FlvMetaData& r) {

    m_length = r.m_length;
    m_meta = new unsigned char[m_length];
    memcpy(m_meta, r.m_meta, m_length);

    m_duration = r.m_duration;
    m_width = r.m_width;
    m_height = r.m_height;
    m_framerate = r.m_framerate;
    m_videodatarate = r.m_videodatarate;
    m_audiodatarate = r.m_audiodatarate;
    m_videocodecid = r.m_videocodecid;
    m_audiosamplerate = r.m_audiosamplerate;
    m_audiosamplesize = r.m_audiosamplesize;
    m_stereo = r.m_stereo;
}

FlvMetaData&  FlvMetaData::operator=(const FlvMetaData& r) {

    if(this == &r) {
        return *this;
    }

    if(m_meta != NULL) {
        delete m_meta;
    }

    m_length = r.m_length;
    m_meta = new unsigned char[m_length];
    memcpy(m_meta, r.m_meta, m_length);

    m_duration = r.m_duration;
    m_width = r.m_width;
    m_height = r.m_height;
    m_framerate = r.m_framerate;
    m_videodatarate = r.m_videodatarate;
    m_audiodatarate = r.m_audiodatarate;
    m_videocodecid = r.m_videocodecid;
    m_audiosamplerate = r.m_audiosamplerate;
    m_audiosamplesize = r.m_audiosamplesize;
    m_stereo = r.m_stereo;

    return *this;
}

FlvMetaData::~FlvMetaData() {

    if(m_meta != NULL) {
        delete m_meta;
        m_meta = NULL;
    }
}

void FlvMetaData::parseMeta() {

    unsigned int arrayLen = 0;
    unsigned int offset = TAG_HEAD_LEN + 13;

    unsigned int nameLen = 0;
    double numValue = 0;
    bool boolValue = false;

    if(m_meta[offset++] == 0x08) {

        arrayLen |= m_meta[offset++];
        arrayLen = arrayLen << 8;
        arrayLen |= m_meta[offset++];
        arrayLen = arrayLen << 8;
        arrayLen |= m_meta[offset++];
        arrayLen = arrayLen << 8;
        arrayLen |= m_meta[offset++];

        //cerr << "ArrayLen = " << arrayLen << endl;
    } else {
        //TODO:
        cerr << "metadata format error!!!" << endl;
        return ;
    }

    for(unsigned int i = 0; i < arrayLen; i++) {

        numValue = 0;
        boolValue = false;

        nameLen = 0;
        nameLen |= m_meta[offset++];
        nameLen = nameLen << 8;
        nameLen |= m_meta[offset++];
        //cerr << "name length=" << nameLen << " ";

        char name[nameLen + 1];
#if DEBUG
        printf("\norign \n");
        for(unsigned int i = 0; i < nameLen + 3; i++) {
            printf("%x ", m_meta[offset + i]);
        }
        printf("\n");
#endif

        memset(name, 0, sizeof(name));
        memcpy(name, &m_meta[offset], nameLen);
        name[nameLen + 1] = '\0';
        offset += nameLen;
        //cerr << "name=" << name << " ";
#if DEBUG
        printf("memcpy\n");
        for(unsigned int i = 0; i < nameLen; i++) {
            printf("%x ", name[i]);
        }
        printf("\n");
#endif
        switch(m_meta[offset++]) {
        case 0x0: //Number type

            numValue = hexStr2double(&m_meta[offset], 8);
            offset += 8;
            break;

        case 0x1: //Boolean type
            if(offset++ != 0x00) {
                boolValue = true;
            }
            break;

        case 0x2: //String type
            nameLen = 0;
            nameLen |= m_meta[offset++];
            nameLen = nameLen << 8;
            nameLen |= m_meta[offset++];
            offset += nameLen;
            break;

        case 0x12: //Long string type
            nameLen = 0;
            nameLen |= m_meta[offset++];
            nameLen = nameLen << 8;
            nameLen |= m_meta[offset++];
            nameLen = nameLen << 8;
            nameLen |= m_meta[offset++];
            nameLen = nameLen << 8;
            nameLen |= m_meta[offset++];
            offset += nameLen;
            break;

        //FIXME:
        default:
            break;
        }

        if(strncmp(name, "duration", 8)	== 0) {
            m_duration = numValue;
        } else if(strncmp(name, "width", 5)	== 0) {
            m_width = numValue;
        } else if(strncmp(name, "height", 6) == 0) {
            m_height = numValue;
        } else if(strncmp(name, "framerate", 9) == 0) {
            m_framerate = numValue;
        } else if(strncmp(name, "videodatarate", 13) == 0) {
            m_videodatarate = numValue;
        } else if(strncmp(name, "audiodatarate", 13) == 0) {
            m_audiodatarate = numValue;
        } else if(strncmp(name, "videocodecid", 12) == 0) {
            m_videocodecid = numValue;
        } else if(strncmp(name, "audiosamplerate", 15) == 0) {
            m_audiosamplerate = numValue;
        } else if(strncmp(name, "audiosamplesize", 15) == 0) {
            m_audiosamplesize = numValue;
        } else if(strncmp(name, "audiocodecid", 12) == 0) {
            m_audiocodecid = numValue;
        } else if(strncmp(name, "stereo", 6) == 0) {
            m_stereo = boolValue;
        }

    }
}

double FlvMetaData::hexStr2double(const unsigned char* hex, const unsigned int length) {

    double ret = 0;
    char hexstr[length * 2];
    memset(hexstr, 0, sizeof(hexstr));

    for(unsigned int i = 0; i < length; i++) {
        sprintf(hexstr + i * 2, "%02x", hex[i]);
    }

    sscanf(hexstr, "%llx", (unsigned long long*)&ret);

    return ret;
}

double FlvMetaData::getDuration() {
    return m_duration;
}

double FlvMetaData::getWidth() {
    return m_width;
}

double FlvMetaData::getHeight() {
    return m_height;
}

double FlvMetaData::getFramerate() {
    return m_framerate;
}

double FlvMetaData::getVideoDatarate() {
    return m_videodatarate;
}

double FlvMetaData::getAudioDatarate() {
    return m_audiodatarate;
}

double FlvMetaData::getVideoCodecId() {
    return m_videocodecid;
}

double FlvMetaData::getAudioSamplerate() {
    return m_audiosamplerate;
}

double FlvMetaData::getAudioSamplesize() {
    return m_audiosamplesize;
}

double FlvMetaData::getAudioCodecId() {
    return m_audiocodecid;
}

bool FlvMetaData::getStereo() {
    return m_stereo;
}

FlvReader::FlvReader() {

    m_fp = NULL;
}

FlvReader::~FlvReader() {

    if(m_fp != NULL) {
        fclose(m_fp);
        m_fp = NULL;
    }
}

bool FlvReader::init(string path) {

    m_file = path;

    if((m_fp = fopen(m_file.c_str(), "rb")) == NULL) {
        cerr << "open file: " << m_file << "error!!!" << endl;
        return false;
    }
    return true;
}

FlvHeader* FlvReader::readHeader() {

    unsigned char *header = new unsigned char[FLV_HEAD_LEN];
    fseek(m_fp, 0, SEEK_SET);
    size_t readBytes = fread(header, sizeof(unsigned char), FLV_HEAD_LEN, m_fp);

    if(readBytes == FLV_HEAD_LEN) {
        if(header[0] != 'F' || header[1] != 'L' || header[2] != 'V') {
            cerr << "Not a FLV file!!!" << endl;
            delete header;
            return NULL;
        }

        return new FlvHeader(header, FLV_HEAD_LEN);
    } else {
        delete header;
        return NULL;
    }
}

FlvMetaData* FlvReader::readMeta() {

    unsigned char tagHeader[TAG_HEAD_LEN];
    size_t readBytes;

    fseek(m_fp, 9 + 4, SEEK_SET);
    //read Tag header
    readBytes = fread(tagHeader, sizeof(unsigned char), TAG_HEAD_LEN, m_fp);

    if(readBytes == TAG_HEAD_LEN) {
        //0x12: meta data
        if(tagHeader[0] == 0x12 ) {
             unsigned int tagBodySize = 0;
             tagBodySize |= tagHeader[1];
             tagBodySize = tagBodySize << 8;
             tagBodySize |= tagHeader[2];
             tagBodySize = tagBodySize << 8;
             tagBodySize |= tagHeader[3];

             //cerr << "MetaBody size: " << tagBodySize << endl;

             unsigned char *tag = new unsigned char[tagBodySize + TAG_HEAD_LEN];
             memcpy(tag, tagHeader, TAG_HEAD_LEN);

             readBytes = fread(&tag[TAG_HEAD_LEN], sizeof(unsigned char), tagBodySize, m_fp);
            if(readBytes == tagBodySize) {
                return new FlvMetaData(tag, tagBodySize + TAG_HEAD_LEN);
            } else {
                delete tag;
                return NULL;
            }
        }
    }
    return NULL;
}

int main(int argc, char **argv) {

    FlvMetaData *flvMetaData;
    FlvHeader   *flvHeader;
    FlvReader   flvReader;

    flvReader.init(argv[1]);
    flvHeader = flvReader.readHeader();
    flvMetaData = flvReader.readMeta();

    cout << "duration: " << flvMetaData->getDuration() << "s" << endl;
    cout << "width: " << flvMetaData->getWidth() << endl;
    cout << "height: " << flvMetaData->getHeight() << endl;
    cout << "framerate: " << flvMetaData->getFramerate() << endl;
    cout << "videodatarate: " << flvMetaData->getVideoDatarate() << endl;
    cout << "audiodatarate: " << flvMetaData->getAudioDatarate() << endl;
    cout << "videocodecid: " << flvMetaData->getVideoCodecId() << endl;
    cout << "audiosamplerate: " << flvMetaData->getAudioSamplerate() << endl;
    cout << "audiosamplesize: " << flvMetaData->getAudioSamplesize() << endl;
    cout << "audiocodecid: " << flvMetaData->getAudioCodecId() << endl;
    cout << "stereo: " << flvMetaData->getStereo() << endl;
}
