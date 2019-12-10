/**
 * \file   apps/demultiplex/functions.h
 * \author Elena Grassi
 * \author Marilisa Montemurro
 * \author Emanuele Parisi
 * \date   February, 2019
 *
 * File containing the core functions implementing the alignment records
 * de-multiplexing process.
 */

#ifndef SCTOOLS_APPS_DEMULTIPLEX_FUNCTIONS_H
#define SCTOOLS_APPS_DEMULTIPLEX_FUNCTIONS_H

#include <iostream>
#include <unordered_map>

#include "sctools/alignments_reader.h"
#include "sctools/alignments_writer.h"
#include "sctools/cell_metrics_record.h"

#include "settings.h"

namespace sctools
{
namespace demultiplex
{

/**
 * \brief Prepare the output for the barcodes to be de-multiplexed.
 *
 * This function creates a file in the output directory for every barcode to be
 * de-multiplexed, and reports the BAM header of the source alignment file to
 * each of them. Additionally, it initializes the map associating each target
 * barcode with is own output path and counter.
 *
 * \param barcodeCSVPath is the CSV file containign the barcodes tto be
 * de-multiplexed as first column, ended by the "-1" string.
 * \param outputDirPath is the path to the output directory where
 * de-multiplexed files will be stored.
 * \param outputExtension is the extension of every output file, copied from
 * the input one.
 * \param reader is the interface used for fetching alignment records from the
 * source file.
 * \param outputDataMap is the map associating each barcode to its output path
 * and counter.
 * \param noisePath is the path to the file which will store the alignment
 * records whose barcode is not in the list read from the input CSV file.
 */
inline void
initializeOutputFiles (const fs::path& barcodeCSVPath,
                       const fs::path& outputDirPath,
                       const fs::path& outputExtension,
                       const AlignmentsReader& reader,
                       std::unordered_map<std::string,
                                          std::pair<fs::path,
                                                    uint64_t>>& outputDataMap,
                       fs::path& noisePath) noexcept
{
	std::vector<CellMetricsRecord> records;
	fs::path                       outputBamFile;
	AlignmentsWriter               writer;

	// Create an output file for every different target barcode in the input
	// CSV file.
	records = CellMetricsRecord::readRecords(barcodeCSVPath);
	for (auto&& r : records)
	{
		std::string rawBarcode = r.get<CellMetricsRecord::BARCODE>();
		uint64_t    dashIndex  = rawBarcode.find_first_of('-');
		std::string barcode    = std::string(rawBarcode.begin(),
		                                     rawBarcode.begin() + dashIndex);

		// Build the current output file path and initialize its writer.
		outputBamFile = outputDirPath / barcode;
		outputBamFile += outputExtension;
		AlignmentsWriter::forwardHeader(outputBamFile, reader);
		//writer.configure(outputBamFile,
		//                 reader,
		//                 false);

		// Create the output data map entry corresponding to the current
		// barcode.
		outputDataMap.emplace(barcode,
		                      std::make_pair(outputBamFile,
		                                     0));
	}

	// Create a dummy 'noise.bam' file for de-multiplexing records whose
	// barcode is not among the target ones.
	noisePath = outputDirPath / "noise";
	noisePath += outputExtension;
	AlignmentsWriter::forwardHeader(noisePath, reader);
	//writer.configure(noisePath,
	//                 reader,
	//                 false);
}

/**
 * \brief Extract the barcode from an alignment record.
 *
 * The barcode of an alignment record is assumed to be the value of the 'CB' tag. If the
 * 'CB' tag is not present, the 'CR' tag is looked for. It is assumed that at least one
 * among those two tags is always present in every alignment record.
 *
 * \param bamRecord is the alignment record the barcode is extracted from.
 * \return a string representing the extracted barcode.
 */
inline std::string
extractBarcode (const seqan::BamAlignmentRecord& bamRecord) noexcept
{
	uint64_t            tagIdx = 0;
	std::string         barcode;
	seqan::String<char> seqanBarcode;
	seqan::BamTagsDict  tagsDict(bamRecord.tags);

	if (seqan::findTagKey(tagIdx,
	                      tagsDict,
	                      "CB"))
	{
		uint64_t dashIndex = 0;

		extractTagValue(seqanBarcode,
		                tagsDict,
		                tagIdx);
		barcode   = seqan::toCString(seqanBarcode);
		dashIndex = barcode.find_first_of('-');
		barcode   = std::string(barcode.begin(),
		                        barcode.begin() + dashIndex);
	}
	else if (findTagKey(tagIdx,
	                    tagsDict,
	                    "CR"))
	{
		extractTagValue(seqanBarcode,
		                tagsDict,
		                tagIdx);
		barcode = seqan::toCString(seqanBarcode);
	}

	return barcode;
}

/**
 * \brief Check if the record given as input has to be de-multiplexed, or if it has to be
 * filtered away.
 *
 * \param record is the alignment record to be analyzed.
 * \param forbiddenTags is the list of tags that cause the analyzed record to be excluded
 * immediately, if any of them is present.
 * \param minMapQuality is the minimum mapping quality score for which the record is
 * considered valid.
 * \return true if the record is valid, false otherwise.
 */
inline bool
filterAlignmentRecord (const seqan::BamAlignmentRecord& record,
                       const std::vector<std::string>& forbiddenTags,
                       uint64_t minMapQuality) noexcept
{
	bool isValid = true;

	// Check if the mapping quality is sufficiently high for the record to be considered.
	if (record.mapQ < minMapQuality)
	{
		isValid = false;
	}

	// Check if the alignment record to be analyzed contains any of the exclude records.
	// If so, it is not considered valid.
	for (auto i = 0ul; i < forbiddenTags.size() && isValid; i++)
	{
		seqan::BamTagsDict tagsDict(record.tags);
		uint64_t           tagIndex = 0;

		if (seqan::findTagKey(tagIndex,
		                      tagsDict,
		                      forbiddenTags[i].data()))
		{
			isValid = false;
		}
	}

	return isValid;
}

/**
 * \brief The function which actually implement the de-multiplexing loop over
 * the records read from the input alignment file.
 *
 * \param bamInputReader is the source of the alignment records to be
 * de-multiplexed.
 * \param outputDataMap is the map which associates each barcode to be
 * de-multiplexed with its own output file, along with a counter storing how
 * many times each barcode has been de-multiplexed.
 * \param noisePath is the path to the file storing the alignment records whose
 * barcode is not in the list provided by the user via the input CSV file.
 * \param batchSize is the maximum number of records read from the input file
 * for every iteration.
 * \param forbiddenTags is the list of alignment record tags that causes any
 * record to be excluded, if present.
 * \param minMapQuality is the minimum mapping quality for which a record is
 * considered.
 */
inline void
demultiplexCore (AlignmentsReader& bamInputReader,
                 std::unordered_map<std::string,
                                    std::pair<fs::path,
                                              uint64_t>>& outputDataMap,
                 fs::path& noisePath,
                 uint64_t batchSize,
                 const std::vector<std::string>& forbiddenTags,
                 uint64_t minMapQuality,
				 const bool writeBed) noexcept
{
	std::vector<seqan::BamAlignmentRecord> buffer;
	uint64_t                               loadedRecords = 0;
	AlignmentsWriter                       noiseWriter;

	// Initialize the noise writer.
	noiseWriter.configure(noisePath,
	                      bamInputReader,
						  true,
	                      writeBed);

	// Start main de-multiplex core loop.
	buffer.resize(batchSize);
	do
	{
		std::unordered_map<std::string,
		                   std::vector<seqan::BamAlignmentRecord>> recordsMap;
		std::vector<seqan::BamAlignmentRecord>                     noiseBuffer;

		// Load a batch of BAM alignment records from the source file and
		// store them in the records map.
		recordsMap.clear();
		noiseBuffer.clear();
		loadedRecords = bamInputReader.read(buffer.begin(),
		                                    buffer.end());
		std::cout << buffer[0].tags << std::endl;
		for (auto i = 0ull; i < loadedRecords; i++)
		{
			if (filterAlignmentRecord(buffer[i],
			                          forbiddenTags,
			                          minMapQuality))
			{
				auto barcode  = extractBarcode(buffer[i]);
				auto outMapIt = outputDataMap.find(barcode);

				if (outMapIt != outputDataMap.end())
				{
					auto recordsMapIt = recordsMap.find(barcode);

					// Increment the counter for the found barcode.
					outMapIt->second.second += 1;

					// If the barcode found has never been inserted into the
					// records map, create a new entry; otherwise, append the
					// current alignment record.
					if (recordsMapIt != recordsMap.end())
					{
						recordsMapIt->second.emplace_back(buffer[i]);
					}
					else
					{
						recordsMap[barcode] = {buffer[i]};
					};
				}
				else
				{
					noiseBuffer.emplace_back(buffer[i]);
				}
			}
		}

		// De-multiplex the records in the records map and store the remaining
		// records in the noise file.
		for (auto p : recordsMap)
		{
			AlignmentsWriter mapWriter;

			mapWriter.configure(outputDataMap[p.first].first,
			                    bamInputReader,
								true,
			                    writeBed);
			mapWriter.write(p.second.begin(),
			                p.second.end());
		}

		// init noise writer
		noiseWriter.write(noiseBuffer.begin(),
		                  noiseBuffer.end());
	}
	while (loadedRecords > 0);
}

/**
 * \brief Entry point of the de-multiplexing process.
 *
 * \param settings is the class representing the de-multiplexer application command line
 * arguments specified by the user.
 */
inline void
demultiplexPipeline (const Settings& settings) noexcept
{
	AlignmentsReader                        bamInputReader;
	std::unordered_map<std::string,
	                   std::pair<fs::path,
	                             uint64_t>> outputDataMap;
	fs::path                                noisePath;

	// Initialize the reader class for accessing the BAM file containing the
	// records to be de-multiplexed.
	bamInputReader.configure(settings.alignmentsFilePath);

	// Parse the CSV file reporting the per-cell summary metrics and extract
	// the list of barcodes to be de-multiplexed. Then, create a file for every
	// target barcode.
	initializeOutputFiles(settings.barcodeCSVFilePath,
	                      settings.outputDirPath,
	                      settings.alignmentsFilePath.extension(),
	                      bamInputReader,
	                      outputDataMap,
	                      noisePath);

	// Start the de-multiplexing procedure.
	demultiplexCore(bamInputReader,
	                outputDataMap,
	                noisePath,
	                settings.maxAlignmentBatchSize,
	                settings.forbiddenTags,
	                settings.minMappingQuality,
					settings.writeBed);

	// Report the details related to how many times each valid barcode is
	// de-multiplexed.
	std::cout << "BARCODE count report" << std::endl;
	for (const auto& p : outputDataMap)
	{
		std::cout << p.first << "\t: " << p.second.second << std::endl;
	}
}

} // demultiplex
} // sctools

#endif // SCTOOLS_APPS_DEMULTIPLEX_FUNCTIONS_H
