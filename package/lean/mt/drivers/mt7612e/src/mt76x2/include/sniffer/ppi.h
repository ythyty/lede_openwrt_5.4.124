/*
 ***************************************************************************
 * MediaTek Inc. 
 *
 *
 * All rights reserved. source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of MediaTek. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of MediaTek, Inc. is obtained.
 ***************************************************************************

 * Per Packet Information Header

	Module Name:
	ppi.h
*/

#define SIZE_PPI_FIELD_80211_COMMON 20
#define SIZE_PPI_FIELD_80211n_MACPHYEXT 48
#define MAX_MCS_INDEX 76
#define MAX_MCS_VHT_INDEX 9

typedef struct ppi_packetheader {
	uint8_t pph_version;
	uint8_t pph_flags;
	uint16_t pph_len;
	uint32_t pph_dlt;
}ppi_packetheader_t;

typedef struct ppi_fieldheader {
	uint16_t pfh_type;
	uint16_t pfh_datalen;
}ppi_fieldheader_t;

static const uint16_t ieee80211_vhtinfo[MAX_MCS_VHT_INDEX+1][4][2][4] = {
                /* MCS  0  */
                {       
                                                /* 20 Mhz */  {   {6,13,19,26,},                /* SGI */   { 7,14,22,29,}, },
                                                /* 40 Mhz */  {   {13,27,40,54,},               /* SGI */   { 15,30,45,60,}, },
                                                /* 80 Mhz */  {   {29,59,88,117,},              /* SGI */   { 32,65,97,130,}, },
                                                /* 160 Mhz */ {   {58,117,175,234,},            /* SGI */   { 65,130,195,260,}, }
                                
                },
                /* MCS  1  */
                {       
                                                /* 20 Mhz */  {   {13,26,39,52,},                /* SGI */   {14,29,43,58,},},
                                                /* 40 Mhz */  {   {27,54,81,108,},                /* SGI */  {30,60,90,120,},},
                                                /* 80 Mhz */  {   {58,117,175,234,},                /* SGI */{65,130,195,260,},},
                                                /* 160 Mhz */ {   {117,234,351,468,},               /* SGI */{130,260,390,520,},}
                                
                },
                /* MCS  2  */
                {       
                                                /* 20 Mhz */  {   {19,39,58,78,},                /* SGI */   {22,43,65,87,}, },
                                                /* 40 Mhz */  {   {40,81,121,162,},                /* SGI */ {45,90,135,180,}, },
                                                /* 80 Mhz */  {   {88,176,263,351,},                /* SGI */{97,195,292,390,}, },
                                                /* 160 Mhz */ {   {175,351,526,702,},               /* SGI */{195,390,585,780,}, }
                                
                },
                /* MCS  3  */
                {       
                                                /* 20 Mhz */  {   {26,52,78,104,},                /* SGI */  {29,58,87,116,}, },
                                                /* 40 Mhz */  {   {54,108,162,216,},                /* SGI */{60,120,180,240,}, },
                                                /* 80 Mhz */  {   {117,234,351,468,},               /* SGI */{130,260,390,520,}, },
                                                /* 160 Mhz */ {   {234,468,702,936,},               /* SGI */{260,520,780,1040,}, }
                                
                },
                /* MCS  4  */
                {       
                                                /* 20 Mhz */  {   {39,78,117,156,},                /* SGI */   {43,87,130,173,}, },
                                                /* 40 Mhz */  {   {81,162,243,324,},                /* SGI */  {90,180,270,360,}, },
                                                /* 80 Mhz */  {   {175,351,526,702,},                /* SGI */ {195,390,585,780,}, },
                                                /* 160 Mhz */ {   {351,702,1053,1404,},               /* SGI */{390,780,1170,1560,}, }
                                
                },
                /* MCS  5  */
                {      
                                                /* 20 Mhz */  {   {52,104,156,208,},                /* SGI */   {58,116,173,351,}, },
                                                /* 40 Mhz */  {   {108,216,324,432,},                /* SGI */  {120,240,360,480,}, },
                                                /* 80 Mhz */  {   {234,468,702,936,},                /* SGI */  {260,520,780,1040,}, },
                                                /* 160 Mhz */ {   {468,936,1404,1872,},                /* SGI */{520,1040,1560,2080,}, }
                                
                },
                /* MCS  6  */
                {       
                                                /* 20 Mhz */  {   {58,117,175,234,},                /* SGI */   {65,130,195,260,}, },
                                                /* 40 Mhz */  {   {121,243,364,486,},                /* SGI */  {135,270,405,540,}, },
                                                /* 80 Mhz */  {   {263,527,790,1053,},                /* SGI */ {292,585,877,1170,}, },
                                                /* 160 Mhz */ {   {526,1053,1579,2106,},               /* SGI */{585,1170,1755,2340,}, }
                                
                },
                /* MCS  7  */
                {      
                                                /* 20 Mhz */  {  {65,130,195,260,},                /* SGI */   {72,144,217,289,}, },
                                                /* 40 Mhz */  {  {135,270,405,540,},                /* SGI */  {150,300,450,600,}, },
                                                /* 80 Mhz */  {  {292,585,877,1170,},                /* SGI */ {325,650,975,1300,}, },
                                                /* 160 Mhz */ {  {585,1170,1755,2340,},               /* SGI */{650,1300,1950,2600,}, }
                                
                },
                /* MCS  8  */
                {       
                                                /* 20 Mhz */  {   {78,156,234,312,},                /* SGI */   {87,173,260,347,}, },
                                                /* 40 Mhz */  {   {162,324,486,648,},                /* SGI */  {180,360,540,720,}, },
                                                /* 80 Mhz */  {   {351,702,1053,1404,},                /* SGI */{390,780,1170,1560,}, },
                                                /* 160 Mhz */ {   {702,1404,2106,2808,},               /* SGI */{780,1560,2340,3120,}, }
                                
                },
                /* MCS  9  */
                {       
                                                /* 20 Mhz */  {   {0,0,0,0,},                       /* SGI */    {0,0,0,0,}, },
                                                /* 40 Mhz */  {   {180,360,540,720,},                /* SGI */  {200,400,600,800,}, },
                                                /* 80 Mhz */  {   {390,780,1170,1560,},                /* SGI */{433,867,1300,1733,}, },
                                                /* 160 Mhz */ {   {780,1560,2340,3120,},               /* SGI */{867,1733,2600,3467,}, }
                                
                }
};
