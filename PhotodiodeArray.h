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

class PhotodiodeArray
{
    public:
      PhotodiodeArray();
      void init(int chipSelectPin, int frameReadyPin);
      unsigned int getCentroidReading(unsigned int integrationTime);
      
    private:
      int _frameReadyPin;
      int _chipSelectPin;
      SPISettings _settings;
	  
	  unsigned int calculateCentroid(uint8_t rxBuffer[], unsigned int length);

};
