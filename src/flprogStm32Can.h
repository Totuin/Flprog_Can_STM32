
#ifndef STM32_CAN_H
#define STM32_CAN_H

// couple of workarounds
#if defined(STM32F3xx)
#define GPIO_AF9_CAN1 GPIO_AF9_CAN
#define CAN1_RX0_IRQn CAN_RX0_IRQn
#define CAN1_TX_IRQn CAN_TX_IRQn
#define GPIO_SPEED_FREQ_VERY_HIGH GPIO_SPEED_FREQ_HIGH
#define CAN1_TX_IRQHandler CAN_TX_IRQHandler
#define CAN1_RX0_IRQHandler CAN_RX0_IRQHandler
#endif

#if defined(STM32F0xx)
#define CAN1_TX_IRQn CEC_CAN_IRQn
#define CAN1_RX0_IRQn CEC_CAN_IRQn
#endif

#include <Arduino.h>
#include "flprogUtilites.h"

#define FLPROG_CAN_BUS_NORMAL_MODE 0
#define FLPROG_CAN_BUS_LOOPBACK_MODE 1
#define FLPROG_CAN_BUS_SILENT_MODE 2
#define FLPROG_CAN_BUS_SILENT_LOOPBACK_MODE 3

#define FLPROG_CAN_BUS_MESSAGE_CHANGE_MODE 0
#define FLPROG_CAN_BUS_MESSAGE_PERIODICALLY_MODE 1
#define FLPROG_CAN_BUS_MESSAGE_CHANGE_PERIODICALLY_MODE 2
#define FLPROG_CAN_BUS_MESSAGE_READ_MODE 3

typedef struct CAN_message_t
{
    uint32_t id = 0;        // can identifier
    uint16_t timestamp = 0; // time when message arrived
    uint8_t idhit = 0;      // filter that id came from
    struct
    {
        bool extended = 0; // identifier is extended (29-bit)
        bool remote = 0;   // remote transmission request packet type
        bool overrun = 0;  // message overrun
        bool reserved = 0;
    } flags;
    uint8_t len = 8;      // length of data
    uint8_t buf[8] = {0}; // data
    int8_t mb = 0;        // used to identify mailbox reception
    uint8_t bus = 1;      // used to identify where the message came (CAN1, CAN2 or CAN3)
    bool seq = 0;         // sequential frames
} CAN_message_t;

typedef struct
{
    uint32_t baudrate;
    uint16_t prescaler;
    uint8_t time_quanta;
    uint8_t timeseg1;
    uint8_t timeseg2;
} Baudrate_entry_t;

typedef enum CAN_PINS
{
    DEF,
    ALT,
    ALT_2,
} CAN_PINS;

typedef enum RXQUEUE_TABLE
{
    RX_SIZE_2 = (uint16_t)2,
    RX_SIZE_4 = (uint16_t)4,
    RX_SIZE_8 = (uint16_t)8,
    RX_SIZE_16 = (uint16_t)16,
    RX_SIZE_32 = (uint16_t)32,
    RX_SIZE_64 = (uint16_t)64,
    RX_SIZE_128 = (uint16_t)128,
    RX_SIZE_256 = (uint16_t)256,
    RX_SIZE_512 = (uint16_t)512,
    RX_SIZE_1024 = (uint16_t)1024
} RXQUEUE_TABLE;

typedef enum TXQUEUE_TABLE
{
    TX_SIZE_2 = (uint16_t)2,
    TX_SIZE_4 = (uint16_t)4,
    TX_SIZE_8 = (uint16_t)8,
    TX_SIZE_16 = (uint16_t)16,
    TX_SIZE_32 = (uint16_t)32,
    TX_SIZE_64 = (uint16_t)64,
    TX_SIZE_128 = (uint16_t)128,
    TX_SIZE_256 = (uint16_t)256,
    TX_SIZE_512 = (uint16_t)512,
    TX_SIZE_1024 = (uint16_t)1024
} TXQUEUE_TABLE;

typedef enum CAN_BANK
{
    MB0 = 0,
    MB1 = 1,
    MB2 = 2,
    MB3 = 3,
    MB4 = 4,
    MB5 = 5,
    MB6 = 6,
    MB7 = 7,
    MB8 = 8,
    MB9 = 9,
    MB10 = 10,
    MB11 = 11,
    MB12 = 12,
    MB13 = 13,
    MB14 = 14,
    MB15 = 15,
    MB16 = 16,
    MB17 = 17,
    MB18 = 18,
    MB19 = 19,
    MB20 = 20,
    MB21 = 21,
    MB22 = 22,
    MB23 = 23,
    MB24 = 24,
    MB25 = 25,
    MB26 = 26,
    MB27 = 27
} CAN_BANK;

typedef enum CAN_FLTEN
{
    ACCEPT_ALL = 0,
    REJECT_ALL = 1
} CAN_FLTEN;

typedef enum IDE
{
    STD = 0,
    EXT = 1,
    AUTO = 2
} IDE;

class FLProgCanBus
{

public:
    FLProgCanBus(CAN_TypeDef *canPort, CAN_PINS pins, RXQUEUE_TABLE rxSize = RX_SIZE_16, TXQUEUE_TABLE txSize = TX_SIZE_16);

    void pool();

    void baudRate(uint32_t baud);
    void mode(uint8_t mode);

    uint32_t baudRate() { return _baud; };
    uint8_t mode() { return _mode; };
    bool hasNewReadMessage() { return _hasNewMessage; };
    CAN_message_t *getReadMessage() { return &_readMessage; };

    bool write(CAN_message_t &CAN_tx_msg, bool sendMB = false);
    bool read(CAN_message_t &CAN_rx_msg);

    // Manually set STM32 filter bank parameters
    bool setFilter(uint8_t bank_num, uint32_t filter_id, uint32_t mask, IDE = AUTO, uint32_t filter_mode = CAN_FILTERMODE_IDMASK, uint32_t filter_scale = CAN_FILTERSCALE_32BIT, uint32_t fifo = CAN_FILTER_FIFO0);
    // Teensy FlexCAN style "set filter" -functions
    bool setMBFilterProcessing(CAN_BANK bank_num, uint32_t filter_id, uint32_t mask, IDE = AUTO);
    void setMBFilter(CAN_FLTEN input);                                           /* enable/disable traffic for all MBs (for individual masking) */
    void setMBFilter(CAN_BANK bank_num, CAN_FLTEN input);                        /* set specific MB to accept/deny traffic */
    bool setMBFilter(CAN_BANK bank_num, uint32_t id1, IDE = AUTO);               /* input 1 ID to be filtered */
    bool setMBFilter(CAN_BANK bank_num, uint32_t id1, uint32_t id2, IDE = AUTO); /* input 2 ID's to be filtered */

    void enableLoopBack(bool yes = 1);
    void enableSilentMode(bool yes = 1);
    void enableSilentLoopBack(bool yes = 1);
    void enableFIFO(bool status = 1);
    void enableMBInterrupts();
    void disableMBInterrupts();

    // These are public because these are also used from interrupts.
    typedef struct RingbufferTypeDef
    {
        volatile uint16_t head;
        volatile uint16_t tail;
        uint16_t size;
        volatile CAN_message_t *buffer;
    } RingbufferTypeDef;

    RingbufferTypeDef rxRing;
    RingbufferTypeDef txRing;

    bool addToRingBuffer(RingbufferTypeDef &ring, const CAN_message_t &msg);
    bool removeFromRingBuffer(RingbufferTypeDef &ring, CAN_message_t &msg);

protected:
    uint16_t sizeRxBuffer;
    uint16_t sizeTxBuffer;
    bool _isInit = false;
    uint32_t _baud = 500000;
    bool _retransmission = false;
    uint8_t _mode = FLPROG_CAN_BUS_NORMAL_MODE;
    CAN_message_t _readMessage;
    bool _hasNewMessage = false;

private:
    void init();
    void privateSetMode();

    void begin(bool retransmission = false);
    void setBaudRate(uint32_t baud);

    void initializeFilters();
    bool isInitialized() { return rx_buffer != 0; }
    void initRingBuffer(RingbufferTypeDef &ring, volatile CAN_message_t *buffer, uint32_t size);
    void initializeBuffers(void);
    bool isRingBufferEmpty(RingbufferTypeDef &ring);
    uint32_t ringBufferCount(RingbufferTypeDef &ring);
    void calculateBaudrate(CAN_HandleTypeDef *CanHandle, int Baudrate);
    void setBaudRateValues(CAN_HandleTypeDef *CanHandle, uint16_t prescaler, uint8_t timeseg1,
                           uint8_t timeseg2, uint8_t sjw);
    uint32_t getAPB1Clock(void);

    volatile CAN_message_t *rx_buffer;
    volatile CAN_message_t *tx_buffer;

    static constexpr Baudrate_entry_t BAUD_RATE_TABLE_48M[]{
        {1000000, 3, 16, 13, 2},
        {800000, 4, 15, 12, 2},
        {500000, 6, 16, 13, 2},
        {250000, 12, 16, 13, 2},
        {125000, 24, 16, 13, 2},
        {100000, 30, 16, 13, 2}};

    static constexpr Baudrate_entry_t BAUD_RATE_TABLE_45M[]{
        {1000000, 3, 15, 12, 2},
        {500000, 5, 18, 15, 2},
        {250000, 10, 18, 15, 2},
        {125000, 20, 18, 15, 2},
        {100000, 25, 18, 15, 2}};

    bool _canIsActive = false;
    CAN_PINS _pins;

    CAN_HandleTypeDef *n_pCanHandle;
    CAN_TypeDef *_canPort;
};

class FLProgCanBusMessage
{
public:
    FLProgCanBusMessage(FLProgCanBus *cunBus, uint32_t id, bool extended = false, uint8_t len = 8);
    void setMode(uint8_t mode) { _mode = mode; };
    void setSendPeriod(uint32_t sendPeriod) { _sendPeriod = sendPeriod; };
    void send(bool value);
    void setData(uint8_t index, uint8_t value);

    uint8_t getMode() { return _mode; };
    uint32_t getSendPeriod() { return _sendPeriod; };
    uint8_t getData(uint8_t index);
    bool hasNewData() { return _hasNewData; };
    CAN_message_t *message() { return &_message; };

    void pool();

private:
    void readPool();
    void sendPool();
    void checkNeedSend();
    CAN_message_t _message;
    FLProgCanBus *_cunBus = 0;
    bool _isNeedSend = true;
    bool _hasNewData = false;
    bool _sendOldValue = false;
    uint8_t _mode = FLPROG_CAN_BUS_MESSAGE_READ_MODE;
    uint32_t _sendPeriod = 50;
    uint32_t _sendTime = flprog::timeBack(_sendPeriod);
};

#endif
