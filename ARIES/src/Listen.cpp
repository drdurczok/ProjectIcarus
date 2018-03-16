#include "../include/Listen.h"

Listen::Listen()
{
	//use the setLMdir and setDICdir to set your own language models
	std::string temp = MODELDIR;
	lmDir = temp  +"/en-us/en-us.lm.bin"; //replace with correct director to the model.
	//lmDir = "aries.gram";
	jsgfDir = "aries.gram";
	dicDir = temp +"/en-us/cmudict-en-us.dict"; //replace with correct director to the model.
}

Listen::~Listen()
{
    fclose(rawfh);
    ps_free(ps);
}

int Listen::init() //this will give us access to pocket sphinx's cmd_ln.c interpreter
{
    //create a config object
	config = cmd_ln_init(NULL, ps_args(), TRUE,
			     "-hmm", MODELDIR "/en-us/en-us",
			     "-jsgf", jsgfDir.c_str(),
			     "-dict", dicDir.c_str(),
			     NULL);
	if (config == NULL)
		return 1;

	//initialize the decoder
	ps = ps_init(config);
	if (ps == NULL)
		return 1;

	return 0;
}


//the ability to set the location for the models will allow new models to be composed and replaced with ease
int Listen::setLMdir(std::string _lmDir)
{
	std::string temp = MODELDIR;

	lmDir = temp+ _lmDir;
	if(lmDir.c_str()==NULL)
		return -1;

	updateModel();//update the LM if config already exists
	return 1;
}

int Listen::setDICdir(std::string _dicDir)
{
	std::string temp = MODELDIR;

	dicDir = temp+ _dicDir;
	if(dicDir.c_str() ==NULL)
		return -1;

	updateModel(); //update Dictionary if config object already exists
	return 1;
}


int Listen::setKeyphrase()
{
	ps_set_keyphrase(ps, "start", "aries");

	ps_set_search(ps, "start");

	return 1;
}

int Listen::unsetKeyphrase()
{
	ps_unset_search(ps, "start");

	ps_set_jsgf_file(ps, "aries", "aries.gram");

	ps_set_search(ps, "aries");

	return 1;
}

int Listen::updateModel()
{
	//if init has not been called we don't need to restart pocketSphinx
	if(config == NULL)
		return 1;

	ps_free(ps); //free the decoder

	init(); //call init to load the decoder with the new language model.

	return 0;
}



//enter the location of the file to be interpreted.
//returns the string that was recognized by the decoder
std::string Listen::rawToString(const char * pathToraw)
{
	std::string er = "error";
	//open the file stream for reading in binary mode
	rawfh = fopen(pathToraw, "rb");
	if (rawfh == NULL) {
		perror("Failed to open raw file");
		return er;
	}

	//decode the binary audio files and return the number of samples recovered
	rv = ps_decode_raw(ps, rawfh, -1);//decoder,file handler, -1 = read to end of file
	if (rv < 0)
		return er;

	//get the hypothesis on the output
	hyp = ps_get_hyp(ps, &score);

	if (hyp == NULL)//if none of our known words match what was said send a flag for the handler to pick up
		{
			return WNF;
		}

	return hyp;
}
