#include <vector>
#include <fstream>
#include <iostream>

#include <lsl_cpp.h>
#include "OTBconfig.h"
#include "tools.h"
#define SAMPLING_FREQUENCY 2048
#define CHUNK_SIZE 1

typedef struct _nw
{
  int portno;
  SOCKET sockfd;
  struct hostent *server;
  SOCKADDR_IN serv_addr={0};
  fd_set set;
  struct timeval timeout;
  
} NW;

void tcp_connect(NW* n, int portno, char* ip)
{
  ////////////////////// OTB IP CONNECTION //////////////////////
  n->portno = portno;
  n->sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (n->sockfd < 0) error("[ERROR] Opening socket");
    
  n->server = gethostbyname(ip);
  if (n->server == NULL) error("[ERROR] No such host\n");


  n->serv_addr.sin_family = AF_INET;
  n->serv_addr.sin_addr = *(IN_ADDR*) n->server->h_addr;
  n->serv_addr.sin_port = htons(portno);
    
  std::cout << "[INFOS] Connecting to OTB quattrocento ...\xd" << std::flush;
  if (connect(n->sockfd,(SOCKADDR *)&(n->serv_addr), sizeof(SOCKADDR)) < 0) error("ERROR connecting");
  std::cout << "[INFOS] Connection to OTB quattrocento established." << std::endl;
  ///////////////////////////////////////////////////////////////
  FD_ZERO(&(n->set)); /* clear the set */
  FD_SET(n->sockfd, &(n->set)); /* add our file descriptor to the set */
  n->timeout.tv_sec = 1;
  n->timeout.tv_usec = 0;
}

/*
** TODO testcpp
   DEADLINE: <2019-05-20 lun.> SCHEDULED: <2019-05-16 jeu.>
 */
/**
 * @brief fill_chunk transform a unsigned char array into a typed vector of vector.
 * @tparam T Type of the vector.
 * @param from Unsigned char array to transform.
 * @param to Resulting vector of vector of type T.
 * @param nb_ch Number of channel of the stream.
 * @param n Number of sample in the array.
 */
template<class T>
void fill_chunk(unsigned char* from, std::vector<std::vector<T>>& to, int nb_ch, int n=CHUNK_SIZE)
{
  to.clear();
  std::vector<T> d;
  for (unsigned i = 0; i < n; ++i)
    {
      to.push_back(d);
      for(unsigned j = 0; j < nb_ch; j++)
	to[i].push_back( ((T*)from)[i*nb_ch+j] );
    }
}


/**
 * @brief fill_chunk transform a unsigned char array into a typed vector of vector.
 * @param path Path of the configuration file..
 * @param config The configuration array used in the program.
 */
void getConf(std::string path, unsigned char *config)
{
  bool opened = false;
  //try to open and read config file given
  std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
  if (file.is_open())
    {
      std::streampos size = file.tellg();
      if(size==CONFIG_SIZE)
	{
	  file.seekg (0, std::ios::beg);
	  file.read ((char*)config, size);
	  file.close();
	  opened = true;
	  std::cout << "Configuration: " << path << " opened successfully. " << std::endl;
	}
      else
	std::cout << "File not conformed" << std::endl;
    }
  else
    std::cout << "Could not open file: " << path << std::endl;


  if(!opened)
    {//default config if none was given/opened
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
    }
  else
    std::cout << "Configuration: " << path << " opened successfully. " << std::endl;
      
}

/**
 * @brief get_sampling_rate Get and interpret the sampling frequency bites of the config array and return the coresponding rate.
 * @param config The configuration array used in the program.
 */
int get_sampling_rate(unsigned char *config)
{
  int rates[] = {512, 2048, 5120, 10240};
  char index = (config[0] >> 3) & 0b11;
  return rates[index];
}

/**
 * @brief get_sampling_rate Get and interpret the number of channels bites of the config array and return the coresponding rate
 * @param config The configuration array used in the program.
 */

int get_nbChannels(unsigned char *config)
{
  char index = (config[0] >> 1) & 0b11;
  return (index+1)*2*16 + (index+1)*64 + 16 + 8;
}



int main(int argc, char ** argv)
{
  std::vector<std::string> opt_flag(
				    {"-n",
					"-c",
					"-s"});
  std::vector<std::string> opt_label(
				     {"Lsl output stream's name",
					 "OTB congiguration file",
					 "TCP chunks's size"  });
  std::vector<std::string> opt_value(
				     {"OTB",
					 "config/conf.cfg",
					 "1"});
  get_arg(argc, argv, opt_flag, opt_label, opt_value);
  
  std::string stream_name = opt_value[0];
  std::string config_file = opt_value[1];
  int chunk_size = std::stoi(opt_value[2]);


  NW network;
  tcp_connect(&network, 23456, "169.254.1.10");
  

  
    

  ////////////////////// OTB CONFIGURATION //////////////////////
  // get The OTB configuration unsigned char string
  unsigned char config[40];
  getConf(config_file, config);
  int rate = get_sampling_rate(config);
  int nb_ch = get_nbChannels(config);
  std::cout << "[INFOS] Reseting  OTB quattrocento acquisition...\xd" << std::flush;
  config[0] -= 1;
  config[39] = crc(config);
  send(network.sockfd,(char*)config,40,0);
  std::cout << "[INFOS] Sending configuration request to OTB quattrocento ...\xd" << std::flush;
  config[0] += 1;
  config[39] = crc(config);
  send(network.sockfd,(char*)config,40,0);
  std::cout << "[INFOS] OTB quattrocento configured.                         " << std::endl;
  ///////////////////////////////////////////////////////////////
  

  try
    {
      lsl::stream_info info(stream_name, "quattrocentoSamples", nb_ch, rate, lsl::cf_int16);
      lsl::stream_outlet outlet(info);
  
      std::vector<std::vector<int16_t>> chunk;
      unsigned char buffer[nb_ch*chunk_size*2];
      std::cout << "[INFOS] Now sending data... " << std::endl;  
      do
	{
	  int data_remaining = nb_ch*chunk_size*2;
	  while(data_remaining > 0)
	    {
	      //std::cout << data_remaining << " ";
	      int rv = select(network.sockfd, &(network.set), NULL, NULL, &(network.timeout));
	      if(rv == SOCKET_ERROR)
		error("select error");
	      else if(rv == 0)
		{
		   //ask to stop the acquisition
		  std::cout << "[INFOS] Ending acquisition ...\xd" << std::endl;
		  /*config[0] = ACQ_SETT | DECIM | FSAMP_2048 | NCH_IN1to8_MIN1to4 | ACQ_OFF;
		  config[39] = crc(config);
		  send(network.sockfd,(char*)config,40,0);
		  closesocket(network.sockfd);
		  std::cout << "[INFOS] Acquisition ended.     " << std::endl;
		  tcp_connect(&network, 23456, "169.254.1.10");
		  unsigned char config[40];
		  getConf(config_file, config);
		  int rate = get_sampling_rate(config);
		  int nb_ch = get_nbChannels(config);
		  std::cout << "[INFOS] Reseting  OTB quattrocento acquisition...\xd" << std::flush;
		  config[0] -= 1;
		  config[39] = crc(config);
		  send(network.sockfd,(char*)config,40,0);
		  std::cout << "[INFOS] Sending configuration request to OTB quattrocento ...\xd" << std::flush;
		  config[0] += 1;
		  config[39] = crc(config);
		  send(network.sockfd,(char*)config,40,0);
		  std::cout << "[INFOS] OTB quattrocento configured.                         " << std::endl;
		  */
		  send(network.sockfd,(char*)config,40,0);
		  data_remaining = -1;
		}
	      else
		{
		  int s = recv(network.sockfd,(char*)(buffer+(nb_ch*chunk_size*2-data_remaining)), data_remaining,0);
		  if(s == SOCKET_ERROR)
		    error("read failed");
		  else if(s == 0)
		    error("peer disconnected");
		  data_remaining -= s;
		}
	    }
	  if(data_remaining==0)
	    {
	      fill_chunk(buffer, chunk, nb_ch);
	      int ch=nb_ch-7;
	      if(chunk[0][nb_ch-8]%1000==0)
		{
		  std::cout << "Sample count ";
		  for(int i=0; i<8 ;i++)
		    std::cout << nb_ch-8+i << ":" << (unsigned short) chunk[0][nb_ch-8+i]<< "  ";
		  std::cout  << "   \xd" << std::flush;
		}

	      // send it
	      outlet.push_chunk(chunk);
	    }
	}
      while (1);

    }
  catch (std::exception& e)
    {
      std::cerr << "[ERROR] Got an exception: " << e.what() << std::endl;
    }

  //ask to stop the acquisition
  std::cout << "[INFOS] Ending acquisition ...\xd" << std::flush;
  config[0] = ACQ_SETT | DECIM | FSAMP_2048 | NCH_IN1to8_MIN1to4 | ACQ_OFF;
  config[39] = crc(config);
  send(network.sockfd,(char*)config,40,0);
  closesocket(network.sockfd);
  std::cout << "[INFOS] Acquisition ended.     " << std::endl;
    
  return 0;
}
