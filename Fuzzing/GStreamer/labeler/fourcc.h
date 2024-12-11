#pragma once

#include <stdint.h>


/* FOURCC data copied from GStreamer project (https://gstreamer.freedesktop.org/) */

#define guint32 uint32_t

#define GST_MAKE_FOURCC(a,b,c,d) \
  ( (guint32)(a) | ((guint32) (b)) << 8  | ((guint32) (c)) << 16 | ((guint32) (d)) << 24 )

#define FOURCC_2vuy     GST_MAKE_FOURCC('2','v','u','y')
#define FOURCC_FMP4     GST_MAKE_FOURCC('F','M','P','4')
#define FOURCC_H264     GST_MAKE_FOURCC('H','2','6','4')
#define FOURCC_H265     GST_MAKE_FOURCC('H','2','6','5')
#define FOURCC_MAC3     GST_MAKE_FOURCC('M','A','C','3')
#define FOURCC_MAC6     GST_MAKE_FOURCC('M','A','C','6')
#define FOURCC_MP4V     GST_MAKE_FOURCC('M','P','4','V')
#define FOURCC_PICT     GST_MAKE_FOURCC('P','I','C','T')
#define FOURCC_QDM2     GST_MAKE_FOURCC('Q','D','M','2')
#define FOURCC_SVQ3     GST_MAKE_FOURCC('S','V','Q','3')
#define FOURCC_VP31     GST_MAKE_FOURCC('V','P','3','1')
#define FOURCC_VP80     GST_MAKE_FOURCC('V','P','8','0')
#define FOURCC_WRLE     GST_MAKE_FOURCC('W','R','L','E')
#define FOURCC_XMP_     GST_MAKE_FOURCC('X','M','P','_')
#define FOURCC_XVID     GST_MAKE_FOURCC('X','V','I','D')
#define FOURCC__ART     GST_MAKE_FOURCC(0xa9,'A','R','T')
#define FOURCC_____     GST_MAKE_FOURCC('-','-','-','-')
#define FOURCC___in     GST_MAKE_FOURCC(' ',' ','i','n')
#define FOURCC___ty     GST_MAKE_FOURCC(' ',' ','t','y')
#define FOURCC__alb     GST_MAKE_FOURCC(0xa9,'a','l','b')
#define FOURCC__cpy     GST_MAKE_FOURCC(0xa9,'c','p','y')
#define FOURCC__day     GST_MAKE_FOURCC(0xa9,'d','a','y')
#define FOURCC__des     GST_MAKE_FOURCC(0xa9,'d','e','s')
#define FOURCC__enc     GST_MAKE_FOURCC(0xa9,'e','n','c')
#define FOURCC__gen     GST_MAKE_FOURCC(0xa9, 'g', 'e', 'n')
#define FOURCC__grp     GST_MAKE_FOURCC(0xa9,'g','r','p')
#define FOURCC__inf     GST_MAKE_FOURCC(0xa9,'i','n','f')
#define FOURCC__lyr     GST_MAKE_FOURCC(0xa9,'l','y','r')
#define FOURCC__mp3     GST_MAKE_FOURCC('.','m','p','3')
#define FOURCC__nam     GST_MAKE_FOURCC(0xa9,'n','a','m')
#define FOURCC__req     GST_MAKE_FOURCC(0xa9,'r','e','q')
#define FOURCC__too     GST_MAKE_FOURCC(0xa9,'t','o','o')
#define FOURCC__wrt     GST_MAKE_FOURCC(0xa9,'w','r','t')
#define FOURCC_aART     GST_MAKE_FOURCC('a','A','R','T')
#define FOURCC_ac_3     GST_MAKE_FOURCC('a','c','-','3')
#define FOURCC_agsm     GST_MAKE_FOURCC('a','g','s','m')
#define FOURCC_ai12     GST_MAKE_FOURCC('a','i','1','2')
#define FOURCC_ai13     GST_MAKE_FOURCC('a','i','1','3')
#define FOURCC_ai15     GST_MAKE_FOURCC('a','i','1','5')
#define FOURCC_ai16     GST_MAKE_FOURCC('a','i','1','6')
#define FOURCC_ai1p     GST_MAKE_FOURCC('a','i','1','p')
#define FOURCC_ai1q     GST_MAKE_FOURCC('a','i','1','q')
#define FOURCC_ai52     GST_MAKE_FOURCC('a','i','5','2')
#define FOURCC_ai53     GST_MAKE_FOURCC('a','i','5','3')
#define FOURCC_ai55     GST_MAKE_FOURCC('a','i','5','5')
#define FOURCC_ai56     GST_MAKE_FOURCC('a','i','5','6')
#define FOURCC_ai5p     GST_MAKE_FOURCC('a','i','5','p')
#define FOURCC_ai5q     GST_MAKE_FOURCC('a','i','5','q')
#define FOURCC_alac     GST_MAKE_FOURCC('a','l','a','c')
#define FOURCC_fLaC     GST_MAKE_FOURCC('f','L','a','C')
#define FOURCC_dfLa     GST_MAKE_FOURCC('d','f','L','a')
#define FOURCC_alaw     GST_MAKE_FOURCC('a','l','a','w')
#define FOURCC_alis     GST_MAKE_FOURCC('a','l','i','s')
#define FOURCC_appl     GST_MAKE_FOURCC('a','p','p','l')
#define FOURCC_avc1     GST_MAKE_FOURCC('a','v','c','1')
#define FOURCC_avc3     GST_MAKE_FOURCC('a','v','c','3')
#define FOURCC_avcC     GST_MAKE_FOURCC('a','v','c','C')
#define FOURCC_c608     GST_MAKE_FOURCC('c','6','0','8')
#define FOURCC_c708     GST_MAKE_FOURCC('c','7','0','8')
#define FOURCC_ccdp     GST_MAKE_FOURCC('c','c','d','p')
#define FOURCC_cdat     GST_MAKE_FOURCC('c','d','a','t')
#define FOURCC_cdt2     GST_MAKE_FOURCC('c','d','t','2')
#define FOURCC_clcp     GST_MAKE_FOURCC('c','l','c','p')
#define FOURCC_clip     GST_MAKE_FOURCC('c','l','i','p')
#define FOURCC_cmov     GST_MAKE_FOURCC('c','m','o','v')
#define FOURCC_cmvd     GST_MAKE_FOURCC('c','m','v','d')
#define FOURCC_co64     GST_MAKE_FOURCC('c','o','6','4')
#define FOURCC_covr     GST_MAKE_FOURCC('c','o','v','r')
#define FOURCC_cpil     GST_MAKE_FOURCC('c','p','i','l')
#define FOURCC_cprt     GST_MAKE_FOURCC('c','p','r','t')
#define FOURCC_crgn     GST_MAKE_FOURCC('c','r','g','n')
#define FOURCC_ctab     GST_MAKE_FOURCC('c','t','a','b')
#define FOURCC_ctts     GST_MAKE_FOURCC('c','t','t','s')
#define FOURCC_cslg     GST_MAKE_FOURCC('c','s','l','g')
#define FOURCC_d263     GST_MAKE_FOURCC('d','2','6','3')
#define FOURCC_dac3     GST_MAKE_FOURCC('d','a','c','3')
#define FOURCC_damr     GST_MAKE_FOURCC('d','a','m','r')
#define FOURCC_data     GST_MAKE_FOURCC('d','a','t','a')
#define FOURCC_dcom     GST_MAKE_FOURCC('d','c','o','m')
#define FOURCC_desc     GST_MAKE_FOURCC('d','e','s','c')
#define FOURCC_dhlr     GST_MAKE_FOURCC('d','h','l','r')
#define FOURCC_dinf     GST_MAKE_FOURCC('d','i','n','f')
#define FOURCC_disc     GST_MAKE_FOURCC('d','i','s','c')
#define FOURCC_disk     GST_MAKE_FOURCC('d','i','s','k')
#define FOURCC_drac     GST_MAKE_FOURCC('d','r','a','c')
#define FOURCC_dref     GST_MAKE_FOURCC('d','r','e','f')
#define FOURCC_drmi     GST_MAKE_FOURCC('d','r','m','i')
#define FOURCC_drms     GST_MAKE_FOURCC('d','r','m','s')
#define FOURCC_dvcp     GST_MAKE_FOURCC('d','v','c','p')
#define FOURCC_dvc_     GST_MAKE_FOURCC('d','v','c',' ')
#define FOURCC_dv5p     GST_MAKE_FOURCC('d','v','5','p')
#define FOURCC_dv5n     GST_MAKE_FOURCC('d','v','5','n')
#define FOURCC_dva1     GST_MAKE_FOURCC('d','v','a','1')
#define FOURCC_dvav     GST_MAKE_FOURCC('d','v','a','v')
#define FOURCC_dvh1     GST_MAKE_FOURCC('d','v','h','1')
#define FOURCC_dvhe     GST_MAKE_FOURCC('d','v','h','e')
#define FOURCC_dvcC     GST_MAKE_FOURCC('d','v','c','C')
#define FOURCC_edts     GST_MAKE_FOURCC('e','d','t','s')
#define FOURCC_elst     GST_MAKE_FOURCC('e','l','s','t')
#define FOURCC_enda     GST_MAKE_FOURCC('e','n','d','a')
#define FOURCC_esds     GST_MAKE_FOURCC('e','s','d','s')
#define FOURCC_fmp4     GST_MAKE_FOURCC('f','m','p','4')
#define FOURCC_free     GST_MAKE_FOURCC('f','r','e','e')
#define FOURCC_frma     GST_MAKE_FOURCC('f','r','m','a')
#define FOURCC_ftyp     GST_MAKE_FOURCC('f','t','y','p')
#define FOURCC_ftab     GST_MAKE_FOURCC('f','t','a','b')
#define FOURCC_gama     GST_MAKE_FOURCC('g','a','m','a')
#define FOURCC_glbl     GST_MAKE_FOURCC('g','l','b','l')
#define FOURCC_gmhd     GST_MAKE_FOURCC('g','m','h','d')
#define FOURCC_gmin     GST_MAKE_FOURCC('g','m','i','n')
#define FOURCC_gnre     GST_MAKE_FOURCC('g','n','r','e')
#define FOURCC_h263     GST_MAKE_FOURCC('h','2','6','3')
#define FOURCC_hdlr     GST_MAKE_FOURCC('h','d','l','r')
#define FOURCC_hev1     GST_MAKE_FOURCC('h','e','v','1')
#define FOURCC_hint     GST_MAKE_FOURCC('h','i','n','t')
#define FOURCC_hmhd     GST_MAKE_FOURCC('h','m','h','d')
#define FOURCC_hndl     GST_MAKE_FOURCC('h','n','d','l')
#define FOURCC_hnti     GST_MAKE_FOURCC('h','n','t','i')
#define FOURCC_hvc1     GST_MAKE_FOURCC('h','v','c','1')
#define FOURCC_hvcC     GST_MAKE_FOURCC('h','v','c','C')
#define FOURCC_ilst     GST_MAKE_FOURCC('i','l','s','t')
#define FOURCC_ima4     GST_MAKE_FOURCC('i','m','a','4')
#define FOURCC_imap     GST_MAKE_FOURCC('i','m','a','p')
#define FOURCC_s16l     GST_MAKE_FOURCC('s','1','6','l')
#define FOURCC_in24     GST_MAKE_FOURCC('i','n','2','4')
#define FOURCC_in32     GST_MAKE_FOURCC('i','n','3','2')
#define FOURCC_fl64     GST_MAKE_FOURCC('f','l','6','4')
#define FOURCC_fl32     GST_MAKE_FOURCC('f','l','3','2')
#define FOURCC_jp2c     GST_MAKE_FOURCC('j','p','2','c')
#define FOURCC_jpeg     GST_MAKE_FOURCC('j','p','e','g')
#define FOURCC_keyw     GST_MAKE_FOURCC('k','e','y','w')
#define FOURCC_kmat     GST_MAKE_FOURCC('k','m','a','t')
#define FOURCC_kywd     GST_MAKE_FOURCC('k','y','w','d')
#define FOURCC_load     GST_MAKE_FOURCC('l','o','a','d')
#define FOURCC_matt     GST_MAKE_FOURCC('m','a','t','t')
#define FOURCC_mdat     GST_MAKE_FOURCC('m','d','a','t')
#define FOURCC_mdhd     GST_MAKE_FOURCC('m','d','h','d')
#define FOURCC_mdia     GST_MAKE_FOURCC('m','d','i','a')
#define FOURCC_mdir     GST_MAKE_FOURCC('m','d','i','r')
#define FOURCC_mean     GST_MAKE_FOURCC('m','e','a','n')
#define FOURCC_meta     GST_MAKE_FOURCC('m','e','t','a')
#define FOURCC_mhlr     GST_MAKE_FOURCC('m','h','l','r')
#define FOURCC_minf     GST_MAKE_FOURCC('m','i','n','f')
#define FOURCC_moov     GST_MAKE_FOURCC('m','o','o','v')
#define FOURCC_mp3_     GST_MAKE_FOURCC('m','p','3',' ')
#define FOURCC_mp4a     GST_MAKE_FOURCC('m','p','4','a')
#define FOURCC_mp4s     GST_MAKE_FOURCC('m','p','4','s')
#define FOURCC_mp4s     GST_MAKE_FOURCC('m','p','4','s')
#define FOURCC_mp4v     GST_MAKE_FOURCC('m','p','4','v')
#define FOURCC_name     GST_MAKE_FOURCC('n','a','m','e')
#define FOURCC_nclc     GST_MAKE_FOURCC('n','c','l','c')
#define FOURCC_nclx     GST_MAKE_FOURCC('n','c','l','x')
#define FOURCC_nmhd     GST_MAKE_FOURCC('n','m','h','d')
#define FOURCC_opus     GST_MAKE_FOURCC('O','p','u','s')
#define FOURCC_dops     GST_MAKE_FOURCC('d','O','p','s')
#define FOURCC_pasp     GST_MAKE_FOURCC('p','a','s','p')
#define FOURCC_colr     GST_MAKE_FOURCC('c','o','l','r')
#define FOURCC_clap     GST_MAKE_FOURCC('c','l','a','p')
#define FOURCC_tapt     GST_MAKE_FOURCC('t','a','p','t')
#define FOURCC_clef     GST_MAKE_FOURCC('c','l','e','f')
#define FOURCC_prof     GST_MAKE_FOURCC('p','r','o','f')
#define FOURCC_enof     GST_MAKE_FOURCC('e','n','o','f')
#define FOURCC_fiel     GST_MAKE_FOURCC('f','i','e','l')
#define FOURCC_pcst     GST_MAKE_FOURCC('p','c','s','t')
#define FOURCC_pgap     GST_MAKE_FOURCC('p','g','a','p')
#define FOURCC_png      GST_MAKE_FOURCC('p','n','g',' ')
#define FOURCC_pnot     GST_MAKE_FOURCC('p','n','o','t')
#define FOURCC_qt__     GST_MAKE_FOURCC('q','t',' ',' ')
#define FOURCC_qtim     GST_MAKE_FOURCC('q','t','i','m')
#define FOURCC_raw_     GST_MAKE_FOURCC('r','a','w',' ')
#define FOURCC_rdrf     GST_MAKE_FOURCC('r','d','r','f')
#define FOURCC_rle_     GST_MAKE_FOURCC('r','l','e',' ')
#define FOURCC_rmda     GST_MAKE_FOURCC('r','m','d','a')
#define FOURCC_rmdr     GST_MAKE_FOURCC('r','m','d','r')
#define FOURCC_rmra     GST_MAKE_FOURCC('r','m','r','a')
#define FOURCC_rmvc     GST_MAKE_FOURCC('r','m','v','c')
#define FOURCC_rtp_     GST_MAKE_FOURCC('r','t','p',' ')
#define FOURCC_rtsp     GST_MAKE_FOURCC('r','t','s','p')
#define FOURCC_s263     GST_MAKE_FOURCC('s','2','6','3')
#define FOURCC_samr     GST_MAKE_FOURCC('s','a','m','r')
#define FOURCC_sawb     GST_MAKE_FOURCC('s','a','w','b')
#define FOURCC_sbtl     GST_MAKE_FOURCC('s','b','t','l')
#define FOURCC_sdp_     GST_MAKE_FOURCC('s','d','p',' ')
#define FOURCC_sidx     GST_MAKE_FOURCC('s','i','d','x')
#define FOURCC_skip     GST_MAKE_FOURCC('s','k','i','p')
#define FOURCC_smhd     GST_MAKE_FOURCC('s','m','h','d')
#define FOURCC_soaa     GST_MAKE_FOURCC('s','o','a','a')
#define FOURCC_soal     GST_MAKE_FOURCC('s','o','a','l')
#define FOURCC_soar     GST_MAKE_FOURCC('s','o','a','r')
#define FOURCC_soco     GST_MAKE_FOURCC('s','o','c','o')
#define FOURCC_sonm     GST_MAKE_FOURCC('s','o','n','m')
#define FOURCC_sosn     GST_MAKE_FOURCC('s','o','s','n')
#define FOURCC_soun     GST_MAKE_FOURCC('s','o','u','n')
#define FOURCC_sowt     GST_MAKE_FOURCC('s','o','w','t')
#define FOURCC_stbl     GST_MAKE_FOURCC('s','t','b','l')
#define FOURCC_stco     GST_MAKE_FOURCC('s','t','c','o')
#define FOURCC_stpp     GST_MAKE_FOURCC('s','t','p','p')
#define FOURCC_stps     GST_MAKE_FOURCC('s','t','p','s')
#define FOURCC_strf     GST_MAKE_FOURCC('s','t','r','f')
#define FOURCC_strm     GST_MAKE_FOURCC('s','t','r','m')
#define FOURCC_stsc     GST_MAKE_FOURCC('s','t','s','c')
#define FOURCC_stsd     GST_MAKE_FOURCC('s','t','s','d')
#define FOURCC_stss     GST_MAKE_FOURCC('s','t','s','s')
#define FOURCC_stsz     GST_MAKE_FOURCC('s','t','s','z')
#define FOURCC_stts     GST_MAKE_FOURCC('s','t','t','s')
#define FOURCC_styp     GST_MAKE_FOURCC('s','t','y','p')
#define FOURCC_subp     GST_MAKE_FOURCC('s','u','b','p')
#define FOURCC_subt     GST_MAKE_FOURCC('s','u','b','t')
#define FOURCC_text     GST_MAKE_FOURCC('t','e','x','t')
#define FOURCC_tcmi     GST_MAKE_FOURCC('t','c','m','i')
#define FOURCC_tkhd     GST_MAKE_FOURCC('t','k','h','d')
#define FOURCC_tmcd     GST_MAKE_FOURCC('t','m','c','d')
#define FOURCC_tmpo     GST_MAKE_FOURCC('t','m','p','o')
#define FOURCC_trak     GST_MAKE_FOURCC('t','r','a','k')
#define FOURCC_tref     GST_MAKE_FOURCC('t','r','e','f')
#define FOURCC_trkn     GST_MAKE_FOURCC('t','r','k','n')
#define FOURCC_tven     GST_MAKE_FOURCC('t','v','e','n')
#define FOURCC_tves     GST_MAKE_FOURCC('t','v','e','s')
#define FOURCC_tvsh     GST_MAKE_FOURCC('t','v','s','h')
#define FOURCC_tvsn     GST_MAKE_FOURCC('t','v','s','n')
#define FOURCC_twos     GST_MAKE_FOURCC('t','w','o','s')
#define FOURCC_tx3g     GST_MAKE_FOURCC('t','x','3','g')
#define FOURCC_udta     GST_MAKE_FOURCC('u','d','t','a')
#define FOURCC_ulaw     GST_MAKE_FOURCC('u','l','a','w')
#define FOURCC_url_     GST_MAKE_FOURCC('u','r','l',' ')
#define FOURCC_uuid     GST_MAKE_FOURCC('u','u','i','d')
#define FOURCC_v210     GST_MAKE_FOURCC('v','2','1','0')
#define FOURCC_vc_1     GST_MAKE_FOURCC('v','c','-','1')
#define FOURCC_vide     GST_MAKE_FOURCC('v','i','d','e')
#define FOURCC_vmhd     GST_MAKE_FOURCC('v','m','h','d')
#define FOURCC_vp08     GST_MAKE_FOURCC('v','p','0','8')
#define FOURCC_vp09     GST_MAKE_FOURCC('v','p','0','9')
#define FOURCC_vpcC     GST_MAKE_FOURCC('v','p','c','C')
#define FOURCC_xvid     GST_MAKE_FOURCC('x','v','i','d')
#define FOURCC_wave     GST_MAKE_FOURCC('w','a','v','e')
#define FOURCC_wide     GST_MAKE_FOURCC('w','i','d','e')
#define FOURCC_zlib     GST_MAKE_FOURCC('z','l','i','b')
#define FOURCC_lpcm     GST_MAKE_FOURCC('l','p','c','m')
#define FOURCC_av01     GST_MAKE_FOURCC('a','v','0','1')
#define FOURCC_av1C     GST_MAKE_FOURCC('a','v','1','C')
#define FOURCC_av1f     GST_MAKE_FOURCC('a','v','1','f')
#define FOURCC_av1m     GST_MAKE_FOURCC('a','v','1','m')
#define FOURCC_av1s     GST_MAKE_FOURCC('a','v','1','s')
#define FOURCC_av1M     GST_MAKE_FOURCC('a','v','1','M')

#define FOURCC_cfhd     GST_MAKE_FOURCC('C','F','H','D')
#define FOURCC_ap4x     GST_MAKE_FOURCC('a','p','4','x')
#define FOURCC_ap4h     GST_MAKE_FOURCC('a','p','4','h')
#define FOURCC_apch     GST_MAKE_FOURCC('a','p','c','h')
#define FOURCC_apcn     GST_MAKE_FOURCC('a','p','c','n')
#define FOURCC_apco     GST_MAKE_FOURCC('a','p','c','o')
#define FOURCC_apcs     GST_MAKE_FOURCC('a','p','c','s')
#define FOURCC_m1v      GST_MAKE_FOURCC('m','1','v',' ')
#define FOURCC_vivo     GST_MAKE_FOURCC('v','i','v','o')
#define FOURCC_saiz     GST_MAKE_FOURCC('s','a','i','z')
#define FOURCC_saio     GST_MAKE_FOURCC('s','a','i','o')

#define FOURCC_3gg6     GST_MAKE_FOURCC('3','g','g','6')
#define FOURCC_3gg7     GST_MAKE_FOURCC('3','g','g','7')
#define FOURCC_3gp4     GST_MAKE_FOURCC('3','g','p','4')
#define FOURCC_3gp6     GST_MAKE_FOURCC('3','g','p','6')
#define FOURCC_3gr6     GST_MAKE_FOURCC('3','g','r','6')
#define FOURCC_3g__     GST_MAKE_FOURCC('3','g',0,0)
#define FOURCC_isml     GST_MAKE_FOURCC('i','s','m','l')
#define FOURCC_iso2     GST_MAKE_FOURCC('i','s','o','2')
#define FOURCC_isom     GST_MAKE_FOURCC('i','s','o','m')
#define FOURCC_mp41     GST_MAKE_FOURCC('m','p','4','1')
#define FOURCC_mp42     GST_MAKE_FOURCC('m','p','4','2')
#define FOURCC_piff     GST_MAKE_FOURCC('p','i','f','f')
#define FOURCC_titl     GST_MAKE_FOURCC('t','i','t','l')

/* SVQ3 fourcc */
#define FOURCC_SEQH     GST_MAKE_FOURCC('S','E','Q','H')
#define FOURCC_SMI_     GST_MAKE_FOURCC('S','M','I',' ')

/* 3gpp asset meta data fourcc */
#define FOURCC_albm     GST_MAKE_FOURCC('a','l','b','m')
#define FOURCC_auth     GST_MAKE_FOURCC('a','u','t','h')
#define FOURCC_clsf     GST_MAKE_FOURCC('c','l','s','f')
#define FOURCC_dscp     GST_MAKE_FOURCC('d','s','c','p')
#define FOURCC_loci     GST_MAKE_FOURCC('l','o','c','i')
#define FOURCC_perf     GST_MAKE_FOURCC('p','e','r','f')
#define FOURCC_rtng     GST_MAKE_FOURCC('r','t','n','g')
#define FOURCC_yrrc     GST_MAKE_FOURCC('y','r','r','c')

/* misc tag stuff */
#define FOURCC_ID32     GST_MAKE_FOURCC('I', 'D','3','2')

/* ISO Motion JPEG 2000 fourcc */
#define FOURCC_cdef     GST_MAKE_FOURCC('c','d','e','f')
#define FOURCC_cmap     GST_MAKE_FOURCC('c','m','a','p')
#define FOURCC_ihdr     GST_MAKE_FOURCC('i','h','d','r')
#define FOURCC_jp2h     GST_MAKE_FOURCC('j','p','2','h')
#define FOURCC_jp2x     GST_MAKE_FOURCC('j','p','2','x')
#define FOURCC_mjp2     GST_MAKE_FOURCC('m','j','p','2')

/* some buggy hardware's notion of mdhd */
#define FOURCC_mhdr     GST_MAKE_FOURCC('m','h','d','r')

/* Fragmented MP4 */
#define FOURCC_btrt     GST_MAKE_FOURCC('b','t','r','t')
#define FOURCC_mehd     GST_MAKE_FOURCC('m','e','h','d')
#define FOURCC_mfhd     GST_MAKE_FOURCC('m','f','h','d')
#define FOURCC_mfra     GST_MAKE_FOURCC('m','f','r','a')
#define FOURCC_mfro     GST_MAKE_FOURCC('m','f','r','o')
#define FOURCC_moof     GST_MAKE_FOURCC('m','o','o','f')
#define FOURCC_mvex     GST_MAKE_FOURCC('m','v','e','x')
#define FOURCC_mvhd     GST_MAKE_FOURCC('m','v','h','d')
#define FOURCC_ovc1     GST_MAKE_FOURCC('o','v','c','1')
#define FOURCC_owma     GST_MAKE_FOURCC('o','w','m','a')
#define FOURCC_sdtp     GST_MAKE_FOURCC('s','d','t','p')
#define FOURCC_tfhd     GST_MAKE_FOURCC('t','f','h','d')
#define FOURCC_tfra     GST_MAKE_FOURCC('t','f','r','a')
#define FOURCC_traf     GST_MAKE_FOURCC('t','r','a','f')
#define FOURCC_trex     GST_MAKE_FOURCC('t','r','e','x')
#define FOURCC_trun     GST_MAKE_FOURCC('t','r','u','n')
#define FOURCC_wma_     GST_MAKE_FOURCC('w','m','a',' ')

/* MPEG DASH */
#define FOURCC_tfdt     GST_MAKE_FOURCC('t','f','d','t')

/* Xiph fourcc */
#define FOURCC_XdxT     GST_MAKE_FOURCC('X','d','x','T')
#define FOURCC_XiTh     GST_MAKE_FOURCC('X','i','T','h')
#define FOURCC_tCtC     GST_MAKE_FOURCC('t','C','t','C')
#define FOURCC_tCtH     GST_MAKE_FOURCC('t','C','t','H')
#define FOURCC_tCt_     GST_MAKE_FOURCC('t','C','t','#')

/* ilst metatags */
#define FOURCC__cmt     GST_MAKE_FOURCC(0xa9, 'c','m','t')

/* apple tags */
#define FOURCC__mak     GST_MAKE_FOURCC(0xa9, 'm','a','k')
#define FOURCC__mod     GST_MAKE_FOURCC(0xa9, 'm','o','d')
#define FOURCC__swr     GST_MAKE_FOURCC(0xa9, 's','w','r')

/* Chapters reference */
#define FOURCC_chap     GST_MAKE_FOURCC('c','h','a','p')

/* For Microsoft Wave formats embedded in quicktime, the FOURCC is
   'm', 's', then the 16 bit wave codec id */
#define MS_WAVE_FOURCC(codecid)  GST_MAKE_FOURCC( \
        'm', 's', ((codecid)>>8)&0xff, ((codecid)&0xff))

/* MPEG Application Format , Stereo Video */
#define FOURCC_ss01     GST_MAKE_FOURCC('s','s','0','1')
#define FOURCC_ss02     GST_MAKE_FOURCC('s','s','0','2')
#define FOURCC_svmi     GST_MAKE_FOURCC('s','v','m','i')
#define FOURCC_scdi     GST_MAKE_FOURCC('s','c','d','i')

/* Protected streams */
#define FOURCC_encv     GST_MAKE_FOURCC('e','n','c','v')
#define FOURCC_enca     GST_MAKE_FOURCC('e','n','c','a')
#define FOURCC_enct     GST_MAKE_FOURCC('e','n','c','t')
#define FOURCC_encs     GST_MAKE_FOURCC('e','n','c','s')
#define FOURCC_sinf     GST_MAKE_FOURCC('s','i','n','f')
#define FOURCC_frma     GST_MAKE_FOURCC('f','r','m','a')
#define FOURCC_schm     GST_MAKE_FOURCC('s','c','h','m')
#define FOURCC_schi     GST_MAKE_FOURCC('s','c','h','i')

/* Common Encryption */
#define FOURCC_pssh     GST_MAKE_FOURCC('p','s','s','h')
#define FOURCC_tenc     GST_MAKE_FOURCC('t','e','n','c')
#define FOURCC_cenc     GST_MAKE_FOURCC('c','e','n','c')
#define FOURCC_cbcs     GST_MAKE_FOURCC('c','b','c','s')

/* Audible AAX encrypted audio */
#define FOURCC_aavd     GST_MAKE_FOURCC('a','a','v','d')
#define FOURCC_adrm     GST_MAKE_FOURCC('a','d','r','m')

#define FOURCC_vttc     GST_MAKE_FOURCC('v','t','t','c')

#define FOURCC_sbgp     GST_MAKE_FOURCC('s','b','g','p')
#define FOURCC_sgpd     GST_MAKE_FOURCC('s','g','p','d')
#define FOURCC_wvtt     GST_MAKE_FOURCC('w','v','t','t')

#define FOURCC_metx     GST_MAKE_FOURCC('m','e','t','x')
#define FOURCC_cstb     GST_MAKE_FOURCC('c','s','t','b')


#define QT_FLAG_CONTAINER true

#include <tuple>

struct fourcc_info {
    uint32_t fourcc;
    std::string name;
    size_t min_size;
};

const fourcc_info CONTAINER_LIST[] = {

    {FOURCC_moov, "movie", 0,},
    {FOURCC_vttc, "VTTCueBox 14496-30", 0},
    {FOURCC_clip, "clipping", 0,},
    {FOURCC_trak, "track", 0,},
    {FOURCC_udta, "user data", 0,},
    {FOURCC_matt, "track matte", 0,},
    {FOURCC_edts, "edit", 0,},
    {FOURCC_tref, "track reference", 0,},
    {FOURCC_imap, "track input map", 0,},
    {FOURCC_mdia, "media", 0},
    {FOURCC_minf, "media information", 0},
    {FOURCC_gmhd, "base media information header", 0},
    {FOURCC_dinf, "data information", 0},
    {FOURCC_stbl, "sample table", 0}, 
    {FOURCC_cmov, "compressed movie", 0}, 
    {FOURCC_mhdr, "mhdr", 0,},
    {FOURCC_jp2h, "jp2h", 0,},
    {FOURCC_wave, "wave", 0},
    {FOURCC_appl, "appl", 0},
    {FOURCC_cfhd, "cfhd", 0},
    {FOURCC_hnti, "hnti", 0}, 
    {FOURCC_ilst, "ilst", 0,},
    {FOURCC__nam, "Name", 0,},
    {FOURCC_titl, "Title", 0,},
    {FOURCC__ART, "Artist", 0,},
    {FOURCC_aART, "Album Artist", 0,},
    {FOURCC_auth, "Author", 0,},
    {FOURCC_perf, "Performer", 0,},
    {FOURCC__wrt, "Writer", 0,},
    {FOURCC__grp, "Grouping", 0,},
    {FOURCC__alb, "Album", 0,},
    {FOURCC_albm, "Album", 0,},
    {FOURCC__day, "Date", 0,},
    {FOURCC__cpy, "Copyright", 0,},
    {FOURCC__cmt, "Comment", 0,},
    {FOURCC__des, "Description", 0,},
    {FOURCC_desc, "Description", 0,},
    {FOURCC_dscp, "Description", 0,},
    {FOURCC__lyr, "Lyrics", 0,},
    {FOURCC__req, "Requirement", 0,},
    {FOURCC__enc, "Encoder", 0,},
    {FOURCC_gnre, "Genre", 0,},
    {FOURCC_trkn, "Track Number", 0,},
    {FOURCC_disc, "Disc Number", 0,},
    {FOURCC_disk, "Disc Number", 0,},
    {FOURCC_cprt, "Copyright", 0,},
    {FOURCC_cpil, "Compilation", 0,},
    {FOURCC_pgap, "Gapless", 0,},
    {FOURCC_pcst, "Podcast", 0,},
    {FOURCC_tmpo, "Tempo", 0,},
    {FOURCC_covr, "Cover", 0,},
    {FOURCC_sonm, "Sort Title", 0,},
    {FOURCC_soal, "Sort Album", 0,},
    {FOURCC_soar, "Sort Artist", 0,},
    {FOURCC_soaa, "Sort Album Artist", 0,},
    {FOURCC_soco, "Sort Composer", 0,},
    {FOURCC_sosn, "Sort TV Show", 0,},
    {FOURCC_tvsh, "TV Show", 0,},
    {FOURCC_tven, "TV Episode ID", 0,},
    {FOURCC_tvsn, "TV Season Number", 0,},
    {FOURCC_tves, "TV Episode Number", 0,},
    {FOURCC_keyw, "Keywords", 0,},
    {FOURCC_kywd, "Keywords", 0,},
    {FOURCC__too, "Encoder", 0,},
    {FOURCC__swr, "Application Name", 0,},
    {FOURCC_____, "----", 0,}, 
    {FOURCC_rmra, "rmra", 0,},
    {FOURCC_rmda, "rmda", 0,},
    {FOURCC__gen, "Custom Genre", 0,},
    {FOURCC_mfra, "movie fragment random access", 0,},
    {FOURCC_moof, "movie fragment", 0,},
    {FOURCC_traf, "track fragment", 0,}, 
    {FOURCC_mvex, "mvex", 0,}, 
    {FOURCC_sinf, "protection scheme information", 0},
    {FOURCC_schi, "scheme information", 0},

    {FOURCC_stsd, "sample description", 16,},

    {FOURCC_mp4a, "mp4a", 72,},
    {FOURCC_alac, "alac", 72,},
    {FOURCC_fLaC, "fLaC", 72,},
    {FOURCC_aavd, "AAX encrypted audio", 72},
    {FOURCC_opus, "opus", 72,},

    {FOURCC_mp4v, "mp4v", 86,},
    {FOURCC_avc1, "AV codec configuration v1", 86},
    {FOURCC_avc3, "AV codec configuration v3", 86},
    {FOURCC_hvc1, "HEVC codec configuration", 86},
    {FOURCC_hev1, "HEVC codec configuration", 86},
    {FOURCC_dvh1, "HEVC-based Dolby Vision codec derived from hvc1 ", 86},
    {FOURCC_dvhe, "HEVC-based Dolby Vision codec derived from hev1 ", 86},
    {FOURCC_mjp2, "mjp2", 86,},
    {FOURCC_encv, "encrypted visual sample entry", 86},
    
    {FOURCC_meta, "meta", 16,},

    {FOURCC_mp4s, "VOBSUB codec configuration", 16},

    {FOURCC_XiTh, "XiTh", 98},

    {FOURCC_in24, "in24", 52,},

    {FOURCC_enca, "encrypted audio sample entry", 54}
};


//3rd field = padding (bytes)
const fourcc_info FOURCC_LIST[] =  {

    {FOURCC_crgn, "clipping region", 0,},
    {FOURCC_kmat, "compressed matte", 0,},
    {FOURCC_elst, "edit list", 0,},
    {FOURCC_load, "track load settings", 0,},
    {FOURCC___in, "track input", 0,},     /* special container */
    {FOURCC___ty, "input type", 0,},
    {FOURCC_mdhd, "media header", 0,},
    {FOURCC_hdlr, "handler reference", 0,},
    {FOURCC_vmhd, "video media information", 0,},
    {FOURCC_smhd, "sound media information", 0},
    {FOURCC_nmhd, "null media information", 0},
    {FOURCC_gmin, "base media info", 0,},
    {FOURCC_dref, "data reference", 0,},

    {FOURCC_stts, "time-to-sample", 0,},
    {FOURCC_stps, "partial sync sample", 0,},
    {FOURCC_stss, "sync sample", 0,},
    {FOURCC_stsc, "sample-to-chunk", 0,},
    {FOURCC_stsz, "sample size", 0,},
    {FOURCC_stco, "chunk offset", 0,},
    {FOURCC_co64, "64-bit chunk offset", 0,},
    {FOURCC_vide, "video media", 0},
    {FOURCC_dcom, "compressed data", 0,},
    {FOURCC_cmvd, "compressed movie data", 0,},
    {FOURCC_hint, "hint", 0,},



    {FOURCC_colr, "colr", 0,},
    {FOURCC_pasp, "pasp", 0,},
    {FOURCC_clap, "clap", 0,},
    {FOURCC_tapt, "tapt", 0,},
    {FOURCC_ihdr, "ihdr", 0,},
    {FOURCC_fiel, "fiel", 0,},
    {FOURCC_jp2x, "jp2x", 0,},
 
    {FOURCC_dfLa, "dfLa", 0,},

    {FOURCC_dops, "dOps", 0,},
    {FOURCC_esds, "esds", 0},
    {FOURCC_rtp_, "rtp ", 0,},
    {FOURCC_sdp_, "sdp ", 0,},

    {FOURCC_data, "data", 0,},
    {FOURCC_free, "free", 0,},
    {FOURCC_skip, "skip", 0,},
    {FOURCC_SVQ3, "SVQ3", 0,},
    {FOURCC_rdrf, "rdrf", 0,},
    {FOURCC_ctts, "Composition time to sample", 0,},
    {FOURCC_cslg, "Composition Shift Least Greatest", 0,},

    {FOURCC_XdxT, "XdxT", 0},
    {FOURCC_loci, "loci", 0},
    {FOURCC_clsf, "clsf", 0},
    {FOURCC_tfra, "track fragment random access", 0,},
    {FOURCC_mfro, "movie fragment random access offset", 0,},
    {FOURCC_mfhd, "movie fragment header", 0,},
    {FOURCC_tfhd, "track fragment header", 0,},
    {FOURCC_sdtp, "independent and disposable samples", 0,},
    {FOURCC_trun, "track fragment run", 0,},
    {FOURCC_mdat, "moovie data", 0,},
    {FOURCC_trex, "moovie data", 0,},
    {FOURCC_mehd, "movie extends header", 0,},
    {FOURCC_ovc1, "ovc1", 0},
    {FOURCC_owma, "owma", 0},
    {FOURCC_avcC, "AV codec configuration container", 0},

    {FOURCC_dva1, "AVC-based Dolby Vision derived from avc1", 0},
    {FOURCC_dvav, "AVC-based Dolby Vision derived from avc3", 0},
    {FOURCC_ai12, "AVC-Intra 100M 1080p25/50", 0},
    {FOURCC_ai13, "AVC-Intra 100M 1080p24/30/60", 0},
    {FOURCC_ai15, "AVC-Intra 100M 1080i50", 0},
    {FOURCC_ai16, "AVC-Intra 100M 1080i60", 0},
    {FOURCC_ai1p, "AVC-Intra 100M 720p24/30/60", 0},
    {FOURCC_ai1q, "AVC-Intra 100M 720p25/50", 0},
    {FOURCC_ai52, "AVC-Intra 50M 1080p25/50", 0},
    {FOURCC_ai53, "AVC-Intra 50M 1080p24/30/60", 0},
    {FOURCC_ai55, "AVC-Intra 50M 1080i50", 0},
    {FOURCC_ai56, "AVC-Intra 50M 1080i60", 0},
    {FOURCC_ai5p, "AVC-Intra 50M 720p24/30/60", 0},
    {FOURCC_ai5q, "AVC-Intra 50M 720p25/50", 0},



    {FOURCC_hvcC, "HEVC codec configuration container", 0},


    {FOURCC_dvcC, "HEVC-based Dolby Vision codec configuration container", 0},
    {FOURCC_tfdt, "Track fragment decode time", 0,},
    {FOURCC_chap, "Chapter Reference", 0},
    {FOURCC_btrt, "Bitrate information", 0},
    {FOURCC_frma, "Audio codec format", 0},
    {FOURCC_name, "name", 0},
    {FOURCC_mean, "mean", 0},
    {FOURCC_svmi, "Stereoscopic Video Media Information", 0,},
    {FOURCC_scdi, "Stereoscopic Camera and Display Information", 0,},
    {FOURCC_saiz, "sample auxiliary information sizes", 0},
    {FOURCC_saio, "sample auxiliary information offsets", 0},


    {FOURCC_enct, "encrypted text sample entry", 0},
    {FOURCC_encs, "encrypted system sample entry", 0},
    {FOURCC_frma, "original format", 0},
    {FOURCC_schm, "scheme type", 0},
    {FOURCC_pssh, "protection system specific header", 0},
    {FOURCC_tenc, "track encryption", 0},
    {FOURCC_sgpd, "sample group description", 0},
    {FOURCC_sbgp, "sample to group", 0},
    {FOURCC_stpp, "XML subtitle sample entry", 0},
    {FOURCC_wvtt, "WebVTT subtitle sample entry", 0},
    {FOURCC_clcp, "Closed Caption", 0},
    {FOURCC_av01, "AV1 Sample Entry", 0},
    {FOURCC_av1C, "AV1 Codec Configuration", 0},
    {FOURCC_av1f, "AV1 Forward Key Frame sample group entry", 0},
    {FOURCC_av1m, "AV1 Multi-Frame sample group entry", 0},
    {FOURCC_av1s, "AV1 S-Frame sample group entry", 0},
    {FOURCC_av1M, "AV1 Metadata sample group entry", 0},

    {FOURCC_adrm, "AAX DRM key data", 0},
    {FOURCC_mvhd, "movie header", 0,},
    {FOURCC_metx, "XML MetaData Sample Entry", 0},
    {FOURCC_cstb, "Correct Start Time Box", 0},
    {FOURCC_ctab, "color table", 0,},
    {FOURCC_tkhd, "track header", 0,}
  };

const uint8_t CONTAINER_LIST_SIZE = sizeof(CONTAINER_LIST)/sizeof(CONTAINER_LIST[0]);
const uint8_t FOURCC_LIST_SIZE = sizeof(FOURCC_LIST)/sizeof(FOURCC_LIST[0]);
