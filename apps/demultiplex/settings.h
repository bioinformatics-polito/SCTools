/**
 * \file   apps/demultiplex/settings.h
 * \author Elena Grassi
 * \author Marilisa Montemurro
 * \author Emanuele Parisi
 * \date   February, 2019
 *
 * File containing facilities for specifying the arguments the demultiplexer accepts on
 * the command line and for retrieving them at runtime.
 */

#ifndef SCTOOLS_APPS_DEMULTIPLEX_SETTINGS_H
#define SCTOOLS_APPS_DEMULTIPLEX_SETTINGS_H

#include <experimental/filesystem>

#include <seqan/bam_io.h>

namespace fs = std::experimental::filesystem;

namespace sctools
{
namespace demultiplex
{

/**
 * \brief Struct providing basic facilities for parsing the arguments the user provides
 * through the command line.
 */
class Settings
{
public:
	/**
	 * Path to the SAM or BAM file containing the alignment records to be de-multiplexed.
	 */
	fs::path                 alignmentsFilePath;
	/**
	 * Path to the CSV file containing the barcodes to be de-multiplexed. Notice that the
	 * barcode is expected to be on the first column of the file, and is supposed to end
	 * with "-1" string.
	 */
	fs::path                 barcodeCSVFilePath;
	/**
	 * Path to the directory where the de-multiplexed files are stored.
	 */
	fs::path                 outputDirPath;
	/**
	 * Maximum number of alignment records the de-multiplexer reads and store in the main
	 * memory.
	 */
	uint64_t                 maxAlignmentBatchSize;
	/**
	 * List of tags that cannot be present in any alignment record considered valid.
	 */
	std::vector<std::string> forbiddenTags;
	/**
	 * Minimum mapping quality for which an alignment record is considered during the
	 * de-multiplexing procedure.
	 */
	uint64_t                 minMappingQuality;

	/**
     * Boolean that records if we need to output also bed entries with read coordinates
	 * of every cell.
	 */
	 bool				 	 writeBed;

	/**
	 * \brief Class constructor.
	 *
	 * It specifies the arguments the de-multiplex application accepts on the command
	 * line.
	 */
	Settings ()
	{
		// Set tool metalignmentsFilea-data.
		seqan::setAppName(parser_,
		                  "sctools_demultiplex");
		seqan::setShortDescription(parser_,
		                           "Alignment files barcode-based de-multiplexer.");
		seqan::addDescription(parser_,
		                      "sctools_demultiplex is a tool for de-multiplexing "
		                      "alignment BAM or SAM files splitting its records in"
		                      "different output files, according to the barcode of each"
		                      "alignment record.");
		seqan::setCategory(parser_,
		                   "SCTools suite");
		seqan::setVersion(parser_,
		                  SCTools_VERSION);
		seqan::setDate(parser_,
		               "2019");

		// Input SAM or BAM file containing the alignment records to be de-multiplexed.
		seqan::addArgument(parser_,
		                   seqan::ArgParseArgument(seqan::ArgParseArgument::INPUT_FILE,
		                                           "ALIGNMENTS"));
		seqan::setHelpText(parser_,
		                   0,
		                   "Path of the SAM or BAM file containing the "
		                   "alignments records to be de-multiplexed.");
		seqan::setValidValues(parser_,
		                      0,
		                      seqan::BamFileIn::getFileExtensions());

		// Input/Output settings.
		seqan::addSection(parser_,
		                  "Input/Output options");
		seqan::addOption(parser_,
		                 seqan::ArgParseOption("",
		                                       "barcodes-csv",
		                                       "Path to the CSV file storing the "
		                                       "barcodes to be de-multiplexed. Notice "
		                                       "that the barcode value is expected to be "
		                                       "found in first position.",
		                                       seqan::ArgParseArgument::INPUT_FILE,
		                                       "INPUT"));
		seqan::setRequired(parser_,
		                   "barcodes-csv",
		                   true);

		seqan::addOption(parser_,
		                 seqan::ArgParseOption("o",
		                                       "output-directory",
		                                       "Path of the directory where "
		                                       "de-multiplexed files are stored.",
		                                       seqan::ArgParseArgument::STRING,
		                                       "OUTPUT-DIRECTORY"));
		seqan::setDefaultValue(parser_,
		                       "output-directory",
		                       ".");

		seqan::addOption(parser_,
		                 seqan::ArgParseOption("",
		                                       "alignment-records-batch",
		                                       "Maximum size of the batch of alignment "
		                                       "records loaded in main memory.",
		                                       seqan::ArgParseArgument::INTEGER,
		                                       "MAX-BATCH-SIZE"));
		seqan::setDefaultValue(parser_,
		                       "alignment-records-batch",
		                       std::to_string(1024ull * 1024ull).data());

		seqan::addOption(parser_, 
						seqan::ArgParseOption("b", 
											  "bed", 
										      "Ouput bed files alongside bam ones."));
		// Issues about paired entries that should be both filtered.
		// Better to have -B/-b to turn on bam/bed output?

		// Filter settings.
		seqan::addSection(parser_,
		                  "Filter options");
		seqan::addOption(parser_,
		                 seqan::ArgParseOption("",
		                                       "forbidden-tags",
		                                       "Coma-separated list of tags that are "
		                                       "forbidden in any valid alignment "
		                                       "records. If an alignment record contains "
		                                       "any of the tags specified here, it is "
		                                       "not considered for the purpose of the "
		                                       "de-multiplexing procedure.",
		                                       seqan::ArgParseOption::STRING,
		                                       "FORBIDDEN-TAGS"));

		seqan::addOption(parser_,
		                 seqan::ArgParseOption("",
		                                       "min-mapq",
		                                       "Minimum mapping quality any alignment "
		                                       "record must have, to be considered for "
		                                       "the purpose of the de-multiplexing "
		                                       "procedure.",
		                                       seqan::ArgParseOption::INTEGER,
		                                       "MIN-MAPQ"));
		seqan::setDefaultValue(parser_,
		                       "min-mapq",
		                       "0");
	}

	/**
	 * \brief Trigger the parsing of the command line arguments the user provided.
	 *
	 * \param argc is the number of arguments present on the command line.
	 * \param argv are the values of the arguments on the command line.
	 * \return a code representing the outcome of the parse result.
	 */
	inline seqan::ArgumentParser::ParseResult
	parseCommandLine (int argc,
	                  char** argv)
	{
		seqan::CharString                  tmpString;
		std::string                        errorMsg;
		seqan::ArgumentParser::ParseResult parseResult = seqan::parse(parser_,
		                                                              argc,
		                                                              argv);

		if (parseResult == seqan::ArgumentParser::PARSE_OK)
		{
			// Retrieve alignments file path.
			seqan::getArgumentValue(alignmentsFilePath,
			                        parser_,
			                        0);

			// Retrieve and validate csv file.
			seqan::getOptionValue(barcodeCSVFilePath,
			                      parser_,
			                      "barcodes-csv");
			if (!fs::is_regular_file(barcodeCSVFilePath))
			{
				errorMsg = "barcode CSV path is not a regular file";
				throw std::invalid_argument(errorMsg);
			}
			if (!barcodeCSVFilePath.is_absolute())
			{
				barcodeCSVFilePath = fs::current_path() / barcodeCSVFilePath;
			}

			// Retrieve and validate output directory.
			seqan::getOptionValue(outputDirPath,
			                      parser_,
			                      "output-directory");
			if (!fs::is_directory(outputDirPath))
			{
				errorMsg = "Output directory path does not exists";
				throw std::invalid_argument(errorMsg);
			}

			// Retrieve the maximum number of alignment records the de-multiplexer read
			// and stores in the main memory.
			seqan::getOptionValue(maxAlignmentBatchSize,
			                      parser_,
			                      "alignment-records-batch");

			// Retrieve the list of tags that causes a record to be excluded from the
			// de-multiplexing procedure, if present.
			if (!seqan::isSet(parser_,
			                  "forbidden-tags"))
			{
				forbiddenTags = {};
			}
			else
			{
				std::string forbiddenTagsString;

				seqan::getOptionValue(forbiddenTagsString,
				                      parser_,
				                      "forbidden-tags");
				parseForbiddenTags_(forbiddenTagsString);
			}

			// Retrieve minimum map quality that, if not met, causes a record to be
			// excluded from the de-multiplexing procedure.
			seqan::getOptionValue(minMappingQuality,
			                      parser_,
			                      "min-mapq");

			// Do we need to write also bed files?
			writeBed = seqan::isSet(parser_, "bed");
		}

		return parseResult;
	}

private:
	/**
     * Instance of the SeqAn2 pargument parser class, providing the core argument
     * parsing capabilities.
     */
	seqan::ArgumentParser parser_;

	/**
	 * \brief Parse the list of forbidden tags from the string passed by the user through
	 * the command line.
	 *
	 * \param forbiddenTagsString string representing a coma-separated list of forbidden
	 * tags.
	 */
	inline void
	parseForbiddenTags_ (const std::string& forbiddenTagsString) noexcept
	{
		uint64_t previousComaIndex = 0;
		uint64_t nextComaIndex     = forbiddenTagsString.find(',');

		forbiddenTags.clear();
		while (nextComaIndex != std::string::npos)
		{
			forbiddenTags.emplace_back(forbiddenTagsString.begin() + previousComaIndex,
			                           forbiddenTagsString.begin() + nextComaIndex);
			previousComaIndex = nextComaIndex + 1;
			nextComaIndex     = forbiddenTagsString.find(',',
			                                             previousComaIndex);
		}
		forbiddenTags.emplace_back(forbiddenTagsString.begin() + previousComaIndex,
		                           forbiddenTagsString.end());
	};
};

} // demultiplex
} // sctools

#endif // SCTOOLS_APPS_DEMULTIPLEX_SETTINGS_H_