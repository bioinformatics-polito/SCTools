/**
 * \file   apps/demultiplex/demultiplex.cpp
 * \author Elena Grassi
 * \author Marilisa Montemurro
 * \author Emanuele Parisi
 * \date   February, 2019
 *
 * Entry point for the de-multiplexer application.
 */

#include <seqan/arg_parse.h>

#include "functions.h"
#include "settings.h"

using namespace sctools::demultiplex;

/**
 * Entry point for the de-multiplexing application.
 *
 * \param argc is the number of arguments provided on the command line.
 * \param argv is the values of the arguments provided on the command line.
 * \return the code 0 if the process ends gracefully; otherwise, it returns -1.
 */
int
main (int argc,
      char** argv)
{
	seqan::ArgumentParser::ParseResult parseResult;
	Settings                           settings;

	try
	{
		parseResult = settings.parseCommandLine(argc,
		                                        argv);
		if (parseResult != seqan::ArgumentParser::PARSE_OK)
		{
			return 0;
		}
		else
		{
			demultiplexPipeline(settings);
		}
	}
	catch (std::exception& e)
	{
		return -1;
	}

	return 0;
}
