#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "SMBSlave.h"

static SMBData smb;                        //!< SMBus driver data


extern void ProcessMessage(SMBData *smb);
extern void ProcessReceiveByte(SMBData *smb);

uint8_t SMBError(void);

/*! \brief Initialize TWI module
 *
 *  This function initializes the TWI module for SMBus operation.
 */
void SMBusInit()
{
    // Set own slave address
    TWAR = SMB_OWN_ADDRESS << 1;
    // Enable TWI-interface, enable ACK, enable interrupt, clear interrupt flag
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
}


/*! \brief Enable the SMBus driver
 *
 * This function enables communications involving this device on the SMBus.
 */
void SMBEnable(void)
{
    smb.enable = TRUE;
}


/*! \brief Disable the SMBus driver
 *
 * This function disables communications involving this device on the SMBus.
 * Note that the slave address will still be ACKed according to the SMBus
 * 2.0 specification.void ProcessMessage(SMBData *smb)
 */
void SMBDisable(void)
{
    smb.enable = FALSE;
}


/*! \brief Returns the error flag for the last message
 *
 *  Returns the error flag for the last SMBus communication.
 *
 *  \retval 1  There was an error in the last communication
 *  \retval 0  The last SMBus communication was successful.
 */
uint8_t SMBError(void)
{
    return smb.error;
}


 /*! \brief TWI interrupt routine
  *
  * The TWI interrupt routine
  */

ISR (TWI_vect)
{
    uint8_t enableACK;
    uint8_t temp;

    // Enable ACKing if not explicitly disabled later in this ISR.
    enableACK = TRUE;

    // Is this the start of a protocol?
    if (smb.state == SMB_STATE_IDLE)
    {
        // Reset SMBus variables.
        smb.txLength = 0;
        smb.txCount = 0;
        smb.rxCount = 0;
        smb.error = FALSE;
    }

    // Use the TWI status information to make desicions.
    switch (TWSR & 0xf8)
    {
        case 0x60:      // SLA + W received, ACK returned
        {
            // State should be IDLE when SLA+W is received. If SLA+W is received
            // and state is not IDLE, an error has probably occured in an earlier
            // transmission that could not be detected at the time. Nothing can be
            // done to rescue the last transmission, but the SMBus driver variables
            // should be reset so the ongoing transmission can complete correctly.
          if (smb.state != SMB_STATE_IDLE)
          {
              smb.txLength = 0;
              smb.txCount = 0;
              smb.rxCount = 0;
              smb.error = FALSE;
          }
          smb.state = SMB_STATE_WRITE_REQUESTED;

          break;
        }

        case 0x80:      // Previously addressed with own SLA+W, data received, ACK returned
        {
            // Store data received in receive buffer and increase receive count.
            temp = TWDR;
            smb.rxBuffer[smb.rxCount] = temp;
            smb.rxCount++;

            // If the receive buffer is full, disable ACKing of the next data byte.
            if (smb.rxCount == SMB_RX_BUFFER_LENGTH)
            {
                enableACK = FALSE;
            }
            break;
        }

        case 0x88:      // Previously addressed with own SLA+W, data received, NACK returned.
        {
            smb.error = TRUE;
            smb.state = SMB_STATE_IDLE;
            break;
        }

        case 0xa0:   // P or Sr received while still addressed.
        {
            ProcessMessage(&smb);
            break;
        }

        case 0xa8:  // Own SLA +R received, ACK returned.
        {
            // Calculate PEC of own address + R.
            if (smb.state == SMB_STATE_IDLE) // Receive byte.
            {
                ProcessReceiveByte(&smb);
                smb.state = SMB_STATE_READ_REQUESTED;
            }

            // Make the first byte of txBuffer ready for transmission.
            temp = smb.txBuffer[0];
            TWDR = temp;
            smb.txCount++;

            break;
        }

        case 0xb8:      // Data byte in TWDR transmitted, ACK received.
        {
            if (smb.txCount == smb.txLength)
            {
                smb.error = TRUE;   // If PEC is disabled, an ACK here is an error.
                enableACK = FALSE;
            }
            else
            {
                temp = smb.txBuffer[smb.txCount];
                TWDR = temp;
                smb.txCount++;
                #ifdef SMB_SUPPORT_PEC
                SMBcrc(temp);
                #endif

            }
            break;
        }

        case 0xc0:      // Data byte in TWDR transmitted, NACK received.
        {
            if (smb.txCount != smb.txLength)
            {
                // Error, NACK is only expected after last data byte or PEC.
                smb.error = TRUE;
            }
            smb.state = SMB_STATE_IDLE;
            break;
        }

        case 0xc8:      // Last data byte in TWDR transmitted, ACK received.
        {
            smb.error = TRUE;
            smb.state = SMB_STATE_IDLE;
            break;
        }

        case 0x00:      // Bus error due to an illegal START or STOP condition.
        {
            TWCR |= (1 << TWSTO);
            smb.error = TRUE;
            smb.state = SMB_STATE_IDLE;
            break;
        }

        default:        // Unexpected TWSR value, error.
        {

            smb.error = TRUE;
            smb.state = SMB_STATE_IDLE;
            break;
        }
    }

    // Issue next TWI command.
    if (enableACK)
    {
        // Set TWEA flag and don't clear TWINT (if set) at the same time.
        temp = TWCR;
        temp |= (1 << TWEA);
        temp &= ~(1 << TWINT);
        TWCR = temp;
    }
    else
    {
        // Clear TWEA flag and make sure that TWINT is not cleared at the same time.
        TWCR &= ~((1 << TWEA) | (1 << TWINT));
    }

    TWCR |= (1 << TWEN) | (1 << TWIE) | (1 << TWINT);
}
