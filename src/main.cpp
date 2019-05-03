#include <vector>
#include <fstream>
#include <iostream>

#include <lsl_cpp.h>
#include "OTBconfig.h"

#define SAMPLING_FREQUENCY 2048
#define CHUNK_SIZE 1


void error(const char *msg)
{
  printf("%s\n",msg);
  exit(0);
}

void usage(std::vector<std::string>& optf,  std::vector<std::string>& optl, std::vector<std::string>& optv)
{
  std::cout << "Usage: ./lslpub_OTB [OPTION ...]" << std::endl;
  std::cout << "Options: " << std::endl;
  for(int i = 0; i< optf.size(); i++)
    std::cout << "         " << optf[i] << "\t" << optl[i] <<" (ex: " << optv[i] << " )"<< std::endl;
  exit(0);
}

void get_arg(int argc, char ** argv, std::vector<std::string>& optf,  std::vector<std::string>& optl, std::vector<std::string>& optv)
{
  int i =1;
  while(i < argc)
    {
      int j = 0;
      while(optf[j].compare(argv[i]) != 0)
	{
	  j++;
	  if(j>= optf.size())
	    usage(optf,optl,optv);
	}
      if(i+1 >= argc || argv[i+1][0] == '-')
	usage(optf,optl,optv);
      optv[j] = argv[i+1];
      i+=2; 
    }
  for(int i = 0; i< optl.size(); i++)
    std::cout << optl[i] <<" : " << optv[i] << std::endl;
}
  

/**
 * store data of array into vectors
 **/
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

//get and interpret the sampling frequency bites of the config array and return th ecoresponding rate
int get_sampling_rate(unsigned char *config)
{
  int rates[] = {512, 2048, 5120, 10240};
  char index = (config[0] >> 3) & 0b11;
  return rates[index];
}

//get and interpret the sampling frequency bites of the config array and return th ecoresponding rate
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
				     {"lsl output stream's name",
					 "OTB congiguration file",
					 "TCP chunks's size"  });
  std::vector<std::string> opt_value(
				     {"OTB",
					 "none",
					 "1"});
  get_arg(argc, argv, opt_flag, opt_label, opt_value);
  
  std::string stream_name = opt_value[0];
  std::string config_file = opt_value[1];
  int chunk_size = std::stoi(opt_value[2]);


  ////////////////////// OTB IP CONNECTION //////////////////////
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
  ///////////////////////////////////////////////////////////////
  

  ////////////////////// OTB CONFIGURATION //////////////////////
  // get The OTB configuration unsigned char string
  unsigned char config[40];
  getConf((argc>1)?argv[1]:"config.cfg",config);
  int rate = get_sampling_rate(config);
  int nb_ch = get_nbChannels(config);
  std::cout << "[INFOS] Reseting  OTB quattrocento acquisition...\xd" << std::flush;
  config[0] -= 1;
  config[39] = crc(config);
  send(sockfd,(char*)config,40,0);
  std::cout << "[INFOS] Sending configuration request to OTB quattrocento ...\xd" << std::flush;
  config[0] += 1;
  config[39] = crc(config);
  send(sockfd,(char*)config,40,0);
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
	    data_remaining -= recv(sockfd,(char*)(buffer+(nb_ch*chunk_size*2-data_remaining)), data_remaining,0);
				
	  fill_chunk(buffer, chunk, nb_ch);

	  std::cout << "Sample counter (ch: " << nb_ch-8 << ") : " << (unsigned short) chunk[0][nb_ch-8] << "  Buffer usage: " <<  (unsigned short)chunk[0][nb_ch-5] << "   \xd" << std::flush;        

	  // send it
	  outlet.push_chunk(chunk);		
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
  send(sockfd,(char*)config,40,0);
  closesocket(sockfd);
  std::cout << "[INFOS] Acquisition ended.     " << std::endl;
    
  return 0;
}
