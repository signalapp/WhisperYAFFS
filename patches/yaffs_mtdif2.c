/*
 * YAFFS: Yet another FFS. A NAND-flash specific file system. 
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/* mtd interface for YAFFS2 */

const char *yaffs_mtdif2_c_version =
    "$Id: yaffs_mtdif2.c,v 1.2 2007-03-07 08:05:58 colin Exp $";

#include "yportenv.h"


#include "yaffs_mtdif2.h"

#include "linux/mtd/mtd.h"
#include "linux/types.h"
#include "linux/time.h"

#include "yaffs_packedtags2.h"


void nandmtd2_pt2buf(yaffs_dev_t *dev, yaffs_PackedTags2 *pt, int is_raw)
{
	struct mtd_info *mtd = (struct mtd_info *)(dev->genericDevice);
	u8 *ptab = (u8 *)pt; /* packed tags as bytes */
	
	int	i, j = 0, k, n;

	/* Pack buffer with 0xff */
	for (i = 0; i < mtd->oobsize; i++)
		dev->spareBuffer[i] = 0xff;
		
	if(!is_raw){
		memcpy(dev->spareBuffer,pt,sizeof(yaffs_PackedTags2));
	} else {
		j = 0;
		k = mtd->oobinfo.oobfree[j][0];
		n = mtd->oobinfo.oobfree[j][1];

		if (n == 0) {
			T(YAFFS_TRACE_ERROR, (TSTR("No OOB space for tags" TENDSTR)));
			YBUG();
		}

		for (i = 0; i < sizeof(yaffs_PackedTags2); i++) {
			if (n == 0) {
				j++;
				k = mtd->oobinfo.oobfree[j][0];
				n = mtd->oobinfo.oobfree[j][1];
				if (n == 0) {
					T(YAFFS_TRACE_ERROR, (TSTR("No OOB space for tags" TENDSTR)));
					YBUG();
				}
			}
			dev->spareBuffer[k] = ptab[i];
			k++;
			n--;
		}
	}

}

void nandmtd2_buf2pt(yaffs_dev_t *dev, yaffs_PackedTags2 *pt, int is_raw)
{
	struct mtd_info *mtd = (struct mtd_info *)(dev->genericDevice);
	int	i, j = 0, k, n;
	u8 *ptab = (u8 *)pt; /* packed tags as bytes */


	if (!is_raw) {
	
		memcpy(pt,dev->spareBuffer,sizeof(yaffs_PackedTags2));
	} else {
		j = 0;
		k = mtd->oobinfo.oobfree[j][0];
		n = mtd->oobinfo.oobfree[j][1];

		if (n == 0) {
			T(YAFFS_TRACE_ERROR, (TSTR("No space in OOB for tags" TENDSTR)));
			YBUG();
		}

		for (i = 0; i < sizeof(yaffs_PackedTags2); i++) {
			if (n == 0) {
				j++;
				k = mtd->oobinfo.oobfree[j][0];
				n = mtd->oobinfo.oobfree[j][1];
				if (n == 0) {
					T(YAFFS_TRACE_ERROR, (TSTR("No space in OOB for tags" TENDSTR)));
					YBUG();
				}
			}
			ptab[i] = dev->spareBuffer[k];
			k++;
			n--;
		}
	}
		
}

int nandmtd2_WriteChunkWithTagsToNAND(yaffs_dev_t * dev, int nand_chunk,
				      const u8 * data,
				      const yaffs_ext_tags * tags)
{
	struct mtd_info *mtd = (struct mtd_info *)(dev->genericDevice);
	size_t dummy;
	int retval = 0;

	loff_t addr = ((loff_t) nand_chunk) * dev->n_bytesPerChunk;

	yaffs_PackedTags2 pt;

	T(YAFFS_TRACE_MTD,
	  (TSTR
	   ("nandmtd2_WriteChunkWithTagsToNAND chunk %d data %p tags %p"
	    TENDSTR), nand_chunk, data, tags));

	if (tags) {
		yaffs_PackTags2(&pt, tags);
	}

	if (data && tags) {
	                nandmtd2_pt2buf(dev, &pt, 0);
			retval =
			    mtd->write_ecc(mtd, addr, dev->n_bytesPerChunk,
					   &dummy, data, dev->spareBuffer,
					   NULL);

	} else {
	
		T(YAFFS_TRACE_ALWAYS,
		  (TSTR
		  ("Write chunk with null tags or data!" TENDSTR)));
		YBUG();
 	}

	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;
}

int nandmtd2_ReadChunkWithTagsFromNAND(yaffs_dev_t * dev, int nand_chunk,
				       u8 * data, yaffs_ext_tags * tags)
{
	struct mtd_info *mtd = (struct mtd_info *)(dev->genericDevice);
	size_t dummy;
	int retval = 0;

	loff_t addr = ((loff_t) nand_chunk) * dev->n_bytesPerChunk;

	yaffs_PackedTags2 pt;

	T(YAFFS_TRACE_MTD,
	  (TSTR
	   ("nandmtd2_ReadChunkWithTagsToNAND chunk %d data %p tags %p"
	    TENDSTR), nand_chunk, data, tags));

	if (0 && data && tags) {
			retval =
			    mtd->read_ecc(mtd, addr, dev->n_bytesPerChunk,
					  &dummy, data, dev->spareBuffer,
					  NULL);
			nandmtd2_buf2pt(dev, &pt, 0);
	} else {
		if (data)
			retval =
			    mtd->read(mtd, addr, dev->n_bytesPerChunk, &dummy,
				      data);
		if (tags) {
			retval =
			    mtd->read_oob(mtd, addr, mtd->oobsize, &dummy,
					  dev->spareBuffer);
			nandmtd2_buf2pt(dev, &pt, 1);
		}
	}

	if (tags)
		yaffs_unpack_tags2(tags, &pt);

	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;
}

int nandmtd2_MarkNANDBlockBad(struct yaffs_dev_s *dev, int block_no)
{
	struct mtd_info *mtd = (struct mtd_info *)(dev->genericDevice);
	int retval;
	T(YAFFS_TRACE_MTD,
	  (TSTR("nandmtd2_MarkNANDBlockBad %d" TENDSTR), block_no));

	retval =
	    mtd->block_markbad(mtd,
			       block_no * dev->chunks_per_block *
			       dev->n_bytesPerChunk);

	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;

}

int nandmtd2_QueryNANDBlock(struct yaffs_dev_s *dev, int block_no,
			    yaffs_block_state_t * state, int *seq_number)
{
	struct mtd_info *mtd = (struct mtd_info *)(dev->genericDevice);
	int retval;

	T(YAFFS_TRACE_MTD,
	  (TSTR("nandmtd2_QueryNANDBlock %d" TENDSTR), block_no));
	retval =
	    mtd->block_isbad(mtd,
			     block_no * dev->chunks_per_block *
			     dev->n_bytesPerChunk);

	if (retval) {
		T(YAFFS_TRACE_MTD, (TSTR("block is bad" TENDSTR)));

		*state = YAFFS_BLOCK_STATE_DEAD;
		*seq_number = 0;
	} else {
		yaffs_ext_tags t;
		nandmtd2_ReadChunkWithTagsFromNAND(dev,
						   block_no *
						   dev->chunks_per_block, NULL,
						   &t);

		if (t.chunk_used) {
			*seq_number = t.seq_number;
			*state = YAFFS_BLOCK_STATE_NEEDS_SCANNING;
		} else {
			*seq_number = 0;
			*state = YAFFS_BLOCK_STATE_EMPTY;
		}

		T(YAFFS_TRACE_MTD,
		  (TSTR("block is OK seq %d state %d" TENDSTR), *seq_number,
		   *state));
	}

	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;
}

