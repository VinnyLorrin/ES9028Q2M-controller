#ES9028Q2M DAC controller

### 1. Purpose
The ES9028Q2M DAC datasheet is NDA protected, this prevent the designers from disclosing the internal register definitions, making the development of an open source driver impossible. To work around this limitation, people use a micro-controller and publish open source driver with custom register definitions for the MCU. The MCU is used to translate the custom register definition to the ES9028Q2M register definition. This way, the NDA protected information is hide in the MCU firmware. In this work, an ATMAL XMEGA 64A4U MCU is used.

### 2. Firmware architecture
The firmware is developed based on the ATMAL TWI (Two Wire Interface) example. 
A I2C slave is initialized on port C. The I2C slave is responsible to receive the command from the RPi, translate the register address, get information from/to the DAC and report back to RPi. 
A I2C master is initialized on port E. The I2C master is responsible to communicate with the DAC. 
In this example, 16-bit register address is used. 

### 3. Source file contents
1.	twi_slave.c/.h: This is the I2C slave driver. To modify, please focus on function TWI_SlaveReadHandler, which is responsible to do the data pre-process and call the actual data processor function; and function TWI_SlaveDecodeRegAddr, which is used to translate the custom register address definition from the MCU driver to the real DAC register address. The actual DAC register addresses are deleted in the source file but included in the pre-compiled binary file.
2.	i2c_translator.c: This is the program entry. It defines the CPU freq, I2C master speed and initialize the program. To modify, please focus on function TWIC_SlaveProcessData, which is responsible to communicate with DAC and process the data received/to be sent to DAC.
3.	twi_master.c/.h: This is the I2C master driver, no need to change anything.
4.	i2c_translator.HEX: This is the pre-compiled binary that can be loaded to the MCU. 

### 4. Development environment
The firmware is developed with IAR embedded IDE. The final release binary is kept less than 8KB so that a free size limited license can be used to compile the source.

### 5. Related resouce
1.	ATMAL example:

  https://github.com/Synapseware/xmega-intro
  In this work, interrupt (PMIC) and TWI are used.

2.	MCU PCB design:

  https://github.com/VinnyLorrin/ES9028-controller-PCB

3.	MCU Linux ALSA driver:

  https://github.com/VinnyLorrin/ES9028-controller-Linux-Driver
