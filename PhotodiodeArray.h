#include <SPI.h>

#define SPI_FREQ_DEFAULT 5000000

// MLX75306 Command bytes
#define MLX75306_START_INTEGRATION 	    0xB8
#define MLX75306_READOUT_8_BIT 		      0x99
#define MLX75306_RESET 				          0xF0
#define MLX75306_WAKEUP 			          0xC3

// MLX75306 Readout parameters
#define MLX75306_READOUT_HEADER_SIZE    13
#define MLX75306_READOUT_FOOTER_SIZE    4
#define MLX75306_ARRAY_BEGIN		        2
#define MLX75306_ARRAY_END			        143
#define MLX75306_READOUT_SIZE		        (MLX75306_ARRAY_END - MLX75306_ARRAY_BEGIN + 1) + MLX75306_READOUT_HEADER_SIZE + MLX75306_READOUT_FOOTER_SIZE

#define MLX75306_CRC16                  ((1 << 16) | (1 << 12) | (1 << 5) | (1 << 0)) 

class PhotodiodeArray
{
    public:
      PhotodiodeArray();
      void init(int16_t chipSelectPin, int16_t frameReadyPin);
      int16_t getCentroidReading(uint32_t& errorSum, uint16_t integrationTime, bool startIntegration);
      void startIntegration(uint16_t integrationTime);


    private:
      int16_t _frameReadyPin;
      int16_t _chipSelectPin;
      SPISettings _settings;
      bool _readingStarted;
      
      uint16_t calculateCentroid(uint8_t rxBuffer[], uint16_t length, uint32_t& errorSum);
      uint16_t calculateCRC(uint8_t rxBuffer[], uint16_t length);
};
