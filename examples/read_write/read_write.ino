#include "flprog_Can.h"
#include "flprogUart.h"
FLProgCanBus canBus(CAN1, ALT_2);
FLProgCanBusMessage message1(&canBus, 0x5);
FLProgCanBusMessage message2(&canBus, 0x10);
FLProgCanBusMessage message3(&canBus, 8);
FlprogUartExecutor FLProgUart;
uint8_t counter = 0;
void setup()
{
    canBus.baudRate(1000000);
    message1.setMode(FLPROG_CAN_BUS_MESSAGE_PERIODICALLY_MODE);
    message1.setSendPeriod(5);
    message2.setMode(FLPROG_CAN_BUS_MESSAGE_PERIODICALLY_MODE);
    message2.setSendPeriod(500);
    message1.setData(1, 150);
    message3.setMode(FLPROG_CAN_BUS_MESSAGE_READ_MODE);
}

void loop()
{
    canBus.pool();
    message1.pool();
    message2.pool();
    message3.pool();

    counter++;
    if (counter > 200)
    {
        counter = 0;
    }
    message1.setData(3, counter);
    message2.setData(3, counter);
    if (message3.hasNewData())
    {
        CAN_message_t *message = message3.message();
        FLProgUart.printUart((String(message->buf[1]) + "\n"), 1);
        FLProgUart.printUart((String(message->timestamp) + "\n"), 1);
        FLProgUart.printUart("\n", 1);
    }
}