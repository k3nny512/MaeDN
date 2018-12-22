#ifndef PACKETS_H
#define PACKETS_H

#include <stdint.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
//
// File:    ftProInterface2013SoecketCom.h
//
// Project: ftPro - fischertechnik Control Graphical Programming System
//
// Module:  TCP/IP communication protocol with interface 2013
//
// Author:  Michael S�gtrop
//
///////////////////////////////////////////////////////////////////////////////
//
// Urheberrecht im Sinne des Urheberrechtsgesetzes bei
//
// Michael S�gtrop
// Germany
//
// Copyright (C) 2014
//
///////////////////////////////////////////////////////////////////////////////
//
// TO DO:
//
///////////////////////////////////////////////////////////////////////////////
//
// Usage details for module ftProInterface2013SoecketCom
//
// see also:
//
///////////////////////////////////////////////////////////////////////////////


//#include "common.h"

//******************************************************************************
//*
//* Definition of constants
//*
//******************************************************************************

enum
{
    ftIF2013_nPwmOutputs = 8,
    ftIF2013_nMotorOutputs = 4,
    ftIF2013_nCounters = 4,
    ftIF2013_nUniversalInputs = 8,
    ftIF2013_nIRChannels = 4
};

//******************************************************************************
//*
//* Definitions
//*
//******************************************************************************

// The commands are 4 byte random numbers and serve as command and magic
enum ftIF2013CommandId
{
    ftIF2013CommandId_QueryStatus = 0xDC21219A,

    ftIF2013CommandId_StartOnline = 0x163FF61D,
    ftIF2013CommandId_UpdateConfig = 0x060EF27E,
    ftIF2013CommandId_ExchangeData = 0xCC3597BA,
    ftIF2013CommandId_ExchangeDataCmpr = 0xFBC56F98,
    ftIF2013CommandId_StopOnline = 0x9BE5082C,

    ftIF2013CommandId_StartCameraOnline = 0x882A40A6,
    ftIF2013CommandId_StopCameraOnline = 0x17C31F2F,

    // Used in camera channel
    ftIF2013AcknowledgeId_CameraOnlineFrame = 0xADA09FBA
};
// The response ids are 4 byte random numbers and serve as packet id and magic
enum ftIF2013ResponseId
{
    ftIF2013ResponseId_QueryStatus = 0xBAC9723E,

    ftIF2013ResponseId_StartOnline = 0xCA689F75,
    ftIF2013ResponseId_UpdateConfig = 0x9689A68C,
    ftIF2013ResponseId_ExchangeData = 0x4EEFAC41,
    ftIF2013ResponseId_ExchangeDataCmpr = 0x6F3B54E6,
    ftIF2013ResponseId_StopOnline = 0xFBF600D2,

    ftIF2013ResponseId_StartCameraOnline = 0xCF41B24E,
    ftIF2013ResponseId_StopCameraOnline = 0x4B3C1EB6,

    // Used in camera channel
    ftIF2013DataId_CameraOnlineFrame = 0xBDC2D7A1,
};

// Base class for command packets

struct ftIF2013Command_Base
{
    ftIF2013Command_Base(ftIF2013CommandId p_id):m_id(p_id){}
    ftIF2013CommandId m_id;
};

// Base class for response packets

struct ftIF2013Response_Base
{
    ftIF2013Response_Base(ftIF2013ResponseId p_id):m_id(p_id){}
    ftIF2013ResponseId m_id;
};

// Command packet for QueryStatus Command

struct ftIF2013Command_QueryStatus : public ftIF2013Command_Base
{
    ftIF2013Command_QueryStatus():ftIF2013Command_Base(ftIF2013CommandId_QueryStatus){}
};

// Response packet for QueryStatus Command

struct ftIF2013Response_QueryStatus : public ftIF2013Response_Base
{
    ftIF2013Response_QueryStatus():ftIF2013Response_Base(ftIF2013ResponseId_QueryStatus){}
    char m_devicename[16];
    unsigned int m_version;
};

// Command packet for StartOnline Command

struct ftIF2013Command_StartOnline : public ftIF2013Command_Base
{
    ftIF2013Command_StartOnline():ftIF2013Command_Base(ftIF2013CommandId_StartOnline){}
    char m_name[64];
};

// Response packet for StartOnline Command

struct ftIF2013Response_StartOnline : public ftIF2013Response_Base
{
    ftIF2013Response_StartOnline():ftIF2013Response_Base(ftIF2013ResponseId_StartOnline){}
};

// Command packet for UpdateConfig Command


enum InputMode
{
    MODE_U = 0,
    MODE_R,
    MODE_R2,
    MODE_ULTRASONIC,
    MODE_INVALID
};

// fish.X1 [struct uni_inp_config], 4 bytes
typedef struct uni_inp_config
{
    uint8_t           mode;        // enum InputMode  mode
    uint8_t           digital;
    char            dummy[2];
} UniversalInputsConfig;


// fish.X1 [struct cnt_inp_config], 4 bytes
typedef struct cnt_inp_config
{
    uint8_t           mode;        // 1=normal, 0=inverted;
    char            dummy[3];
} CounterConfig;

// fish.X1 config structure
// [shm_if_config_s], 88 bytes
typedef struct ftX1config
{
    // TX-only: Program run state
    /*uint8_t           pgm_state_req;        // enum PgmState    pgm_state_req;
    uint8_t           old_FtTransfer;
    char              dummy[2];
    // Configuration of motrs
    // 0=single output O1/O2, 1=motor output M1
    uint8_t           motor[IZ_MOTOR];
    // Universal input mode, see enum InputMode
    UNI_CONFIG      uni[IZ_UNI_INPUT];
    // 0=normal, 1=inverted (not really used)
    CNT_CONFIG      cnt[IZ_COUNTER];
    // additional motor configuration data (currently not used)
    int16_t           motor_config[IZ_MOTOR][4];*/
    // TX-only: Program run state
    uint8_t           pgm_state_req;        // enum PgmState    pgm_state_req;
    uint8_t           old_FtTransfer;
    char              dummy[2];
    // Configuration of motrs
    // 0=single output O1/O2, 1=motor output M1
    uint8_t motor[ftIF2013_nMotorOutputs];
    // Universal input mode, see enum InputMode
    UniversalInputsConfig universalInputs[ftIF2013_nUniversalInputs];
    // 0=normal, 1=inverted (not really used)
    CounterConfig counters[ftIF2013_nCounters];
    // additional motor configuration data (currently not used)
    int16_t motor_config[ftIF2013_nMotorOutputs][4];
} FTX1_CONFIG;

struct ftIF2013Command_UpdateConfig : public ftIF2013Command_Base
{
    ftIF2013Command_UpdateConfig():ftIF2013Command_Base(ftIF2013CommandId_UpdateConfig){
       memset( this, 0, sizeof(ftIF2013Command_UpdateConfig));
       m_id = ftIF2013CommandId_UpdateConfig;
       //for(int i = 0;i<ftIF2013_nUniversalInputs;i+=1)
       // m_config.universalInputs[i].mode =MODE_ULTRASONIC;
       m_config_id = 1;
    }
    // config id from FTX1_CONFIG
    int16_t m_config_id;
    int16_t m_extension_id;
    FTX1_CONFIG m_config;
};

// Response packet for UpdateConfig Command

struct ftIF2013Response_UpdateConfig : public ftIF2013Response_Base
{
    ftIF2013Response_UpdateConfig():ftIF2013Response_Base(ftIF2013ResponseId_UpdateConfig){

    }
};

// Command packet for ExchangeData Command

struct ftIF2013Command_ExchangeData : public ftIF2013Command_Base
{
    ftIF2013Command_ExchangeData():ftIF2013Command_Base(ftIF2013CommandId_ExchangeData){
        memset( this + sizeof(ftIF2013Command_Base), 0, sizeof(ftIF2013Command_ExchangeData)-sizeof(ftIF2013Command_Base));
    }
    // Motor / Output PWM data
    // Also for bipolar motor outputs both values are used
    int16_t m_pwmOutputValues[ftIF2013_nPwmOutputs];
    int16_t m_motor_master[ftIF2013_nMotorOutputs];
    int16_t m_motor_distance[ftIF2013_nMotorOutputs];
    int16_t m_motor_command_id[ftIF2013_nMotorOutputs];
    int16_t m_counter_reset_command_id[ftIF2013_nCounters];
    uint16_t  m_sound_command_id;
    uint16_t  m_sound_index;
    uint16_t  m_sound_repeat;
};

// Response packet for ExchangeData Command

struct ftIF2013Response_ExchangeData : public ftIF2013Response_Base
{
    ftIF2013Response_ExchangeData():ftIF2013Response_Base(ftIF2013ResponseId_ExchangeData){}
    // Universal input values
    int16_t m_universalInputs[ftIF2013_nUniversalInputs];
    int16_t m_counter_input[ftIF2013_nCounters];
    int16_t m_counter_value[ftIF2013_nCounters];
    int16_t m_counter_command_id[ftIF2013_nCounters];
    int16_t m_motor_command_id[ftIF2013_nMotorOutputs];
    uint16_t  m_sound_command_id;
    struct IR
    {
        int8_t  m_ir_leftX;  // left  handle, horizontal, -15..15
        int8_t  m_ir_leftY;  // left  handle, vertical,   -15..15
        int8_t  m_ir_rightX; // right handle, horizontal, -15..15
        int8_t  m_ir_rightY; // right handle, vertical,   -15..15
        uint8_t m_ir_bits;   // 2^0=on, 2^1=off, 2^2=switch1, 2^3=switch2
    } m_ir[5];
};

// Command packet for StopOnline Command

struct ftIF2013Command_StopOnline : public ftIF2013Command_Base
{
    ftIF2013Command_StopOnline():ftIF2013Command_Base(ftIF2013CommandId_StopOnline){}
};
// Response packet for StopOnline Command

struct ftIF2013Response_StopOnline : public ftIF2013Response_Base
{
    ftIF2013Response_StopOnline():ftIF2013Response_Base(ftIF2013ResponseId_StopOnline){}
};



// Command packet for StartCameraOnline Command

/*
struct ftIF2013Command_StartCameraOnline : public ftIF2013Command_Base
{
    int32_t m_width;
    int32_t m_height;
    int32_t m_framerate;
    int32_t m_powerlinefreq; // 0=auto, 1=50Hz, 2=60Hz
};
// Response packet for StartCameraOnline Command

struct ftIF2013Response_StartCameraOnline : public ftIF2013Response_Base
{
};

// Command packet for CameraOnlineFrame Command

struct ftIF2013Acknowledge_CameraOnlineFrame : public ftIF2013Command_Base
{
};

// Response packet for CameraOnlineFrame Command

struct ftIF2013Response_CameraOnlineFrame : public ftIF2013Response_Base
{
    int32_t m_numframeready;
    int16_t m_framewidth;
    int16_t m_frameheight;
    int32_t m_framesizeraw;
    int32_t m_framesizecompressed;
    uint8_t m_framedata[0];
};


// Command packet for StopCameraOnline Command

struct ftIF2013Command_StopCameraOnline : public ftIF2013Command_Base
{
};

// Response packet for StopCameraOnline Command

struct ftIF2013Response_StopCameraOnline : public ftIF2013Response_Base
{
};*/
/*
// Computation of CRC32 for a stream of 16 bit numbers
struct CRC32
{
    CRC32();
    void Reset();
    void Add16bit(Uint16_t val);
    uint32_t m_crc;
    uint32_t m_table[256];
};

// Expansion of IO data
class ExpansionBuffer
{
public:
    ExpansionBuffer( UINT8 *buffer, int bufsize );
    void Reset();
    void Rewind();
    uint32_t GetBits( INT32 count );
    Uint16_t GetUint16_t();
    int16_t Getint16_t() { return (int16_t) GetUint16_t(); }
    uint32_t GetCrc() { return m_crc.m_crc; }
    bool GetError() { return m_error; }
    UINT8 *GetBuffer() { return m_compressed; }
    uint32_t GetMaxBufferSize() { return max_compressed_size; }
    void SetBufferSize(uint32_t size) { m_compressed_size_limit = size; }

protected:
    enum {max_word_count=4096};
    Uint16_t m_previous_words[max_word_count];
    uint32_t max_compressed_size;
    UINT8 *m_compressed;
    INT32 m_word_count;
    uint32_t m_compressed_size;
    uint32_t m_compressed_size_limit;
    INT32 m_nochange_count;
    uint32_t m_bitbuffer;
    INT32 m_bitcount;
    CRC32 m_crc;
    bool m_error;
};

// Compression of IO data
class CompressionBuffer
{
public:
    CompressionBuffer(UINT8 *buffer, int bufsize);
    ~CompressionBuffer();
    void Reset();
    void Rewind();
    void PushBits( INT32 count, uint32_t bits );
    void EncodeNoChangeCount();
    void AddWord(Uint16_t word);
    void Finish();
    uint32_t GetCrc() { return m_crc.m_crc; }
    bool GetError() { return m_error; }
    uint32_t GetCompressedSize() { return m_compressed_size; }
    const UINT8 *GetBuffer() { return m_compressed; }
    INT32 GetWordCount() { return m_word_count; }
    Uint16_t GetPrevWord(INT32 i) { return m_previous_words[i]; }

protected:
    enum {max_word_count=4096};
    Uint16_t m_previous_words[max_word_count];
    uint32_t max_compressed_size;
    UINT8 *m_compressed;
    INT32 m_word_count;
    uint32_t m_compressed_size;
    INT32 m_nochange_count;
    uint32_t m_bitbuffer;
    INT32 m_bitcount;
    CRC32 m_crc;
    bool m_error;
#ifdef _DEBUG
    ExpansionBuffer m_check_expand;
#endif
};*/


class ConfigPackageWriter {
protected:
    friend class Controller;
    virtual void writeToConfigPackage(ftIF2013Command_UpdateConfig&)=0;
    virtual ~ConfigPackageWriter();
};

class CommandPackageWriter {
protected:
    friend class Controller;
    virtual void writeToPackage(ftIF2013Command_ExchangeData&)=0;
    virtual ~CommandPackageWriter();
};

class CommandPackageReader {
protected:
    friend class Controller;
    virtual void readFromPackage(ftIF2013Response_ExchangeData&)=0;
    virtual ~CommandPackageReader();
};

#endif // PACKETS_H
