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
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(CAN.init(SystemCoreClock,CAN_BPS_1000K)==false){
  }  
}

void loop() {
  Serial.print("helloworld\n");
  //Reset all CAN0 and CAN1 mailboxes:
  CAN.reset_all_mailbox();

  CAN.mailbox_set_mode(0, CAN_MB_TX_MODE);
  CAN.mailbox_set_priority(0, TEST1_CAN0_TX_PRIO);
  CAN.mailbox_set_accept_mask(0, 0x7FF, false);
  CAN.mailbox_set_id(0, TEST1_CAN_TRANSFER_ID, false);
  CAN.mailbox_tx_frame(0x201);
  
  //request data
  CAN.mailbox_set_datalen(0, 3);
  CAN.mailbox_set_datah(0, 0x00000000); // byte 7-4
  CAN.mailbox_set_datal(0, 0x0000403d); // byte 3-0
  
  Serial.println("Sending data");

  // Send out the information in the mailbox
  CAN.global_send_transfer_cmd(CAN_TCR_MB0);
  
  CAN.mailbox_set_mode(0, CAN_MB_RX_MODE); 
  
  // Wait for the communication to be completed.
  while (!CAN.rx_avail()) { //while no frame is received
  }
  
  Serial.println("Got something!");
  
  RX_CAN_FRAME inFrame;
  CAN.get_rx_buff(&inFrame);
  
  if (inFrame.id == 0x181) { // Speed reference
    Serial.print("Motor Controller");
  }
}

