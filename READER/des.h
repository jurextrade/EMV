/* crypto/des/des.h */
/* Copyright (C) 1995-1997 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 *
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */

#ifndef HEADER_DES_H
#define HEADER_DES_H

#ifdef NO_DES
#error DES is disabled.
#endif

#ifdef _KERBEROS_DES_H
#error <openssl/des.h> replaces <kerberos/des.h>.
#endif


#include "opensslconf.h" /* DES_LONG */
#include "e_os2.h"	/* OPENSSL_EXTERN */
#include "chiffrer.h"

#define DES_KEY_SZ 	(sizeof(des_cblock))
#define DES_SCHEDULE_SZ (sizeof(des_key_schedule))

#define DES_ENCRYPT	0
#define DES_DECRYPT	1

#define DES_CBC_MODE	0
#define DES_PCBC_MODE	1

#define des_ecb2_encrypt(i,o,k1,k2,e) \
	des_ecb3_encrypt((i),(o),(k1),(k2),(k1),(e))

//#define des_ede2_cbc_encrypt(i,o,l,k1,k2,iv,e,pP_Handle) \
//	des_ede3_cbc_encrypt((i),(o),(l),(iv),(e),p_WorkData)

#define des_ede2_cfb64_encrypt(i,o,l,k1,k2,iv,n,e) \
	des_ede3_cfb64_encrypt((i),(o),(l),(k1),(k2),(k1),(iv),(n),(e))

#define des_ede2_ofb64_encrypt(i,o,l,k1,k2,iv,n) \
	des_ede3_ofb64_encrypt((i),(o),(l),(k1),(k2),(k1),(iv),(n))

#define HPERM_OP(a,t,n,m) ((t)=((((a)<<(16-(n)))^(a))&(m)),\
	(a)=(a)^(t)^(t>>(16-(n))))

//OPENSSL_EXTERN int des_check_key;	/* defaults to false */
OPENSSL_EXTERN int des_rw_mode;		/* defaults to DES_PCBC_MODE */
OPENSSL_EXTERN int des_set_weak_key_flag; /* set the weak key flag */

const char *des_options(void);
void des_ecb3_encrypt(unsigned char *input, unsigned char *output,
		      des_key_schedule ks1,des_key_schedule ks2,
		      des_key_schedule ks3, int enc);
DES_LONG des_cbc_cksum(const unsigned char *input,unsigned char *output,
		       long length,des_key_schedule schedule,
		       unsigned char *ivec);
/* des_cbc_encrypt does not update the IV!  Use des_ncbc_encrypt instead. */
void des_cbc_encrypt(const unsigned char *input,unsigned char *output,
		     long length,des_key_schedule schedule,unsigned char *ivec,
		     int enc);
void des_ncbc_encrypt(const unsigned char *input,unsigned char *output,
		      long length,des_key_schedule schedule,unsigned char *ivec,
		      int enc);
void des_xcbc_encrypt(const unsigned char *input,unsigned char *output,
		      long length,des_key_schedule schedule,unsigned char *ivec,
		      unsigned char *inw,unsigned char *outw,int enc);
void des_cfb_encrypt(const unsigned char *in,unsigned char *out,int numbits,
		     long length,des_key_schedule schedule,unsigned char *ivec,
		     int enc);
void des_ecb_encrypt(unsigned char *input,unsigned char *output,
		     des_key_schedule ks,int enc);

void des_encrypt(DES_LONG *data, des_key_schedule ks1);
void des_decrypt(DES_LONG *data, des_key_schedule ks1);
/* 	This is the DES encryption function that gets called by just about
	every other DES routine in the library.  You should not use this
	function except to implement 'modes' of DES.  I say this because the
	functions that call this routine do the conversion from 'char *' to
	long, and this needs to be done to make sure 'non-aligned' memory
	access do not occur.  The characters are loaded 'little endian'.
	Data is a pointer to 2 unsigned long's and ks is the
	des_key_schedule to use.  enc, is non zero specifies encryption,
	zero if decryption. */
void des_encrypt1(DES_LONG *data,des_key_schedule ks, int enc);

/* 	This functions is the same as des_encrypt1() except that the DES
	initial permutation (IP) and final permutation (FP) have been left
	out.  As for des_encrypt1(), you should not use this function.
	It is used by the routines in the library that implement triple DES.
	IP() des_encrypt2() des_encrypt2() des_encrypt2() FP() is the same
	as des_encrypt1() des_encrypt1() des_encrypt1() except faster :-). */
void des_encrypt2(DES_LONG *data,des_key_schedule ks, int enc);

void des_encrypt3(DES_LONG *data, des_key_schedule ks1,
	des_key_schedule ks2, des_key_schedule ks3);
void des_decrypt3(DES_LONG *data, des_key_schedule ks1,
	des_key_schedule ks2, des_key_schedule ks3);
void des_ede3_cbc_encrypt(unsigned char *input,
			  unsigned char *output,
			  unsigned short length,
			  unsigned char *ivec,
			  int enc,
			  TDES_WORK_DATA   * p_WorkData);
void des_ede3_cbcm_encrypt(const unsigned char *in,unsigned char *out,
			   long length,
			   des_key_schedule ks1,des_key_schedule ks2,
			   des_key_schedule ks3,
			   unsigned char *ivec1,unsigned char *ivec2,
			   int enc);
void des_ede3_cfb64_encrypt(const unsigned char *in,unsigned char *out,
			    long length,des_key_schedule ks1,
			    des_key_schedule ks2,des_key_schedule ks3,
			    unsigned char *ivec,int *num,int enc);
void des_ede3_ofb64_encrypt(const unsigned char *in,unsigned char *out,
			    long length,des_key_schedule ks1,
			    des_key_schedule ks2,des_key_schedule ks3,
			    unsigned char *ivec,int *num);

void des_xwhite_in2out(unsigned char *des_key,unsigned char *in_white,
		       unsigned char *out_white);

int des_enc_read(int fd,void *buf,int len,des_key_schedule sched,
		 unsigned char *iv);
int des_enc_write(int fd,const void *buf,int len,des_key_schedule sched,
		  unsigned char *iv);
char *des_fcrypt(const char *buf,const char *salt, char *ret);
char *des_crypt(const char *buf,const char *salt);
#if !defined(PERL5) && !defined(__FreeBSD__) && !defined(NeXT)
char *crypt(const char *buf,const char *salt);
#endif
void des_ofb_encrypt(const unsigned char *in,unsigned char *out,int numbits,
		     long length,des_key_schedule schedule,unsigned char *ivec);
void des_pcbc_encrypt(const unsigned char *input,unsigned char *output,
		      long length,des_key_schedule schedule,unsigned char *ivec,
		      int enc);
DES_LONG des_quad_cksum(const unsigned char *input,unsigned char output[],
			long length,int out_count,unsigned char *seed);
void des_random_seed(unsigned char *key);
int des_random_key(unsigned char *ret);
int des_read_password(unsigned char *key,const char *prompt,int verify);
int des_read_2passwords(unsigned char *key1,unsigned char *key2,
			const char *prompt,int verify);
int des_read_pw_string(char *buf,int length,const char *prompt,int verify);
void des_set_odd_parity(unsigned char *key);
int des_check_key_parity(unsigned char *key);
int des_is_weak_key(unsigned char *key);
/* des_set_key (= set_key = des_key_sched = key_sched) calls
 * des_set_key_checked if global variable des_check_key is set,
 * des_set_key_unchecked otherwise. */
int des_set_key(unsigned char *,des_key_schedule ,TDES_WORK_DATA*);
int des_key_sched(unsigned char *key,des_key_schedule schedule,TDES_WORK_DATA*);
int des_set_key_checked(unsigned char *key,des_key_schedule schedule);
void des_set_key_unchecked(unsigned char *key,des_key_schedule schedule);
void des_string_to_key(const char *str,unsigned char *key);
void des_string_to_2keys(const char *str,unsigned char *key1,unsigned char *key2);
void des_cfb64_encrypt(const unsigned char *in,unsigned char *out,long length,
		       des_key_schedule schedule,unsigned char *ivec,int *num,
		       int enc);
void des_ofb64_encrypt(const unsigned char *in,unsigned char *out,long length,
		       des_key_schedule schedule,unsigned char *ivec,int *num);
int des_read_pw(char *buf,char *buff,int size,const char *prompt,int verify);

/* The following definitions provide compatibility with the MIT Kerberos
 * library. The des_key_schedule structure is not binary compatible. */

#define _KERBEROS_DES_H

#define KRBDES_ENCRYPT DES_ENCRYPT
#define KRBDES_DECRYPT DES_DECRYPT

#ifdef KERBEROS
#  define ENCRYPT DES_ENCRYPT
#  define DECRYPT DES_DECRYPT
#endif

#ifndef NCOMPAT
#  define C_Block des_cblock
#  define Key_schedule des_key_schedule
#  define KEY_SZ DES_KEY_SZ
#  define string_to_key des_string_to_key
#  define read_pw_string des_read_pw_string
#  define random_key des_random_key
#  define pcbc_encrypt des_pcbc_encrypt
#  define set_key des_set_key
#  define key_sched des_key_sched
#  define ecb_encrypt des_ecb_encrypt
#  define cbc_encrypt des_cbc_encrypt
#  define ncbc_encrypt des_ncbc_encrypt
#  define xcbc_encrypt des_xcbc_encrypt
#  define cbc_cksum des_cbc_cksum
#  define quad_cksum des_quad_cksum
#  define check_parity des_check_key_parity
#endif

typedef des_key_schedule bit_64;
#define des_fixup_key_parity des_set_odd_parity

#endif
