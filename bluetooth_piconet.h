/* -*- c -*- */
/*
 * Copyright 2007 - 2010 Dominic Spill, Michael Ossmann                                                                                            
 * 
 * This file is part of libbtbb
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with libbtbb; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#ifndef INCLUDED_BLUETOOTH_PICONET_H
#define INCLUDED_BLUETOOTH_PICONET_H

#include "bluetooth_packet.h"

/* maximum number of hops to remember */
#define MAX_PATTERN_LENGTH 1000

/* number of channels in use */
#define BT_NUM_CHANNELS 79

/* number of hops in the hopping sequence (i.e. number of possible values of CLK1-27) */
#define SEQUENCE_LENGTH 134217728

/* number of aliased channels received */
#define ALIASED_CHANNELS 25

/* Packet queue (linked list) */
typedef struct pkt_queue {
	bt_packet *pkt;

	struct pkt_queue *next;

} pkt_queue;

typedef struct bt_piconet {
	/* true if using a particular aliased receiver implementation */
	int aliased;

	/* using adaptive frequency hopping (AFH) */
	int afh;

	/* observed pattern that looks like AFH */
	int looks_like_afh;

	/* AFH channel map - either read or derived from observed packets */
	uint8_t afh_map[10];

	/* lower address part (of master's BD_ADDR) */
	uint32_t LAP;

	/* upper address part (of master's BD_ADDR) */
	uint8_t UAP;

	/* non-significant address part (of master's BD_ADDR) */
	uint16_t NAP;

	/* CLK1-27 candidates */
	uint32_t *clock_candidates;

	/* these values for hop() can be precalculated */
	int b, e;

	/* these values for hop() can be precalculated in part (e.g. a1 is the
	 * precalculated part of a) */
	int a1, c1, d1;

	/* frequency register bank */
	int bank[BT_NUM_CHANNELS];

	/* this holds the entire hopping sequence */
	char *sequence;

	/* number of candidates for CLK1-27 */
	int num_candidates;

	/* have we collected the first packet in a UAP discovery attempt? */
	int got_first_packet;

	/* number of packets observed during one attempt at UAP/clock discovery */
	int packets_observed;

	/* total number of packets observed */
	int total_packets_observed;

	/* number of observed packets that have been used to winnow the candidates */
	int winnowed;

	/* CLK1-6 candidates */
	int clock6_candidates[64];

	/* remember patterns of observed hops */
	int pattern_indices[MAX_PATTERN_LENGTH];
	uint8_t pattern_channels[MAX_PATTERN_LENGTH];

	int hop_reversal_inited;

	/* offset between CLKN (local) and CLK of piconet */
	int clk_offset;

	/* local clock (clkn) at time of first packet */
	uint32_t first_pkt_time;

	/* Whether LAP is valid */
	int have_LAP;

	/* discovery status */
	int have_UAP;
	int have_NAP;
	int have_clk6;
	int have_clk27;

	/* queue of packets to be decoded */
	pkt_queue *queue;
} bt_piconet;

/* do all the precalculation that can be done before knowing the address */
void precalc(bt_piconet *pnet);

/* do precalculation that requires the address */
void address_precalc(int address, bt_piconet *pnet);

/* drop-in replacement for perm5() using lookup table */
int fast_perm(int z, int p_high, int p_low, bt_piconet *pnet);

/* 5 bit permutation */
/* assumes z is constrained to 5 bits, p_high to 5 bits, p_low to 9 bits */
int perm5(int z, int p_high, int p_low);

/* determine channel for a particular hop */
/* replaced with gen_hops() for a complete sequence but could still come in handy */
char single_hop(int clock, bt_piconet *pnet);

/* initialize the hop reversal process */
/* returns number of initial candidates for CLK1-27 */
int init_hop_reversal(int aliased, bt_piconet *pnet);

/* narrow a list of candidate clock values based on all observed hops */
int winnow(bt_piconet *pnet);

/* use packet headers to determine UAP */
int UAP_from_header(bt_packet *pkt, bt_piconet *pnet);

/* look up channel for a particular hop */
char hop(int clock, bt_piconet *pnet);

void init_piconet(bt_piconet *pnet);

/* decode a whole packet from the given piconet */
int decode(bt_packet* p, bt_piconet *pnet);

/* Print hexadecimal representation of the derived AFH map */
void btbb_print_afh_map(bt_piconet *pnet);

#endif /* INCLUDED_BLUETOOTH_PICONET_H */
