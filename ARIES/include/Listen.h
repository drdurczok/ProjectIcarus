#ifndef LISTEN_H
#define LISTEN_H

#include <pocketsphinx.h>
#include <ps_search.h>
#include <cstdlib>
#include <string>
#include <cstring>

#define WNF "wnf" //word not found flag

class Listen
{
    public:
        Listen();
        ~Listen();

        int init();
        //int keyphrase();
		int setLMdir(std::string _lmDir);
		int setDICdir(std::string _dicDir);
		int setKeyphrase();
		int unsetKeyphrase();
		std::string rawToString(const char * pathToraw);
		
    private:
        int updateModel();


		std::string lmDir, dicDir, jsgfDir;

		ps_decoder_t *ps;
		cmd_ln_t *config;
		FILE *rawfh;
		char const *hyp;
		int16 buf[512];
		int rv;
		int32 score;
};

#endif // LISTEN_H
