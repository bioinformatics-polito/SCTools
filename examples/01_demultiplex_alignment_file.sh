#!/bin/bash

# De-multiplex single-cell BAM alignment file
# ===========================================
#
# The following script provides a use-case for the single-cell BAM files
# de-multiplexer in the SCTools suite. It takes as input a BAM file and 
# splits it according to the information provided in the barcode file, 
# applying the given filter to mapping qualities and SAM record tags.

# Download the BAM file to be de-multiplexed
wget --output-document=input.bam	\
     http://s3-us-west-2.amazonaws.com/10x.files/samples/cell-dna/1.1.0/breast_tissue_A_2k/breast_tissue_A_2k_possorted_bam.bam


# Download the file storing barcodes
wget --output-document=barcodes.csv	\
     http://cf.10xgenomics.com/samples/cell-dna/1.1.0/breast_tissue_A_2k/breast_tissue_A_2k_per_cell_summary_metrics.csv

# Create the output directory and run the de-multiplexer
mkdir output
sctools_demultiplex --barcodes-csv barcodes.csv	\
		    --forbidden-tags XA,SA 	\
		    --min-mapq 30 		\
		    -o output	 		\
		    input.bam &> log.txt

