

#include <vector>
#include <iostream>

#include <lsl_cpp.h>
#include "OTBconfig.h"
#include "conio.h"

#define NB_CHANNELS 408
#define SAMPLING_FREQUENCY 2048
#define CHUNK_SIZE 512


void error(const char *msg)
{
	printf("%s",msg);
	exit(0);
}

void fill_chunk(unsigned char* from, std::vector<std::vector<int16_t>>& to)
{
	to.clear();
	for (unsigned i = 0; i < CHUNK_SIZE; ++i)
		{
			std::vector<int16_t> d;
			to.push_back(d);
			for(unsigned j = 0; j < NB_CHANNELS; j++)
				{
					to[i].push_back( (short)( (from[(i*NB_CHANNELS+j)*2]) |  (from[(i*NB_CHANNELS+j)*2+1]<<8)) );
					/*std::cout << (int)from[(i*NB_CHANNELS+j)*2] << std::endl;
					printBIN(from[(i*NB_CHANNELS+j)*2]);
					std::cout << (int)from[(i*NB_CHANNELS+j)*2+1] << std::endl;
					printBIN(from[(i*NB_CHANNELS+j)*2]);
					std::cout << to[i].channel[j] << std::endl;
				        std::cout << std::endl;*/
				}
					
		}
}




int main()
{
	int portno = 23456;
	SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) error("[ERROR] Opening socket");
    
	struct hostent *server = gethostbyname("169.254.1.10");
	if (server == NULL) error("[ERROR] No such host\n");
    
    
	SOCKADDR_IN serv_addr={0};
	//bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr = *(IN_ADDR*) server->h_addr;
	//bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(portno);
    
    
	std::cout << "[INFOS] Connecting to OTB quattrocento ...\xd" << std::flush;
	if (connect(sockfd,(SOCKADDR *) &serv_addr,sizeof(SOCKADDR)) < 0) error("ERROR connecting");
	std::cout << "[INFOS] Connection to OTB quattrocento established." << std::endl;

	unsigned char config[40];
        config[0] = ACQ_SETT | DECIM | FSAMP_2048 | NCH_IN1to8_MIN1to4 | ACQ_ON;
	config[1]= AN_OUT_IN_SET | ANOUT_GAIN_1 | INSEL3 | INSEL0;
	config[2]= AN_OUT_CH_SET;
	for(int i=3;i<39;i+=3)
		{
			config[i] = INX_CONF0 | Not_defined;
			config[i+1] = INX_CONF1;
			config[i+2] = INX_CONF2 | SIDE_UNDEFINED | HIGH_PASS_FILTER_10 | LOW_PASS_FILTER_500 | DETECTION_MODE_MONOPOLAR;
		}
	config[39] = crc(config);


	
	std::cout << "[INFOS] Reseting  OTB quattrocento acquisition...\xd" << std::flush;
	config[0] -= 1;
	config[39] = crc(config);
	send(sockfd,config,40,0);
	std::cout << "[INFOS] Sending configuration request to OTB quattrocento ...\xd" << std::flush;
	config[0] += 1;
	config[39] = crc(config);
	send(sockfd,config,40,0);
	std::cout << "[INFOS] OTB quattrocento configured.                         " << std::endl;

    	try {
	        lsl::stream_info info("OTB", "quattrocentoSamples", NB_CHANNELS, lsl::IRREGULAR_RATE,lsl::cf_int16);
		lsl::stream_outlet outlet(info);

		std::cout << "[INFOS] Now sending data... (press k to exit)" << std::endl;
		std::cout << "+--------+-------+-------+-------+-------+" << std::endl;
	        for(int i=0;i<5;i++)
			std::cout << "|\t" << i ;
		std::cout << "|" << std::endl;
		std::cout << "+--------+-------+-------+-------+-------+" << std::endl;
		std::vector<std::vector<int16_t>> chunk;
		unsigned char buffer[NB_CHANNELS*CHUNK_SIZE*2];
		bool Kpressed = true;
		do {
			int data_remaining = NB_CHANNELS*CHUNK_SIZE*2;
			while(data_remaining > 0)
				data_remaining -= recv(sockfd,buffer+(NB_CHANNELS*CHUNK_SIZE*2-data_remaining), data_remaining,0);
				
			fill_chunk(buffer,chunk);
			for(int i=0;i<5;i++)
				std::cout << "|" << chunk[0][i] << "\t " ;
			std::cout << "|\xd" << std::flush;
		        

			// send it
			outlet.push_chunk(chunk);
			
			if (_kbhit())
				Kpressed = (toupper((char)_getch()) != 'K');
			
		}
		while (Kpressed);

	} catch (std::exception& e) { std::cerr << "[ERROR] Got an exception: " << e.what() << std::endl; }

	//ask to stop the acquisition
	std::cout << "[INFOS] Ending acquisition ...\xd" << std::flush;
        config[0] = ACQ_SETT | DECIM | FSAMP_2048 | NCH_IN1to8_MIN1to4 | ACQ_OFF;
        config[39] = crc(config);
        send(sockfd,config,40,0);
	closesocket(sockfd);
	std::cout << "[INFOS] Acquisition ended.     " << std::endl;
    
	return 0;
}
