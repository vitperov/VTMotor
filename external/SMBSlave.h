#ifndef __SMB_SLAVE_H__
#define __SMB_SLAVE_H__

//! The 7 bit slave address of this device.
#define SMB_OWN_ADDRESS       0x28

/*!
 *  Maximum number of data bytes received for Block write and Block write,
 *  block read process call. (Max value is 32).
 */
#define SMB_RX_MAX_LENGTH       32

/*
 *  Maximum number of data bytes transmitted for Block read and Block write,
 *  block read process call. (Max value is 32).
 */
#define SMB_TX_MAX_LENGTH       32

//! Length of command code.
#define SMB_COMMAND_CODE_LENGTH   1

//! Length of byte count.
#define SMB_BYTE_COUNT_LENGTH     1

//! Length of receive buffer, must be large enough to include control bytes.
#define SMB_RX_BUFFER_LENGTH    (SMB_COMMAND_CODE_LENGTH + SMB_BYTE_COUNT_LENGTH + SMB_RX_MAX_LENGTH)

/*!
 * Length of transmit buffer, must be large enough to include control bytes.
 */
#define SMB_TX_BUFFER_LENGTH    (SMB_BYTE_COUNT_LENGTH + SMB_TX_MAX_LENGTH)

//! Value of write bit appended after slave address in SMBus communication.
#define SMB_WRITE                       0

//! Value of read bit appended after slave address in SMBus communication.
#define SMB_READ                        1

//! Value of slave address with write bit appended (used for PEC calculation/lookup).
#define SMB_OWN_ADDRESS_W               ((SMB_OWN_ADDRESS << 1) | SMB_WRITE)

//! Value of slave address with reaad bit appended (used for PEC calculation/lookup).
#define SMB_OWN_ADDRESS_R               ((SMB_OWN_ADDRESS << 1) | SMB_READ)

#define SMB_STATE_IDLE                  0x00    //!< Idle state flag.
#define SMB_STATE_READ_REQUESTED        0x01    //!< Read requested flag.
#define SMB_STATE_WRITE_REQUESTED       0x02    //!< Write requested flag.
#define SMB_STATE_WRITE_READ_REQUESTED  0x03    //!< Write, then read requested flag.


#define TRUE    1
#define FALSE   0


// Function prototypes.
void SMBusInit(void);
void SMBEnable(void);
void SMBDisable(void);

/*!
 *  The SMBData struct contains all the variables used internally by the SMBus slave.
 */
typedef struct SMBData
{
    uint8_t txLength;                         //!< Transmit length.
    uint8_t txCount;                          //!< Transmit counter.
    uint8_t rxCount;                          //!< Receive counter.
    uint8_t state:2;                          //!< SMBus driver state flag.
    uint8_t volatile enable : 1;              //!< Enable ACK on requests.
    uint8_t volatile error : 1;               //!< Error flag.

    uint8_t rxBuffer[SMB_RX_BUFFER_LENGTH];   //!< Receive buffer.
    uint8_t txBuffer[SMB_TX_BUFFER_LENGTH];   //!< Transmit buffer.
} SMBData;

#endif
