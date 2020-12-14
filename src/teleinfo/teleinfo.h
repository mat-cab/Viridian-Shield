#pragma once

#include <Arduino.h>

// Inspired by arduino-teleInfo by jaysee available at 
// https://github.com/jaysee/teleInfo

// Size for the teleinfo buffers
// max length for the labels
static const uint8_t TELEINFO_LABEL_BUFFER_SIZE = 10;
// max length for the values
static const uint8_t TELEINFO_VALUE_BUFFER_SIZE = 16;

typedef struct teleinfo_t teleinfo_t;
struct teleinfo_t {
	char ADCO[12]; 
	char OPTARIF[4]; 
	uint8_t ISOUSC; 
	char PTEC[4]; 
	uint8_t IINST; 
	uint8_t IINST1; 
	uint8_t IINST2; 
	uint8_t IINST3; 
	uint8_t ADPS; 
	uint8_t IMAX; 
	uint8_t IMAX1; 
	uint8_t IMAX2; 
	uint8_t IMAX3; 
	uint32_t PAPP; 
	uint32_t PMAX; 

	uint32_t BASE; 

	uint32_t HCHC; 
	uint32_t HCHP; 

	uint32_t EJP_HN; 
	uint32_t EJP_HPM; 
	uint8_t PEJP; 

	uint32_t BBR_HC_JB; 
	uint32_t BBR_HP_JB; 
	uint32_t BBR_HC_JW; 
	uint32_t BBR_HP_JW; 
	uint32_t BBR_HC_JR; 
	uint32_t BBR_HP_JR; 
	char DEMAIN[4]; 
	char HHPHC;

    char MOTDETAT[6]; 
};

class teleinfo {
    public:
        static void initialize();
        
        static teleinfo_t read();     
    private:
        static void clearBuffer();
        static bool readLine();
        static inline bool readWord(char* buffer, uint8_t maxBufferLength, uint8_t &cks);
        static inline bool record(const char* label, char* destination);
        static inline bool record(const char* label, uint8_t &destination);
        static inline bool record(const char* label, uint32_t &destination);

        static char labelBuffer[TELEINFO_LABEL_BUFFER_SIZE];
        static char valueBuffer[TELEINFO_VALUE_BUFFER_SIZE];
};