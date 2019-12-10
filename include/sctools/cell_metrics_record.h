/**
 * \file   include/sctools/cell_metrics_record.h
 * \author Elena Grassi
 * \author Marilisa Montemurro
 * \author Emanuele Parisi
 * \date   February, 2019
 *
 * File containing the facilities for reading 10X per-cell summary metrics records and
 * accessing their fields.
 */

#ifndef SCTOOLS_INCLUDE_SCTOOLS_CELL_METRICS_RECORD_H
#define SCTOOLS_INCLUDE_SCTOOLS_CELL_METRICS_RECORD_H

#include <experimental/filesystem>
#include <fstream>
#include <string>
#include <tuple>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace sctools
{

/**
 * \brief Class representing a records of a 10x per-cell summary metrics CSV file.
 */
class CellMetricsRecord
{
public:

	static constexpr std::size_t BARCODE                     = 0;
	static constexpr std::size_t CELL_ID                     = 1;
	static constexpr std::size_t TOTAL_NUM_READS             = 2;
	static constexpr std::size_t NUM_UNMAPPED_READS          = 3;
	static constexpr std::size_t NUM_LOWMAPQ_READS           = 4;
	static constexpr std::size_t NUM_DUPLICATE_READS         = 5;
	static constexpr std::size_t NUM_MAPPED_DEDUP_READS      = 6;
	static constexpr std::size_t FRAC_MAPPED_DUPLICATES      = 7;
	static constexpr std::size_t EFFECTIVE_DEPTH_OF_COVERAGE = 8;
	static constexpr std::size_t EFFECTIVE_READS_PER_MBP     = 9;
	static constexpr std::size_t RAW_MAPD                    = 10;
	static constexpr std::size_t NORMALIZED_MAPD             = 11;
	static constexpr std::size_t RAW_DIMAPD                  = 12;
	static constexpr std::size_t NORMALIZED_DIMAPD           = 13;
	static constexpr std::size_t MEAN_PLOIDY                 = 14;
	static constexpr std::size_t PLOIDY_CONFIDENCE           = 15;
	static constexpr std::size_t IS_HIGH_DIMAPD              = 16;
	static constexpr std::size_t IS_NOISY                    = 17;

	/**
	 * \brief Static method for reading all the records stored in a 10X per-cell summary
	 * metrics file at once.
	 *
	 * \param inputFilePath is the path to the file to be parsed.
	 * \return the sequence of records read from the file.
	 */
	static inline std::vector<CellMetricsRecord>
	readRecords (const fs::path& inputFilePath) noexcept
	{
		std::ifstream                         inputFile;
		std::string                           line;
		std::vector<CellMetricsRecord> records;

		// Open input file stream and read the CSV file header.
		inputFile.open(inputFilePath);
		std::getline(inputFile,
		             line);

		// Read all the records one at a time.
		while (!inputFile.eof())
		{
			std::getline(inputFile,
			             line);
			if (!line.empty()) {
				records.emplace_back(line);
			}
		}
		inputFile.close();

		return records;
	}

	/**
	 * \brief Class constructor.
	 *
	 * Given the string representation of a per-cell metrics file record, parse it and
	 * store each field in a class attribute.
	 *
	 * \param stringRecord is the line of the source file containing the information to
	 * be parsed.
	 */
	explicit CellMetricsRecord (const std::string& stringRecord) noexcept
	{
		std::istringstream stringStream(stringRecord);
		std::string        token;

		// Parse field 0 : BARCODE
		std::getline(stringStream,
		             token,
		             ',');
		std::get<0>(fields_) = token;

		// Parse field 1 : CELL_ID
		std::getline(stringStream,
		             token,
		             ',');
		std::get<1>(fields_) = std::strtoull(token.data(),
		                                     nullptr,
		                                     10);

		// Parse field 2 : TOTAL_NUM_READ
		std::getline(stringStream,
		             token,
		             ',');
		std::get<2>(fields_) = std::strtoull(token.data(),
		                                     nullptr,
		                                     10);

		// Parse field 3 : NUM_UNMAPPED_READS
		std::getline(stringStream,
		             token,
		             ',');
		std::get<3>(fields_) = std::strtoull(token.data(),
		                                     nullptr,
		                                     10);

		// Parse field 4 : NUM_LOWMAPQ_READS
		std::getline(stringStream,
		             token,
		             ',');
		std::get<4>(fields_) = std::strtoull(token.data(),
		                                     nullptr,
		                                     10);

		// Parse field 5 : NUM_DUPLICATE_READS
		std::getline(stringStream,
		             token,
		             ',');
		std::get<5>(fields_) = std::strtoull(token.data(),
		                                     nullptr,
		                                     10);

		// Parse field 6 : NUM_MAPPED_DEDUP_READS
		std::getline(stringStream,
		             token,
		             ',');
		std::get<6>(fields_) = std::strtoull(token.data(),
		                                     nullptr,
		                                     10);

		// Parse field 7 : FRAC_MAPPED_DUPLICATES
		std::getline(stringStream,
		             token,
		             ',');
		std::get<7>(fields_) = std::strtod(token.data(),
		                                   nullptr);

		// Parse field 8 : EFFECTIVE_DEPTH_OF_COVERAGE
		std::getline(stringStream,
		             token,
		             ',');
		std::get<8>(fields_) = std::strtod(token.data(),
		                                   nullptr);

		// Parse field 9 : EFFECTIVE_READS_PER_MBP
		std::getline(stringStream,
		             token,
		             ',');
		std::get<9>(fields_) = std::strtoull(token.data(),
		                                     nullptr,
		                                     10);

		// Parse field 10 : RAW_MAPD
		std::getline(stringStream,
		             token,
		             ',');
		std::get<10>(fields_) = std::strtod(token.data(),
		                                    nullptr);

		// Parse field 11 : NORMALIZED_MAPD
		std::getline(stringStream,
		             token,
		             ',');
		std::get<11>(fields_) = std::strtod(token.data(),
		                                    nullptr);

		// Parse field 12 : RAW_DIMAPD
		std::getline(stringStream,
		             token,
		             ',');
		std::get<12>(fields_) = std::strtod(token.data(),
		                                    nullptr);

		// Parse field 13 : NORMALIZED_DIMAPD
		std::getline(stringStream,
		             token,
		             ',');
		std::get<13>(fields_) = std::strtod(token.data(),
		                                    nullptr);

		// Parse field 14 : MEAN_PLOIDY
		std::getline(stringStream,
		             token,
		             ',');
		std::get<14>(fields_) = std::strtod(token.data(),
		                                   nullptr);

		// Parse field 15 : PLOIDY_CONFIDENCE
		std::getline(stringStream,
		             token,
		             ',');
		std::get<15>(fields_) = std::strtoull(token.data(),
		                                     nullptr,
		                                     10);

		// Parse field 16 : IS_HIGH_DIMAPD
		std::getline(stringStream,
		             token,
		             ',');
		std::get<16>(fields_) = (token != "0");

		// Parse field 17 : IS_NOISY
		std::getline(stringStream,
		             token,
		             ',');
		std::get<17>(fields_) = (token != "0");
	}

	/**
	 * \brief Getter method for accessing any field of the per-cell metrics record.
	 *
	 * \tparam FIELD_ID is the name of the field to be accessed.
	 * \return the value of the specified field.
	 */
	template <std::size_t FIELD_ID>
	inline auto
	get () noexcept {
		return std::get<FIELD_ID>(fields_);
	}

private:

	using TRecordCore = std::tuple<std::string,
	                               uint64_t,
	                               uint64_t,
	                               uint64_t,
	                               uint64_t,
	                               uint64_t,
	                               uint64_t,
	                               double,
	                               double,
	                               uint64_t,
	                               double,
	                               double,
	                               double,
	                               double,
	                               double,
	                               uint64_t,
	                               bool,
	                               bool>;

	/**
	 * Tuple of types composing a per-cell metrics record.
	 */
	TRecordCore fields_;
};

} // sctools

#endif // SCTOOLS_INCLUDE_SCTOOLS_CELL_METRICS_RECORD_H
