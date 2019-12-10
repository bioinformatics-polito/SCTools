/**
 * \file   include/sctools/alignments_writer.h
 * \author Elena Grassi
 * \author Marilisa Montemurro
 * \author Emanuele Parisi
 * \date   February, 2019
 *
 * File containing facilities for writing BAM and SAM files.
 */

#ifndef SCTOOLS_INCLUDE_SCTOOLS_ALIGNMENTS_WRITER_H
#define SCTOOLS_INCLUDE_SCTOOLS_ALIGNMENTS_WRITER_H

#include <experimental/filesystem>
#include <fstream>
#include <string>

#include <seqan/bam_io.h>
#include <seqan/bed_io.h>

#include "alignments_reader.h"

namespace fs = std::experimental::filesystem;

namespace sctools
{

/**
 * \brief Class providing facilities for writing SAM and BAM files.
 */
class AlignmentsWriter
{

public:

	/**
	 * \brief Copy the header of the source alignment reader to the output file
	 * path.
	 *
	 * \param outPath is the path to the file where the header will be
	 * forwarded.
	 * \param reader is the alignment reader header is got from.
	 */
	static inline void
	forwardHeader (const fs::path& outputFilePath,
	               const AlignmentsReader& reader) noexcept
	{
		std::ofstream     proxyWriterCore(outputFilePath,
		                                  std::ios::binary);
		seqan::BamFileOut proxyWriter;

		seqan::open(proxyWriter,
		            proxyWriterCore,
		            reader.getFormat());
		seqan::context(proxyWriter) = reader.getContext();
		seqan::writeHeader(proxyWriter,
		                   reader.getHeader());
		seqan::close(proxyWriter);
	}

	/**
	 * \brief Class constructor.
	 */
	AlignmentsWriter () = default;

	/**
	 * \brief Class copy constructor.
	 *
	 * \param other is the object the current instance is initialized from.
	 */
	AlignmentsWriter (const AlignmentsWriter& other) = delete;

	/**
	 * \brief Class copy assignment operator.
	 *
	 * \param other is the object the current instance is initialized from.
	 * \return a reference to the assigned object.
	 */
	AlignmentsWriter&
	operator= (const AlignmentsWriter& other) = delete;

	/**
	 * \brief Reset the status of the writer instance.
	 */
	inline void
	reset () noexcept
	{
		sinkPath_ = fs::path("");
		sinkStreamCore_.close();
		seqan::close(sinkStream_);
		if (writeBed_) {
			sinkStreamBed_.close();
			seqan::close(bedOut_);
		}
	}

	/**
	 * \brief Initialize the writer instance.
	 *
	 * \param sinkPath is the path to the file the current object will write
	 * to.
	 * \param bamReader is the reader object used for initialize the writer
	 * instance.
	 * \param configureAppend is a flag which append the new record to the
	 * output file, if it true.
	 */
	inline void
	configure (const fs::path& sinkPath,
	           const AlignmentsReader& bamReader,
	           bool configureAppend,
		   const bool writeBed) noexcept
	{
		writeBed_ = writeBed;
		reset();
		sinkPath_ = sinkPath;
		if (configureAppend)
		{
			sinkStreamCore_.open(sinkPath_,
			                     std::ios::app | std::ios::binary);
		}
		else
		{
			sinkStreamCore_.open(sinkPath_,
			                     std::ios::binary);
		}
		seqan::open(sinkStream_,
		            sinkStreamCore_,
		            bamReader.getFormat());
		seqan::context(sinkStream_) = bamReader.getContext();
		if (writeBed_) {
			fs::path bedPath = fs::path(sinkPath);
			bedPath.replace_extension(".bed");
			std::string bedName = bedPath.string();
			// but configureAppend == FALSE makes sense here?
			// where we aiming at more general classes and we should have a bed_writer?
			sinkStreamBed_.open(bedPath, std::ios::app | std::ios::binary);
			//auto res = seqan::open(bedOut_,  bedName.c_str(), seqan::OPEN_WRONLY | seqan::OPEN_APPEND);
			seqan::open(bedOut_,  sinkStreamBed_);
		}
	}

	/**
	 * \brief Write a set of alignment records to the output sink file.
	 *
	 * \param itBegin is the iterator to the first record to be written.
	 * \param itEnd is the iterator to the first record not to be written.
	 * \return the number of records written.
	 */
	inline uint64_t
	write (std::vector<seqan::BamAlignmentRecord>::iterator itBegin,
	       std::vector<seqan::BamAlignmentRecord>::iterator itEnd) noexcept
	{
		uint64_t written = 0;

		for (auto it = itBegin;
		     it != itEnd;
		     it++)
		{
			seqan::writeRecord(sinkStream_,
			                   *it);
			written += 1;
			if (writeBed_) {
				seqan::BedRecord<seqan::Bed3> record;
				record.beginPos = (*it).beginPos;
				record.endPos = seqan::getAlignmentLengthInRef(*it) + record.beginPos;
				record.ref = seqan::getContigName(*it, sinkStream_);
				//record. = (*it).qName; TODO FIXME
				//seqan::writeRecord(bedOut_, seqan::BedRecord( *it)); // not automatic, need to build record?
				seqan::writeRecord(bedOut_, record);
			}
		}

		return written;
	}

private:
	/**
	 * Path to the file data are written to.
	 */
	fs::path      sinkPath_;
	/**
	 * Inner file stream used for forwarding write requests.
	 */
	std::ofstream sinkStreamCore_;
	/**
	 * Stream representing a SAM or BAM data sink.
	 */
	seqan::BamFileOut sinkStream_;

	/**
	 * If and BedFile out where bam entries are mirrored.
	 */
	bool		  writeBed_; // std::optional not available?
	std::ofstream	sinkStreamBed_;
	seqan::BedFileOut bedOut_;
};

}

#endif // SCTOOLS_INCLUDE_SCTOOLS_ALIGNMENTS_WRITER_H

