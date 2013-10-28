// Arduino Due - CAN Sample 2
// Brief CAN example for Arduino Due
// Test the transmission from CAN0 Mailbox 0 to CAN1 Mailbox 0 using interruption
// By Thibaut Viard/Wilfredo Molina/Collin Kidder 2013

// Required libraries
#include "variant.h"
#include "due_can.h"

#define TEST1_CAN_COMM_MB_IDX    0
#define TEST1_CAN_TRANSFER_ID    0x07
#define TEST1_CAN0_TX_PRIO       15
#define CAN_MSG_DUMMY_DATA       0x55AABB66

// CAN frame max data length
#define MAX_CAN_FRAME_DATA_LEN   3

//Message variable to be send
uint32_t CAN_MSG_1 = 0xF;

void setup() {

// start serial port at 9600 bps: 
  Serial.begin(9600);
  Serial.println("Type anything");
  while (Serial.available() == 0);  
}

double parseSpeedReference(uint8_t *data) {
  uint16_t value = data[1] | (data[2] << 8);
  return value / 32767.0;
}

uint32_t parsePositionsReference(uint8_t *data) {
  return data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24);
}

double parseTorqueReference(uint8_t *data) {
  uint16_t value = data[1] | (data[2] << 8);
  return value / 32760.0;
}

// Test the transmission from CAN0 Mailbox 0 to CAN1 Mailbox 0
static void test_1(void)
{
  //Reset all CAN0 and CAN1 mailboxes:
  CAN.reset_all_mailbox();
  CAN2.reset_all_mailbox();

  CAN.mailbox_set_mode(0, CAN_MB_RX_MODE); 
  CAN.mailbox_set_accept_mask(0, 0, false);
  CAN.mailbox_set_id(0, TEST1_CAN_TRANSFER_ID, false);
  
  CAN2.mailbox_set_mode(0, CAN_MB_TX_MODE);
  CAN2.mailbox_set_priority(0, TEST1_CAN0_TX_PRIO);
  CAN2.mailbox_set_accept_mask(0, 0x7FF, false);
  CAN2.mailbox_set_id(0, TEST1_CAN_TRANSFER_ID, false);
  
  // Speed reference
  /*
  CAN2.mailbox_set_datalen(0, 3);
  CAN2.mailbox_set_datah(0, 0x00000000); // byte 7-4
  CAN2.mailbox_set_datal(0, 0x000CCD31); // byte 3-0
  */
  
  // Positions reference
  /*
  CAN2.mailbox_set_datalen(0, 5);
  CAN2.mailbox_set_datah(0, 0x00000000); // byte 7-4
  CAN2.mailbox_set_datal(0, 0x2DC6C06E); // byte 3-0
  */
  
  // Torque reference
  CAN2.mailbox_set_datalen(0, 3);
  CAN2.mailbox_set_datah(0, 0x00000000); // byte 7-4
  CAN2.mailbox_set_datal(0, 0x003FFC90); // byte 3-0

  // Enable interrupt for mailbox 0 on first CAN
  CAN.enable_interrupt(CAN_IER_MB0);

  // Send out the information in the mailbox
  CAN2.global_send_transfer_cmd(CAN_TCR_MB0);
  // Wait for the communication to be completed.
  while (!CAN.rx_avail()) { //while no frame is received
  }
  RX_CAN_FRAME inFrame;
  CAN.get_rx_buff(&inFrame);

  Serial.println("CAN message received= ");
  
  if (inFrame.data[0] == 0x31) { // Speed reference
    double value = parseSpeedReference(inFrame.data);
    Serial.println(value);
  } else if (inFrame.data[0] == 0x6E) { // Positions reference
    uint32_t value = parsePositionsReference(inFrame.data);
    Serial.println(value);
  } else if (inFrame.data[0] == 0x90) { // Torque reference
    double value = parseTorqueReference(inFrame.data);
    Serial.println(value);
  }
  
}

// can_example application entry point
void loop()
{
  while (Serial.available() > 0) {
     CAN_MSG_1 = Serial.parseInt();
      if (Serial.read() == '\n') {      
      Serial.print("Sent value= ");
      Serial.println(CAN_MSG_1);
      delay(1000);
    }
  }

// Verify CAN0 and CAN1 initialization, baudrate is 1Mb/s:
if (CAN.init(SystemCoreClock, CAN_BPS_1000K) &&
CAN2.init(SystemCoreClock, CAN_BPS_1000K)) {

// Disable all CAN0 & CAN1 interrupts
CAN.disable_interrupt(CAN_DISABLE_ALL_INTERRUPT_MASK);
CAN2.disable_interrupt(CAN_DISABLE_ALL_INTERRUPT_MASK);

// Configure and enable interrupt of CAN1, as the tests will use receiver interrupt
NVIC_EnableIRQ(CAN0_IRQn);

// Run test
test_1();

// Disable CAN0 Controller
CAN.disable();
// Disable CAN1 Controller
CAN2.disable();
} else {
Serial.println("CAN initialization (sync) ERROR");
}
while(1){}
}
