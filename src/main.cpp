#include <stdlib.h>  
#include <string.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <netdb.h>

#include <vector>
#include <iostream>

#include <lsl_cpp.h>
#include "OTBconfig.h"

#define NB_CHANNELS 408
#define SAMPLING_FREQUENCY 2048
#define CHUNK_SIZE 1024

#pragma pack(1)
typedef struct _data_sample {
        short channel[NB_CHANNELS];
} data_sample;


void error(const char *msg)
{
	perror(msg);
	exit(0);
}

void fill_chunk(char* from, std::vector<data_sample>& to)
{
	to.clear();
	for (unsigned i = 0; i < CHUNK_SIZE; ++i)
		{
			data_sample d;
			to.push_back(d);
			for(unsigned j = 0; j < NB_CHANNELS; j++)
				to[i].channel[j] = ((from[i*2]&0xFF) << 8) |  (from[i*2+1]&0xFF);
		}
}




int main()
{
	int portno = 23456;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) error("[ERROR] Opening socket");
    
	struct hostent *server = gethostbyname("169.254.1.10");
	if (server == NULL) error("[ERROR] No such host\n");
    
	struct sockaddr_in serv_addr;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(portno);
    
    
	std::cout << "[INFOS] Connecting to OTB quattrocento ...\xd" << std::flush;
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) error("ERROR connecting");
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


	
	std::cout << "[INFOS] Sending configuration request to OTB quattrocento ...\xd" << std::flush;
	write(sockfd,config,40);
	std::cout << "[INFOS] OTB quattrocento configured." << std::endl;

    	try {
	        lsl::stream_info info("OTB", "quattrocentoSamples", NB_CHANNELS);
		lsl::stream_outlet outlet(info);

		std::cout << "[INFOS] Now sending data..." << std::endl;
		std::vector<data_sample> chunk;
		char buffer[NB_CHANNELS*CHUNK_SIZE*2];
		for (;;)
			{
				int data_remaining = NB_CHANNELS*CHUNK_SIZE*2;
				while(data_remaining > 0)
						data_remaining -= read(sockfd,buffer+(NB_CHANNELS*CHUNK_SIZE*2-data_remaining), data_remaining);
				
				fill_chunk(buffer,chunk);

				// send it
				outlet.push_chunk_numeric_structs(chunk);
			}

	} catch (std::exception& e) { std::cerr << "[ERROR] Got an exception: " << e.what() << std::endl; }

	//ask to stop the acquisition
	std::cout << "[INFOS] Ending acquisition ...\xd" << std::flush;
        config[0] = ACQ_SETT | DECIM | FSAMP_2048 | NCH_IN1to8_MIN1to4 | ACQ_OFF;
        config[39] = crc(config);
        write(sockfd,config,40);
	close(sockfd);
	std::cout << "[INFOS] Acquisition ended." << std::endl;
    
	return 0;
}
