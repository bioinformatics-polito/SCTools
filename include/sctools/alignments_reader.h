/**
 * \file   include/sctools/alignments_reader.h
 * \author Elena Grassi
 * \author Marilisa Montemurro
 * \author Emanuele Parisi
 * \date   February, 2019
 *
 * File containing facilities for reading BAM and SAM files.
 */

#ifndef SCTOOLS_INCLUDE_SCTOOLS_ALIGNMENTS_READER_H
#define SCTOOLS_INCLUDE_SCTOOLS_ALIGNMENTS_READER_H

#include <experimental/filesystem>

#include <seqan/bam_io.h>

namespace fs = std::experimental::filesystem;

namespace sctools
{

/**
 * \brief Class providing facilities for reading SAM and BAM files.
 */
class AlignmentsReader
{

public:

	/**
	 * Class constructor.
	 */
	AlignmentsReader () = default;

	/**
	 * \brief Class copy constructor.
	 *
	 * \param other is the object the current instance is initialized from.
	 */
	AlignmentsReader (const AlignmentsReader& other) = delete;

	/**
	 * \brief Class copy assignment operator.
	 *
	 * \param other is the object the current instance is initialized from.
	 * \return a reference to the assigned object.
	 */
	AlignmentsReader&
	operator= (const AlignmentsReader& other) = delete;

	/**
	 * \brief Reset the status of the reader instance.
	 */
	inline void
	reset () noexcept
	{
		sourcePath_ = fs::path("");
		seqan::close(sourceStream_);
		seqan::resize(bamHeader_,
		              0,
		              seqan::Exact());
	}

	/**
	 * \brief Initialize the reader instance.
	 *
	 * \param sourcePath is the path to the file the current object will read from.
	 */
	inline void
	configure (const fs::path& sourcePath) noexcept
	{
		reset();
		sourcePath_ = sourcePath;
		seqan::open(sourceStream_,
		            sourcePath_.generic_string().data());
		seqan::readHeader(bamHeader_,
		                  sourceStream_);
	}

	/**
	 * \brief access the context of the alignment records reader.
	 *
	 * \return a copy of the reader context.
	 */
	inline auto
	getContext () const noexcept
	{
		return seqan::context(sourceStream_);
	}

	/**
	 * \brief access the format of the alignment records reader.
	 *
	 * \return a copy of the reader format.
	 */
	inline auto
	getFormat () const noexcept
	{
		return seqan::format(sourceStream_);
	}

	/**
	 * \brief access the header of the alignment records reader.
	 *
	 * \return a copy of the reader header.
	 */
	inline auto
	getHeader () const noexcept {
		return bamHeader_;
	}

	/**
	 * \brief Read a set of alignment records from the input source file.
	 *
	 * \param itBegin is the iterator to the first buffer entry to be filled.
	 * \param itEnd is the iterator to the first buffer entry not to be filled.
	 * \return the number of records read.
	 */
	inline uint64_t
	read (std::vector<seqan::BamAlignmentRecord>::iterator itBegin,
	      std::vector<seqan::BamAlignmentRecord>::iterator itEnd) noexcept
	{
		uint64_t loaded = 0;

		for (auto it = itBegin;
		     it != itEnd && !seqan::atEnd(sourceStream_);
		     it++)
		{
			seqan::readRecord(*it,
			                  sourceStream_);
			loaded++;
		}

		return loaded;
	}

private:
	/**
	 * Path to the file data are read from.
	 */
	fs::path sourcePath_;
	/**
	 * Stream SAM and BAM records are read from.
	 */
	seqan::BamFileIn sourceStream_;
	/**
	 * Header retrieved from the input source.
	 */
	seqan::BamHeader bamHeader_;
};

}

#endif // SCTOOLS_INCLUDE_SCTOOLS_ALIGNMENTS_READER_H
