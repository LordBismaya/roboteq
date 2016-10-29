roboteq
=======

ROS driver for serial Roboteq motor controllers. This driver is suitable for use with Roboteq's
Advanced Digital Motor Controllers, as described in [this document][1]. Devices include:

Brushed DC: HDC24xx, VDC24xx, MDC22xx, LDC22xx, LDC14xx, SDC1130, SDC21xx  
Brushless DC: HBL16xx, VBL16xx, HBL23xx, VBL23xx, LBL13xx, MBL16xx, SBL13xx  
Sepex: VSX18xx

The node works by downloading a MicroBasic script to the driver, which then publishes ASCII sentences at 10Hz and 50Hz with the data corresponding to the Status and per-channel Feedback messages published by the driver.

[1]: http://www.roboteq.com/index.php/docman/motor-controllers-documents-and-files/documentation/user-manual/7-nextgen-controllers-user-manual/file

development log
===============

July 27
Made changes to the code. Parity. Stop bits. and Initialization.
The code had worked once. Not working now.
Possible cause. random values in the _txBuffer. 

July 28
Code working. But is still buggy. Strangely enough, the Controller::Write() function doesnt work properly to send the brake commands. This probably might be due to the mutexes that lock during read and write. What worked was storing the message as a string in the tx_buffer and using serial_->write() and then flushing it. NOte the tx_buffer was cleared prior to these operations.

Sept 28
Added fail safe loops. Roboteq now connects by default. Arduino node published brake feedback on brakeInfo msg of rosserial_adc type.

TODO
=====
1. Feedback loops.
2. Initial Position Fwd or Reverse
3. A dedicated button for connect .
4. Fail Safe brakes.
